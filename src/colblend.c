/*
** $Id: colblend.c,v 1.4 2006-01-14 05:26:07 xwyan Exp $
**
** Port to MiniGUI by Feynman from allegro.
**
** Copyright (C) 2006 Feynman Software.
 *
 * Interpolation routines for hicolor and truecolor pixels.
 *
 * By Shawn Hargreaves.
 *
 * Allegro is a gift-ware.
**/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "mg3dintern.h"

#define BLEND(bpp, r, g, b)   _blender_trans##bpp(makecol##bpp(r, g, b), y, n)

#define T(x, y, n)            (((y) - (x)) * (n) / 255 + (x))

/* _blender_black:
 *  Fallback routine for when we don't have anything better to do.
 */
unsigned long _blender_black(unsigned long x, unsigned long y, unsigned long n)
{
   return 0;
}

#if (defined MINIGUI_COLOR24) || (defined MINIGUI_COLOR32)

/* _blender_trans24:
 *  24 bit trans blender function.
 */
unsigned long _blender_trans24(unsigned long x, unsigned long y, unsigned long n)
{
   unsigned long res, g;

   if (n)
      n++;

   res = ((x & 0xFF00FF) - (y & 0xFF00FF)) * n / 256 + y;
   y &= 0xFF00;
   x &= 0xFF00;
   g = (x - y) * n / 256 + y;

   res &= 0xFF00FF;
   g &= 0xFF00;

   return res | g;
}

#endif      /* end of 24/32 bit routines */

#if (defined MINIGUI_COLOR15) || (defined MINIGUI_COLOR16)

/* _blender_trans16:
 *  16 bit trans blender function.
 */
unsigned long _blender_trans16(unsigned long x, unsigned long y, unsigned long n)
{
   unsigned long result;

   if (n)
      n = (n + 1) / 8;

   x = ((x & 0xFFFF) | (x << 16)) & 0x7E0F81F;
   y = ((y & 0xFFFF) | (y << 16)) & 0x7E0F81F;

   result = ((x - y) * n / 32 + y) & 0x7E0F81F;

   return ((result & 0xFFFF) | (result >> 16));
}

/* _blender_trans15:
 *  15 bit trans blender function.
 */
unsigned long _blender_trans15(unsigned long x, unsigned long y, unsigned long n)
{
   unsigned long result;

   if (n)
      n = (n + 1) / 8;

   x = ((x & 0xFFFF) | (x << 16)) & 0x3E07C1F;
   y = ((y & 0xFFFF) | (y << 16)) & 0x3E07C1F;

   result = ((x - y) * n / 32 + y) & 0x3E07C1F;

   return ((result & 0xFFFF) | (result >> 16));
}

/* _blender_alpha15_rgb
 *  Combines a 32 bit RGBA sprite with a 15 bit RGB destination, optimised
 *  for when both pixels are in an RGB layout.
 */
unsigned long _blender_alpha15_rgb(unsigned long x, unsigned long y, unsigned long n)
{
   unsigned long result;

   n = x >> 24;

   if (n)
      n = (n + 1) / 8;

   x = ((x>>3)&0x001F) | ((x>>6)&0x03E0) | ((x>>9)&0xEC00);

   x = (x | (x << 16)) & 0x3E07C1F;
   y = ((y & 0xFFFF) | (y << 16)) & 0x3E07C1F;

   result = ((x - y) * n / 32 + y) & 0x3E07C1F;

   return ((result & 0xFFFF) | (result >> 16));
}
#endif      /* end of 15/16 bit routines */



#ifdef MINIGUI_COLOR16
   #define BF16(name)   name
#else
   #define BF16(name)   _blender_black
#endif


#if (defined MINIGUI_COLOR24) || (defined MINIGUI_COLOR32)
   #define BF24(name)   name
#else
   #define BF24(name)   _blender_black
#endif



/* these functions are all the same, so we can generate them with a macro */
#define SET_BLENDER_FUNC(name)                                 \
   void mg3dSet##name##Blender(int r, int g, int b, int a)       \
   {                                                           \
      set_blender_mode(BF16(_blender_##name##15),              \
		       BF16(_blender_##name##16),              \
		       BF24(_blender_##name##24),              \
		       r, g, b, a);                            \
   }


SET_BLENDER_FUNC(trans);
