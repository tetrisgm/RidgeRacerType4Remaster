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
    psx_mod_write_half(0x800AC802u, (uint16_t)item);
    psx_mod_write_word(0x800F3C10u, 1); // native menu confirm edge, after input polling
    r4_menu_request.store(0);
    r4_menu_actions.store(0);
    r4_menu_result.store(1);
}
void r4_menu_frame(CPUState* cpu, uint32_t) {
    if (!cpu || g_psx_render_pass_active || cpu->gpr[31] != 0x8001E764u) return;
    const uint32_t handler = r4_frame_handler(psx_mod_read_word, psx_mod_read_half);
    const uint32_t phase = psx_mod_read_word(R4_RACE_PHASE_ADDR);
    const unsigned mode = psx_mod_read_half(0x800F4EF4u);
    unsigned available = 0;
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
        }
    }
    r4_menu_actions.store(available);
    if ((r4_menu_request.load() & 0x3F00u) && (available & 8u)) return;
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
const int r4_menu_select_hook = psx_game_register_function_entry(0x8002F5E8u, r4_menu_select);
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
