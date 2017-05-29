/*
** $Id: czscan15.c,v 1.3 2006-01-14 05:44:14 xwyan Exp $
**
** Port to MiniGUI by Feynman from allegro.
**
** Copyright (C) 2006 Feynman Software.
 *
 * 15 bit color polygon scanline filler helpers (gouraud shading, tmapping, etc).
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
#include "mg3drw.h"
#ifdef MINIGUI_COLOR16

#include "mg3dintern.h"
#include "cdefs15.h"

#define FUNC_POLY_ZBUF_FLAT			_poly_zbuf_flat15
#define FUNC_POLY_ZBUF_GRGB			_poly_zbuf_grgb15
#define FUNC_POLY_ZBUF_ATEX			_poly_zbuf_atex15
#define FUNC_POLY_ZBUF_ATEX_MASK		_poly_zbuf_atex_mask15
#define FUNC_POLY_ZBUF_ATEX_LIT			_poly_zbuf_atex_lit15
#define FUNC_POLY_ZBUF_ATEX_MASK_LIT		_poly_zbuf_atex_mask_lit15
#define FUNC_POLY_ZBUF_PTEX			_poly_zbuf_ptex15
#define FUNC_POLY_ZBUF_PTEX_MASK		_poly_zbuf_ptex_mask15
#define FUNC_POLY_ZBUF_PTEX_LIT			_poly_zbuf_ptex_lit15
#define FUNC_POLY_ZBUF_PTEX_MASK_LIT		_poly_zbuf_ptex_mask_lit15
#define FUNC_POLY_ZBUF_ATEX_TRANS		_poly_zbuf_atex_trans15
#define FUNC_POLY_ZBUF_ATEX_MASK_TRANS		_poly_zbuf_atex_mask_trans15
#define FUNC_POLY_ZBUF_PTEX_TRANS		_poly_zbuf_ptex_trans15
#define FUNC_POLY_ZBUF_PTEX_MASK_TRANS		_poly_zbuf_ptex_mask_trans15

#undef _bma_zbuf_gcol

#include "czscan.h"

#endif
