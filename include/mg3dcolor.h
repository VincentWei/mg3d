/**
 * \file mg3dcolor.h
 * \author Feynman Software
 * \date 2006/01/06
 * 
 * This file includes mg3d color manipulation routines.
 *
 \verbatim
    Copyright (C) 2006 ~ 2008 Feynman Software.

    This file is part of mg3d which is one of components of MiniGUI.
\endverbatim
*/

/*
** $Id: mg3dcolor.h,v 1.9 2006-01-14 05:07:51 xwyan Exp $
**
** mg3dcolor.h: mg3d  color manipulation routines.
**
** Copyright (C) 2006 ~ 2008 Feynman Software, all rights reserved.
**
** Use of this source package is subject to specific license terms
** from Beijing Feynman Software Technology Co., Ltd.
**
** URL: http://www.minigui.com
**
** Author: Feynman Software
**
*/


#ifndef __mg3D_COLOR_H__
#define __mg3D_COLOR_H__

#ifdef __cplusplus
   extern "C" {
#endif

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/fixedmath.h>

#include "mg3dbase.h"

/**
 * \defgroup color_fns color manipulation routines
 * @{
 */

#define PAL_SIZE           256
#define MASK_COLOR_8       0
#define MASK_COLOR_15      0x7C1F
#define MASK_COLOR_16      0xF81F
#define MASK_COLOR_24      0xFF00FF
#define MASK_COLOR_32      0xFF00FF

typedef GAL_Color PALETTE[PAL_SIZE];

extern PALETTE black_palette;
extern PALETTE desktop_palette;
extern PALETTE default_palette;

typedef struct {
   unsigned char data[32][32][32];
} MG3D_RGB_MAP;

typedef struct {
   unsigned char data[PAL_SIZE][PAL_SIZE];
} MG3D_COLOR_MAP;

extern MG3D_RGB_MAP *rgb_map;
extern MG3D_COLOR_MAP *color_map;

extern PALETTE _current_palette;

extern int _rgb_r_shift_15;
extern int _rgb_g_shift_15;
extern int _rgb_b_shift_15;
extern int _rgb_r_shift_16;
extern int _rgb_g_shift_16;
extern int _rgb_b_shift_16;
extern int _rgb_r_shift_24;
extern int _rgb_g_shift_24;
extern int _rgb_b_shift_24;
extern int _rgb_r_shift_32;
extern int _rgb_g_shift_32;
extern int _rgb_b_shift_32;
extern int _rgb_a_shift_32;

extern int _rgb_scale_5[];
extern int _rgb_scale_6[];
extern int * palette_color;


/**
 * \fn typedef unsigned long (*BLENDER_FUNC)(unsigned long x, unsigned long y, unsigned long n)
 * \brief Set blender function. 
 */
typedef unsigned long (*BLENDER_FUNC)(unsigned long x, unsigned long y, unsigned long n);


/**
 * \fn void mg3dCreateRgbTable(MG3D_RGB_MAP *table, const PALETTE pal, void (*callback)(int pos))
 * \brief Fills an MG3D_RGB_MAP lookup table with conversion data for the specified
 *  palette.
 *
 * \param table The MG3D_RGB_MAP lookup table.
 * \param pal The specified palette. 
 * \param callback The user-defined callback function.
 *
 */
void mg3dCreateRgbTable(MG3D_RGB_MAP *table, const PALETTE pal, void (*callback)(int pos));

/**
 * \fn void mg3dCreateLightTable(MG3D_COLOR_MAP *table, const PALETTE pal, int r, int g, int b, void (*callback)(int pos))
 * \brief  Constructs a lighting color table for the specified palette.
 *
 * \param table The MG3D_COLOR_MAP table. 
 * \param pal The specified palette.
 * \param r The red component of color. 
 * \param g The green component of color. 
 * \param b The blue component of color. 
 * \param callback The user-defined callback function.
 *
 */
void mg3dCreateLightTable(MG3D_COLOR_MAP *table, const PALETTE pal, int r, int g, int b, void (*callback)(int pos));

/**
 * \fn void mg3dCreateTransTable(MG3D_COLOR_MAP *table, const PALETTE pal, int r, int g, int b, void (*callback)(int pos)) 
 * \brief  Constructs a translucency color table for the specified palette.
 *
 * \param table The MG3D_COLOR_MAP table. 
 * \param pal The specified palette. 
 * \param r The red component of the solidity of each color. 
 * \param g The green component of  the solidity of each color. 
 * \param b The blue component of  the solidity of each color. 
 * \param callback The user-defined callback function.
 *
 */
void mg3dCreateTransTable(MG3D_COLOR_MAP *table, const PALETTE pal, int r, int g, int b, void (*callback)(int pos));

/**
 * \fn void mg3dCreateColorTable(MG3D_COLOR_MAP *table, const PALETTE pal, void (*blend)(const PALETTE pal, int x, int y, GAL_Color *rgb), void (*callback)(int pos))
 * \brief Creates a color mapping table, using a user-supplied callback to blend
 *  each pair of colors. Your blend routine will be passed a pointer to the
 *  palette and the two colors to be blended (x is the source color, y is
 *  the destination), and should return the desired output RGB for this
 *  combination. If the callback function is not NULL, it will be called 
 *  256 times during the calculation, allowing you to display a progress 
 *  indicator.
 *
 * \param table The MG3D_COLOR_MAP table. 
 * \param pal The specified palette. 
 * \param blend  The blend routine.
 * \param callback The user-defined callback function.
 *
 */
void mg3dCreateColorTable(MG3D_COLOR_MAP *table, const PALETTE pal, void (*blend)(const PALETTE pal, int x, int y, GAL_Color *rgb), void (*callback)(int pos));

/**
 * \fn void mg3dSetalphaBlender(void)
 * \brief  Combines a 32 bit RGBA sprite with a 15 bit RGB destination, optimised
 *  for when both pixels are in an RGB layout.
 *
 */
void mg3dSetalphaBlender(void);

/**
 * \fn void mg3dSettransBlender(int r, int g, int b, int a)
 * \brief Set trans blender function. 
 *
 * \param r The red component of color. 
 * \param g The green component of color. 
 * \param b The blue component of color. 
 * \param a The alpha component of color. 
 *
 */
void mg3dSettransBlender(int r, int g, int b, int a);

/**
 * \fn void mg3dSetcolorBlender(int r, int g, int b, int a)
 * \brief Set color blender function. 
 *
 * \param r The red component of color. 
 * \param g The green component of color. 
 * \param b The blue component of color. 
 * \param a The alpha component of color. 
 *
 */
void mg3dSetcolorBlender(int r, int g, int b, int a);

/**
 * \fn void mg3dSetscreenBlender(int r, int g, int b, int a)
 * \brief Set screen blender function.
 *
 * \param r The red component of color. 
 * \param g The green component of color. 
 * \param b The blue component of color. 
 * \param a The alpha component of color. 
 *
 */
void mg3dSetscreenBlender(int r, int g, int b, int a);


#ifdef __cplusplus
   }
#endif

    /** @} */
#endif          /* ifndef __mg3D_COLOR_H__ */


