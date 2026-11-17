# Handoff

## Current: unified menu, local split screen and issue review

Owner approved the menu redesign followed by offline 2–4-player split screen,
then added the open GitHub issues. Keep single-player rewind, performance and
controller fixes. Online remains separate, one full-screen view per peer.

Implemented: native main/race actions through the custom menu, Start/Select
pause, consistent child Back, safe snapshots, embedded disc browser and first-run
controls/multiplayer introduction. Local 2P uses native horizontal split; 3/4P
uses independent quadrants, controllers, cameras and projected trail histories.
Multiplayer rewind remains disabled. See [menu integration](MENU_INTEGRATION.md)
and [local multiplayer](LOCAL_MULTIPLAYER.md) for implementation and receipts.

The contributed modern HUD was already merged. It is now exposed separately
from HD HUD textures; fixed duplicate rank/RPM, P4 timer/map boundaries and
copied-label anchoring. 2160-line visuals: local `probe45` (2P), `probe41` (3P),
`probe46` (4P); `analysis/input-recovery-20261117/race.png` (1P). Four-player
natural finish, Results and title return pass in `probe35` with zero misses.
VBlank pacing is not proof of 60 rendered split-screen FPS.

The final Release build, 45 runtime tests, 19 launcher tests and fresh full
boot/menu/settings/race smoke pass: `analysis/menu-redesign-20261117-final11/`
(7,157,625 static hits, zero misses). The rapid Resume-click regression failed
on the old build's first cycle; all 30 corrected cycles pass in
`analysis/menu-input-queue-20261117-fixed/`. Launcher `b1fd491` clears stale menu
input and prevents a second close pass; the integrated smoke includes the case.
A fresh 120-second 2160-line single-player performance/rewind check is running
at `analysis/menu-local-20261117-performance/` before packaging.

[Issue review](ISSUE_REVIEW_20261117.md): #110 has an invalid main-dispatch target
but no identified build/reproduction; #111 virtual SDL reconnect/menu-release
regression passes, not a reporter-hardware diagnosis. Raw attachments stay local
under `analysis/issues-20261117/`. Do not declare these reports fixed or close them.

Engine local commits: `f44bd952`, `24bda286`, `561942ca`; UI: `64a9004`,
`713154b`, `60a7293`, `b1fd491`. Root integration is ready for commit. No current menu/local changes are in release packages or the owner's
desktop app. Final coherent commits/pushes and cross-platform packaging remain.

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
