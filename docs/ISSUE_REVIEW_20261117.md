# Open issue review — 2026-11-17

Owner requested these reports be handled alongside the unified menu and local
2–4-player work. No issues have been closed or reported fixed based on age alone.

| Issue | Evidence and disposition |
| --- | --- |
| [51](https://github.com/tetrisgm/RidgeRacerType4Remaster/issues/51) | PAL PR49 merged as `3a34e847`; modern HUD/effects PR50 merged as `d535b605`. Code and packages exist. Curated Graphics currently exposes the separate T4HDHUD texture pack, not the modern Race HUD. Qualify and expose the modern HUD; check local 3/4-view compatibility (current compact layout is two-player only). PAL runtime qualification still requires a matching disc. |
| [110](https://github.com/tetrisgm/RidgeRacerType4Remaster/issues/110) | Active crash report. Two fatal dumps show main dispatcher return address `0x8001E7B8` and target `0x00D80000`. This is not evidence of a missing valid code seed: trace the state/table corruption. Other attachments marked spin-freeze continue advancing VBlanks, so distinguish watchdog diagnostics from the fatal crash. Reproduce repeated race transitions on current source. |
| [111](https://github.com/tetrisgm/RidgeRacerType4Remaster/issues/111) | Active input report: UI accepts controller while gameplay does not. No hardware/driver details or logs supplied. Verify device assignment, Modern/Classic mappings, reconnect and overlay input release. Do not equate lost input with proven driver damage. |
| [106](https://github.com/tetrisgm/RidgeRacerType4Remaster/issues/106) | Describes the superseded packet renderer. Match against current renderer and accepted camera/rewind visual evidence; retain tunnel/sky/ground checks for current changes. Do not restore old packet/surface patches wholesale. |
| [107](https://github.com/tetrisgm/RidgeRacerType4Remaster/issues/107) | Old branch-based live-settings handoff. Audit against current menu and adapter; preserve required PGXP per current owner rules. Classify live versus restart-required settings accurately. |
| [108](https://github.com/tetrisgm/RidgeRacerType4Remaster/issues/108) | `game.toml` already enables idle_skip and store-counter skipping with explicit sites. Check history/tests before declaring the research request complete; do not duplicate obsolete configuration. |
| [109](https://github.com/tetrisgm/RidgeRacerType4Remaster/issues/109) | Research preservation request, not a demonstrated current bug. Existing performance recovery references remain. No NAS access or wholesale revival of experiments is needed. |

Public report attachments and fetched issue snapshots are kept only under the
ignored `analysis/issues-20261117/` directory. No reporter data belongs in source.
