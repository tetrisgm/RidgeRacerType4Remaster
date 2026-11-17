# Open issue review — 2026-11-17

Owner requested these reports be handled alongside the unified menu and local
2–4-player work. No issues have been closed or reported fixed based on age alone.

| Issue | Evidence and disposition |
| --- | --- |
| [51](https://github.com/tetrisgm/RidgeRacerType4Remaster/issues/51) | PAL PR49 merged as `3a34e847`; modern HUD/effects PR50 merged as `d535b605`. Code and packages exist. Graphics now exposes Modern race HUD separately from HD HUD textures. The compact layout supports 2–4 local players. Fixed duplicate stock rank/RPM, missing P4 timer/map, expanded-heap traversal and copied-label widescreen anchors. Visual receipts: probe45 (2P), probe41 (3P), probe46 (4P), and input-recovery (1P), under the 20261117 analysis directories. PAL runtime qualification still requires a matching disc. |
| [110](https://github.com/tetrisgm/RidgeRacerType4Remaster/issues/110) | Active crash report. Two fatal dumps show main dispatcher return address `0x8001E7B8` and target `0x00D80000`. This is not evidence of a missing valid code seed: trace the state/table corruption. Other attachments marked spin-freeze continue advancing VBlanks, so distinguish watchdog diagnostics from the fatal crash. The report names the second/third stages but no build or mode. Current Wonderhill and Edge of the Earth Time Attack loads pass at 2160 lines with zero misses (`analysis/issue110-courses-20261117/`); this does not reproduce the exact reported progression. |
| [111](https://github.com/tetrisgm/RidgeRacerType4Remaster/issues/111) | Active input report: UI accepts controller while gameplay does not. No hardware/driver details or logs supplied. Current Mac SDL virtual-pad regressions pass acceleration/braking, menu close/input release and disconnect/reconnect in both Modern and Classic modes: `analysis/input-recovery-20261117/result.json` and `analysis/input-recovery-classic-20261117/result.json`. This is synthetic-device evidence, not a reproduction on the reporter’s hardware. Classic mapping reload is checked by the integrated menu smoke. Do not equate lost input with proven driver damage. |
| [106](https://github.com/tetrisgm/RidgeRacerType4Remaster/issues/106) | Describes the superseded packet renderer. Match against current renderer and accepted camera/rewind visual evidence; retain tunnel/sky/ground checks for current changes. Do not restore old packet/surface patches wholesale. |
| [107](https://github.com/tetrisgm/RidgeRacerType4Remaster/issues/107) | Old branch-based live-settings handoff. Audit against current menu and adapter; preserve required PGXP per current owner rules. Classify live versus restart-required settings accurately. |
| [108](https://github.com/tetrisgm/RidgeRacerType4Remaster/issues/108) | `game.toml` already enables idle_skip and store-counter skipping with explicit sites. The pin history records the implemented upstream work (`d686cacd`, PR590) and a matched 3,600-frame state comparison on 2026-10-15. The requested optimization is present; do not duplicate obsolete configuration. |
| [109](https://github.com/tetrisgm/RidgeRacerType4Remaster/issues/109) | Research preservation request, not a demonstrated current bug. Existing performance recovery references remain. No NAS access or wholesale revival of experiments is needed. |

Public report attachments and fetched issue snapshots are kept only under the
ignored `analysis/issues-20261117/` directory. No reporter data belongs in source.

The only current public release is v0.2.0 (2026-10-24). The reports do not name
their build, so they cannot yet be attributed to the current development code.
Do not call them fixed merely because later local candidates exist. Current
controller routing/recovery changes include engine `fb34f318` and `3f3cfb75`.
