#!/usr/bin/env python3
"""Stage the Mac-hosted Linux build without game data or runtime compilers."""
import argparse
import json
from pathlib import Path
import re
import shutil
import subprocess
import sys

from check_static_overlays import check as check_static_overlays
from deliver_windows import verify
from stage_windows import digest

ROOT = Path(__file__).resolve().parents[1]
ALLOWED = re.compile(r'(?:lib(?:c|m|dl|pthread|rt)\.so\.\d+|ld-linux-x86-64\.so\.2|lib(?:GL|OpenGL|GLX)\.so\.[01])')


def audit_elf(report):
    if 'ELF64' not in report or not re.search(r'Machine:.*[xX]86-64', report):
        raise ValueError('Expected Linux x86-64 ELF')
    needed = re.findall(r'Shared library: \[([^]]+)\]', report)
    if not needed or any(not ALLOWED.fullmatch(name) for name in needed):
        raise ValueError(f'Unexpected dynamic dependencies: {needed}')
    versions = [tuple(map(int, v.split('.'))) for v in re.findall(r'GLIBC_([0-9.]+)', report)]
    if not versions or max(versions) > (2, 31):
        raise ValueError('Build exceeds the glibc 2.31 compatibility baseline')
    return needed, '.'.join(map(str, max(versions)))


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('version')
    p.add_argument('--build', type=Path, default=ROOT / 'build-linux')
    p.add_argument('--llvm', required=True, type=Path, help='Mac-hosted LLVM tools directory')
    p.add_argument('--sdk-digest', required=True)
    a = p.parse_args()
    if not re.fullmatch(r'[A-Za-z0-9][A-Za-z0-9._-]*', a.version):
        p.error('version must be a simple folder name')
    if subprocess.check_output(['git', '-C', str(ROOT), 'status', '--porcelain', '--untracked-files=no']):
        p.error('commit the verified source before packaging')
    build = a.build.resolve()
    stage = ROOT / 'dist/linux' / a.version
    if stage.exists():
        p.error('version already exists; never reuse a delivery folder')
    cache = (build / 'CMakeCache.txt').read_text().splitlines()
    for setting in ('CMAKE_BUILD_TYPE:STRING=Release', 'PSX_DEBUG_TOOLS:BOOL=OFF',
                    'PSX_RUNTIME_IPO:BOOL=ON', 'PSXRECOMP_BIOS_STEMS:STRING=OpenBIOS'):
        if setting not in cache:
            p.error(f'unqualified build: requires {setting}')
    binary = build / 'r4-runtime'
    report = subprocess.check_output([str(a.llvm / 'llvm-readelf'), '-h', '-d', '--version-info', str(binary)], text=True)
    needed, glibc = audit_elf(report)
    symbols = subprocess.check_output([str(a.llvm / 'llvm-nm'), '--defined-only', str(binary)], text=True)
    variants = re.findall(r'\b[tT] ov_[A-Fa-f0-9_]+_func_[A-Fa-f0-9]+$', symbols, re.M)
    audit = check_static_overlays()
    if len(variants) != audit['published_variants']:
        raise ValueError(f'Linked overlay count mismatch: {len(variants)}')
    stage.mkdir(parents=True)
    for name in ('r4-runtime', 'game.toml', 'game_options.toml', 'assets', 'postfx'):
        src = build / name
        if src.is_dir():
            shutil.copytree(src, stage / name)
        else:
            shutil.copy2(src, stage / name)
    (stage / 'r4-runtime').chmod(0o755)
    shutil.copytree(build / 'mods/bundled', stage / 'mods/bundled')
    (stage / 'mods/state.toml').write_text('''format_version = 2
[[package]]
id = "psx.enhancement.internal-resolution"
[[feature]]
package_id = "psx.enhancement.internal-resolution"
id = "internal-resolution"
enabled = true
[feature.values]
resolution = "9x"
[[package]]
id = "r4.enhancement.frame-rate"
[[feature]]
package_id = "r4.enhancement.frame-rate"
id = "frame-rate"
enabled = true
[feature.values]
rate = "60"
[[package]]
id = "psx.feature.rewind"
[[feature]]
package_id = "psx.feature.rewind"
id = "rewind"
enabled = true
''')
    subprocess.run([sys.executable, str(ROOT / 'psxrecomp/tools/release_stage.py'),
                    'stage-execution', '--binary', str(stage / 'r4-runtime'),
                    '--manifest', str(build / 'r4-runtime.execution.json')], check=True)
    (stage / 'bios').mkdir()
    for name in ('openbios.bin', 'OpenBIOS.LICENSE'):
        shutil.copy2(build / 'bios' / name, stage / 'bios' / name)
    shutil.copy2(ROOT / 'LICENSE', stage / 'LICENSE')
    shutil.copytree(ROOT / 'THIRD-PARTY-LICENSES', stage / 'THIRD-PARTY-LICENSES')
    notices = {
        ROOT / 'psxrecomp/LICENSE': 'psxrecomp-LICENSE',
        build / '_deps/sdl3-src/LICENSE.txt': 'SDL3-LICENSE.txt',
        build / '_deps/psx_zlib-src/LICENSE': 'zlib-LICENSE',
        build / '_deps/psx_libwebp-src/COPYING': 'libwebp-COPYING',
        build / '_deps/psx_libchdr-src/LICENSE.txt': 'libchdr-LICENSE.txt',
        build / '_deps/libjuice-src/LICENSE': 'libjuice-LICENSE',
        ROOT / 'recomp-ui/LICENSE': 'recomp-ui-LICENSE',
        ROOT / 'recomp-ui/LICENSES/recomp-ui-MIT.txt': 'recomp-ui-upstream-MIT.txt',
        ROOT / 'recomp-ui/third_party/imgui/LICENSE.txt': 'imgui-LICENSE.txt',
        ROOT / 'psxrecomp/lib/recomp-net/LICENSE': 'recomp-net-LICENSE',
        ROOT / 'psxrecomp/lib/retcomm-rbengine/LICENSE': 'rbengine-LICENSE',
        build / 'licenses-sdk/GCC-COPYRIGHT': 'GCC-COPYRIGHT',
        build / 'licenses-sdk/GPL-3': 'GPL-3',
    }
    (stage / 'licenses').mkdir()
    for src, name in notices.items():
        shutil.copy2(src, stage / 'licenses' / name)
    (stage / 'Start R4.sh').write_text('#!/bin/sh\nset -eu\ncd -- "$(dirname -- "$0")"\nexec ./r4-runtime "$@"\n')
    (stage / 'Start R4.sh').chmod(0o755)
    (stage / 'README.txt').write_text('''R4 — Linux x86-64

Extract this folder into a writable location and run ./r4-runtime or
./Start\\ R4.sh. Choose your own USA BIN/CUE disc in the game menu.
Requires glibc 2.31 or newer and a working desktop OpenGL driver.
No game data or retail BIOS is included. OpenBIOS is bundled.

Select/Back or Esc opens the in-game menu, including controls and settings.
Hold Y/Triangle/F8 to rewind; release to resume. Right stick looks around;
R1 changes view. Keyboard: X accelerates, Z brakes, arrow keys steer.
Settings and saves remain beside the executable; preserve them when updating.

To use Steam Input, add r4-runtime as a non-Steam game and select the Gamepad
layout. No compiler, Python, SDK or overlay preparation is needed to play.
Fresh defaults enable 2160-line rendering, 60-FPS races and rewind. Adjust
resolution in the menu for your hardware; no Steam Deck performance claim.
''')
    receipt = {key: audit[key] for key in ('game_id', 'recipe_count', 'published_variants',
        'all_guards_match_known_input_bytes', 'full_static_coverage_proven',
        'profile_sha256', 'original_disc_sha256', 'image_coverage', 'files')}
    receipt['linked_variants'] = len(variants)
    (stage / 'NATIVE-OVERLAYS.json').write_text(json.dumps(receipt, indent=2)+'\n')
    revisions = {name: subprocess.check_output(['git', '-C', str(path), 'rev-parse', 'HEAD'], text=True).strip()
        for name, path in [('game', ROOT), ('engine', ROOT/'psxrecomp'), ('ui', ROOT/'recomp-ui')]}
    revisions.update(working_tree_dirty=False, sdk_image=a.sdk_digest, glibc_max=glibc, dependencies=needed)
    (stage / 'BUILD.json').write_text(json.dumps(revisions, indent=2)+'\n')
    (stage / 'ELF-IMPORTS.txt').write_text(report)
    (stage / 'DELIVERY.json').write_text(json.dumps(dict(platform='Linux x86-64', published=False,
        binary_sha256=digest(binary), linux_execution_qualified=False, release_qualified=False), indent=2)+'\n')
    (stage / 'SHA256SUMS').write_text(''.join(f'{digest(f)}  {f.relative_to(stage).as_posix()}\n'
        for f in sorted(stage.rglob('*')) if f.is_file()))
    verify(stage)
    print(stage)


if __name__ == '__main__':
    main()
