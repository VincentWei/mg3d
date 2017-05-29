/*
** $Id: czscan8.c,v 1.3 2006-01-14 05:44:14 xwyan Exp $
**
** Port to MiniGUI by Feynman from allegro.
**
** Copyright (C) 2006 Feynman Software.
 *
 * Z-buffered polygon filler helpers (gouraud shading, tmapping, etc).
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
#ifdef MINIGUI_COLOR8

#include "mg3dintern.h"
#include "cdefs8.h"

#define FUNC_POLY_ZBUF_FLAT			_poly_zbuf_flat8
#define FUNC_POLY_ZBUF_GCOL			_poly_zbuf_gcol8
#define FUNC_POLY_ZBUF_GRGB			_poly_zbuf_grgb8
#define FUNC_POLY_ZBUF_ATEX			_poly_zbuf_atex8
#define FUNC_POLY_ZBUF_ATEX_MASK		_poly_zbuf_atex_mask8
#define FUNC_POLY_ZBUF_ATEX_LIT			_poly_zbuf_atex_lit8
#define FUNC_POLY_ZBUF_ATEX_MASK_LIT		_poly_zbuf_atex_mask_lit8
#define FUNC_POLY_ZBUF_PTEX			_poly_zbuf_ptex8
#define FUNC_POLY_ZBUF_PTEX_MASK		_poly_zbuf_ptex_mask8
#define FUNC_POLY_ZBUF_PTEX_LIT			_poly_zbuf_ptex_lit8
#define FUNC_POLY_ZBUF_PTEX_MASK_LIT		_poly_zbuf_ptex_mask_lit8
#define FUNC_POLY_ZBUF_ATEX_TRANS		_poly_zbuf_atex_trans8
#define FUNC_POLY_ZBUF_ATEX_MASK_TRANS		_poly_zbuf_atex_mask_trans8
#define FUNC_POLY_ZBUF_PTEX_TRANS		_poly_zbuf_ptex_trans8
#define FUNC_POLY_ZBUF_PTEX_MASK_TRANS		_poly_zbuf_ptex_mask_trans8

#define _bma_zbuf_gcol

#include "czscan.h"

#undef _bma_zbuf_gcol

#endif
