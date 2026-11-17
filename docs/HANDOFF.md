# Handoff

Owner update (2026-11-17): sound is enabled for playtests going forward on all
platforms; this supersedes earlier requests for muted testing.

## Current candidate

Menu/local multiplayer integration is committed and pushed as `8d4557ff`.
Engine pin: `561942ca`; launcher pin: `b1fd491`. No public release/feed was
published. No owner desktop app or player data was changed.

Implemented: unified native main/race actions, Start/Select pause, consistent
Back and snapshot navigation, embedded disc selection, first-run controls,
offline 2–4-player split screen, and optional Modern race HUD. Fixed stale
Resume clicks reopening/closing the menu, duplicate split-screen rank/RPM,
missing P4 timer/map, persistent HUD tags and cross-viewport taillight history.
See [menu integration](MENU_INTEGRATION.md), [local multiplayer](LOCAL_MULTIPLAYER.md)
and [issue review](ISSUE_REVIEW_20261117.md).

## Verification

- Release build, 45 runtime tests, 19 launcher tests and fresh menu/race smoke
  pass. `analysis/menu-redesign-20261117-final11/`: 7,157,625 static hits, zero
  dispatch misses. The pause-acknowledgement checker also passes in
  `analysis/menu-local-20261117-final-ack/` (7,173,882 hits, zero misses).
  `menu-input-queue-20261117-fixed/`: 30 rapid-click cycles pass;
  the old build failed its first cycle.
- `analysis/menu-local-20261117-performance/`: native Mac 2160 lines, rewind on,
  120 seconds, 59.97 VBlanks/s, all 3,599 requested intermediate frames rendered.
  Three observed rewind releases: 46–64 ms. Longest sampled frame: 63 ms.
  Finite-route evidence, not a hitch-free/all-course or split-screen 60-FPS claim.
- Modern/Classic virtual SDL acceleration, braking, menu return and reconnect
  pass in `analysis/input-recovery[-classic]-20261117/`.
- `analysis/issue110-courses-20261117/`: Wonderhill and Edge of the Earth load
  in Time Attack at 2160 lines with zero misses. This does not reproduce the
  reporter's exact mode/progression; #110/#111 remain unconfirmed.
- Local views, independent inputs/cameras, restart, natural finish and Results/
  title return receipts are in [local multiplayer](LOCAL_MULTIPLAYER.md).

## Packages and remaining checks

All source packages use `0.2.3-menu-local-20261117-rc1` under `dist/macos`,
`dist/windows` and `dist/linux`. Archives are under
`dist/releases/0.2.3-menu-local-20261117-rc1/`.

- Mac: Developer-ID signed, notarized, stapled and Gatekeeper accepted. Notary
  `783a763d-025f-4d54-a88c-cb431838583c`. Actual bundle menu/race check passes:
  `analysis/menu-local-20261117-mac-bundle/`, zero misses.
- Windows: PE/import/checksum audits pass. CrossOver-on-Mac full menu/race check
  passes at native internal resolution, zero misses:
  `analysis/menu-local-20261117-windows-compat/`. The earlier high-resolution
  translated run hit its 240-second limit; it is not a pass or native PC timing.
- Linux: pinned Steam Runtime SDK build; ELF, glibc 2.31 baseline, dependency
  and checksum audits pass. QEMU/software-GL direct-race check passes driving,
  hold/release rewind, pause/resume and snapshot save/load with zero misses:
  `analysis/menu-local-20261117-linux-race-c/result.json`. This uses a checkpoint
  at native internal resolution, not full boot or native GPU qualification.
  Full-boot attempts timed out. The direct-race checker needed actual guest
  pause acknowledgement and one second between UI inputs on the translated
  software renderer; earlier failures are retained alongside the passing receipt.
- Current additions still need native Windows/Linux GPU and audio listening
  acceptance. PAL runtime qualification lacks a matching disc. Do not claim
  the unresolved public reports fixed. Older manual snapshots may be incompatible
  with the expanded local-player state; preserve old apps/snapshots and career cards.

The owner's `~/Desktop/R4.app` still points to
`~/Applications/R4-0.2.3-CameraFix/R4.app`; leave it alone. Prior candidate details
are retained in commit `8d4557ff` and `analysis/release-023-final/`.

All three archives have verified SHA256 manifests under the release folder.
The task-started Colima VM is stopped again; Docker context is restored to `default`.
