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

#ifdef ENABLE_LUAREMOTE
void luaconsole_remote_start(guint16 port);
void luaconsole_remote_stop(void);
#endif

#endif  /* FC__LUACONSOLE_H */
