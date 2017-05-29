/*
** $Id: gfx.c,v 1.6 2006-01-14 10:10:40 xwyan Exp $
**
** Port to MiniGUI by Feynman from allegro.
**
** Copyright (C) 2006 Feynman Software.
 *
 * Graphics routines: palette fading, circles, etc.
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

#include <math.h>
#include "mg3dintern.h"
#include "mg3dgfx.h"

int makecol15 (int r, int g, int b)
{
   return (((r >> 3) << _rgb_r_shift_15) |
           ((g >> 3) << _rgb_g_shift_15) |
           ((b >> 3) << _rgb_b_shift_15));
}

int makecol16 (int r, int g, int b)
{
   return (((r >> 3) << _rgb_r_shift_16) |
           ((g >> 2) << _rgb_g_shift_16) |
           ((b >> 3) << _rgb_b_shift_16));
}

int makecol24 (int r, int g, int b)
{
   return ((r << _rgb_r_shift_24) |
           (g << _rgb_g_shift_24) |
           (b << _rgb_b_shift_24));
}

int makecol32 (int r, int g, int b)
{
   return ((r << _rgb_r_shift_32) |
           (g << _rgb_g_shift_32) |
           (b << _rgb_b_shift_32));
}

int makeacol32 (int r, int g, int b, int a)
{
   return ((r << _rgb_r_shift_32) |
           (g << _rgb_g_shift_32) |
           (b << _rgb_b_shift_32) |
           (a << _rgb_a_shift_32));
}

/* info about the current graphics drawing mode */
int _drawing_mode = DRAW_MODE_SOLID;
HDC _drawing_pattern = 0;
int _drawing_x_anchor = 0;
int _drawing_y_anchor = 0;
unsigned int _drawing_x_mask = 0;
unsigned int _drawing_y_mask = 0;

void drawing_mode(int mode, HDC pattern, int x_anchor, int y_anchor)
{
    _drawing_mode = mode;
    _drawing_pattern = pattern;
    _drawing_x_anchor = x_anchor;
    _drawing_y_anchor = y_anchor;
    
    unsigned long w, h;

    if (pattern) 
    {
        h = GetGDCapability(pattern, GDCAP_MAXY);
        h++;
        w = GetGDCapability(pattern, GDCAP_MAXX);
        w++;
          
        _drawing_x_mask = 1;
        while (_drawing_x_mask < w)
            _drawing_x_mask <<= 1;        /* find power of two greater than w */

        if (_drawing_x_mask > w) {
            //ASSERT(FALSE);
            _drawing_x_mask >>= 1;        /* round down if required */
        }

        _drawing_x_mask--;               /* convert to AND mask */

        _drawing_y_mask = 1;
        while (_drawing_y_mask < h)
            _drawing_y_mask <<= 1;        /* find power of two greater than h */

        if (_drawing_y_mask > h) {
            //ASSERT(FALSE);
            _drawing_y_mask >>= 1;        /* round down if required */
        }

        _drawing_y_mask--;               /* convert to AND mask */
    }
    else
        _drawing_x_mask = _drawing_y_mask = 0;

    //if ((gfx_driver) && (gfx_driver->drawing_mode) && (!_dispsw_status))
    //  gfx_driver->drawing_mode();
}

void solid_mode(void)
{
    drawing_mode(DRAW_MODE_SOLID, 0, 0, 0);
}

/* set_blender_mode:
 *  Specifies a custom set of blender functions for interpolating between
 *  truecolor pixels. The 24 bit blender is shared between the 24 and 32 bit
 *  modes. Pass a NULL table for unused color depths (you must not draw
 *  translucent graphics in modes without a handler, though!). Your blender
 *  will be passed two 32 bit colors in the appropriate format (5.5.5, 5.6.5,
 *  or 8.8.8), and an alpha value, should return the result of combining them.
 *  In translucent drawing modes, the two colors are taken from the source
 *  and destination images and the alpha is specified by this function. In
 *  lit modes, the alpha is specified when you call the drawing routine, and
 *  the interpolation is between the source color and the RGB values you pass
 *  to this function.
 */
void set_blender_mode(BLENDER_FUNC b15, BLENDER_FUNC b16, BLENDER_FUNC b24, int r, int g, int b, int a)
{
   _blender_func15 = b15;
   _blender_func16 = b16;
   _blender_func24 = b24;
   _blender_func32 = b24;


   _blender_func16x = _blender_black;
   _blender_func24x = _blender_black;

   _blender_col_15 = makecol15(r, g, b);
   _blender_col_16 = makecol16(r, g, b);
   _blender_col_24 = makecol24(r, g, b);
   _blender_col_32 = makecol32(r, g, b);

   _blender_alpha = a;
}

/* set_blender_mode_ex
 *  Specifies a custom set of blender functions for interpolating between
 *  truecolor pixels, providing a more complete set of routines, which
 *  differentiate between 24 and 32 bit modes, and have special routines
 *  for blending 32 bit RGBA pixels onto a destination of any format.
 */
void set_blender_mode_ex(BLENDER_FUNC b15, BLENDER_FUNC b16, BLENDER_FUNC b24, BLENDER_FUNC b32, BLENDER_FUNC b15x, BLENDER_FUNC b16x, BLENDER_FUNC b24x, int r, int g, int b, int a)
{
   _blender_func15 = b15;
   _blender_func16 = b16;
   _blender_func24 = b24;
   _blender_func32 = b32;

   _blender_func15x = b15x;
   _blender_func16x = b16x;
   _blender_func24x = b24x;

   _blender_col_15 = makecol15(r, g, b);
   _blender_col_16 = makecol16(r, g, b);
   _blender_col_24 = makecol24(r, g, b);
   _blender_col_32 = makecol32(r, g, b);

   _blender_alpha = a;
}

/* ======================================================================================= */
unsigned long bmp_write_line(HDC mem_dc, int line)
{ 
    int width, height, pitch;
    RECT rc = {0, line, 10, line + 1};
    Uint8 *frame_buffer;

    frame_buffer = LockDC(mem_dc, &rc, &width, &height, &pitch);
    UnlockDC(mem_dc);
    return (unsigned long)(frame_buffer);
}

unsigned long bmp_read_line(HDC mem_dc, int line)
{
    int width, height, pitch;
    RECT rc = {0, line, 1, line + 1};
    Uint8 *frame_buffer;

    frame_buffer = LockDC(mem_dc, &rc, &width, &height, &pitch);
    UnlockDC(mem_dc);
    return (unsigned long)(frame_buffer);
}
