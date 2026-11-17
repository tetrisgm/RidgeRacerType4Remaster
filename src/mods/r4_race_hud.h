#ifndef R4_RACE_HUD_H
#define R4_RACE_HUD_H
#ifdef __cplusplus
/* Attach the shared HUD/effects layer to the current activated session. */
void r4_race_hud_bind();
extern "C" {
#endif
/* True only after a replacement position panel has actually been drawn. */
int r4_race_hud_replaces_rank(void);
int r4_race_hud_replaces_gauge(void);
#ifdef __cplusplus
}
#endif
#endif
