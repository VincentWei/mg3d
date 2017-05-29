/*
** $Id: cdefs8.h,v 1.3 2006-01-14 05:44:14 xwyan Exp $
**
** Port to MiniGUI by Feynman from allegro.
**
** Copyright (C) 2006 Feynman Software.
 *
 * Defines for 256 color graphics primitives.
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

#ifndef __bma_cdefs8_h
#define __bma_cdefs8_h

#define PP_DEPTH               8

#define PIXEL_PTR              unsigned char*
#define OFFSET_PIXEL_PTR(p,x)  ((PIXEL_PTR) (p) + (x))
#define INC_PIXEL_PTR(p)       ((p)++)
#define DEC_PIXEL_PTR(p)       ((p)--)

#define PUT_PIXEL(p,c)         bmp_write8((unsigned long) (p), (c))
#define PUT_MEMORY_PIXEL(p,c)  (*(p) = (c))
#define PUT_RGB(p,mem_dc,r,g,b)       bmp_write8((unsigned long) (p), RGB2Pixel((mem_dc), (r), (g), (b)))
#define GET_PIXEL(p)           bmp_read8((unsigned long) (p))
#define GET_MEMORY_PIXEL(p)    (*(p))

#define IS_MASK(c)             ((c) == 0)
#define IS_SPRITE_MASK(b,c)    ((c) == 0)

/* Blender for putpixel (DRAW_MODE_TRANS).  */
#define PP_BLENDER             unsigned char*
#define MAKE_PP_BLENDER(c)     (color_map->data[(c) & 0xFF])
#define PP_BLEND(b,o,n)        ((b)[(o) & 0xFF])

/* Blender for draw_trans_*_sprite.  */
#define DTS_BLENDER            MG3D_COLOR_MAP*
#define MAKE_DTS_BLENDER()     color_map
#define DTS_BLEND(b,o,n)       ((b)->data[(n)& 0xFF][(o) & 0xFF])

/* Blender for draw_lit_*_sprite.  */
#define DLS_BLENDER            unsigned char*
#define MAKE_DLS_BLENDER(a)    (color_map->data[(a) & 0xFF])
#define DLS_BLEND(b,a,c)       ((b)[(c) & 0xFF])

/* Blender for poly_scanline_*_lit.  */
#define PS_BLENDER             MG3D_COLOR_MAP*
#define MAKE_PS_BLENDER()      color_map
#define PS_BLEND(b,o,c)        ((b)->data[(o) & 0xFF][(c) & 0xFF])
#define PS_ALPHA_BLEND(b,o,c)  ((b)->data[(o) & 0xFF][(c) & 0xFF])

#define PATTERN_LINE(y)        _drawing_pattern->line[((y) - _drawing_y_anchor) & _drawing_y_mask]
#define GET_PATTERN_PIXEL(x,y) GET_MEMORY_PIXEL(OFFSET_PIXEL_PTR(PATTERN_LINE(y), \
                                                ((x) - _drawing_x_anchor) & _drawing_x_mask))

#define RLE_PTR                signed char*
#define RLE_IS_EOL(c)          ((c) == 0)

#define FUNC_LINEAR_CLEAR_TO_COLOR          _linear_clear_to_color8
#define FUNC_LINEAR_BLIT                    _linear_blit8
#define FUNC_LINEAR_BLIT_BACKWARD           _linear_blit_backward8
#define FUNC_LINEAR_MASKED_BLIT             _linear_masked_blit8

#define FUNC_LINEAR_PUTPIXEL                _linear_putpixel8
#define FUNC_LINEAR_GETPIXEL                _linear_getpixel8
#define FUNC_LINEAR_HLINE                   _linear_hline8
#define FUNC_LINEAR_VLINE                   _linear_vline8

#define FUNC_LINEAR_DRAW_SPRITE             _linear_draw_sprite8
#define FUNC_LINEAR_DRAW_256_SPRITE         _linear_draw_256_sprite8
#define FUNC_LINEAR_DRAW_SPRITE_V_FLIP      _linear_draw_sprite_v_flip8
#define FUNC_LINEAR_DRAW_SPRITE_H_FLIP      _linear_draw_sprite_h_flip8
#define FUNC_LINEAR_DRAW_SPRITE_VH_FLIP     _linear_draw_sprite_vh_flip8
#define FUNC_LINEAR_DRAW_TRANS_SPRITE       _linear_draw_trans_sprite8
#define FUNC_LINEAR_DRAW_TRANS_RGBA_SPRITE  _linear_draw_trans_rgba_sprite8
#define FUNC_LINEAR_DRAW_LIT_SPRITE         _linear_draw_lit_sprite8
#define FUNC_LINEAR_DRAW_CHARACTER          _linear_draw_character8
#define FUNC_LINEAR_DRAW_RLE_SPRITE         _linear_draw_rle_sprite8
#define FUNC_LINEAR_DRAW_TRANS_RLE_SPRITE   _linear_draw_trans_rle_sprite8
#define FUNC_LINEAR_DRAW_TRANS_RGBA_RLE_SPRITE _linear_draw_trans_rgba_rle_sprite8
#define FUNC_LINEAR_DRAW_LIT_RLE_SPRITE     _linear_draw_lit_rle_sprite8

#define FUNC_LINEAR_DRAW_SPRITE_END         _linear_draw_sprite8_end
#define FUNC_LINEAR_BLIT_END                _linear_blit8_end

#define FUNC_POLY_SCANLINE_GCOL             _poly_scanline_gcol8
#define FUNC_POLY_SCANLINE_GRGB             _poly_scanline_grgb8
#define FUNC_POLY_SCANLINE_ATEX             _poly_scanline_atex8
#define FUNC_POLY_SCANLINE_ATEX_MASK        _poly_scanline_atex_mask8
#define FUNC_POLY_SCANLINE_ATEX_LIT         _poly_scanline_atex_lit8
#define FUNC_POLY_SCANLINE_ATEX_MASK_LIT    _poly_scanline_atex_mask_lit8
#define FUNC_POLY_SCANLINE_PTEX             _poly_scanline_ptex8
#define FUNC_POLY_SCANLINE_PTEX_MASK        _poly_scanline_ptex_mask8
#define FUNC_POLY_SCANLINE_PTEX_LIT         _poly_scanline_ptex_lit8
#define FUNC_POLY_SCANLINE_PTEX_MASK_LIT    _poly_scanline_ptex_mask_lit8
#define FUNC_POLY_SCANLINE_ATEX_TRANS       _poly_scanline_atex_trans8
#define FUNC_POLY_SCANLINE_ATEX_MASK_TRANS  _poly_scanline_atex_mask_trans8
#define FUNC_POLY_SCANLINE_PTEX_TRANS       _poly_scanline_ptex_trans8
#define FUNC_POLY_SCANLINE_PTEX_MASK_TRANS  _poly_scanline_ptex_mask_trans8

#endif /* !__bma_cdefs8_h */

