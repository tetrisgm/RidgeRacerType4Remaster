# Handoff

## Current: unified menu first, then local 2–4-player split-screen

Owner approved menu work followed by local multiplayer. Online remains separate:
one full-screen view per network player. Local play must work offline with 2–4
controllers and shared split-screen, extending the existing two-player experience.
Reuse player-state logic, not original link transport. Current online code replaces
SIO1 receive bytes with synchronized seat commands; original RAM init is retained.

Menu scope: replace native race pause with Start/Select menu and original
Restart/Retire actions; main-menu integration; consistent Back; snapshots separate
from career saves; integrated file browsing; short controls/rewind + multiplayer
intro (one visual-improvements bullet). No between-race/replay settings or
controller-disconnect screen. Keyboard and controller coexist.

Menu commits are local and unpushed: engine `f44bd952`, `24bda286`, `561942ca`;
launcher `64a9004`, `713154b`, `60a7293`. Root integration and the local player picker are
uncommitted. Snapshot navigation, native/custom Grand Prix Retire, Time Attack
Restart and the first-launch layout pass. Full boot/menu/settings smoke passes
at `analysis/menu-redesign-20261117-check5/` (zero misses); 19 UI and 45 runtime
tests pass. See [menu integration](MENU_INTEGRATION.md).

Local 3/4-player qualification is in progress. Four independently assigned virtual
pads now drive their own cars offline, and R1 toggles only that seat's camera once
per press. Fixed missing interior viewport hooks, expanded-heap traversal, two-seat
HUD record overruns, and visibility lists incorrectly based on another car's course
section. `analysis/local-four-player-20261117-probe17/` has fresh boot, independent
camera tests, moving 2160-line views and 59.94 VBlanks/s with zero dispatch misses.
The local grid uses native 4:3 framing; PGXP and Max Detail remain enabled. Current
HUD cleanup restores each viewport's timer, units and extra racers' actual RPM.

Three-player native completion passed in `analysis/local-four-player-20261117-probe19/`.
Two-player entry and the three-player empty quadrant passed in `probe26/` and
`probe28/` under the same local-four-player analysis prefix. Restart corruption
was traced to persistent fixed HUD tags retaining pointers into an expired heap;
tags are now restored before the next main frame. Fresh boot/restart and moving
track images pass in `probe25/`, with 3,834 VBlanks / 63.966 seconds and zero misses.
This measures native pacing, not 60 rendered split-screen frames per second.

Per-view taillight history now passes its unit test and initial tunnel-image
checks (`probe33/tunnel-0.png`, `tunnel-35.png`); moving-car trail verification
remains before claiming full visual qualification. Release build33 and 45 runtime
tests pass; 19 UI tests pass. Final full menu/settings/race smoke passes at
`analysis/menu-redesign-20261117-final3/` with zero dispatch misses.
Four-player native race completion also passed (`probe31/drive.json`, phase 4,
laps 4/3/3/3). Fresh build33 restart passed. These are not release packages.
No current work is in the owner's desktop app. Online pause remains synchronized
native behavior; multiplayer rewind retains the existing disabled policy.

Owner added the open GitHub issues to this work. See [issue review](ISSUE_REVIEW_20261117.md).
The HD HUD/PAL contribution is already merged, but the separate modern race HUD
is missing from the curated Graphics menu. Crash #110 and input #111 are active
investigations, not declared obsolete. Raw public crash attachments are local in
`analysis/issues-20261117/`. Finish these alongside menu/local multiplayer;
then final verification, coherent commits/pushes and packaging remain.

## Previous release candidates

Owner accepted the camera/car-motion fixes and performance, and confirmed normal
DualSense input. Runtime fix: `040a3455`; packaging: `55e3f5a2`.
[Camera analysis and verification](recovery/CAMERA_MOTION_FIX_20261116.md).
Release build, 45/45 CTests and hidden menu/settings/race smoke pass.

Mac native owner-control timing: 59.94 VBlanks/s. Native Windows corrected build:
59.96 VBlanks/s at 2160 lines; moving camera, three rewind releases (67–78 ms
including input observation) and Select/Back pass; zero dispatch misses.
PC tunnel/sky/road images inspected without observed seams or misplaced HUD.
Temporary PC tasks and test folders were removed; no security exception needed.

## Artifacts and remaining checks

- Final archives: `dist/releases/0.2.3/`. All three archives are prepared. Linux menu, direct race, Y hold/release and
  Back passed under Rosetta/software GL. Its full boot smoke hit the VM timeout;
  native desktop/Linux GPU qualification is still outstanding.
- Mac source package: `dist/macos/0.2.3-final`. Developer-ID signed, notarized,
  stapled and accepted by Gatekeeper. Notary ID
  `84b9eea4-71fe-4ef4-a92c-ba374bb5cce1`.
- Windows source package: `dist/windows/0.2.3-release`. Exact tested executable;
  unsigned. Old top-level Windows ZIP and PC Steam installation predate this fix.
- Audio listening acceptance remains outstanding; all automated runs are muted.
  Owner was asked to raise the in-game volume and confirm music/engine sounds.
- Linux testing in a translated local VM establishes compatibility, not native
  GPU/Steam Deck performance. Do not claim that performance was verified.
- No public release or feed was published. NAS is unrelated and untouched.

## Owner playtest — leave it alone

`~/Desktop/R4.app` points to `~/Applications/R4-0.2.3-CameraFix/R4.app`.
It is the corrected signed playtest, not the final notarized archive above.
Owner controls the app (PID 66841 when handed back; debug port 4799). No test
driver or virtual pad remains. Player 1 is auto; volume is zero. Player data:
`~/Library/Application Support/net.ramine.r4recomp/`. Do not stop the owner's app.

## Evidence and references

- `analysis/release-023-final/`: notarization, PC receipts and route images.
- `analysis/linux-final-023-race-c/result.json`: passing Linux compatibility
  check at native internal resolution; `linux-final-023-smoke-b` records menu
  success followed by the full-boot timeout. No Linux 4K performance claim.
- `analysis/camera-fix-visible-20261116/release-qualification.json`.
- `analysis/camera-shake-20261116/owner-control-timing.json`.
- `analysis/mac-final-023-bundle/native-overlay-check.json`: 7,179,901 static
  overlay hits, zero misses in the signed bundle.
- [Mac workflow](MAC_BUILD.md), [Windows workflow](WINDOWS_BUILD.md),
  [Linux workflow](STEAM_DECK.md), [upstream tracking](UPSTREAM.md).
- [Prior handoff](archive/HANDOFF_BEFORE_CAMERA_FIX_20261116.md).
