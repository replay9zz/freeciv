/* Minimal client-side Lua actions for testing agents. */
#ifndef FC__API_CLIENT_ACTIONS_H
#define FC__API_CLIENT_ACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

struct lua_State;

bool api_client_move_dir(struct lua_State *L, int unit_id, int dir8);
bool api_client_build_city(struct lua_State *L, int unit_id);
bool api_client_found_city(struct lua_State *L, int unit_id,
                           const char *city_name);
bool api_client_set_city_production(struct lua_State *L, int city_id,
                                    const char *kind,
                                    const char *rule_name);
bool api_client_end_turn(struct lua_State *L);
bool api_client_start_revolution(struct lua_State *L);
bool api_client_set_government(struct lua_State *L, const char *gov_identifier);
bool api_client_attack_tile(struct lua_State *L, int unit_id,
                            int tile_index);
bool api_client_attack_dir(struct lua_State *L, int unit_id, int dir8);
bool api_client_attack_dir_gui(struct lua_State *L, int unit_id,
                               int gui_dir8);
bool api_client_attack_native(struct lua_State *L, int unit_id,
                              int nat_x, int nat_y);
bool api_client_attack_city(struct lua_State *L, int unit_id,
                            int city_id);
bool api_client_conquer_city(struct lua_State *L, int unit_id,
                             int city_id);
bool api_client_set_research(struct lua_State *L, int player_id,
                             const char *tech_identifier);
bool api_client_set_research_goal(struct lua_State *L, int player_id,
                                  const char *tech_identifier);
int api_client_tile_known(struct lua_State *L, int tile_index);
bool api_client_tile_seen(struct lua_State *L, int tile_index,
                          int vlayer);
int api_client_unit_vision_radius_sq(struct lua_State *L, int unit_id);

#ifdef __cplusplus
}
#endif

#endif /* FC__API_CLIENT_ACTIONS_H */
