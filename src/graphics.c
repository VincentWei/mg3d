/*
** $Id: graphics.c,v 1.4 2006-01-14 05:26:07 xwyan Exp $
**
** Port to MiniGUI by Feynman from allegro.
**
** Copyright (C) 2006 Feynman Software.
 *
 * Graphics mode set and bitmap creation routines.
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

#include <string.h>

#include "mg3dcolor.h"
#include "mg3dintern.h"

MG3D_RGB_MAP *rgb_map = NULL;               /* RGB -> palette entry conversion */

MG3D_COLOR_MAP *color_map = NULL;           /* translucency/lighting table */

int _color_depth = 8;                  /* how many bits per pixel? */

int _palette_color8[256];               /* palette -> pixel mapping */
int _palette_color15[256];
int _palette_color16[256];
int _palette_color24[256];
int _palette_color32[256];

int *palette_color = _palette_color8; 

BLENDER_FUNC _blender_func15 = NULL;   /* truecolor pixel blender routines */
BLENDER_FUNC _blender_func16 = NULL;
BLENDER_FUNC _blender_func24 = NULL;
BLENDER_FUNC _blender_func32 = NULL;

BLENDER_FUNC _blender_func15x = NULL;
BLENDER_FUNC _blender_func16x = NULL;
BLENDER_FUNC _blender_func24x = NULL;

int _blender_col_15 = 0;               /* for truecolor lit sprites */
int _blender_col_16 = 0;
int _blender_col_24 = 0;
int _blender_col_32 = 0;

int _blender_alpha = 0;                /* for truecolor translucent drawing */

int _rgb_r_shift_15 = DEFAULT_RGB_R_SHIFT_15;     /* truecolor pixel format */
int _rgb_g_shift_15 = DEFAULT_RGB_G_SHIFT_15;
int _rgb_b_shift_15 = DEFAULT_RGB_B_SHIFT_15;
int _rgb_r_shift_16 = DEFAULT_RGB_R_SHIFT_16;
int _rgb_g_shift_16 = DEFAULT_RGB_G_SHIFT_16;
int _rgb_b_shift_16 = DEFAULT_RGB_B_SHIFT_16;
int _rgb_r_shift_24 = DEFAULT_RGB_R_SHIFT_24;
int _rgb_g_shift_24 = DEFAULT_RGB_G_SHIFT_24;
int _rgb_b_shift_24 = DEFAULT_RGB_B_SHIFT_24;
int _rgb_r_shift_32 = DEFAULT_RGB_R_SHIFT_32;
int _rgb_g_shift_32 = DEFAULT_RGB_G_SHIFT_32;
int _rgb_b_shift_32 = DEFAULT_RGB_B_SHIFT_32;
int _rgb_a_shift_32 = DEFAULT_RGB_A_SHIFT_32;

/* lookup table for scaling 5 bit colors up to 8 bits */
int _rgb_scale_5[32] =
{
   0,   8,   16,  24,  32,  41,  49,  57,
   65,  74,  82,  90,  98,  106, 115, 123,
   131, 139, 148, 156, 164, 172, 180, 189,
   197, 205, 213, 222, 230, 238, 246, 255
};

/* lookup table for scaling 6 bit colors up to 8 bits */
int _rgb_scale_6[64] =
{
   0,   4,   8,   12,  16,  20,  24,  28,
   32,  36,  40,  44,  48,  52,  56,  60,
   64,  68,  72,  76,  80,  85,  89,  93,
   97,  101, 105, 109, 113, 117, 121, 125,
   129, 133, 137, 141, 145, 149, 153, 157,
   161, 165, 170, 174, 178, 182, 186, 190,
   194, 198, 202, 206, 210, 214, 218, 222,
   226, 230, 234, 238, 242, 246, 250, 255
};
