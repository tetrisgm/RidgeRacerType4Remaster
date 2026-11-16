/* Run the production hooks against guest memory and a temporal redraw. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include "r4_camera_look_plugin.c"
int g_psx_render_pass_active;
static uint8_t ram[2 * 1024 * 1024], scratch[1024];
static uint8_t *at(uint32_t a) {
    return (a & 0xfffffc00u) == 0x1f800000u ? scratch + (a & 1023) : ram + (a & 0x1fffff);
}
uint8_t psx_mod_read_byte(uint32_t a) { return *at(a); }
uint16_t psx_mod_read_half(uint32_t a) { uint16_t v; memcpy(&v, at(a), 2); return v; }
uint32_t psx_mod_read_word(uint32_t a) { uint32_t v; memcpy(&v, at(a), 4); return v; }
void psx_mod_write_word(uint32_t a, uint32_t v) { memcpy(at(a), &v, 4); }
int psx_mod_option_value(const char *p,const char *f,const char *o,char *v,uint32_t n) { return 0; }
int psx_mod_read_local_pad_sticks(uint32_t p,uint8_t out[4]) { memset(out,128,4); out[2]=255; return 1; }
int psx_rewind_register_region(void *p,uint32_t n) { return p && n; }
int psx_mod_register_activation_plugin(const char *p,PSXModActivationCallback cb) { return cb != 0; }
int psx_mod_register_vblank_plugin(const char *p,PSXModVBlankCallback cb) { return cb != 0; }
int psx_mod_register_option_changed_plugin(const char *p,PSXModOptionChangedCallback cb) { return cb != 0; }
int psx_mod_register_function_entry_plugin(const char *p,uint32_t a,PSXModFunctionEntryCallback cb) { return cb != 0; }
int main(void) {
    wr32(R4_RACE_PHASE_ADDR,2);
    wr32(R4_HANDLER_TABLE_ADDR,0x80010000);
    wr32(0x80010000,0x8011729c);
    wr32(0x8011729c,0x3c04800f);
    wr32(0x8011729c+4,0x3c038010);
    r4_camera_look_activate();
    for (int i=0;i<10;i++) r4_camera_look_vblank();
    wr32(R4_CAMERA_INPUT_POS+8,800);
    wr32(R4_CAMERA_INPUT_YAW,100);
    r4_camera_look_build(0,0);
    assert(s_restore_pending);
    R4CameraState before=s_main;
    uint32_t adjusted[5];
    for (int i=0;i<5;i++) adjusted[i]=rd32(k_camera_inputs[i]);
    /* Interpolation changes the captured adjusted inputs. A second look must
     * not be applied, nor may sandbox callbacks replace native restore state. */
    g_psx_render_pass_active=1;
    wr32(R4_CAMERA_INPUT_YAW,adjusted[4]+13);
    r4_camera_look_build(0,0);
    assert(rd32(R4_CAMERA_INPUT_YAW)==adjusted[4]+13);
    assert(memcmp(&s_main,&before,sizeof before)==0);
    g_psx_render_pass_active=0;
    for (int i=0;i<5;i++) wr32(k_camera_inputs[i],adjusted[i]);
    r4_camera_look_restore(0,0);
    assert(rd32(R4_CAMERA_INPUT_YAW)==100);
    assert(rd32(R4_CAMERA_INPUT_POS+8)==800);
    assert(!s_restore_pending);
    return 0;
}
