/* Exercise actual state publication and contention without a game process. */
#include <assert.h>
#include <string.h>
#include "../src/mods/r4_race_state.c"

int psx_rewind_register_region(void *state,uint32_t bytes) { return state && bytes; }
static uint32_t test_frame = 1;
static unsigned guest_writes;
uint8_t psx_mod_read_byte(uint32_t a) { (void)a; return 0; }
uint16_t psx_mod_read_half(uint32_t a) {
    if (a == RACE_WORD) return 0x180;
    if (a == CAR_COUNT) return 1;
    return 0;
}
uint32_t psx_mod_read_word(uint32_t a) {
    if (a == FRAMES) return test_frame;
    if (a == CAR_TABLE) return 0x80010000;
    if (a == 0x80010010) return test_frame;
    if (a == CAM_EYE) return test_frame * 10;
    if (a == CAM_YAW) return 4090 + test_frame;
    if (a == VIEW_H) return 256;
    return 0;
}
void psx_mod_write_word(uint32_t a, uint32_t v) { (void)a; (void)v; ++guest_writes; }
void psx_mod_counter_add(const char *name, uint32_t delta) { (void)name; (void)delta; }
int psx_mod_register_command(const char *name, PSXModCommandFn fn) {
    (void)name; (void)fn; return 1;
}
int psx_mod_register_function_entry_plugin(const char *id, uint32_t address,
                                          PSXModFunctionEntryCallback fn) {
    (void)id; (void)address; (void)fn; return 1;
}

int main(void) {
    CPUState cpu = {0};
    R4RaceState sample, unchanged;
    memset(&sample, 0x5a, sizeof sample);
    unchanged = sample;
    assert(!r4_race_state_read(&sample));
    assert(!memcmp(&sample, &unchanged, sizeof sample));
    view_build(&cpu, VIEW_BUILD);
    assert(r4_race_state_read(&sample));
    assert(sample.frame == 1 && sample.cars[0].x == 1);

    /* A busy producer must not leak a partially written state to a reader. */
    assert(!atomic_flag_test_and_set(&s_state_lock));
    unchanged = sample;
    assert(!r4_race_state_read(&sample));
    assert(!memcmp(&sample, &unchanged, sizeof sample));
    test_frame = 2;
    view_build(&cpu, VIEW_BUILD); /* Optional publication skips, never blocks. */
    atomic_flag_clear(&s_state_lock);
    assert(r4_race_state_read(&sample) && sample.frame == 1);
    view_build(&cpu, VIEW_BUILD);
    assert(r4_race_state_read(&sample));
    assert(sample.frame == 2 && sample.cars[0].x == 2);
    assert(r4_race_state_read_delayed(&sample, 1));
    assert(sample.frame == 1 && sample.cars[0].x == 1);
    /* Observation must not shift the camera relative to the moving car,
     * or alter a lookaround angle after its restore bookkeeping. */
    for (test_frame=3; test_frame<30; ++test_frame) {
        view_build(&cpu, VIEW_BUILD);
        assert(r4_race_state_read(&sample));
        assert(sample.cameras[0].eye[0] == (int32_t)test_frame * 10);
        assert(sample.cameras[0].yaw == (int32_t)(4090 + test_frame));
        view_build(&cpu, VIEW_BUILD); /* intermediate draw */
    }
    assert(guest_writes == 0);
    return 0;
}
