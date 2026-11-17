# Ridge Racer Type 4 Remaster

R4: Ridge Racer Type 4 (USA, **SLUS-00797**), statically recompiled to a
native runtime using [psxrecomp](https://github.com/RetroPortingToolKit/psxrecomp)
and [our custom menu](https://github.com/tetrisgm/recomp-launcher).
Release qualification is in progress.

## Current status

Version 0.2.3 is being prepared for Windows x64, Linux x64 and macOS Apple
silicon. See [release notes](RELEASE_NOTES.md) and
[current qualification status](docs/HANDOFF.md) before publishing a build.

The custom R4 menu opens at startup and with **Select/Back** or Esc during play.
It includes display settings, Modern/Classic controls, keyboard/gamepad bindings,
input devices, deadzones, mods and volume. Live settings apply immediately;
restart-only changes are saved and labelled. PGXP stays enabled.

Hold **Y/Triangle/F8** to rewind at gameplay resolution; release to resume.
The right stick looks around and R1 changes view. The camera feedback and
moving-car shaking are fixed. Native Mac DualSense and PC Steam Controller
playtests passed; the owner accepted the camera and performance.

Fresh packages enable 2160-line rendering, 60-FPS races and rewind. Resolution
can be lowered through the menu for slower hardware. Mac and PC finite race
checks averaged about 60 VBlanks/s; this is not an every-course performance
promise. Linux GPU and Steam Deck performance are not yet qualified. Audio
listening acceptance remains pending after muted automated tests.

All native overlay variants are linked into the executable. No runtime compiler,
Python installation or overlay preparation is needed to play. Supply your own
USA disc image. Preserve your saves and settings when updating.

## Build on the Mac

Install Xcode command-line tools and Homebrew `cmake`, `ninja`, and Python
3.11 or newer. Supply your own USA BIN/CUE disc; verify it against [DISC.md](DISC.md).
No game data or retail BIOS is required to compile the committed native C.
The bundled OpenBIOS is open source.

```sh
git clone --branch master https://github.com/tetrisgm/RidgeRacerType4Remaster.git ridgeracertype4
cd ridgeracertype4
git submodule update --init --recursive
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DR4_BUILD_TESTS=ON -DR4_PGXP=ON -DPSX_DEBUG_TOOLS=ON \
  -DPSX_EXECUTION_PROFILE=ENHANCED -DPSX_RUNTIME_IPO=OFF
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

Tests that inspect disc sites require the USA disc in `disc/`; CMake warns
when it is absent. Select your disc in `build/r4-runtime`, or place the
BIN/CUE at `disc/R4 - Ridge Racer Type 4 (USA).{bin,cue}` and use the helper.
The nine distinct code-overlay images are compiled into the executable from
committed generated C; no runtime compiler or cache preparation is needed.
See [native overlay evidence](aot/README.md).

Automated verification uses an isolated runtime/profile and never copies
player settings or saves back:

```sh
python3 tools/verify_native_overlays.py analysis/native-check-unique-name
```

An ordinary clone/build needs no regeneration. Only after seed, annotation,
recompiler configuration or engine pin changes, run `tools/regen.sh` and
commit its generated output with the change. Never edit `generated/` by hand.

## Release packages

Build and packaging instructions: [Mac](docs/MAC_BUILD.md),
[Windows](docs/WINDOWS_BUILD.md), [Linux](docs/STEAM_DECK.md).
Development and cross-builds run on the Mac. Packages are checksummed and
contain no disc image, retail BIOS or player data. Mac apps use Developer ID
signing and notarization; Windows portable executables are unsigned.

On Windows/Linux, extract the complete folder into a writable location and
run `r4-runtime`. To use Steam Input, add the executable as a non-Steam game
with the Gamepad layout. On Mac, run the native R4 app directly; a normal
Bluetooth or USB controller does not require Steam.

## References

- [Workflow](CLAUDE.md), [pin history](docs/framework_pin_history.md),
  [upstream PRs](docs/UPSTREAM.md).
- [Consolidation decisions and recovery](docs/CONSOLIDATION.md).
- [Historical fork README](docs/archive/README-before-consolidation-20261108.md)
  records the old feature set; it is not a current feature claim.
- [Widescreen](docs/WIDESCREEN.md), [disc identity](DISC.md),
  [performance investigation](docs/recovery/PERFORMANCE_HANDOFF_20261105.md).

## License

MIT for this repository's own code — see `LICENSE`. Files adapted from
MegaManX6Recomp (listed in `THIRD-PARTY-LICENSES/README.md`) stay under
PolyForm Noncommercial 1.0.0, and the
`psxrecomp` and `recomp-ui` (recomp-launcher) submodules carry their own licenses; release zips
carry their notices in `licenses/` and `assets/`. R4: Ridge Racer Type 4 is
© Namco (Bandai Namco Entertainment). Neither this repository nor its release
zips contain the disc image or the game's data (models, textures, audio,
movies, the `R4.BIN` overlays). Two things in them do come from the game:

- `generated/` is the boot EXE's code translated to C by psxrecomp, and the
  compiled game is built from it. psxrecomp keeps each original MIPS
  instruction word beside its translation (in a comment and as an argument
  to the PGXP hooks), so the EXE's code section can be read back out of
  `generated/`; its data section is not there.
- The launcher's box art (`recomp/launcher/boxart.*`, shipped as
  `assets/img/boxart.tga`) is the game's cover; see
  `THIRD-PARTY-LICENSES/README.md`.

The bundled HD HUD pack is Kuid0us's artwork of the HUD (see
`docs/HD_HUD.md`).

The compiled game needs your own disc to run.

### Rewind

The current recovery uses upstream's filmstrip selector with HD residency
preserved across restore. It does not use the old fork's hold-to-rewind pipeline.
Rewind is unavailable in netplay. Historical phase-5 reports describe the old
architecture and are not current instructions.
