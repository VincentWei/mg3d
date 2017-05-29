/*
** $Id: cscan24.c,v 1.4 2006-01-14 05:44:14 xwyan Exp $
**
** Port to MiniGUI by Feynman from allegro.
**
** Copyright (C) 2006 Feynman Software.
 *
 * 24 bit color polygon scanline filler helpers (gouraud shading, tmapping, etc).
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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/fixedmath.h>
#include "mg3drw.h"
#ifdef MINIGUI_COLOR24

#undef _bma_scan_gcol

#include "mg3dintern.h"
#include "cdefs24.h"
int bmp_read24(unsigned long addr)
{
      unsigned char *p = (unsigned char *)addr;
      int c;

      c = READ3BYTES(p);

      return c;
}

void bmp_write24(unsigned long addr, unsigned int c)
{
      unsigned char *p = (unsigned char *)addr;

      WRITE3BYTES(p, c);
}

#include "cscan.h"
#endif

