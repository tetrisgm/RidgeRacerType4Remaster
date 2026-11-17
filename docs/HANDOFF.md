# Handoff

## Current: preparing 0.2.3 for Mac, Windows and Linux

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
