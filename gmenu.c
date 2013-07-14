/**
 * gmenu.c -- goaccess menus
 * Copyright (C) 2009-2013 by Gerardo Orellana <goaccess@prosoftcorp.com>
 * GoAccess - An Ncurses apache weblog analyzer & interactive viewer
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * A copy of the GNU General Public License is attached to this
 * source distribution for its full text.
 *
 * Visit http://goaccess.prosoftcorp.com for new releases.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gmenu.h"

#include "error.h"
#include "xmalloc.h"
#include "settings.h"
#include "util.h"
#include "ui.h"

GMenu *
new_gmenu (WINDOW * parent, int h, int w, int y, int x)
{
   GMenu *menu = xmalloc (sizeof (GMenu));

   memset (menu, 0, sizeof *menu);
   menu->count = 0;
   menu->idx = 0;
   menu->multiple = 0;
   menu->selectable = 0;
   menu->start = 0;
   menu->status = 0;

   menu->h = h;
   menu->w = w;
   menu->x = x;
   menu->y = y;
   menu->win = derwin (parent, menu->h, menu->w, menu->y, menu->x);

   return menu;
}

int
post_gmenu (GMenu * menu)
{
   if (menu == NULL)
      return 1;

   int i = 0, j = 0, k = 0, start, end, height, total;
   werase (menu->win);

   height = menu->h;
   start = menu->start;
   total = menu->size;
   end = height < total ? start + height : total;
   for (i = start; i < end; i++, j++) {
      k = i == menu->idx ? 1 : 0;
      int checked = menu->items[i].checked ? 1 : 0;
      draw_menu_item (menu, menu->items[i].name, 0, j, menu->w, k, checked);
   }
   wrefresh (menu->win);
   return 0;
}

void
gmenu_driver (GMenu * menu, int c)
{
   int i;
   switch (c) {
    case REQ_DOWN:
       if (menu->idx >= menu->size - 1)
          break;
       ++menu->idx;
       if (menu->idx >= menu->h && menu->idx >= menu->start + menu->h)
          menu->start++;
       post_gmenu (menu);
       break;
    case REQ_UP:
       if (menu->idx <= 0)
          break;
       --menu->idx;
       if (menu->idx < menu->start)
          --menu->start;
       post_gmenu (menu);
       break;
    case REQ_SEL:
       if (!menu->multiple) {
          for (i = 0; i < menu->size; i++)
             menu->items[i].checked = 0;
       }
       if (menu->items[menu->idx].checked)
          menu->items[menu->idx].checked = 0;
       else
          menu->items[menu->idx].checked = 1;
       post_gmenu (menu);
       break;
   }
}

void
draw_menu_item (GMenu * menu, char *s, int x, int y, int w, int color,
                int checked)
{
   if (conf.color_scheme == MONOCHROME) {
      init_pair (1, COLOR_BLACK, COLOR_WHITE);
      init_pair (2, COLOR_WHITE, -1);
   } else {
      init_pair (1, COLOR_BLACK, COLOR_GREEN);
      init_pair (2, COLOR_BLACK, COLOR_CYAN);
   }
   wattron (menu->win, COLOR_PAIR (color));
   mvwhline (menu->win, y, x, ' ', w);
   if (menu->selectable) {
      char check = checked ? 'x' : ' ';
      mvwprintw (menu->win, y, x, "[%c] %s", check, s);
   } else
      mvwprintw (menu->win, y, x, "%s", s);
   wattroff (menu->win, COLOR_PAIR (color));
}