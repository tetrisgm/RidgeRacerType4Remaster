# Local multiplayer

Choose **Local multiplayer** in the custom main menu, then two, three or four
players. Two players use the original horizontal VS Battle layout. Three and
four use a four-quadrant layout; the unused fourth quadrant stays empty in
three-player races. Assign controllers in Controls; keyboard bindings remain
available alongside controllers.

Each seat controls its own car. Modern controls use RT/R2 to accelerate,
LT/L2 to brake and RB/R1 to change that seat's view. Start or Select/Back opens
the integrated pause menu. Restart and Retire use the original race transitions.
Rewind remains unavailable in multiplayer, including the original two-player mode.

Local play uses no room, networking or serial transport. It shares the native
car-command bridge with online play, but samples locally assigned controllers.
Extra cars come from the race roster; extra views have separate camera buffers,
ordering tables and projected taillight histories. These histories are guest
memory so snapshots restore them consistently.

## Qualification

This is a development candidate, not a published release. Evidence is under
`analysis/local-four-player-20261117-*`:

- `probe14/input-check.json`: independent four-seat acceleration/braking.
- `probe17/camera-check-r1.json`: each seat's camera changes once per press.
- `probe26/race.png` and `probe28/race.png`: native two-player and three-player entry.
- `probe35/drive.json`, `finish.png`: natural four-player completion with zero
  dispatch misses and clean finish views after the stale-HUD fix.
- `probe35/tunnel-moving.png`: moving cars in tunnel; projected trails no longer
  connect different viewports.
- `probe33/restart.png`: clean native restart after persistent HUD-tag restoration.

- `probe35/results-confirm.png` and `exit.png`: Results and return to title.
- `probe45/race.png`, `probe41/race.png`, `probe46/hud.png`: optional modern HUD
  in two, three and four views, with native timers/maps and no duplicate rank/RPM.

Final release packaging remains in [the handoff](HANDOFF.md).
Native VBlank pacing receipts do not establish 60 rendered split-screen FPS.
