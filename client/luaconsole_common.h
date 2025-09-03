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

#ifndef FC__LUACONSOLE_COMMON_H
#define FC__LUACONSOLE_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* utility */
#include "support.h"            /* bool type */

struct ft_color;
struct text_tag_list;

void luaconsole_append(const struct ft_color color,
                       const char *featured_text);
void luaconsole_vprintf(const struct ft_color color,
                        const char *format, va_list args);
void luaconsole_printf(const struct ft_color color,
                       const char *format, ...)
                       fc__attribute((__format__ (__printf__, 2, 3)));
void luaconsole_event(const char *plain_text,
                      const struct text_tag_list *tags);
void luaconsole_welcome_message(void);

/* === add: mirror console output to nc when ENABLE_LUAREMOTE === */
/* luaconsole_common.h の関数プロトタイプ群の直後あたりに追加 */
#ifdef ENABLE_LUAREMOTE
typedef struct _GOutputStream GOutputStream;  /* 前方宣言 */
extern GOutputStream *luaremote_current_ostream;
void luaremote_begin_capture(GOutputStream *ostream);
void luaremote_end_capture(void);
void luaremote_mirror_console_line(const char *text);
#endif

/* === end add === */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FC__LUACONSOLE_COMMON_H */
