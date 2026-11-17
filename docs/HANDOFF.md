# Handoff

## Current: camera/car shaking fixed; native Mac playtest handed back

The look offset now restores before the next chase-camera update. Removed the
car-effects sampler's conflicting camera hysteresis, which also ran again on
intermediate redraws. Effects, menu, controller mappings, PGXP and rewind remain.
Owner visibly confirmed the moving car no longer shakes, left/right work well,
and the camera seems fixed. Normal DualSense input works after reconnecting.

Evidence and exact scope: [camera motion fix](recovery/CAMERA_MOTION_FIX_20261116.md).
Release build, 45/45 CTests and hidden menu/settings/boot-to-race smoke pass.
Capture-free interactive 4K run: 59.52 VBlanks/s, 3578/3578 intermediate frames,
zero dispatch misses/pass faults; rewind release 70.7 ms internally. A 268.4 ms
sampled presentation gap occurred, so perfectly flat pacing is not established.
Audio was muted and has not been listening-tested.

`~/Desktop/R4.app` points directly to
`~/Applications/R4-0.2.3-CameraFix/R4.app`, Developer-ID signed local playtest,
not notarized/final release. Previous installs remain intact. The app is open
under the owner's control, PID 66841 at handoff, debug port 4799. No test driver
or virtual pad remains. Do not stop the owner's playtest. Player 1 is now auto;
keyboard acceleration and reconnected DualSense were verified. Player data stays
in `~/Library/Application Support/net.ramine.r4recomp/`; volume is zero.

## Remaining release work

- Package and qualify the camera fix for Windows. Existing
  `dist/windows/R4-0.2.3-Windows-x64.zip` predates this fix; PC Steam desktop entry
  still points to 0.2.2. Do not label either as the corrected final release.
- Finish capture-free performance qualification and audio listening tests;
  complete final Mac packaging/notarization with Keychain profile `pp-notary`.
- PC test folder/task `R4ControllerCheck20261116` remains for cleanup after final
  verification. Owner explicitly authorized PC SSH testing in this conversation;
  read `~/pc-inventory/STEAMBOX.md` before accessing it. Keep tests muted unless
  asked otherwise. NAS is unrelated; do not use it.
- Pending upstream integration is tracked in [UPSTREAM](UPSTREAM.md).

## References

- [Prior handoff and 0.2.2 receipts](archive/HANDOFF_BEFORE_CAMERA_FIX_20261116.md)
- [Mac workflow](MAC_BUILD.md) / [Windows workflow](WINDOWS_BUILD.md)
- [Rewind](REWIND_PLAYBACK.md) / [framework pins](framework_pin_history.md)
- Current isolated grid: `analysis/rewind-release-grid-20261112/memcards`.
  Execution identity `f3a59eb52c450e6d9f22c0cd78332c02932ca5ef11cbc3c5ffeb9787b8bb9135`.
  Never bypass execution identity.
