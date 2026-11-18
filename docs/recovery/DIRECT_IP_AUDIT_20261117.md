# Mac / STEAMBOX direct-IP audit — 2026-11-17

## Follow-up: rebuilt Windows lobby retest

The follow-up **passed the integrated lobby Start race path** with engine
`7614acb5` on both platforms and launcher `0dfae8c`. The rebuilt Windows SHA-256
`56d54862911c95d75f4dd406ad9b2aade91ea237b8ad7dac5ba25842672e5f40` was read back
from the executed file before cleanup. Mac used the ad-hoc diagnostic hash below.
This supersedes the historical lobby-start blocker described later in this report.

Evidence: `AUDIT/retest/`, including `start-result.json`, `controls.json`,
`return-result.json`, both runtime logs, screenshots, `sync-comparison.json` and
`cleanup.json`. The PC hosted a two-seat LAN lobby; Mac joined
`192.168.53.188:7777`. Clicking Start race at (435,570) restarted into active P2P
netplay on both machines (session 2, rollback enabled), instead of immediately
exiting. Native Link entry reached a two-car Helter Skelter race, phase 2.

Independent controls were read from guest car state on the Mac:

- PC acceleration: P1 x changed 10064896 → 9985824, speed 0 → 324;
  P2 stayed at x=10312704, speed 0.
- Mac acceleration: P2 x changed 10312704 → 10200528, speed 0 → 548;
  PC braking reduced P1 speed 324 → 166.
- Opposite steering changed P1 yaw 3072 → 3032 and P2 3072 → 3170.
- Mac Start opened shared pause; PC Down twice + Start retired; Results
  Down + A exited to native title (mode 0 on both); Back opened both integrated
  menus. Netplay remained active at the title, as recorded in `return-result.json`.

Final sampled dispatch and segment misses were zero on both machines; static
hits PC 2832740, Mac 2114708. Sampled netplay status reported desync=0. The full
logs are more qualified: 259 of 260 shared live-core samples matched through
sim 9248; sim 6080 differed during the return sequence. PC logged a resimulation
core-divergence ABORT and realigned from 6048. Both subsequently replayed sim 6080
with matching final audit core `52979d92`, and later live hashes matched.
This is a recovered rollback divergence, **not a clean all-hashes-match run**.
Investigate it before release qualification. Natural finish/rematch, adverse
network conditions and physical controllers remain untested. Own-view screenshots
still show faint duplicate HUD content; pause shows split-view/overlap artifacts.

Both games used dummy audio and retained PGXP. The Mac remained hidden. Only the
existing finished diagnostic artifacts ran on the PC; no compilation or NAS use.
The owner disabled Smart App Control. Defender ASR rule
`01443614-CD74-433A-B99E-2ECDC07BFC25` then blocked the EXE (event 1121). Under the
owner's existing exact-file exception approval, an ASR-only exclusion was added
for `C:\Users\Public\R4LobbyRetest20261117\r4-runtime.exe`; after propagation,
the diagnostic launch succeeded. No other global protection was changed.

Cleanup verified both runtimes stopped, the exact ASR exclusion removed (no
exclusions remained), the temporary Windows firewall rule and two interactive
tasks removed, disposable PC folder and named staging files removed. The two
live UDP7777 router rules were removed; before/after rule listings are identical.
The persistent router script was not changed in this follow-up. Smart App Control
remains **Off by the owner's action**. Installed games, original saves/settings
and other jobs/services remain untouched. No release/feed was published.

## Original direct-start run: outcome and qualification boundary

**PASS: an actual two-machine race, independent car controls, shared pause,
retire/results, exit to title, and opening the integrated menu on both peers.**
The muted test used explicit IPv4 addresses, not loopback. The successful run
used supported direct-start netplay arguments, not the integrated lobby's Start
race action. The lobby-start path is **not qualified**. No natural race finish,
rollback gameplay, public matchmaking, physical-controller, audible-audio, 4K,
Linux or release qualification is implied. Nothing was published to a release/feed.

Evidence is local in `analysis/direct-ip-audit-20261117/` (`AUDIT` below).
Screenshots were inspected, not inferred from logs. No game data, saves or
screenshots are added to Git. The installed Mac application was not updated.

## Sources and binaries

- Title baseline: `aeb77d20749381c34d5ce6349131085dc4ece6fd`.
- Launcher: `0dfae8cacef08e17e1e617f1e1ff52cff03a939c`.
- Windows tested engine: `1e95d2af1a744fb792ac60701e62378d9355fbcf`.
- Mac tested engine: that pin plus the teardown correction now committed as
  `7614acb55e547a80c9f66d26c13a5ecd97e5350e`.

The peers have the same game code/protocol but are **not an identical engine
revision pair**. The newer Windows EXE compiled but did not run.

| Artifact | SHA-256 |
|---|---|
| Mac `build/r4-runtime` | `a018bf7c7e101599b1e7a4fff2e7848e3c41a832fee3057cf4a10303402f5bca` |
| Executed Mac diagnostic app, ad-hoc signed | `2d70263ea9505e04592807bc0217bcb7d20cb80a246d0ab493270c903413b4ad` |
| Executed Windows original audit EXE | `2cb8446c5309ff0a08845c3a8342217a25be1ae3aa4647d7685fdac7d5d526e7` |
| Rebuilt Windows EXE, blocked before startup | `56d54862911c95d75f4dd406ad9b2aade91ea237b8ad7dac5ba25842672e5f40` |
| Installed Mac app, unchanged | `3892d831e46685b664c25c8b3035f968c47153a1e5b0b57fc7d97bc2930aa029` |

Receipts: `binary-hashes.json`, `pc-before-cleanup.txt`. Executed Mac path:
`analysis/menu-visual-audit-20261117/Menu Audit.app/Contents/MacOS/r4-runtime`.
PC path was `C:\Users\Public\R4DirectIPAudit20261117\r4-runtime.exe` (removed).
Its original artifact remains `AUDIT/windows/r4-runtime.exe`, original package
`AUDIT/windows.zip`; pre-fix Mac binary is `AUDIT/mac-runtime-before-exit-fix`.

Both builds were Release with debug tools enabled and OpenBIOS selected.
Mac IPO was off; Windows IPO was on. All compilation occurred on the Mac.
Windows ran finished artifacts and control/diagnostic scripts only.

## Lobby reproduction and lifecycle correction

On resumption the Mac process was absent; the PC displayed two lobby members
with P2 Ready. That was stale membership, not a live race. The owner authorized
relaunching the muted Mac app. A fresh direct-IP join succeeded again:
`mac-rejoined.png`, `pc-rejoined.png`. At 1280×720 the PC Start button was around
`435,570`, below the earlier `435,544` coordinates. This does not establish the
cause of the earlier failed attempt.

The new Start click created a network session and reached tick-zero entry,
then immediately soft-returned with `menu_to_frontend`. Proof:
`r4-direct-mac-resume.log`, especially `rematch enter-guest sim=0` followed by
that menu exit. `*-start-resume.png` shows the subsequent local menu. No dispatch
or segment misses occurred before this restart.

Framework `7614acb5` clears `g_ovl_exit` after draw callbacks stop and the provider
is detached. The main-thread lobby pump and provider draw thread can both
request the transition; a request posted after the first was consumed otherwise
survives into the new session. This closes the lifecycle hole without changing
guest execution, protocol, controls or renderer behavior. **It is a candidate
correction for the observed symptom; the repaired integrated transition still
needs an end-to-end retest.** The passing direct-start run exercises the new Mac
binary but does not establish this fix. Do not claim Windows menu-start is fixed.
Isolated upstream review: [draft PR #967](https://github.com/RetroPortingToolKit/psxrecomp/pull/967),
review commit `03af1d145d20b9b4e4023ec43a7af000524de597`. The integration remains
on fork master; the review branch is not separately runtime-qualified.

## Windows application-control blocker

Smart App Control blocked the rebuilt unsigned EXE before `main`: Code Integrity
3077, policy `0283ac0f-fff1-49ae-ada1-8a933130cad6`. See `launch-error.txt`,
`policies-before.json`, `policies-after.json`. This was not an SSH, router,
Defender-firewall or game-network error.

The owner approved a temporary exact-file exception. A supplemental policy
containing only the rebuilt EXE's Authenticode SHA-256 was installed via CiTool.
The update returned success, but Windows marked it `IsAuthorized=false` and
`IsEnforced=false`; startup remained blocked. Policy
`74f9b06a-11b4-4ecd-af94-b4042b340fc2` was removed and absence verified. Smart
App Control remained enabled. No global protection, Secure Boot, trust stores,
credentials or Windows-owned policies were changed.

The original audit EXE remained allowed. Its supported direct-start route
completed gameplay testing. A trusted runnable Windows build is still needed
for the new lifecycle code. Microsoft documents the lack of per-app exclusions
in its [Smart App Control FAQ](https://support.microsoft.com/en-gb/windows/security/threat-malware-protection/smart-app-control-frequently-asked-questions)
and the system policy in [inbox policies](https://learn.microsoft.com/en-us/windows/security/application-security/application-control/app-control-for-business/operations/inbox-appcontrol-policies).

## Race and control evidence

PC host/seat 0: `192.168.53.188:7777`; Mac guest/seat 1: `192.168.1.216:7777`.
Session `171117`, delay 6, two seats, P1 host input device on each machine.
This was delay/lockstep (`rollback=0`), not the failed lobby's rollback setup.
Both used `SDL_AUDIODRIVER=dummy`; Mac stayed hidden. One game per machine.
Diagnostic cards/settings were isolated; installed games and owner saves/settings
were not modified.

Existing vanilla-netplay setup clears the PGXP package. The passing run explicitly
set `PSX_GEOMETRY_CORRECTION=1`, `PSX_PERSPECTIVE_TEXTURING=1` and
`PSX_PGXP_CPU_MODE=1` on both peers; both logs confirm geometry/texture correction
on. The runtime suppresses precise culling during netplay because it changes
shared NCLIP results. The failed lobby attempt inherited the existing PGXP-clearing
behavior and is not PGXP-on qualification.

Virtual SDL pads drove native entry, car/course selection and racing. No RAM
writes, race snapshots or synthetic success flags were used.

| Check | Evidence / observation |
|---|---|
| Native entry and course | `*-direct-entry.png`, `*-direct-course.png`; Link mode 4 on both |
| Actual race | `direct-grid.json`, `*-direct-grid.png`; phase 2, two active cars, Helter Skelter, 3 laps |
| PC acceleration alone | `independent-controls.json`: P1 moved 1090.25 units (RAM delta / 256), P2 exactly 0 |
| Mac acceleration | P2 moved 993.5 units, speed 0→507; P1 finished braking 191→0 with 199.125 units of deceleration travel |
| Independent steering | `steering-controls.json`: P1 yaw 3072→2861 with left stick; P2 3072→3268 with right stick |
| Shared pause | `*-direct-paused.png`; Mac Start displayed Cancel/Restart/Retire on both |
| Retire/results | PC Down twice and Start; `*-direct-retire-selected.png`, `*-direct-retire-confirm.png`, `*-direct-results.png` |
| Exit to title | PC Down + A selected Results Exit; both modes became 0; `*-direct-return-menu.png` |
| Integrated menus | Back on each peer; `*-direct-home-menu.png` and JSON show both open |

Race captures expose unresolved visuals: a duplicate upper view/rear-view region
in the Mac view and oversized/overlapping HUD/view regions in the shared pause
presentation. Native Circle/Cross prompts still conflict with Modern A/B.
Gameplay/control success does not qualify those visuals. This was retirement,
not a natural three-lap finish.

### Synchronization and dispatch

`sync-comparison.json`: **345 common core-digest samples**, tick 0 through 11360,
**zero mismatches**. Logs: `r4-direct-mac-cli.log`, Windows `direct-stderr.log`.
These are shared-tick samples, not exhaustive every-tick checks or simultaneous
screenshots. No rollback episodes occurred in this delay-mode run.

Final pre-shutdown status (`final-runtime-status.json`):

| Peer | Tick | Static hits | Dispatch misses | Segment misses | Desync |
|---|---:|---:|---:|---:|---:|
| PC | 11310 | 1,761,260 | 0 | 0 | 0 |
| Mac | 11362 | 1,765,233 | 0 | 0 | 0 |

Sequential SSH reads explain different queried ticks. No full-speed/frame-cadence
claim was measured. Wi-Fi stalls and command latency were not classified as
renderer regressions.

## Prior menu audit carried forward

Evidence: `analysis/menu-visual-audit-20261117/handoff-receipt.json`, `visual/`
(187 PNGs), `logs/`, `helpers/`. Prior tested Mac binary SHA-256:
`8806def012af0b5f8c2f8897903b2d585356cc051ee267c963cce6436c53c4da`.

- Time Attack: moving race and restart/retire/back paths.
- GP: moving race, natural two-lap eighth-place finish, Retry → second attempt,
  End GP → Game Over → Home.
- Local 2/3/4: actual multi-view races, independent inputs, retire/results/exit;
  restart in local 2/4.
- Garage native save/sample decal load, Records car-data, copied career save/load.
- Gaps: successful GP qualifying/next heat, natural TA finish, fresh natural
  local-mode finishes. Earlier lobby evidence did not prove a network race.

Retained fixes: popup Cancel stays in its widget; modern A/B frontend mappings
include Decal Edit/Game Over; integrated netplay callbacks initialize with the
preboot launcher skipped; smoke requires a playable race. Existing TA SECTION/HUD
overlap, local 3/4 start-light boundary artifacts, native control prompts and
LinkEntry/LinkBattle naming remain unresolved.

## Verification and cleanup

- Mac Release and Windows Release cross-build pass.
- Runtime/title CTest **47/47**; launcher CTest **19/19**.
- `tools/regen.sh` completes; tracked generated C unchanged.
- Hidden/dummy-audio `tools/smoke.py` reaches an actual GP race, zero dispatch
  and segment misses: `fixed-smoke/`, `r4-direct-smoke.log`.
- Both virtual pads detached and processes stopped. Debug quit can reply
  `emu busy or frozen` during exit; process absence was checked separately.
- Removed only the two UDP 7777 host-pair lines/comment in
  `~/bin/router-console-rules`, then their exact live rules. Before/after files
  verify every unrelated live rule unchanged. The existing reapplication job
  remains installed; the prior backup was preserved.
- Removed Windows firewall rule `R4 direct-IP audit game 20261117`; probe rule
  was already absent. Removed task `R4DirectIPAudit20261117`, disposable folder,
  the audit ZIP and named helpers in `C:\Windows\Temp`. `debug.ps1` was
  hash-checked first. Cleanup removed its own script.
- `cleanup-pc.json`: folder/task/rules/runtime/temporary policy absent;
  Smart App Control enforced. No installed game, owner disc, unrelated job,
  SSH/audio/streaming rule or service changed. No PC reboot or NAS use.

## Continuation

Resolve trusted execution of the new Windows artifact, then repeat the integrated
lobby Start race with engine `7614acb5` on both platforms. This passing direct-start
run is not proof of that repair. Follow with rollback gameplay, natural
finish/rematch, clean own-view/HUD rendering and physical controller/audio checks
before release qualification.

Local helpers: `AUDIT/control.py` (`ask`, `shot`), `drive_live.py`. PC setup,
helpers and allowances no longer exist: recreate the narrowly scoped test setup
before using them. `setup.ps1`, `firewall-probe.ps1`, `launch-direct.ps1`, and
`cleanup-pc.ps1` retain the setup/control/cleanup recipe. `launch-direct.ps1`
records exact Windows arguments and unbuffered stdout/stderr redirection.
This is not a persistent service or permanent change to the project PC rules.

Mac direct-start command, after confirming no existing runtime:

```sh
env PSX_GEOMETRY_CORRECTION=1 PSX_PERSPECTIVE_TEXTURING=1 \
  PSX_PGXP_CPU_MODE=1 PSX_DEBUG_SERVER=1 PSX_VSYNC=0 SDL_AUDIODRIVER=dummy \
  PSX_BUNDLE_DATA_DIR="$PWD/analysis/menu-visual-audit-20261117" \
  'analysis/menu-visual-audit-20261117/Menu Audit.app/Contents/MacOS/r4-runtime' \
  --hidden-window --debug-port 4798 --no-launcher --netplay --net-slot 1 \
  --net-input-player 0 --net-bind 0.0.0.0:7777 --net-peer 192.168.53.188:7777 \
  --net-session-id 171117 --net-delay 6
```

Build/check commands:

```sh
cmake --build build --config Release -j 4
cmake --build build-windows --config Release -j 4
ctest --test-dir build --output-on-failure
ctest --test-dir build-custom-ui --output-on-failure
JOBS=4 tools/regen.sh
# Against an isolated non-netplay runtime:
python3 tools/smoke.py analysis/<new-run>/smoke --port 4798
```

Installed `~/Applications/Ridge Racer Type 4 Remixed.app` and desktop shortcut
still use the earlier signed build. The diagnostic app is ad-hoc signed, not a
newly notarized or release-qualified replacement.
