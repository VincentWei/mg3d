/*
** $Id: cscan.h,v 1.2 2006-01-14 05:44:14 xwyan Exp $
**
** Port to MiniGUI by Feynman from allegro.
**
** Copyright (C) 2006 Feynman Software.
 *
 * Polygon scanline filler helpers (gouraud shading, tmapping, etc).
 *
 * Scanline subdivision in *_PTEX functions and transparency modes
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

#ifndef __bma_cscan_h
#define __bma_cscan_h
#ifdef MINIGUI_COLOR8
 #define _bma_scan_gcol
#else
 #undef _bma_scan_gcol
#endif
#ifdef _bma_scan_gcol

/* _poly_scanline_gcol:
 *  Fills a single-color gouraud shaded polygon scanline.
 */
void FUNC_POLY_SCANLINE_GCOL(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info)
{
   int x;
   fixed c = info->c;
   fixed dc = info->dc;
   PIXEL_PTR d = (PIXEL_PTR) addr;

   for (x = w - 1; x >= 0; INC_PIXEL_PTR(d), x--) {
      PUT_PIXEL(d, (c >> 16));
      c += dc;
   }
}
#endif

/* _bma_scan_gcol */
void FUNC_POLY_SCANLINE_GRGB(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info)
{
   int x;
   fixed r = info->r;
   fixed g = info->g;
   fixed b = info->b;
   fixed dr = info->dr;
   fixed dg = info->dg;
   fixed db = info->db;
   PIXEL_PTR d = (PIXEL_PTR) addr;

   for (x = w - 1; x >= 0; INC_PIXEL_PTR(d), x--) {
      /*PUT_RGB(d, (r >> 16), (g >> 16), (b >> 16));*/
       
      PUT_RGB(d, mem_dc, (r >> 16), (g >> 16), (b >> 16));
      r += dr;
      g += dg;
      b += db;
   }
}

/* _poly_scanline_atex:
 *  Fills an affine texture mapped polygon scanline.
 */
void FUNC_POLY_SCANLINE_ATEX(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info)
{
   int x;
   int vmask = info->vmask << info->vshift;
   int vshift = 16 - info->vshift;
   int umask = info->umask;
   fixed u = info->u;
   fixed v = info->v;
   fixed du = info->du;
   fixed dv = info->dv;
   PIXEL_PTR texture = (PIXEL_PTR) (info->texture);
   PIXEL_PTR d = (PIXEL_PTR) addr;

   for (x = w - 1; x >= 0; INC_PIXEL_PTR(d), x--) {
      PIXEL_PTR s = OFFSET_PIXEL_PTR(texture, ((v >> vshift) & vmask) + ((u >> 16) & umask));
      unsigned long color = GET_MEMORY_PIXEL(s);

      PUT_PIXEL(d, color);
      u += du;
      v += dv;
   }
}

/* _poly_scanline_atex_mask:
 *  Fills a masked affine texture mapped polygon scanline.
 */
void FUNC_POLY_SCANLINE_ATEX_MASK(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info)
{
   int x;
   int vmask = info->vmask << info->vshift;
   int vshift = 16 - info->vshift;
   int umask = info->umask;
   fixed u = info->u;
   fixed v = info->v;
   fixed du = info->du;
   fixed dv = info->dv;
   PIXEL_PTR texture = (PIXEL_PTR) (info->texture);
   PIXEL_PTR d = (PIXEL_PTR) addr;

   for (x = w - 1; x >= 0; INC_PIXEL_PTR(d), x--) {
      PIXEL_PTR s = OFFSET_PIXEL_PTR(texture, ((v >> vshift) & vmask) + ((u >> 16) & umask));
      unsigned long color = GET_MEMORY_PIXEL(s);

      if (!IS_MASK(color)) {
	 PUT_PIXEL(d, color);
      }
      u += du;
      v += dv;
   }
}

/* _poly_scanline_atex_lit:
 *  Fills a lit affine texture mapped polygon scanline.
 */
void FUNC_POLY_SCANLINE_ATEX_LIT(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info)
{
   int x;
   int vmask = info->vmask << info->vshift;
   int vshift = 16 - info->vshift;
   int umask = info->umask;
   fixed u = info->u;
   fixed v = info->v;
   fixed c = info->c;
   fixed du = info->du;
   fixed dv = info->dv;
   fixed dc = info->dc;
   PS_BLENDER blender = MAKE_PS_BLENDER();
   PIXEL_PTR texture = (PIXEL_PTR) (info->texture);
   PIXEL_PTR d = (PIXEL_PTR) addr;

   for (x = w - 1; x >= 0; INC_PIXEL_PTR(d), x--) {
      PIXEL_PTR s = OFFSET_PIXEL_PTR(texture, ((v >> vshift) & vmask) + ((u >> 16) & umask));
      unsigned long color = GET_MEMORY_PIXEL(s);
      color = PS_BLEND(blender, (c >> 16), color);

      PUT_PIXEL(d, color);
      u += du;
      v += dv;
      c += dc;
   }
}

/* _poly_scanline_atex_mask_lit:
 *  Fills a masked lit affine texture mapped polygon scanline.
 */
void FUNC_POLY_SCANLINE_ATEX_MASK_LIT(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info)
{
   int x;
   int vmask = info->vmask << info->vshift;
   int vshift = 16 - info->vshift;
   int umask = info->umask;
   fixed u = info->u;
   fixed v = info->v;
   fixed c = info->c;
   fixed du = info->du;
   fixed dv = info->dv;
   fixed dc = info->dc;
   PS_BLENDER blender = MAKE_PS_BLENDER();
   PIXEL_PTR texture = (PIXEL_PTR) (info->texture);
   PIXEL_PTR d = (PIXEL_PTR) addr;

   for (x = w - 1; x >= 0; INC_PIXEL_PTR(d), x--) {
      PIXEL_PTR s = OFFSET_PIXEL_PTR(texture, ((v >> vshift) & vmask) + ((u >> 16) & umask));
      unsigned long color = GET_MEMORY_PIXEL(s);

      if (!IS_MASK(color)) {
	 color = PS_BLEND(blender, (c >> 16), color);
	 PUT_PIXEL(d, color);
      }
      u += du;
      v += dv;
      c += dc;
   }
}

/* _poly_scanline_ptex:
 *  Fills a perspective correct texture mapped polygon scanline.
 */
void FUNC_POLY_SCANLINE_PTEX(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info)
{
   int x, i, imax = 3;
   int vmask = info->vmask << info->vshift;
   int vshift = 16 - info->vshift;
   int umask = info->umask;
   double fu = info->fu;
   double fv = info->fv;
   double fz = info->z;
   double dfu = info->dfu * 4;
   double dfv = info->dfv * 4;
   double dfz = info->dz * 4;
   double z1 = 1. / fz;
   PIXEL_PTR texture = (PIXEL_PTR) (info->texture);
   PIXEL_PTR d = (PIXEL_PTR) addr;
   long u = fu * z1;
   long v = fv * z1;

   /* update depth */
   fz += dfz;
   z1 = 1. / fz;

   for (x = w - 1; x >= 0; x -= 4) {
      long nextu, nextv, du, dv;
      PIXEL_PTR s;
      unsigned long color;

      fu += dfu;
      fv += dfv;
      fz += dfz;
      nextu = fu * z1;
      nextv = fv * z1;
      z1 = 1. / fz;
      du = (nextu - u) >> 2;
      dv = (nextv - v) >> 2;

      /* scanline subdivision */
      if (x < 3) 
         imax = x;
      for (i = imax; i >= 0; i--, INC_PIXEL_PTR(d)) {
         s = OFFSET_PIXEL_PTR(texture, ((v >> vshift) & vmask) + ((u >> 16) & umask));
         color = GET_MEMORY_PIXEL(s);

         PUT_PIXEL(d, color);
         u += du;
         v += dv;
      }
   }
}

/* _poly_scanline_ptex_mask:
 *  Fills a masked perspective correct texture mapped polygon scanline.
 */
void FUNC_POLY_SCANLINE_PTEX_MASK(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info)
{
   int x, i, imax = 3;
   int vmask = info->vmask << info->vshift;
   int vshift = 16 - info->vshift;
   int umask = info->umask;
   double fu = info->fu;
   double fv = info->fv;
   double fz = info->z;
   double dfu = info->dfu * 4;
   double dfv = info->dfv * 4;
   double dfz = info->dz * 4;
   double z1 = 1. / fz;
   PIXEL_PTR texture = (PIXEL_PTR) (info->texture);
   PIXEL_PTR d = (PIXEL_PTR) addr;
   long u = fu * z1;
   long v = fv * z1;

   /* update depth */
   fz += dfz;
   z1 = 1. / fz;

   for (x = w - 1; x >= 0; x-= 4) {
      long nextu, nextv, du, dv;
      PIXEL_PTR s;
      unsigned long color;

      fu += dfu;
      fv += dfv;
      fz += dfz;
      nextu = fu * z1;
      nextv = fv * z1;
      z1 = 1. / fz;
      du = (nextu - u) >> 2;
      dv = (nextv - v) >> 2;

      /* scanline subdivision */
      if (x < 3) 
         imax = x;
      for (i = imax; i >= 0; i--, INC_PIXEL_PTR(d)) {
         s = OFFSET_PIXEL_PTR(texture, ((v >> vshift) & vmask) + ((u >> 16) & umask));
         color = GET_MEMORY_PIXEL(s);

         if (!IS_MASK(color)) {
	    PUT_PIXEL(d, color);
         }
         u += du;
         v += dv;
      }
   }
}

/* _poly_scanline_ptex_lit:
 *  Fills a lit perspective correct texture mapped polygon scanline.
 */
void FUNC_POLY_SCANLINE_PTEX_LIT(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info)
{
   int x, i, imax = 3;
   int vmask = info->vmask << info->vshift;
   int vshift = 16 - info->vshift;
   int umask = info->umask;
   fixed c = info->c;
   fixed dc = info->dc;
   double fu = info->fu;
   double fv = info->fv;
   double fz = info->z;
   double dfu = info->dfu * 4;
   double dfv = info->dfv * 4;
   double dfz = info->dz * 4;
   double z1 = 1. / fz;
   PS_BLENDER blender = MAKE_PS_BLENDER();
   PIXEL_PTR texture = (PIXEL_PTR) (info->texture);
   PIXEL_PTR d = (PIXEL_PTR) addr;
   long u = fu * z1;
   long v = fv * z1;

   /* update depth */
   fz += dfz;
   z1 = 1. / fz;

   for (x = w - 1; x >= 0; x-= 4) {
      long nextu, nextv, du, dv;

      fu += dfu;
      fv += dfv;
      fz += dfz;
      nextu = fu * z1;
      nextv = fv * z1;
      z1 = 1. / fz;
      du = (nextu - u) >> 2;
      dv = (nextv - v) >> 2;

      /* scanline subdivision */
      if (x < 3) 
         imax = x;
      for (i = imax; i >= 0; i--, INC_PIXEL_PTR(d)) {
         PIXEL_PTR s = OFFSET_PIXEL_PTR(texture, ((v >> vshift) & vmask) + ((u >> 16) & umask));
         unsigned long color = GET_MEMORY_PIXEL(s);
         color = PS_BLEND(blender, (c >> 16), color);

         PUT_PIXEL(d, color);
         u += du;
         v += dv;
	 c += dc;
      }
   }
}

/* _poly_scanline_ptex_mask_lit:
 *  Fills a masked lit perspective correct texture mapped polygon scanline.
 */
void FUNC_POLY_SCANLINE_PTEX_MASK_LIT(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info)
{
   int x, i, imax = 3;
   int vmask = info->vmask << info->vshift;
   int vshift = 16 - info->vshift;
   int umask = info->umask;
   fixed c = info->c;
   fixed dc = info->dc;
   double fu = info->fu;
   double fv = info->fv;
   double fz = info->z;
   double dfu = info->dfu * 4;
   double dfv = info->dfv * 4;
   double dfz = info->dz * 4;
   double z1 = 1. / fz;
   PS_BLENDER blender = MAKE_PS_BLENDER();
   PIXEL_PTR texture = (PIXEL_PTR) (info->texture);
   PIXEL_PTR d = (PIXEL_PTR) addr;
   long u = fu * z1;
   long v = fv * z1;

   /* update depth */
   fz += dfz;
   z1 = 1. / fz;

   for (x = w - 1; x >= 0; x-= 4) {
      long nextu, nextv, du, dv;

      fu += dfu;
      fv += dfv;
      fz += dfz;
      nextu = fu * z1;
      nextv = fv * z1;
      z1 = 1. / fz;
      du = (nextu - u) >> 2;
      dv = (nextv - v) >> 2;

      /* scanline subdivision */
      if (x < 3) 
         imax = x;
      for (i = imax; i >= 0; i--, INC_PIXEL_PTR(d)) {
         PIXEL_PTR s = OFFSET_PIXEL_PTR(texture, ((v >> vshift) & vmask) + ((u >> 16) & umask));
         unsigned long color = GET_MEMORY_PIXEL(s);

         if (!IS_MASK(color)) {
            color = PS_BLEND(blender, (c >> 16), color);
	    PUT_PIXEL(d, color);
         }
         u += du;
         v += dv;
	 c += dc;
      }
   }
}

/* _poly_scanline_atex_trans:
 *  Fills a trans affine texture mapped polygon scanline.
 */
void FUNC_POLY_SCANLINE_ATEX_TRANS(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info)
{
   int x;
   int vmask = info->vmask << info->vshift;
   int vshift = 16 - info->vshift;
   int umask = info->umask;
   fixed u = info->u;
   fixed v = info->v;
   fixed du = info->du;
   fixed dv = info->dv;
   PS_BLENDER blender = MAKE_PS_BLENDER();
   PIXEL_PTR texture = (PIXEL_PTR) (info->texture);
   PIXEL_PTR d = (PIXEL_PTR) addr;
   PIXEL_PTR r = (PIXEL_PTR) info->read_addr;

   for (x = w - 1; x >= 0; INC_PIXEL_PTR(d), INC_PIXEL_PTR(r), x--) {
      PIXEL_PTR s = OFFSET_PIXEL_PTR(texture, ((v >> vshift) & vmask) + ((u >> 16) & umask));
      unsigned long color = GET_MEMORY_PIXEL(s);
      color = PS_ALPHA_BLEND(blender, color, GET_PIXEL(r));
      //color = 100;
      PUT_PIXEL(d, color);
      u += du;
      v += dv;
   }
}

/* _poly_scanline_atex_mask_trans:
 *  Fills a trans masked affine texture mapped polygon scanline.
 */
void FUNC_POLY_SCANLINE_ATEX_MASK_TRANS(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info)
{
   int x;
   int vmask = info->vmask << info->vshift;
   int vshift = 16 - info->vshift;
   int umask = info->umask;
   fixed u = info->u;
   fixed v = info->v;
   fixed du = info->du;
   fixed dv = info->dv;
   PS_BLENDER blender = MAKE_PS_BLENDER();
   PIXEL_PTR texture = (PIXEL_PTR) (info->texture);
   PIXEL_PTR d = (PIXEL_PTR) addr;
   PIXEL_PTR r = (PIXEL_PTR) info->read_addr;

   for (x = w - 1; x >= 0; INC_PIXEL_PTR(d), INC_PIXEL_PTR(r), x--) {
      PIXEL_PTR s = OFFSET_PIXEL_PTR(texture, ((v >> vshift) & vmask) + ((u >> 16) & umask));
      unsigned long color = GET_MEMORY_PIXEL(s);

      if (!IS_MASK(color)) {
         color = PS_ALPHA_BLEND(blender, color, GET_PIXEL(r));
         PUT_PIXEL(d, color);
      }
      u += du;
      v += dv;
   }
}

/* _poly_scanline_ptex_trans:
 *  Fills a trans perspective correct texture mapped polygon scanline.
 */
void FUNC_POLY_SCANLINE_PTEX_TRANS(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info)
{
   int x, i, imax = 3;
   int vmask = info->vmask << info->vshift;
   int vshift = 16 - info->vshift;
   int umask = info->umask;
   double fu = info->fu;
   double fv = info->fv;
   double fz = info->z;
   double dfu = info->dfu * 4;
   double dfv = info->dfv * 4;
   double dfz = info->dz * 4;
   double z1 = 1. / fz;
   PS_BLENDER blender = MAKE_PS_BLENDER();
   PIXEL_PTR texture = (PIXEL_PTR) (info->texture);
   PIXEL_PTR d = (PIXEL_PTR) addr;
   PIXEL_PTR r = (PIXEL_PTR) info->read_addr;
   long u = fu * z1;
   long v = fv * z1;

   /* update depth */
   fz += dfz;
   z1 = 1. / fz;

   for (x = w - 1; x >= 0; x-= 4) {
      long nextu, nextv, du, dv;

      fu += dfu;
      fv += dfv;
      fz += dfz;
      nextu = fu * z1;
      nextv = fv * z1;
      z1 = 1. / fz;
      du = (nextu - u) >> 2;
      dv = (nextv - v) >> 2;

      /* scanline subdivision */
      if (x < 3) 
         imax = x;
      for (i = imax; i >= 0; i--, INC_PIXEL_PTR(d), INC_PIXEL_PTR(r)) {
         PIXEL_PTR s = OFFSET_PIXEL_PTR(texture, ((v >> vshift) & vmask) + ((u >> 16) & umask));
         unsigned long color = GET_MEMORY_PIXEL(s);

         color = PS_ALPHA_BLEND(blender, color, GET_PIXEL(r));
         PUT_PIXEL(d, color);
         u += du;
         v += dv;
      }
   }
}

/* _poly_scanline_ptex_mask_trans:
 *  Fills a trans masked perspective correct texture mapped polygon scanline.
 */
void FUNC_POLY_SCANLINE_PTEX_MASK_TRANS(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info)
{
   int x, i, imax = 3;
   int vmask = info->vmask << info->vshift;
   int vshift = 16 - info->vshift;
   int umask = info->umask;
   double fu = info->fu;
   double fv = info->fv;
   double fz = info->z;
   double dfu = info->dfu * 4;
   double dfv = info->dfv * 4;
   double dfz = info->dz * 4;
   double z1 = 1. / fz;
   PS_BLENDER blender = MAKE_PS_BLENDER();
   PIXEL_PTR texture = (PIXEL_PTR) (info->texture);
   PIXEL_PTR d = (PIXEL_PTR) addr;
   PIXEL_PTR r = (PIXEL_PTR) info->read_addr;
   long u = fu * z1;
   long v = fv * z1;

   /* update depth */
   fz += dfz;
   z1 = 1. / fz;

   for (x = w - 1; x >= 0; x-= 4) {
      long nextu, nextv, du, dv;

      fu += dfu;
      fv += dfv;
      fz += dfz;
      nextu = fu * z1;
      nextv = fv * z1;
      z1 = 1. / fz;
      du = (nextu - u) >> 2;
      dv = (nextv - v) >> 2;

      /* scanline subdivision */
      if (x < 3) 
         imax = x;
      for (i = imax; i >= 0; i--, INC_PIXEL_PTR(d), INC_PIXEL_PTR(r)) {
         PIXEL_PTR s = OFFSET_PIXEL_PTR(texture, ((v >> vshift) & vmask) + ((u >> 16) & umask));
         unsigned long color = GET_MEMORY_PIXEL(s);

	 if (!IS_MASK(color)) {
            color = PS_ALPHA_BLEND(blender, color, GET_PIXEL(r));
            PUT_PIXEL(d, color);
	 }
         u += du;
         v += dv;
      }
   }
}

#endif /* !__bma_cscan_h */

