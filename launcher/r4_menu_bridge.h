// Native race actions run on the guest thread, never on the presentation thread.
#pragma once
#if __has_include("psx_menu_host.h")
#include "mod_plugins.h"
#include "psx_netplay.h"
#include "cpu_state.h"
#include "psx_cycle_freeze.h"
#include "../src/mods/r4_widescreen_scene.h"
#include "../src/mods/r4_local_battle.h"
#include <atomic>
namespace {
std::atomic<unsigned> r4_menu_actions{0}, r4_menu_request{0};
std::atomic<int> r4_menu_result{1};
std::atomic<unsigned> r4_local_menu_players{2};
// Guest-thread cache survives the native title -> demonstration -> title loop.
std::atomic<unsigned> r4_home_actions{0};
std::atomic<bool> r4_home_leaving{false};
void r4_home_reset() { r4_home_actions = 0; r4_home_leaving = false; r4_menu_actions = 0; }
void r4_home_input(CPUState* cpu, uint32_t address) {
    if (!cpu || g_psx_render_pass_active || psx_netplay_active()) return;
    if (address == 0x8002F314u && psx_mod_read_half(0x800F4F3Au) == 1u) {
        // Enter the native menu normally after the intro (including its setup).
        psx_mod_write_half(0x800F3BF0u, psx_mod_read_half(0x800F3BF0u) | 0x0800u);
    } else if (address == 0x8005E280u && r4_home_actions && r4_menu_request.load()) {
        // A queued home choice dismisses the demo through its normal exit.
        psx_mod_write_word(0x800F3C10u, 1u);
    }
}
unsigned r4_menu_available() { return r4_menu_actions.load(); }
bool r4_menu_queue(unsigned action) {
    if (!action || (action & (action - 1u)) || !(r4_menu_available() & action) || action == 1u || action == 8u) return false;
    int ready = r4_menu_result.load();
    if (!ready || !r4_menu_result.compare_exchange_strong(ready, 0)) return false;
    r4_menu_request.store(action);
    return true;
}
bool r4_menu_local_game(unsigned players) {
    if (players < 2 || players > 4 || !(r4_menu_available() & 8u)) return false;
    r4_local_menu_players.store(players);
    return r4_menu_queue(0x400u);
}
int r4_menu_status() { return r4_menu_result.load(); }
void r4_menu_select(CPUState* cpu, uint32_t) {
    if (!cpu || g_psx_render_pass_active || psx_netplay_active()) return;
    const unsigned action = r4_menu_request.load();
    if (!(action & 0x3F00u)) return;
    const uint32_t table = psx_mod_read_word(0x800F4F30u);
    unsigned item = 0;
    while ((0x100u << item) != action && item < 6u) ++item;
    if (item == 2u && r4_local_menu_players.load() > 2u)
        item = psx_mod_read_half(0x800F4E80u); // native additional Battle entry
    if (item >= 9u || !(r4_menu_available() & action) ||
        psx_mod_read_half(table + item * 36u + 32u) == 1u) {
        r4_menu_request.store(0); r4_menu_result.store(-1); return;
    }
    if (action == 0x400u) {
        if (!r4_local_battle_set_players(r4_local_menu_players.load())) {
            r4_menu_request.store(0); r4_menu_result.store(-1); return;
        }
    } else (void)r4_local_battle_set_players(2);
    // Selection and attract timeout are evaluated in the same native frame.
    // Clear the accelerated idle timer before confirming, including when the
    // requested item already matches the cursor (no native cursor-change reset).
    psx_mod_write_word(0x800AC068u, 0u);
    psx_mod_write_half(0x800AC802u, (uint16_t)item);
    psx_mod_write_word(0x800F3C10u, 1); // native menu confirm edge, after input polling
    r4_menu_request.store(0);
    // Keep the frontend and its retained background until native selection
    // has actually handed off to the selected mode, not just accepted input.
    r4_home_leaving = true;
}
void r4_menu_frame(CPUState* cpu, uint32_t) {
    if (!cpu || g_psx_render_pass_active || cpu->gpr[31] != 0x8001E764u) return;
    const uint32_t handler = r4_frame_handler(psx_mod_read_word, psx_mod_read_half);
    const uint32_t phase = psx_mod_read_word(R4_RACE_PHASE_ADDR);
    const unsigned mode = psx_mod_read_half(0x800F4EF4u);
    unsigned available = 0;
    // The two shared loader handlers precede the selected mode. Keep its
    // title/interstitial hidden too; acknowledge only the actual destination.
    if (r4_home_leaving && handler != 0x8002F110u &&
        handler != 0x8003A7F4u && handler != 0x8003AA64u) {
        r4_home_leaving = false;
        r4_home_actions = 0;
        r4_menu_result.store(1);
    }
    if (!psx_netplay_active() && phase >= 1 && phase <= 2 && mode < 7 &&
        (handler == 0x80114A38u || handler == 0x8011729Cu || handler == 0x80114C30u || handler == 0x80115770u) &&
        r4_is_race_handler(psx_mod_read_word, handler) &&
        psx_mod_read_word(0x80034E08u) == 0x27BDFFE8u &&
        psx_mod_read_word(0x80034E0Cu) == 0x3C02800Fu) {
        const unsigned last = psx_mod_read_byte(0x8009F8B8u + mode);
        available = last == 2 ? 7u : last == 1 ? 5u : 1u;
    }
    if (!psx_netplay_active() && handler == 0x8002F110u &&
        psx_mod_read_half(0x800F4F3Au) == 2u) {
        const uint32_t table = psx_mod_read_word(0x800F4F30u);
        if ((table & 0xFFE00003u) == 0x80000000u && psx_mod_read_byte(table + 20u) == 2u) {
            available = 8u;
            for (unsigned i = 0; i < 6; ++i)
                if (i == 2u || psx_mod_read_half(table + i * 36u + 32u) != 1u)
                    available |= 0x100u << i;
            r4_home_actions = available;
            if (!r4_menu_request.load() && !r4_home_leaving) {
                // Skip only the idle wait; native loading and fades still run.
                psx_mod_write_word(0x800AC068u, 3600u);
                // The stock attract loop alternates the intro and racing.
                psx_mod_write_word(0x8010C0ECu, psx_mod_read_word(0x8010C0ECu) & ~1u);
            }
        }
    }
    if (available & 7u) r4_home_actions = 0;
    if (!available && r4_home_actions && !psx_netplay_active())
        available = r4_home_actions.load();
    // Capture only established racing images, never the Demonstration title.
    if (r4_home_actions && (r4_menu_request.load() || r4_home_leaving || handler != 0x8005E118u ||
        psx_mod_read_word(0x800ACCC4u) < 60u ||
        (int16_t)psx_mod_read_half(0x800AC788u) > 0)) available |= 16u;
    r4_menu_actions.store(available);
    if ((r4_menu_request.load() & 0x3F00u) && r4_home_actions) return;
    const unsigned action = r4_menu_request.exchange(0);
    if (!action) return;
    if (!(available & action)) { r4_menu_result.store(-1); return; }
    // This is the same cursor and dispatch routine used by the original
    // pause handler. Preserve its career bookkeeping and state transitions.
    const unsigned last = psx_mod_read_byte(0x8009F8B8u + mode);
    psx_mod_write_byte(0x800F318Cu, action == 2u ? 1u : last);
    psx_mod_write_byte(0x800F4E18u, 0);
    if (mode == 4u) psx_mod_call_guest(cpu, 0x8004FD58u, cpu->pc, 0, 0, 0, 0);
    psx_mod_call_guest(cpu, 0x80034E08u, cpu->pc, mode, 0, 0, 0);
    if (mode == 4u) {
        psx_mod_call_guest(cpu, 0x80114B70u, cpu->pc, 0, 0, 0, 0);
        psx_mod_call_guest(cpu, 0x8003658Cu, cpu->pc, 0, 0, 0, 0);
    } else psx_mod_call_guest(cpu, 0x8004FD58u, cpu->pc, 0, 0, 0, 0);
    r4_menu_actions.store(0);
    r4_menu_result.store(1);
}
// Keep each native menu item's animation/state updates, but unlink its
// drawing commands. The title backdrop uses the same frame and stays visible.
uint32_t r4_menu_saved_ot[0x2C0];
uint32_t r4_menu_saved_ot_base;
void r4_menu_draw_begin(CPUState* cpu, uint32_t) {
    r4_menu_saved_ot_base = 0;
    if (!cpu || g_psx_render_pass_active || psx_netplay_active() ||
        cpu->gpr[31] != 0x8002F5B4u ||
        r4_frame_handler(psx_mod_read_word, psx_mod_read_half) != 0x8002F110u) return;
    const uint32_t base = psx_mod_read_word(0x800ACDCCu) + 0x70u;
    if ((base & 0xFFE00003u) != 0x80000000u) return;
    r4_menu_saved_ot_base = base;
    for (unsigned i = 0; i < 0x2C0; ++i) r4_menu_saved_ot[i] = psx_mod_read_word(base + i * 4u);
}
void r4_menu_draw_end(CPUState*, uint32_t) {
    if (!r4_menu_saved_ot_base) return;
    for (unsigned i = 0; i < 0x2C0; ++i)
        psx_mod_write_word(r4_menu_saved_ot_base + i * 4u, r4_menu_saved_ot[i]);
    r4_menu_saved_ot_base = 0;
}
// The title logo and "Press Start" are separate sprites in the title atlas.
// Keep its animated light trails and copyright backdrop, but do not draw a
// second game heading or a stale Start prompt behind the replacement menu.
void r4_menu_title_draw(CPUState* cpu, uint32_t) {
    if (!cpu || g_psx_render_pass_active || psx_netplay_active() ||
        r4_frame_handler(psx_mod_read_word, psx_mod_read_half) != 0x8002F110u) return;
    const unsigned stage = psx_mod_read_half(0x800F4F3Au);
    if (stage < 2u || stage > 4u) return;
    uint32_t link = cpu->gpr[4] & 0xFFFFFFu;
    // Bound malformed/cyclic lists and retain all commands except the three
    // audited title-atlas sprites. Packet allocation and guest logic still run.
    for (unsigned count = 0; link < 0x800000u && !(link & 3u) && count < 8192u; ++count) {
        const uint32_t tag = psx_mod_read_word(0x80000000u | link);
        const uint32_t next = tag & 0xFFFFFFu;
        if (next >= 0x800000u || (next & 3u)) break;
        const uint32_t packet = 0x80000000u | next;
        const uint32_t next_tag = psx_mod_read_word(packet);
        bool hide = false;
        if ((next_tag >> 24) == 4u && (psx_mod_read_word(packet + 4u) >> 24) == 0x66u) {
            const uint32_t uv = psx_mod_read_word(packet + 12u);
            const uint32_t size = psx_mod_read_word(packet + 16u);
            hide = (uv == 0x7F380000u && size == 0x00C800E8u) ||
                   (uv == 0x7F38C800u && size == 0x00180008u) ||
                   (uv == 0x7F38C808u && size == 0x00180070u);
        }
        if (hide) psx_mod_write_word(0x80000000u | link, (tag & 0xFF000000u) | (next_tag & 0xFFFFFFu));
        else link = next;
    }
}
const int r4_menu_title_draw_hook = psx_game_register_function_entry(0x80093520u, r4_menu_title_draw);
const int r4_menu_draw_hook = psx_game_register_function_entry(0x8003F150u, r4_menu_draw_begin);
const int r4_menu_draw_end_hook = psx_game_register_instruction(0x8003FDACu, 0x8FA80030u, r4_menu_draw_end);
const int r4_menu_select_hook = psx_game_register_function_entry(0x8002F5E8u, r4_menu_select);
const int r4_home_title_hook = psx_game_register_instruction(0x8002F314u, 0x3C02800Fu, r4_home_input);
const int r4_home_demo_hook = psx_game_register_instruction(0x8005E280u, 0x3C02800Fu, r4_home_input);
const int r4_menu_hook = psx_game_register_function_entry(0x80093418u, r4_menu_frame);
}
#else
namespace {
unsigned r4_menu_available() { return 0; }
bool r4_menu_queue(unsigned) { return false; }
bool r4_menu_local_game(unsigned) { return false; }
int r4_menu_status() { return -1; }
}
#endif
