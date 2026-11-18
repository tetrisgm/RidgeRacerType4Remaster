# Handoff

## Current work

The muted Mac ↔ STEAMBOX direct-IP test passed through the **integrated lobby**:
join, Start race, two-car Helter Skelter race, independent acceleration/steering,
shared pause, retire/results, native title and integrated menus on both machines.
Engine `7614acb5` and launcher `0dfae8c` ran on both platforms. The earlier
immediate menu exit no longer reproduced. Draft upstream review: PR #967.

Full report, binaries, evidence, prior coverage and cleanup:
[direct-IP audit](recovery/DIRECT_IP_AUDIT_20261117.md), especially its follow-up.
Local follow-up evidence: `analysis/direct-ip-audit-20261117/retest/`.

Rollback was enabled. Status samples reported no desync and zero dispatch/segment
misses, but logs show one recovered resimulation divergence at sim 6080:
259/260 shared live-core samples matched; replay recovered and later hashes match.
Do not describe this as a clean rollback qualification. The earlier direct-start
lockstep run had 345 matching samples with no mismatch.

## Verification and state

Mac Release and Windows cross-build pass, runtime/title tests 47/47, launcher
19/19, regeneration unchanged, real-race smoke passes. Gameplay explicitly
kept PGXP geometry/texture/CPU tracking on. Diagnostic only; no release/feed.

Both games are stopped. Temporary router/firewall rules, tasks, PC files,
ineffective exact-hash policy and the successful temporary ASR-only exact-file
exception are removed and verified. Existing services/jobs/settings/saves are
preserved. Smart App Control is Off, disabled by the owner during the retest.
No NAS use or PC restart. Owner authorized this PC test and waived unavailable
Ghidra as a prerequisite for its menu/network work; no contract files changed.

Installed `~/Applications/Ridge Racer Type 4 Remixed.app` and desktop shortcut
remain on the older signed build. Current diagnostic Mac app is
`analysis/menu-visual-audit-20261117/Menu Audit.app` (ad-hoc signed).

## Remaining blockers

Recovered rollback divergence, natural finish/rematch and adverse-network coverage; multiplayer own-view/HUD artifacts;
TA SECTION/HUD overlap; local 3/4 start-light boundaries; native Circle/Cross
prompts and LinkEntry/LinkBattle naming. No new physical-controller/audio/4K,
Linux or public-release qualification. Exact continuation is in the audit report.

Pre-audit state: [archived handoff](archive/HANDOFF_20261117_BEFORE_MODE_AUDIT.md).
