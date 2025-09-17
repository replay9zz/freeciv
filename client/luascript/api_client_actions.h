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

/* Orders a unit to attack the given target tile. */
bool api_client_attack_tile(struct lua_State *L, int unit_id, int tile_index);

/* Orders a unit to attack a neighboring tile by direction (0..7). */
bool api_client_attack_dir(struct lua_State *L, int unit_id, int dir8);

/* Orders a unit to attack using GUI/native direction (0..7). */
bool api_client_attack_dir_gui(struct lua_State *L, int unit_id, int gui_dir8);

/* Orders a unit to attack a tile specified by native coordinates. */
bool api_client_attack_native(struct lua_State *L, int unit_id, int nat_x, int nat_y);

#ifdef __cplusplus
}
#endif

#endif /* FC__API_CLIENT_ACTIONS_H */
