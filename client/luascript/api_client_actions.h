/* Minimal client-side Lua actions for testing agents. */
#ifndef FC__API_CLIENT_ACTIONS_H
#define FC__API_CLIENT_ACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

struct lua_State;

/* Moves a unit one step in map-direction dir8 (0..7). */
bool api_client_move_dir(struct lua_State *L, int unit_id, int dir8);

/* Ends the current player's turn. */
bool api_client_end_turn(struct lua_State *L);

#ifdef __cplusplus
}
#endif

#endif /* FC__API_CLIENT_ACTIONS_H */

