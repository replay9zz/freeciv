/*****************************************************************************
 Freeciv - Copyright (C) 1996 - A Kjeldberg, L Gregersen, P Unold
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
*****************************************************************************/
#ifndef FC__LUACONSOLE_H
#define FC__LUACONSOLE_H


/* client */
#include "luaconsole_g.h"

void luaconsole_dialog_init(void);
void luaconsole_dialog_done(void);

void luaconsole_dialog_popdown(void);

#endif  /* FC__LUACONSOLE_H */

/* luaconsole.h の末尾あたりに追加 */
#ifdef ENABLE_LUAREMOTE
/* remote Lua listener の開始/停止（GTK起動時に start、終了時に stop 推奨） */
void luaconsole_remote_start(guint16 port);   /* 例: 4444 */
void luaconsole_remote_stop(void);
#endif
