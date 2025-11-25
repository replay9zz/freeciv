/*****************************************************************************
  Minimal client-side Lua actions to drive the game from LuaRemote.
*****************************************************************************/

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

#include <stdbool.h>

/* common/scriptcore */
#include "luascript.h"

/* common */
#include "game.h"
#include "map.h"          /* enum direction8, DIR8_MAGIC_MAX */
#include "player.h"
#include "requirements.h"
#include "tile.h"
#include "unit.h"
#include "vision.h"       /* enum vision_layer, V_COUNT */

/* client */
#include "citydlg_common.h"
#include "climap.h"
#include "client_main.h"
#include "control.h"

#include "api_client_actions.h"

/*************************************************************************//**
  Move unit by id one step in a map direction (0..7).
*****************************************************************************/
bool api_client_move_dir(lua_State *L, int unit_id, int dir8)
{
  LUASCRIPT_CHECK_STATE(L, FALSE);

  struct player *pplayer = client_player();
  LUASCRIPT_CHECK(L, pplayer != NULL, "no client player", FALSE);

  LUASCRIPT_CHECK_ARG(L, dir8 >= 0 && dir8 < DIR8_MAGIC_MAX, 3,
                      "invalid dir8 (expected 0..7)", FALSE);

  struct unit *punit = player_unit_by_number(pplayer, unit_id);
  LUASCRIPT_CHECK_ARG(L, punit != NULL, 2, "unknown unit id", FALSE);

  request_move_unit_direction(punit, (enum direction8)dir8);
  return TRUE;
}

/*************************************************************************//**
  Order a unit to build or add to a city on its current tile.
*****************************************************************************/
bool api_client_build_city(lua_State *L, int unit_id)
{
  LUASCRIPT_CHECK_STATE(L, FALSE);

  struct player *pplayer = client_player();
  LUASCRIPT_CHECK(L, pplayer != NULL, "no client player", FALSE);

  struct unit *punit = player_unit_by_number(pplayer, unit_id);
  LUASCRIPT_CHECK_ARG(L, punit != NULL, 2, "unknown unit id", FALSE);

  request_unit_build_city(punit);
  return TRUE;
}

/*************************************************************************//**
  Found a city and supply the city name explicitly (no popup).
*****************************************************************************/
bool api_client_found_city(lua_State *L, int unit_id, const char *city_name)
{
  LUASCRIPT_CHECK_STATE(L, FALSE);

  struct player *pplayer = client_player();
  LUASCRIPT_CHECK(L, pplayer != NULL, "no client player", FALSE);

  struct unit *punit = player_unit_by_number(pplayer, unit_id);
  LUASCRIPT_CHECK_ARG(L, punit != NULL, 2, "unknown unit id", FALSE);

  struct tile *ptile = unit_tile(punit);
  LUASCRIPT_CHECK(L, ptile != NULL, "unit not on map", FALSE);

  if (tile_city(ptile)) {
    request_do_action(ACTION_JOIN_CITY, punit->id, tile_city(ptile)->id, 0, "");
  } else {
    const char *lname = city_name ? city_name : "";

    request_do_action(ACTION_FOUND_CITY, punit->id, tile_index(ptile), 0,
                      lname);
  }
  return TRUE;
}

/*************************************************************************//**
  Change the current production of a city.
*****************************************************************************/
bool api_client_set_city_production(lua_State *L, int city_id,
                                    const char *kind, const char *rule_name)
{
  LUASCRIPT_CHECK_STATE(L, FALSE);

  const char *type = (kind && kind[0]) ? kind : "UnitType";
  LUASCRIPT_CHECK_ARG(L, rule_name != NULL && rule_name[0] != '\0', 3,
                      "missing rule name", FALSE);

  struct city *pcity = game_city_by_number(city_id);
  LUASCRIPT_CHECK_ARG(L, pcity != NULL, 2, "unknown city id", FALSE);

  struct universal target = universal_by_rule_name(type, rule_name);
  LUASCRIPT_CHECK(L, target.kind != VUT_NONE, "invalid production target",
                  FALSE);

  city_change_production(pcity, &target);
  return TRUE;
}

/*************************************************************************//**
  End current player's turn.
*****************************************************************************/
bool api_client_end_turn(lua_State *L)
{
  LUASCRIPT_CHECK_STATE(L, FALSE);

  send_turn_done();
  return TRUE;
}

/*************************************************************************//**
  Order a unit to attack a target tile (adjacent as per rules).
*****************************************************************************/
bool api_client_attack_tile(lua_State *L, int unit_id, int tile_index)
{
  LUASCRIPT_CHECK_STATE(L, FALSE);

  struct player *pplayer = client_player();
  LUASCRIPT_CHECK(L, pplayer != NULL, "no client player", FALSE);

  struct unit *punit = player_unit_by_number(pplayer, unit_id);
  LUASCRIPT_CHECK_ARG(L, punit != NULL, 2, "unknown unit id", FALSE);

  struct tile *ptile = index_to_tile(&(wld.map), tile_index);
  LUASCRIPT_CHECK_ARG(L, ptile != NULL, 3, "invalid target tile", FALSE);

  request_do_action(ACTION_ATTACK, unit_id, tile_index, 0, "");
  return TRUE;
}

/*************************************************************************//**
  Order a unit to attack a neighboring tile by map direction (0..7).
*****************************************************************************/
bool api_client_attack_dir(lua_State *L, int unit_id, int dir8)
{
  LUASCRIPT_CHECK_STATE(L, FALSE);

  struct player *pplayer = client_player();
  LUASCRIPT_CHECK(L, pplayer != NULL, "no client player", FALSE);

  LUASCRIPT_CHECK_ARG(L, dir8 >= 0 && dir8 < DIR8_MAGIC_MAX, 3,
                      "invalid dir8 (expected 0..7)", FALSE);

  struct unit *punit = player_unit_by_number(pplayer, unit_id);
  LUASCRIPT_CHECK_ARG(L, punit != NULL, 2, "unknown unit id", FALSE);

  struct tile *dest_tile = mapstep(&(wld.map), unit_tile(punit),
                                   (enum direction8)dir8);
  LUASCRIPT_CHECK(L, dest_tile != NULL, "destination off map", FALSE);

  request_do_action(ACTION_ATTACK, unit_id, tile_index(dest_tile), 0, "");
  return TRUE;
}

/*************************************************************************//**
  Order a unit to attack using GUI/native direction (0..7).
*****************************************************************************/
bool api_client_attack_dir_gui(lua_State *L, int unit_id, int gui_dir8)
{
  LUASCRIPT_CHECK_STATE(L, FALSE);

  struct player *pplayer = client_player();
  LUASCRIPT_CHECK(L, pplayer != NULL, "no client player", FALSE);

  LUASCRIPT_CHECK_ARG(L, gui_dir8 >= 0 && gui_dir8 < DIR8_MAGIC_MAX, 3,
                      "invalid gui_dir8 (expected 0..7)", FALSE);

  struct unit *punit = player_unit_by_number(pplayer, unit_id);
  LUASCRIPT_CHECK_ARG(L, punit != NULL, 2, "unknown unit id", FALSE);

  enum direction8 map_dir = gui_to_map_dir((enum direction8)gui_dir8);
  struct tile *dest_tile = mapstep(&(wld.map), unit_tile(punit), map_dir);
  LUASCRIPT_CHECK(L, dest_tile != NULL, "destination off map", FALSE);

  request_do_action(ACTION_ATTACK, unit_id, tile_index(dest_tile), 0, "");
  return TRUE;
}

/*************************************************************************//**
  Order a unit to attack a tile given in native coordinates.
*****************************************************************************/
bool api_client_attack_native(lua_State *L, int unit_id,
                              int nat_x, int nat_y)
{
  LUASCRIPT_CHECK_STATE(L, FALSE);

  struct player *pplayer = client_player();
  LUASCRIPT_CHECK(L, pplayer != NULL, "no client player", FALSE);

  struct unit *punit = player_unit_by_number(pplayer, unit_id);
  LUASCRIPT_CHECK_ARG(L, punit != NULL, 2, "unknown unit id", FALSE);

  struct tile *ptile = native_pos_to_tile(&(wld.map), nat_x, nat_y);
  LUASCRIPT_CHECK_ARG(L, ptile != NULL, 3,
                      "invalid native coordinates", FALSE);

  request_do_action(ACTION_ATTACK, unit_id, tile_index(ptile), 0, "");
  return TRUE;
}

/*************************************************************************//**
  Return the knowledge state for the given tile as enum known_type value.
*****************************************************************************/
int api_client_tile_known(lua_State *L, int tile_index)
{
  LUASCRIPT_CHECK_STATE(L, TILE_UNKNOWN);

  struct player *pplayer = client_player();
  LUASCRIPT_CHECK(L, pplayer != NULL, "no client player", TILE_UNKNOWN);

  struct tile *ptile = index_to_tile(&(wld.map), tile_index);
  LUASCRIPT_CHECK_ARG(L, ptile != NULL, 2, "invalid tile index",
                      TILE_UNKNOWN);

  return (int)tile_get_known(ptile, pplayer);
}

/*************************************************************************//**
  Return TRUE if the tile is currently seen in the requested vision layer.
*****************************************************************************/
bool api_client_tile_seen(lua_State *L, int tile_index, int vlayer)
{
  LUASCRIPT_CHECK_STATE(L, FALSE);

  struct player *pplayer = client_player();
  LUASCRIPT_CHECK(L, pplayer != NULL, "no client player", FALSE);

  LUASCRIPT_CHECK_ARG(L, vlayer >= 0 && vlayer < V_COUNT, 3,
                      "invalid vision layer", FALSE);

  struct tile *ptile = index_to_tile(&(wld.map), tile_index);
  LUASCRIPT_CHECK_ARG(L, ptile != NULL, 2, "invalid tile index", FALSE);

  return client_map_is_known_and_seen(ptile, pplayer,
                                      (enum vision_layer)vlayer);
}

/*************************************************************************//**
  Return the squared vision radius for the given unit (main vision layer).
*****************************************************************************/
int api_client_unit_vision_radius_sq(lua_State *L, int unit_id)
{
  LUASCRIPT_CHECK_STATE(L, -1);

  struct player *pplayer = client_player();
  LUASCRIPT_CHECK(L, pplayer != NULL, "no client player", -1);

  struct unit *punit = player_unit_by_number(pplayer, unit_id);
  LUASCRIPT_CHECK_ARG(L, punit != NULL, 2, "unknown unit id", -1);

  return unit_type_get(punit)->vision_radius_sq;
}
