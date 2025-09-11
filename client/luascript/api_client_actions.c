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
#include "control.h"      /* request_move_unit_direction() */

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

