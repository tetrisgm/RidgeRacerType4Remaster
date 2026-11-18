# Handoff

## Current work

The muted Mac ↔ STEAMBOX direct-IP **gameplay test passed**: real two-player
Helter Skelter race, independent acceleration/steering, shared pause,
retire/results, exit to title and integrated menus on both machines.
345 shared core-digest samples match; zero dispatch/segment misses.
The successful run used direct-start arguments with delay/lockstep, not the
integrated lobby's Start race action or rollback gameplay.

Full report, hashes, prior menu coverage/gaps, screenshots, commands and cleanup:
[direct-IP audit](recovery/DIRECT_IP_AUDIT_20261117.md).
Local evidence: `analysis/direct-ip-audit-20261117/` and the preceding
`analysis/menu-visual-audit-20261117/`.

Engine `7614acb5` clears stale exit requests at menu-provider teardown; launcher
`0dfae8c` retains popup cancellation and integrated Netplay fixes. The old lobby
start reached tick zero then exited through `menu_to_frontend`. The lifecycle
correction still needs an end-to-end lobby-start retest on both platforms.
Windows Smart App Control blocked its rebuilt EXE. The gameplay test used the
previous Windows engine `1e95d2af` and the new Mac engine; do not describe them
as identical engine builds or claim the Windows menu-start fix is verified.

## Verification and state

Mac Release and Windows cross-build pass, runtime/title tests 47/47, launcher
19/19, regeneration unchanged, real-race smoke passes. Gameplay explicitly
kept PGXP geometry/texture/CPU tracking on. Diagnostic only; no release/feed.

Both games are stopped. Temporary router rules, Windows firewall rule, task,
PC files and ineffective exact-hash policy are removed and verified. Existing
services/jobs/settings/saves preserved; Smart App Control remains enabled.
No NAS use or PC restart. Owner authorized this PC test and waived unavailable
Ghidra as a prerequisite for its menu/network work; no contract files changed.

Installed `~/Applications/Ridge Racer Type 4 Remixed.app` and desktop shortcut
remain on the older signed build. Current diagnostic Mac app is
`analysis/menu-visual-audit-20261117/Menu Audit.app` (ad-hoc signed).

## Remaining blockers

Trusted execution of the rebuilt Windows EXE, integrated lobby-start validation,
rollback and natural finish/rematch coverage; multiplayer own-view/HUD artifacts;
TA SECTION/HUD overlap; local 3/4 start-light boundaries; native Circle/Cross
prompts and LinkEntry/LinkBattle naming. No new physical-controller/audio/4K,
Linux or public-release qualification. Exact continuation is in the audit report.

Pre-audit state: [archived handoff](archive/HANDOFF_20261117_BEFORE_MODE_AUDIT.md).
