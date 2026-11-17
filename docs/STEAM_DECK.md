# Linux and Steam Deck

The Linux x86-64 package requires glibc 2.31 or newer and desktop OpenGL.
SDL and all 7,459 guarded native overlay variants are linked into the executable.
No Python, compiler, runtime overlay toolchain or retail BIOS is required.
Supply your own USA BIN/CUE disc; no game data is included.

Extract into a writable folder and run `./r4-runtime` or `./Start\ R4.sh`.
Settings and saves live beside the executable; preserve them when updating.
Select/Back or Esc opens the integrated menu, including controls and settings.
Hold Y/Triangle/F8 for rewind and release to resume. Right stick looks around;
R1 changes view. Modern controls use R2 for gas and L2 for brake.

For Steam Input, add the executable as a non-Steam game and use the Gamepad
layout. Steam Deck hardware has not been performance-qualified. Fresh delivery
defaults select 2160-line rendering, 60-FPS races and rewind; lower resolution
through the menu to suit your hardware. PGXP stays enabled.

## Mac-hosted build

Use the Valve Steam Runtime 3 sniper SDK, pinned for this release to:

`registry.gitlab.steamos.cloud/steamrt/sniper/sdk@sha256:bb30053d3cd19d35c76ae69b2790dbaa1430448006f8240cb6d7e723f2d1940e`

The Apple silicon build uses the existing local Colima VM with Rosetta amd64
containers, GCC 14, CMake/Ninja and a host-mounted Python 3.12 installation.
Build Release with IPO, static runtime, OpenBIOS and enhanced execution; disable
PSX_DEBUG_TOOLS and Vulkan. Force bundled SDL3/zlib; statically link libstdc++
and libgcc. Build outputs remain in `build-linux` on the Mac.

`tools/stage_linux.py VERSION --llvm LLVM_BIN --sdk-digest IMAGE_DIGEST`
stages the result into a new `dist/linux/VERSION` directory. It rejects newer
glibc symbol requirements, unexpected dynamic dependencies, missing native
overlay symbols, dirty tracked source and reused delivery folders. Copy the
SDK GCC copyright and GPL-3 notices into `build-linux/licenses-sdk` first.

Run the hidden menu/settings/boot-to-race verification under Xvfb in the SDK,
using an isolated directory and dummy audio. This checks Linux compatibility;
software OpenGL under a translated VM is not a native GPU performance test.
The old `package_release.sh` runtime-compiler packaging path is retired.
