#ifndef R4_LOCAL_BATTLE_H
#define R4_LOCAL_BATTLE_H
#ifdef __cplusplus
extern "C" {
#endif
/* Guest-thread selection, snapshotted with the race. Two uses retail VS. */
int r4_local_battle_set_players(unsigned players);
#ifdef __cplusplus
}
#endif
#endif
