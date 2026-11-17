# Menu integration

The custom frontend uses the game's original actions and state transitions.
Menu code never invokes guest code from the render thread.

- `launcher/r4_menu_bridge.h` publishes available actions atomically at the
  main-loop ClearOTagR boundary (0x80093418, return 0x8001E764). Private redraws
  and online sessions are excluded.
- Original main menu 0x8002F110/substate 2 supplies enabled entries from its
  native records. The queued choice sets cursor 0x800AC802 and confirm edge
  0x800F3C10 at 0x8002F5E8, after normal input polling. The original handler
  performs the transition. Settings use the custom frontend.
- Race actions call original 0x80034E08 and audio-resume 0x8004FD58. The native
  maximum pause item at 0x8009F8B8 determines availability: Grand Prix offers
  Retire only. Confirmations keep Cancel first. Start/keyboard Start intercept
  only a supported offline race; Select/Back remains available.
- Save waits for the safe snapshot boundary then stays paused. Load closes the
  menu only after success. Cancelling a child page returns to its parent.
- Disc browsing is embedded, supports UTF-8 paths, and requires Continue.
  First boot presents controls/rewind, multiplayer and one visual-improvements
  bullet. Its acknowledgement lives with player preferences, not in the app.

## Verification

- `analysis/menu-redesign-20261117-check1/`: actual-game snapshot navigation,
  settings persistence and zero dispatch misses.
- `analysis/menu-redesign-20261117-check2/`: subsequent boot-to-race/menu pass,
  7,187,749 static overlay hits and zero dispatch misses.
- `analysis/menu-native-pause-20261117/pause.png`: original Grand Prix pause
  visually confirms Cancel/Retire, with no Restart.
- Native/custom Retire receipts in `analysis/menu-native-retire-20261117/` and
  `analysis/menu-custom-retire-20261117/` match major 1, minor 0x30, race phase 6,
  transition word -1 and championship flag 9. Custom Start leaves native pause
  byte clear; the action completes with zero dispatch misses.
- 19 launcher tests and 45 runtime tests pass. Final post-pin integrated smoke:
  `analysis/menu-redesign-20261117-final11/` passes (7,157,625 static hits, zero misses).
  It includes rapid Resume clicks followed by reopening: queued input is cleared
  on close, so a stale click cannot immediately dismiss the next menu.
- Time Attack Restart: `analysis/menu-time-attack-restart-20261117/race.json`.
- First-launch layout: `analysis/menu-intro-layout-20261117/intro.png`.

## Remaining work

Local 2–4-player entry, per-seat controls/cameras, four-player completion and
return flows now have passing evidence in [local multiplayer](LOCAL_MULTIPLAYER.md).
Final cross-platform packaging remains in [the handoff](HANDOFF.md). Online Start/actions keep synchronized native behavior; the
custom bridge must not mutate only one peer.
