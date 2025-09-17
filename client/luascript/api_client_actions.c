/*****************************************************************************
  Minimal client-side Lua actions to drive the game from LuaRemote.
*****************************************************************************/

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

/* common/scriptcore */
#include "luascript.h"

/* common */
#include "map.h"          /* enum direction8, DIR8_MAGIC_MAX */
#include "player.h"
#include "unit.h"

/* client */
#include "client_main.h"  /* client_player(), send_turn_done() */
#include "control.h"      /* request_move_unit_direction(), request_do_action */
#include "climap.h"       /* gui_to_map_dir */

#include "api_client_actions.h"

/*************************************************************************//**
  Move unit by id one step in a map direction (0..7).
*****************************************************************************/
bool api_client_move_dir(lua_State *L, int unit_id, int dir8)
{
  LUASCRIPT_CHECK_STATE(L, FALSE);

  /* Validate player context */
  struct player *pplayer = client_player();
  LUASCRIPT_CHECK(L, pplayer != NULL, "no client player", FALSE);

  /* Validate direction */
  LUASCRIPT_CHECK_ARG(L, dir8 >= 0 && dir8 < DIR8_MAGIC_MAX, 3,
                      "invalid dir8 (expected 0..7)", FALSE);

  struct unit *punit = player_unit_by_number(pplayer, unit_id);
  LUASCRIPT_CHECK_ARG(L, punit != NULL, 2, "unknown unit id", FALSE);

  request_move_unit_direction(punit, (enum direction8)dir8);
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
  This mirrors selecting the "Attack" option in the action dialog.
*****************************************************************************/
bool api_client_attack_tile(lua_State *L, int unit_id, int tile_index)
{
  LUASCRIPT_CHECK_STATE(L, FALSE);

  /* Validate player context */
  struct player *pplayer = client_player();
  LUASCRIPT_CHECK(L, pplayer != NULL, "no client player", FALSE);

  /* Validate unit belongs to client player */
  struct unit *punit = player_unit_by_number(pplayer, unit_id);
  LUASCRIPT_CHECK_ARG(L, punit != NULL, 2, "unknown unit id", FALSE);

  /* Validate tile index */
  struct tile *ptile = index_to_tile(&(wld.map), tile_index);
  LUASCRIPT_CHECK_ARG(L, ptile != NULL, 3, "invalid target tile index", FALSE);

  /* Send the attack request (target is a tile for ACTION_ATTACK). */
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
  Converts GUI dir to map dir based on current tileset orientation.
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
  nat_x, nat_y are native tile coordinates (not screen pixels).
*****************************************************************************/
bool api_client_attack_native(lua_State *L, int unit_id, int nat_x, int nat_y)
{
  LUASCRIPT_CHECK_STATE(L, FALSE);

  struct player *pplayer = client_player();
  LUASCRIPT_CHECK(L, pplayer != NULL, "no client player", FALSE);

  struct unit *punit = player_unit_by_number(pplayer, unit_id);
  LUASCRIPT_CHECK_ARG(L, punit != NULL, 2, "unknown unit id", FALSE);

  struct tile *ptile = native_pos_to_tile(&(wld.map), nat_x, nat_y);
  LUASCRIPT_CHECK_ARG(L, ptile != NULL, 3, "invalid native coordinates", FALSE);

  request_do_action(ACTION_ATTACK, unit_id, tile_index(ptile), 0, "");
  return TRUE;
}
