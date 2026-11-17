# Camera and moving-car motion correction — 2026-11-16

The camera look override was restored at object update (0x800738C4), after
Grand Prix chase-camera update (0x800340EC). The chase damper therefore consumed
the previous look offset. Restore now runs at the first main-loop ClearOTagR
(return 0x8001E764), before the handler, excluding private redraws and the second
OT clear. The override remains available throughout native drawing and capture.

A separate car-effects sampler modified eye coordinates by up to six world units
and angles by one unit. It ran after lookaround bookkeeping and again on temporal
redraws. Those writes could defeat exact restoration and changed the camera
relative to the interpolated player car. The sampler now observes camera state;
its old hysteresis and rewind regions are removed. Particles and lighting remain.
No change to menu, bindings, controller selection, PGXP, rewind or render quality.

The moving-race trace with the conflicting filter had repeated yaw excursions
while holding right (e.g. 4090 → 848 → 708). With it removed, right settles at
0 and rear at 1024 on the straight; the only large changes are requested view
transitions. Local evidence: `analysis/camera-shake-20261116/no-filter-proof.json`.
Addresses are verified against the owner's US executable in the adjacent local
`race-update-calls.txt`, `chase-update.txt` and `player-draw.txt`; game data stays
untracked. The proposed mirror-wrap theory was not the cause and was discarded.

The camera hook test covers private-redraw and second-OT-clear isolation and
restoration before the next native update. The race sampler test checks moving
eye/angle samples and zero guest writes; it fails against the previous production
sampler and passes after removal. Release build and all 45 CTests pass.

The signed native app was checked visibly with the owner, who confirmed the
moving car no longer shakes, left/right work well, and the camera appears fixed.
Tunnel, road and sky were inspected on screen. This is sampled visual inspection,
not an exhaustive every-frame artifact claim. The visible run used no runtime
frame dumps, pixel verification, RAM verification or rewind tracing.

Two-minute mixed route/control run: 59.52 VBlanks/s, all 3,578 requested
intermediates rendered, zero dispatch misses and render-pass faults. Full
3834×2160 internal rendering, PGXP and rewind enabled, 1280×720 window output.
Rewind delivered 58.30 backward updates/s; release measured 70.7 ms internally
and 86.9 ms through the polling harness. The maximum sampled presentation gap
was 268.4 ms during this interactive run; therefore this is not evidence of
perfectly flat pacing or a no-hitch guarantee. It included device selection,
external-view switching and the owner's requested manual camera tests; the
scripted driver was briefly paused for those tests. Receipt:
`analysis/camera-fix-visible-20261116/result.json`.

Menu/settings restart and hidden boot-to-race smoke passed with 7,172,585 static
overlay hits and zero dispatch misses:
`analysis/camera-fix-smoke-20261116/native-overlay-check.json`.

Desktop `~/Desktop/R4.app` now points to the Developer-ID-signed local build at
`~/Applications/R4-0.2.3-CameraFix/R4.app`; the previous install remains intact.
The owner's Player 1 source was keyboard; it is now auto (backed up locally in
`analysis/camera-shake-20261116/native-settings-before-handoff.toml`). All test
input overrides are gone. Normal keyboard acceleration was verified, and the
owner confirmed normal DualSense input after reconnecting Bluetooth. The native
app remains open for the owner; do not close it. Audio volume remains zero as
requested; audio was not listening-tested. This is not a final notarized release.
Windows packaging/qualification of this fix remains pending.
