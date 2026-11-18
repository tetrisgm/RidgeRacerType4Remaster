# Handoff

## Current Mac work

Owner requests one runtime instance at a time, with sound enabled. Product name
is **Ridge Racer Type 4 Remixed**. Updated local app:
`~/Applications/Ridge Racer Type 4 Remixed.app`; desktop shortcut has the same
name. Bundle ID and player data remain `net.ramine.r4recomp.menu20261117`.
Owner saves and 6x/borderless preferences are preserved. This updated app is
Developer-ID signed, not a newly notarized release. Prior app is retained in
`analysis/remixed-app-before-final-20261117/`.

Home menu stays over background demonstrations; cached clean racing images
cover native loading and fade transitions. The idle wait is removed. A queued
mode choice clears the accelerated idle timer and completes only after the
selected mode leaves the shared loader. Modern A/Cross confirms native menus,
B/Circle cancels; Classic and driving mappings are unchanged. Native menu
artwork still labels the original Circle/Cross controls.

Engine `753e32df` maintains textured stencil masks without repeated full-surface
rebuilds, preserving overlap ordering. Launcher `7b77092` retains the background
and clears home state after loading snapshots; includes Remixed branding.
See [menu integration](MENU_INTEGRATION.md), [pin history](framework_pin_history.md)
and [modern controls](MODERN_CONTROLS.md).

## Verification and limits

Release build, 46 runtime tests and 19 launcher tests pass. New real-GL mask
fixture passes 180 color/depth/stencil checks with PGXP enabled at 1x/5x/9x.
Modern frontend confirmation regression tests pass. Isolated Time Attack
course → car selection and B-back were checked with an SDL virtual controller:
`analysis/home-timeattack-20261117/`.

Final corrected renderer replay run at 9x with sound:
`analysis/replay-stutter-20261117/final-mask-order/`. Normal game speed
(60.09 VBlanks/s), snapshot load 38 ms, zero dispatch misses. 752 of 904 requested
intermediate frames rendered: this is not a perfect 60-unique-frame claim.
User's source slot 1 and sidecars are preserved under that analysis directory's
`source/saves/`; original player saves are untouched.

Final native boot/race and save/load/menu smoke passes with sound: 7,172,603
static overlay hits and zero dispatch misses in
`analysis/remixed-home-final-20261117-b/`. The signed updated app was visually
checked with its preserved settings: home overlay over a replay, covered mode
transition and A/Cross advancing through Time Attack course, car and transmission
selection into a moving race. Virtual pad detached; only the visible app remains,
paused for the owner. Receipts:
`analysis/remixed-home-app-final-20261117/`.
Do not call new Windows/Linux packages or public release qualification complete.
Prior release candidate and wider platform limitations are archived in
[previous handoff](archive/HANDOFF_20261117_MENU.md). No release/feed published.

Follow-up observed during Time Attack: original SECTION/split readouts overlap
the optional Modern HUD lap panel. Existing native prompts still show original
Circle/Cross artwork. These are not resolved by the menu navigation fix.
