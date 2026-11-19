# Handoff — release qualification

**Status (2026-11-18, evening):** the v19 Windows GPU out-of-memory blocker did not reproduce, and no renderer defect was found. Source is unchanged at root `83ad239f`, engine `ac591d54`, launcher `dff6eb9`. NetPlay qualification is clean. Publishing still needs the owner's go.

## Windows GPU out-of-memory: diagnosis

- **Mac reproduction:** single-player at 4× and two-peer loopback NetPlay. GPU memory stayed flat. Pass-owned GL allocations stayed constant (7) across about 3,700 own-view frames, so backups are not reallocated per frame.
- **STEAMBOX reproduction:** the saved 4× profile, with PC GPU memory sampled every ~6 s. All runs stayed flat:
  - single-player race: 1.09 GiB, 6 min;
  - delay-mode NetPlay: 1.25 GiB, 7 min;
  - delay mode with the launcher layer plus the crash run's PGXP and diagnostic environment: 1.25 GiB, 8 min;
  - rollback mode, as in the crash run: 1.24 GiB, 8 min;
  - final rollback run: peak 1.29 GiB, 11 min.
- **Probable cause: system commit exhaustion on the PC, not GPU memory.**
  - The WSL VM (`vmmemWSL`) commits about 40 GiB of the 59 GiB commit limit (48 GiB RAM + 10.5 GiB pagefile). Only 8.8 GiB was free when measured.
  - The NVIDIA OpenGL driver reports a failed system-memory commit as "out of memory".
  - Its 12:54 events fit that pressure. So does a Volsnap "shadow copy storage could not grow" error at 12:38.
  - This is inferred: commit was not sampled at 12:54. Windows Error Reporting has no r4-runtime crash record.
- **Decision (delegated by the owner, 2026-11-18): leave WSL as is.** The cause is unproven, and a cap could break whatever WSL runs on the console. If the out-of-memory error returns, sample system commit at the time of the crash before changing anything.

## Final NetPlay check (muted, Mac host ↔ STEAMBOX guest, direct IP, rollback, 4× on the PC)

- 11 minutes of racing.
- 1,181 matched shared-state digests and 0 mismatches.
- 0 dispatch and segment misses on both peers.
- 17,641 and 17,642 own views, with 0 aborted, 0 VRAM leaks and 0 verify mismatches.
- 20 status pairs stayed within 0–7 simulation ticks, with no desync.
- 16-frame present bursts were captured on both sides. The HUD and geometry are coherent and move between frames. The views are limited, though:
  - the scripted driver held the PC car against a wall;
  - the fresh Mac profile showed the first-run introduction card over the race.
- Not exercised in this run: the integrated-lobby join path that the v19i run used.

Receipts, in the ignored `analysis/netplay-fixes-20261118/v19-final-20261118/`:

- `report.json`, `sync.json`, `qual.log`;
- PC memory logs `pcmon*.log`;
- the PC runtime log;
- one frame per side.

Earlier evidence: `analysis/netplay-fixes-20261118/CURRENT.md`.

## Cleanup done

On STEAMBOX, removed:

- `C:\Users\Public\R4Fixes20261118`;
- the scheduled tasks `R4Fixes20261118` and `R4FixesShow20261118`;
- the exact-file ASR exclusion (none existed before);
- the firewall rule `R4 fixes test 20261118`.

The two live UDP 7777 router rules were deleted. `SDN_IA` again holds only the rules from `~/bin/router-console-rules`.

Untouched:

- the installed game and native saves;
- Smart App Control;
- the NAS.

## Open

- **Root `game.toml`:** the uncommitted `multitap_analog = true` line (12:44, author unknown) is still uncommitted and preserved.
- **Release:** none published. A release needs the owner's go, ideally after an integrated-lobby NetPlay pass.

Procedures: `docs/WINDOWS_BUILD.md`, `tools/r4_online_battle_regression.py`. The `--remote-seat` option has the PC peer started by the operator.
