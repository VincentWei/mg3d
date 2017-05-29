/*
** $Id: poly3d.c,v 1.14 2006-01-14 09:48:09 xwyan Exp $
**
** Port to MiniGUI by Feynman from allegro.
**
** Copyright (C) 2006 Feynman Software.
 *
 * Hicolor support
 *
 * Complete support for all drawing modes in every color depth MMX 
 * optimisations and z-buffer
 *
 * Subpixel and subtexel accuracy, triangle functions and speed 
 * enhancements 
 *
 * Functions adapted to handle two coincident vertices 
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

#include <limits.h>
#include <float.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/fixedmath.h>

#include "mg3drw.h"
#include "mg3dintern.h"
#include "mg3dbase.h"

int cpu_capabilities = 0;
void _poly_scanline_dummy(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info) { ;}

MG3DZBUFFER _zbuffer = 0;

SCANLINE_FILLER _optim_alternative_drawer;

void hfill(HDC hdc,int x, int y, int w, gal_pixel color)
{
    int i;

    for(i=w; i>=x; i--){
        SetPixel (hdc, i, y, color);
    }
}

/* _fill_3d_edge_structure:
 *  Polygon helper function: initialises an edge structure for the 3d 
 *  rasterising code, using fixed point vertex structures. Returns 1 on
 *  success, or 0 if the edge is horizontal or clipped out of existence.
 */
int _fill_3d_edge_structure(POLYGON_EDGE *edge, const mg3dVp *v1, const mg3dVp *v2, int flags, HDC mem_dc)
{
 /*  int r1, r2, g1, g2, b1, b2;*/
    Uint8 r1, r2, g1, g2, b1,b2;
    fixed h, step;

    /* swap vertices if they are the wrong way up */
    if (v2->y < v1->y) {
        const mg3dVp *vt;
        
        vt = v1;
        v1 = v2;
        v2 = vt;
    }

    /* set up screen rasterising parameters */
    edge->top = fixceil(v1->y);
    edge->bottom = fixceil(v2->y) - 1;

    if (edge->bottom < edge->top) return 0;
    
    h = v2->y - v1->y;
    step = (edge->top << 16) - v1->y;
    
    edge->dx = fixdiv(v2->x - v1->x, h);
    edge->x = v1->x + fixmul(step, edge->dx);
    
    edge->prev = NULL;
    edge->next = NULL;
    edge->w = 0;

    if (flags & INTERP_Z) {
        float h1 = 65536. / h;
        float step_f = fixtof(step);
        
        /* Z (depth) interpolation */
        float z1 = 65536. / v1->z;
        float z2 = 65536. / v2->z;
        
        edge->dat.dz = (z2 - z1) * h1;
        edge->dat.z = z1 + edge->dat.dz * step_f;
        
        if (flags & INTERP_FLOAT_UV) {
            /* floating point (perspective correct) texture interpolation */
            float fu1 = v1->u * z1;
            float fv1 = v1->v * z1;
            float fu2 = v2->u * z2;
            float fv2 = v2->v * z2;
            
            edge->dat.dfu = (fu2 - fu1) * h1;
            edge->dat.dfv = (fv2 - fv1) * h1;
            edge->dat.fu = fu1 + edge->dat.dfu * step_f;
            edge->dat.fv = fv1 + edge->dat.dfv * step_f;
        }
    }
    
    int memcb = GetGDCapability(mem_dc, GDCAP_MAXY);
    if (flags & INTERP_FLAT) {
        /* if clipping is enabled then clip edge */
        if (edge->top < 0) {
            edge->x += (0 - edge->top) * edge->dx;
            edge->top = 0;
        }
        if (edge->bottom >= memcb)
            edge->bottom = memcb-1;
        return (edge->bottom >= edge->top);
    }
    
    if (flags & INTERP_1COL) {
        /* single color shading interpolation */
        edge->dat.dc = fixdiv(itofix(v2->c - v1->c), h);
        edge->dat.c = itofix(v1->c) + fixmul(step, edge->dat.dc);
    }
    
    if (flags & INTERP_3COL) {
        /* RGB shading interpolation */
        if (flags & COLOR_TO_RGB) {
            Pixel2RGB(mem_dc, v1->c, &r1, &g1, &b1);
            Pixel2RGB(mem_dc, v2->c, &r2, &g2, &b2);
        }
        else {
            r1 = (v1->c >> 16) & 0xFF;
            r2 = (v2->c >> 16) & 0xFF;
            g1 = (v1->c >> 8) & 0xFF;
            g2 = (v2->c >> 8) & 0xFF;
            b1 = v1->c & 0xFF;
            b2 = v2->c & 0xFF;
        }
        
        edge->dat.dr = fixdiv(itofix(r2 - r1), h);
        edge->dat.dg = fixdiv(itofix(g2 - g1), h);
        edge->dat.db = fixdiv(itofix(b2 - b1), h);
        edge->dat.r = itofix(r1) + fixmul(step, edge->dat.dr);
        edge->dat.g = itofix(g1) + fixmul(step, edge->dat.dg);
        edge->dat.b = itofix(b1) + fixmul(step, edge->dat.db);
    }
    
    if (flags & INTERP_FIX_UV) {
        /* fixed point (affine) texture interpolation */
        edge->dat.du = fixdiv(v2->u - v1->u, h);
        edge->dat.dv = fixdiv(v2->v - v1->v, h);
        edge->dat.u = v1->u + fixmul(step, edge->dat.du);
        edge->dat.v = v1->v + fixmul(step, edge->dat.dv);
    }
    /* if clipping is enabled then clip edge */
    if (edge->top < 0) {
        int gap = 0 - edge->top;
        edge->top = 0;
        edge->x += gap * edge->dx;
        _clip_polygon_segment_f(&(edge->dat), gap, flags);
    }
    if (edge->bottom >= memcb)
        edge->bottom = memcb-1 ;
    return (edge->bottom >= edge->top);
}

/* _fill_3d_edge_structure_f:
 *  Polygon helper function: initialises an edge structure for the 3d 
 *  rasterising code, using floating point vertex structures. Returns 1 on
 *  success, or 0 if the edge is horizontal or clipped out of existence.
 */
int _fill_3d_edge_structure_f(POLYGON_EDGE *edge, const mg3dVpf *v1, const mg3dVpf *v2, int flags, HDC mem_dc)
{
  /* int r1, r2, g1, g2, b1, b2;*/
    Uint8 r1, r2, g1, g2, b1,b2;
   fixed h, step;
   float h1;

   /* swap vertices if they are the wrong way up */
   if (v2->y < v1->y) {
      const mg3dVpf *vt;

      vt = v1;
      v1 = v2;
      v2 = vt;
   }

   /* set up screen rasterising parameters */
   edge->top = fixceil(ftofix(v1->y));
   edge->bottom = fixceil(ftofix(v2->y)) - 1;

   if (edge->bottom < edge->top) return 0;

   h1 = 1.0 / (v2->y - v1->y);
   h = ftofix(v2->y - v1->y);
   step = (edge->top << 16) - ftofix(v1->y);

   edge->dx = ftofix((v2->x - v1->x)  * h1);
   edge->x = ftofix(v1->x) + fixmul(step, edge->dx);

   edge->prev = NULL;
   edge->next = NULL;
   edge->w = 0;

   if (flags & INTERP_Z) {
      float step_f = fixtof(step);

      /* Z (depth) interpolation */
      float z1 = 1. / v1->z;
      float z2 = 1. / v2->z;

      edge->dat.dz = (z2 - z1) * h1;
      edge->dat.z = z1 + edge->dat.dz * step_f;

      if (flags & INTERP_FLOAT_UV) {
	 /* floating point (perspective correct) texture interpolation */
	 float fu1 = v1->u * z1 * 65536.;
	 float fv1 = v1->v * z1 * 65536.;
	 float fu2 = v2->u * z2 * 65536.;
	 float fv2 = v2->v * z2 * 65536.;

	 edge->dat.dfu = (fu2 - fu1) * h1;
	 edge->dat.dfv = (fv2 - fv1) * h1;
	 edge->dat.fu = fu1 + edge->dat.dfu * step_f;
	 edge->dat.fv = fv1 + edge->dat.dfv * step_f;
      }
   }
   
    int memcb = GetGDCapability(mem_dc, GDCAP_MAXY);
    if (flags & INTERP_FLAT) {
        /* if clipping is enabled then clip edge */
        if (edge->top < 0) {
            edge->x += (0 - edge->top) * edge->dx;
            edge->top = 0;
        }
        if (edge->bottom >= memcb)
            edge->bottom = memcb-1;
        return (edge->bottom >= edge->top);
   }

   if (flags & INTERP_1COL) {
      /* single color shading interpolation */
      edge->dat.dc = fixdiv(itofix(v2->c - v1->c), h);
      edge->dat.c = itofix(v1->c) + fixmul(step, edge->dat.dc);
   }

   if (flags & INTERP_3COL) {
      /* RGB shading interpolation */
      if (flags & COLOR_TO_RGB) {
	 Pixel2RGB(mem_dc, v1->c, &r1, &g1, &b1);
	 Pixel2RGB(mem_dc, v2->c, &r2, &g2, &b2);    
      } 
      else {
	 r1 = (v1->c >> 16) & 0xFF;
	 r2 = (v2->c >> 16) & 0xFF;
	 g1 = (v1->c >> 8) & 0xFF;
	 g2 = (v2->c >> 8) & 0xFF;
	 b1 = v1->c & 0xFF;
	 b2 = v2->c & 0xFF;
      }

      edge->dat.dr = fixdiv(itofix(r2 - r1), h);
      edge->dat.dg = fixdiv(itofix(g2 - g1), h);
      edge->dat.db = fixdiv(itofix(b2 - b1), h);
      edge->dat.r = itofix(r1) + fixmul(step, edge->dat.dr);
      edge->dat.g = itofix(g1) + fixmul(step, edge->dat.dg);
      edge->dat.b = itofix(b1) + fixmul(step, edge->dat.db);
   }

   if (flags & INTERP_FIX_UV) {
      /* fixed point (affine) texture interpolation */
      edge->dat.du = ftofix((v2->u - v1->u) * h1);
      edge->dat.dv = ftofix((v2->v - v1->v) * h1);
      edge->dat.u = ftofix(v1->u) + fixmul(step, edge->dat.du);
      edge->dat.v = ftofix(v1->v) + fixmul(step, edge->dat.dv);
   }
   
    /* if clipping is enabled then clip edge */
   if (edge->top < 0) {
       int gap = 0 - edge->top;
       edge->top = 0;
       edge->x += gap * edge->dx;
       _clip_polygon_segment_f(&(edge->dat), gap, flags);
   }
   if (edge->bottom >= memcb)
       edge->bottom = memcb-1 ;
   return (edge->bottom >= edge->top);
}

/* _get_scanline_filler:
 *  Helper function for deciding which rasterisation function and 
 *  interpolation flags we should use for a specific polygon type.
 */
SCANLINE_FILLER _get_scanline_filler(int type, int *flags, POLYGON_SEGMENT *info, HDC texture, HDC mem_dc)
{
   typedef struct POLYTYPE_INFO 
   {
      SCANLINE_FILLER filler;
      SCANLINE_FILLER alternative;
   } POLYTYPE_INFO;

   static int polytype_interp_pal[] = 
   {
      INTERP_FLAT,
      INTERP_1COL,
      INTERP_3COL,
      INTERP_FIX_UV,
      INTERP_Z | INTERP_FLOAT_UV | OPT_FLOAT_UV_TO_FIX,
      INTERP_FIX_UV,
      INTERP_Z | INTERP_FLOAT_UV | OPT_FLOAT_UV_TO_FIX,
      INTERP_FIX_UV | INTERP_1COL,
      INTERP_Z | INTERP_FLOAT_UV | INTERP_1COL | OPT_FLOAT_UV_TO_FIX,
      INTERP_FIX_UV | INTERP_1COL,
      INTERP_Z | INTERP_FLOAT_UV | INTERP_1COL | OPT_FLOAT_UV_TO_FIX,
      INTERP_FIX_UV,
      INTERP_Z | INTERP_FLOAT_UV | OPT_FLOAT_UV_TO_FIX,
      INTERP_FIX_UV,
      INTERP_Z | INTERP_FLOAT_UV | OPT_FLOAT_UV_TO_FIX
   };

   static int polytype_interp_tc[] = 
   {
      INTERP_FLAT,
      INTERP_3COL | COLOR_TO_RGB,
      INTERP_3COL,
      INTERP_FIX_UV,
      INTERP_Z | INTERP_FLOAT_UV | OPT_FLOAT_UV_TO_FIX,
      INTERP_FIX_UV,
      INTERP_Z | INTERP_FLOAT_UV | OPT_FLOAT_UV_TO_FIX,
      INTERP_FIX_UV | INTERP_1COL,
      INTERP_Z | INTERP_FLOAT_UV | INTERP_1COL | OPT_FLOAT_UV_TO_FIX,
      INTERP_FIX_UV | INTERP_1COL,
      INTERP_Z | INTERP_FLOAT_UV | INTERP_1COL | OPT_FLOAT_UV_TO_FIX,
      INTERP_FIX_UV,
      INTERP_Z | INTERP_FLOAT_UV | OPT_FLOAT_UV_TO_FIX,
      INTERP_FIX_UV,
      INTERP_Z | INTERP_FLOAT_UV | OPT_FLOAT_UV_TO_FIX
   };

   #ifdef MINIGUI_COLOR8
   static POLYTYPE_INFO polytype_info8[] =
   {
      {  _poly_scanline_dummy,            NULL },
      {  _poly_scanline_gcol8,            NULL },
      {  _poly_scanline_grgb8,            NULL },
      {  _poly_scanline_atex8,            NULL },
      {  _poly_scanline_ptex8,            _poly_scanline_atex8 },
      {  _poly_scanline_atex_mask8,       NULL },
      {  _poly_scanline_ptex_mask8,       _poly_scanline_atex_mask8 },
      {  _poly_scanline_atex_lit8,        NULL },
      {  _poly_scanline_ptex_lit8,        _poly_scanline_atex_lit8 },
      {  _poly_scanline_atex_mask_lit8,   NULL },
      {  _poly_scanline_ptex_mask_lit8,   _poly_scanline_atex_mask_lit8 },
      {  _poly_scanline_atex_trans8,      NULL },
      {  _poly_scanline_ptex_trans8,      _poly_scanline_atex_trans8 },
      {  _poly_scanline_atex_mask_trans8, NULL },
      {  _poly_scanline_ptex_mask_trans8, _poly_scanline_atex_mask_trans8 }
   };

   #endif

   #ifdef MINIGUI_COLOR16
   static POLYTYPE_INFO polytype_info15[] =
   {
      {  _poly_scanline_dummy,             NULL },
      {  _poly_scanline_grgb15,            NULL },
      {  _poly_scanline_grgb15,            NULL },
      {  _poly_scanline_atex16,            NULL },
      {  _poly_scanline_ptex16,            _poly_scanline_atex16 },
      {  _poly_scanline_atex_mask15,       NULL },
      {  _poly_scanline_ptex_mask15,       _poly_scanline_atex_mask15 },
      {  _poly_scanline_atex_lit15,        NULL },
      {  _poly_scanline_ptex_lit15,        _poly_scanline_atex_lit15 },
      {  _poly_scanline_atex_mask_lit15,   NULL },
      {  _poly_scanline_ptex_mask_lit15,   _poly_scanline_atex_mask_lit15 },
      {  _poly_scanline_atex_trans15,      NULL },
      {  _poly_scanline_ptex_trans15,      _poly_scanline_atex_trans15 },
      {  _poly_scanline_atex_mask_trans15, NULL },
      {  _poly_scanline_ptex_mask_trans15, _poly_scanline_atex_mask_trans15 }
   };

      static POLYTYPE_INFO polytype_info16[] =
   {
      {  _poly_scanline_dummy,             NULL },
      {  _poly_scanline_grgb16,            NULL },
      {  _poly_scanline_grgb16,            NULL },
      {  _poly_scanline_atex16,            NULL },
      {  _poly_scanline_ptex16,            _poly_scanline_atex16 },
      {  _poly_scanline_atex_mask16,       NULL },
      {  _poly_scanline_ptex_mask16,       _poly_scanline_atex_mask16 },
      {  _poly_scanline_atex_lit16,        NULL },
      {  _poly_scanline_ptex_lit16,        _poly_scanline_atex_lit16 },
      {  _poly_scanline_atex_mask_lit16,   NULL },
      {  _poly_scanline_ptex_mask_lit16,   _poly_scanline_atex_mask_lit16 },
      {  _poly_scanline_atex_trans16,      NULL },
      {  _poly_scanline_ptex_trans16,      _poly_scanline_atex_trans16 },
      {  _poly_scanline_atex_mask_trans16, NULL },
      {  _poly_scanline_ptex_mask_trans16, _poly_scanline_atex_mask_trans16 }
   };

   #endif

   #ifdef MINIGUI_COLOR24
   static POLYTYPE_INFO polytype_info24[] =
   {
      {  _poly_scanline_dummy,             NULL },
      {  _poly_scanline_grgb24,            NULL },
      {  _poly_scanline_grgb24,            NULL },
      {  _poly_scanline_atex24,            NULL },
      {  _poly_scanline_ptex24,            _poly_scanline_atex24 },
      {  _poly_scanline_atex_mask24,       NULL },
      {  _poly_scanline_ptex_mask24,       _poly_scanline_atex_mask24 },
      {  _poly_scanline_atex_lit24,        NULL },
      {  _poly_scanline_ptex_lit24,        _poly_scanline_atex_lit24 },
      {  _poly_scanline_atex_mask_lit24,   NULL },
      {  _poly_scanline_ptex_mask_lit24,   _poly_scanline_atex_mask_lit24 },
      {  _poly_scanline_atex_trans24,      NULL },
      {  _poly_scanline_ptex_trans24,      _poly_scanline_atex_trans24 },
      {  _poly_scanline_atex_mask_trans24, NULL },
      {  _poly_scanline_ptex_mask_trans24, _poly_scanline_atex_mask_trans24 }
   };

   #endif

   #ifdef MINIGUI_COLOR32
   static POLYTYPE_INFO polytype_info32[] =
   {
      {  _poly_scanline_dummy,             NULL },
      {  _poly_scanline_grgb32,            NULL },
      {  _poly_scanline_grgb32,            NULL },
      {  _poly_scanline_atex32,            NULL },
      {  _poly_scanline_ptex32,            _poly_scanline_atex32 },
      {  _poly_scanline_atex_mask32,       NULL },
      {  _poly_scanline_ptex_mask32,       _poly_scanline_atex_mask32 },
      {  _poly_scanline_atex_lit32,        NULL },
      {  _poly_scanline_ptex_lit32,        _poly_scanline_atex_lit32 },
      {  _poly_scanline_atex_mask_lit32,   NULL },
      {  _poly_scanline_ptex_mask_lit32,   _poly_scanline_atex_mask_lit32 },
      {  _poly_scanline_atex_trans32,      NULL },
      {  _poly_scanline_ptex_trans32,      _poly_scanline_atex_trans32 },
      {  _poly_scanline_atex_mask_trans32, NULL },
      {  _poly_scanline_ptex_mask_trans32, _poly_scanline_atex_mask_trans32 }
   };

   #endif

   #ifdef MINIGUI_COLOR8
   static POLYTYPE_INFO polytype_info8z[] =
   {
      {  _poly_zbuf_flat8,            NULL },
      {  _poly_zbuf_gcol8,            NULL },
      {  _poly_zbuf_grgb8,            NULL },
      {  _poly_zbuf_atex8,            NULL },
      {  _poly_zbuf_ptex8,            _poly_zbuf_atex8 },
      {  _poly_zbuf_atex_mask8,       NULL },
      {  _poly_zbuf_ptex_mask8,       _poly_zbuf_atex_mask8 },
      {  _poly_zbuf_atex_lit8,        NULL },
      {  _poly_zbuf_ptex_lit8,        _poly_zbuf_atex_lit8 },
      {  _poly_zbuf_atex_mask_lit8,   NULL },
      {  _poly_zbuf_ptex_mask_lit8,   _poly_zbuf_atex_mask_lit8 },
      {  _poly_zbuf_atex_trans8,      NULL },
      {  _poly_zbuf_ptex_trans8,      _poly_zbuf_atex_trans8 },
      {  _poly_zbuf_atex_mask_trans8, NULL },
      {  _poly_zbuf_ptex_mask_trans8, _poly_zbuf_atex_mask_trans8 }
   };
   #endif

   #ifdef MINIGUI_COLOR16
   static POLYTYPE_INFO polytype_info15z[] =
   {
      {  _poly_zbuf_flat16,            NULL },
      {  _poly_zbuf_grgb15,            NULL },
      {  _poly_zbuf_grgb15,            NULL },
      {  _poly_zbuf_atex16,            NULL },
      {  _poly_zbuf_ptex16,            _poly_zbuf_atex16 },
      {  _poly_zbuf_atex_mask15,       NULL },
      {  _poly_zbuf_ptex_mask15,       _poly_zbuf_atex_mask15 },
      {  _poly_zbuf_atex_lit15,        NULL },
      {  _poly_zbuf_ptex_lit15,        _poly_zbuf_atex_lit15 },
      {  _poly_zbuf_atex_mask_lit15,   NULL },
      {  _poly_zbuf_ptex_mask_lit15,   _poly_zbuf_atex_mask_lit15 },
      {  _poly_zbuf_atex_trans15,      NULL },
      {  _poly_zbuf_ptex_trans15,      _poly_zbuf_atex_trans15 },
      {  _poly_zbuf_atex_mask_trans15, NULL },
      {  _poly_zbuf_ptex_mask_trans15, _poly_zbuf_atex_mask_trans15 }
   };

   static POLYTYPE_INFO polytype_info16z[] =
   {
      {  _poly_zbuf_flat16,            NULL },
      {  _poly_zbuf_grgb16,            NULL },
      {  _poly_zbuf_grgb16,            NULL },
      {  _poly_zbuf_atex16,            NULL },
      {  _poly_zbuf_ptex16,            _poly_zbuf_atex16 },
      {  _poly_zbuf_atex_mask16,       NULL },
      {  _poly_zbuf_ptex_mask16,       _poly_zbuf_atex_mask16 },
      {  _poly_zbuf_atex_lit16,        NULL },
      {  _poly_zbuf_ptex_lit16,        _poly_zbuf_atex_lit16 },
      {  _poly_zbuf_atex_mask_lit16,   NULL },
      {  _poly_zbuf_ptex_mask_lit16,   _poly_zbuf_atex_mask_lit16 },
      {  _poly_zbuf_atex_trans16,      NULL },
      {  _poly_zbuf_ptex_trans16,      _poly_zbuf_atex_trans16 },
      {  _poly_zbuf_atex_mask_trans16, NULL },
      {  _poly_zbuf_ptex_mask_trans16, _poly_zbuf_atex_mask_trans16 }
   };
   #endif

   #ifdef MINIGUI_COLOR24
   static POLYTYPE_INFO polytype_info24z[] =
   {
      {  _poly_zbuf_flat24,            NULL },
      {  _poly_zbuf_grgb24,            NULL },
      {  _poly_zbuf_grgb24,            NULL },
      {  _poly_zbuf_atex24,            NULL },
      {  _poly_zbuf_ptex24,            _poly_zbuf_atex24 },
      {  _poly_zbuf_atex_mask24,       NULL },
      {  _poly_zbuf_ptex_mask24,       _poly_zbuf_atex_mask24 },
      {  _poly_zbuf_atex_lit24,        NULL },
      {  _poly_zbuf_ptex_lit24,        _poly_zbuf_atex_lit24 },
      {  _poly_zbuf_atex_mask_lit24,   NULL },
      {  _poly_zbuf_ptex_mask_lit24,   _poly_zbuf_atex_mask_lit24 },
      {  _poly_zbuf_atex_trans24,      NULL },
      {  _poly_zbuf_ptex_trans24,      _poly_zbuf_atex_trans24 },
      {  _poly_zbuf_atex_mask_trans24, NULL },
      {  _poly_zbuf_ptex_mask_trans24, _poly_zbuf_atex_mask_trans24 }
   };
   #endif

   #ifdef MINIGUI_COLOR32
   static POLYTYPE_INFO polytype_info32z[] =
   {
      {  _poly_zbuf_flat32,            NULL },
      {  _poly_zbuf_grgb32,            NULL },
      {  _poly_zbuf_grgb32,            NULL },
      {  _poly_zbuf_atex32,            NULL },
      {  _poly_zbuf_ptex32,            _poly_zbuf_atex32 },
      {  _poly_zbuf_atex_mask32,       NULL },
      {  _poly_zbuf_ptex_mask32,       _poly_zbuf_atex_mask32 },
      {  _poly_zbuf_atex_lit32,        NULL },
      {  _poly_zbuf_ptex_lit32,        _poly_zbuf_atex_lit32 },
      {  _poly_zbuf_atex_mask_lit32,   NULL },
      {  _poly_zbuf_ptex_mask_lit32,   _poly_zbuf_atex_mask_lit32 },
      {  _poly_zbuf_atex_trans32,      NULL },
      {  _poly_zbuf_ptex_trans32,      _poly_zbuf_atex_trans32 },
      {  _poly_zbuf_atex_mask_trans32, NULL },
      {  _poly_zbuf_ptex_mask_trans32, _poly_zbuf_atex_mask_trans32 }
   };
   #endif

   int zbuf = type & MG3D_POLYTYPE_ZBUF;

   int *interpinfo;
   POLYTYPE_INFO *typeinfo, *typeinfo_zbuf;
   Uint8 * frame_buffer;
   RECT rc = {0, 0, 1, 1};

   switch (GetGDCapability(mem_dc, GDCAP_DEPTH)) {

      #ifdef MINIGUI_COLOR8

	 case 8:
	    interpinfo = polytype_interp_pal;
	    typeinfo = polytype_info8;
	    typeinfo_zbuf = polytype_info8z;
	    break;

      #endif

      #ifdef MINIGUI_COLOR16

	 case 15:
	    interpinfo = polytype_interp_tc;
	    typeinfo = polytype_info15;
	    typeinfo_zbuf = polytype_info15z;
	    break;

	 case 16:
	    interpinfo = polytype_interp_tc;
	    typeinfo = polytype_info16;
	    typeinfo_zbuf = polytype_info16z;
	    break;

      #endif

      #ifdef MINIGUI_COLOR24

	 case 24:
	    interpinfo = polytype_interp_tc;
	    typeinfo = polytype_info24;
	    typeinfo_zbuf = polytype_info24z;
	    break;

      #endif

      #ifdef MINIGUI_COLOR32

	 case 32:
	    interpinfo = polytype_interp_tc;
	    typeinfo = polytype_info32;
	    typeinfo_zbuf = polytype_info32z;
	    break;

      #endif

      default:
	 return NULL;
   }

   type = MID(0, type & ~MG3D_POLYTYPE_ZBUF, MG3D_POLYTYPE_MAX-1);
   *flags = interpinfo[type];

   if (texture) {
      frame_buffer = LockDC(texture, &rc, NULL, NULL, NULL);
      info->texture = (unsigned char *)frame_buffer; 
      UnlockDC(texture);
      info->umask = GetGDCapability(texture, GDCAP_MAXX);
      info->vmask = GetGDCapability(texture, GDCAP_MAXY);
      info->vshift = 0;
      while ((1 << info->vshift) < GetGDCapability(texture, GDCAP_HPIXEL))
	  info->vshift++;
   }
   else {
      info->texture = NULL;
      info->umask = info->vmask = info->vshift = 0;
   }

   info->seg = 0;

   if (zbuf) {
      *flags |= INTERP_Z + INTERP_ZBUF;
      _optim_alternative_drawer = typeinfo_zbuf[type].alternative;
      return typeinfo_zbuf[type].filler;
   }

   _optim_alternative_drawer = typeinfo[type].alternative;

   return typeinfo[type].filler;
}

/* _clip_polygon_segment_f:
 *  Updates interpolation state values when skipping several places, eg.
 *  clipping the first part of a scanline.
 */
void _clip_polygon_segment_f(POLYGON_SEGMENT *info, int gap, int flags)
{
   if (flags & INTERP_1COL)
      info->c += info->dc * gap;

   if (flags & INTERP_3COL) {
      info->r += info->dr * gap;
      info->g += info->dg * gap;
      info->b += info->db * gap;
   }

   if (flags & INTERP_FIX_UV) {
      info->u += info->du * gap;
      info->v += info->dv * gap;
   }

   if (flags & INTERP_Z) {
      info->z += info->dz * gap;

      if (flags & INTERP_FLOAT_UV) {
	 info->fu += info->dfu * gap;
	 info->fv += info->dfv * gap;
      }
   }
}

/* draw_polygon_segment: 
 *  Polygon helper function to fill a scanline. Calculates deltas for 
 *  whichever values need interpolating, clips the segment, and then calls
 *  the lowlevel scanline filler.
 */
static void draw_polygon_segment(HDC mem_dc, int ytop, int ybottom, POLYGON_EDGE *e1, POLYGON_EDGE *e2, SCANLINE_FILLER drawer, int flags, gal_pixel color, POLYGON_SEGMENT *info)
{
    int x, y, w, gap;
    fixed step, width;
    POLYGON_SEGMENT *s1, *s2;
    const SCANLINE_FILLER save_drawer = drawer;
    
    /* ensure that e1 is the left edge and e2 is the right edge */
    if ((e2->x < e1->x) || ((e1->x == e2->x) && (e2->dx < e1->dx))) {
        POLYGON_EDGE *et = e1;
        e1 = e2;
        e2 = et;
    }
    
    s1 = &(e1->dat);
    s2 = &(e2->dat);
    
    if (flags & INTERP_FLAT)
        info->c = color;
    
    /* for each scanline in the polygon... */
    for (y=ytop; y<=ybottom; y++) {
        x = fixceil(e1->x);
        w = fixceil(e2->x) - x;
        drawer = save_drawer;
        
        if (drawer == _poly_scanline_dummy) {
            if (w>0){
                hfill(mem_dc, x, y, x+w-1, color);}
        }
        else {
            step = (x << 16) - e1->x;
            width = e2->x - e1->x;
/*
 *  Nasty trick :
 *  In order to avoid divisions by zero, width is set to -1. This way s1 and s2
 *  are still being updated but the scanline is not drawn since w == 0.
 */
            if (width == 0)
                width = -1 << 16;
/*
 *  End of nasty trick.
 */
            if (flags & INTERP_1COL) {
                info->dc = fixdiv(s2->c - s1->c, width);
                info->c = s1->c + fixmul(step, info->dc);
                s1->c += s1->dc;
                s2->c += s2->dc;
            }
            
            if (flags & INTERP_3COL) {
                info->dr = fixdiv(s2->r - s1->r, width);
                info->dg = fixdiv(s2->g - s1->g, width);
                info->db = fixdiv(s2->b - s1->b, width);
                info->r = s1->r + fixmul(step, info->dr);
                info->g = s1->g + fixmul(step, info->dg);
                info->b = s1->b + fixmul(step, info->db);
                
                s1->r += s1->dr;
                s2->r += s2->dr;
                s1->g += s1->dg;
                s2->g += s2->dg;
                s1->b += s1->db;
                s2->b += s2->db;
            }
            
            if (flags & INTERP_FIX_UV) {
                info->du = fixdiv(s2->u - s1->u, width);
                info->dv = fixdiv(s2->v - s1->v, width);
                info->u = s1->u + fixmul(step, info->du);
                info->v = s1->v + fixmul(step, info->dv);
                
                s1->u += s1->du;
                s2->u += s2->du;
                s1->v += s1->dv;
                s2->v += s2->dv;
            }
            
            if (flags & INTERP_Z) {
                float step_f = fixtof(step);
                float w1 = 65536. / width;
                info->dz = (s2->z - s1->z) * w1;
                info->z = s1->z + info->dz * step_f;
                s1->z += s1->dz;
                s2->z += s2->dz;
                
                if (flags & INTERP_FLOAT_UV) {
                    info->dfu = (s2->fu - s1->fu) * w1;
                    info->dfv = (s2->fv - s1->fv) * w1;
                    info->fu = s1->fu + info->dfu * step_f;
                    info->fv = s1->fv + info->dfv * step_f;
                    
                    s1->fu += s1->dfu;
                    s2->fu += s2->dfu;
                    s1->fv += s1->dfv;
                    s2->fv += s2->dfv;
                }
            }
            
            int memcr = GetGDCapability(mem_dc, GDCAP_MAXX);
            /* if clipping is enabled then clip the segment */
            if (x < 0) {
                gap = 0 - x;
                x = 0;
                w -= gap;
                _clip_polygon_segment_f(info, gap, flags);
            }
            
            if (x+w > memcr)
                w = memcr- x;
            
            if (w > 0) {
                int dx = x * GetGDCapability(mem_dc, GDCAP_BPP);
                
                if ((flags & OPT_FLOAT_UV_TO_FIX) && (info->dz == 0)) {
                    float z1 = 1. / info->z;
                    info->u = info->fu * z1;
                    info->v = info->fv * z1;
                    info->du = info->dfu * z1;
                    info->dv = info->dfv * z1;
                    drawer = _optim_alternative_drawer;
                }
                
                if (flags & INTERP_ZBUF)
                    info->zbuf_addr = bmp_write_line(_zbuffer, y) + x * sizeof(float);
                
                info->read_addr = bmp_read_line(mem_dc, y) + dx;
                drawer(mem_dc, bmp_write_line(mem_dc ,y) + dx, w, info);
            }
        }
        
        e1->x += e1->dx;
        e2->x += e2->dx;
    }
}

/* do_polygon3d:
 *  Helper function for rendering 3d polygon, used by both the fixed point
 *  and floating point drawing functions.
 */
static void do_polygon3d(HDC mem_dc, int top, int bottom, POLYGON_EDGE *left_edge, SCANLINE_FILLER drawer, int flags, gal_pixel color, POLYGON_SEGMENT *info)
{
    int ytop, ybottom;
#ifdef MINIGUI_DOS
    int old87 = 0;
#endif
    POLYGON_EDGE *right_edge;

    /* set fpu to single-precision, truncate mode */
#ifdef MINIGUI_DOS
    if (flags & (INTERP_Z | INTERP_FLOAT_UV))
        old87 = _control87(PC_24 | RC_CHOP, MCW_PC | MCW_RC);
#endif


    if ((left_edge->prev != left_edge->next) && (left_edge->prev->top == top))
        left_edge = left_edge->prev;
    right_edge = left_edge->next;

    ytop = top;
    for (;;) {
        if (right_edge->bottom <= left_edge->bottom)
            ybottom = right_edge->bottom;
        else
            ybottom = left_edge->bottom;
        
        /* fill the scanline */
        draw_polygon_segment(mem_dc ,ytop, ybottom, left_edge, right_edge, drawer, flags, color, info);

        if (ybottom >= bottom) break;
        
        /* update edges */
        if (ybottom >= left_edge->bottom)
            left_edge = left_edge->prev;
        if (ybottom >= right_edge->bottom)
            right_edge = right_edge->next;
        ytop = ybottom + 1;
    }

}

/* mg3dPolygon:
 *  Draws a 3d polygon in the specified mode. The vertices parameter should
 *  be followed by that many pointers to mg3dVp structures, which describe each
 *  vertex of the polygon.
 */
void mg3dPolygon(HDC mem_dc, int type, HDC texture, int vc, mg3dVp *vtx[])
{
   int c;
   int flags;
   int top = INT_MAX;
   int bottom = INT_MIN;
   void * _scratch_mem;
   mg3dVp *v1, *v2;
   POLYGON_EDGE *edge, *edge0, *start_edge;
   POLYGON_EDGE *list_edges = NULL;
   POLYGON_SEGMENT info;
   SCANLINE_FILLER drawer;

   if (vc < 3)
      return;

   /* set up the drawing mode */
   /*drawer = _get_scanline_filler(type, &flags, &info, texture, bmp);*/
   drawer = _get_scanline_filler(type, &flags, &info, texture, mem_dc);
   if (!drawer)
      return;

   /* allocate some space for the active edge table */
    _scratch_mem = malloc(sizeof(POLYGON_EDGE)*vc);
    if (_scratch_mem == NULL)
    {
	return ;
    }
   
    start_edge = edge0 = edge = (POLYGON_EDGE *)_scratch_mem;
   /* fill the double-linked list of edges (order unimportant) */
   v2 = vtx[vc-1];

   for (c=0; c<vc; c++) {
      v1 = v2;
      v2 = vtx[c];

      if (_fill_3d_edge_structure(edge, v1, v2, flags, mem_dc)) {
	 if (edge->top < top) {
            top = edge->top;
	    start_edge = edge;
         }

	 if (edge->bottom > bottom)
            bottom = edge->bottom;

         if (list_edges) {
            list_edges->next = edge;
	    edge->prev = list_edges;
         }

	 list_edges = edge;
	 edge++;
      }
   }

   if (list_edges) {
      /* close the double-linked list */
      edge0->prev = --edge;
      edge->next = edge0;

      /* render the polygon */
      do_polygon3d(mem_dc, top, bottom, start_edge, drawer, flags, vtx[0]->c, &info);
   }
   free(_scratch_mem);
}

/* mg3dPolygonF:
 *  Floating point version of polygon3d().
 */
void mg3dPolygonF(HDC mem_dc, int type, HDC texture, int vc, mg3dVpf *vtx[])
{
   int c;
   int flags;
   int top = INT_MAX;
   int bottom = INT_MIN;
   void * _scratch_mem;
   mg3dVpf *v1, *v2;
   POLYGON_EDGE *edge, *edge0, *start_edge;
   POLYGON_EDGE *list_edges = NULL;
   POLYGON_SEGMENT info;
   SCANLINE_FILLER drawer;

   if (vc < 3)
      return;

   /* set up the drawing mode */
   drawer = _get_scanline_filler(type, &flags, &info, texture, mem_dc);
   if (!drawer)
      return;

   /* allocate some space for the active edge table */
   _scratch_mem = malloc(sizeof(POLYGON_EDGE) * vc);
   if (_scratch_mem == NULL)
   {
       return ;
   }

   start_edge = edge0 = edge = (POLYGON_EDGE *)_scratch_mem;

   /* fill the double-linked list of edges in clockwise order */
   v2 = vtx[vc-1];

   for (c=0; c<vc; c++) {
      v1 = v2;
      v2 = vtx[c];

      if (_fill_3d_edge_structure_f(edge, v1, v2, flags, mem_dc)) {
         if (edge->top < top) {
            top = edge->top;
	    start_edge = edge;
         }

	 if (edge->bottom > bottom)
            bottom = edge->bottom;

         if (list_edges) {
            list_edges->next = edge;
	    edge->prev = list_edges;
         }

	 list_edges = edge;
	 edge++;
      }
   }

   if (list_edges) {
      /* close the double-linked list */
      edge0->prev = --edge;
      edge->next = edge0;

      /* render the polygon */
      do_polygon3d(mem_dc, top, bottom, start_edge, drawer, flags, vtx[0]->c, &info);
   }
   free(_scratch_mem);
}

/* draw_triangle_part:
 *  Triangle helper function to fill a triangle part. Computes interpolation,
 *  /clips the segment, and then calls the lowlevel scanline filler.
 */
static void draw_triangle_part(HDC mem_dc, HDC texture, int ytop, int ybottom, POLYGON_EDGE *left_edge, POLYGON_EDGE *right_edge, SCANLINE_FILLER drawer, int flags, gal_pixel color, POLYGON_SEGMENT *info)
{
   int x, y, w;
   int gap;
   const int test_optim = (flags & OPT_FLOAT_UV_TO_FIX) && (info->dz == 0);
   fixed step;
   POLYGON_SEGMENT *s1;

   /* ensure that left_edge and right_edge are the right way round */
   if ((right_edge->x < left_edge->x) ||
     ((left_edge->x == right_edge->x) && (right_edge->dx < left_edge->dx))) {
      POLYGON_EDGE *other_edge = left_edge;
      left_edge = right_edge;
      right_edge = other_edge;
   }

   s1 = &(left_edge->dat);

   if (flags & INTERP_FLAT)
      info->c = color;

   for (y=ytop; y<=ybottom; y++) {
      x = fixceil(left_edge->x);
      w = fixceil(right_edge->x) - x;
      step = (x << 16) - left_edge->x;

      if (drawer == _poly_scanline_dummy) {
	  if (w > 0)
	      hfill(mem_dc, x, y, x+w-1, color);
      }
      else {
	 if (flags & INTERP_1COL) {
	    info->c = s1->c + fixmul(step, info->dc);
            s1->c += s1->dc;
         }

	 if (flags & INTERP_3COL) {
	    info->r = s1->r + fixmul(step, info->dr);
	    info->g = s1->g + fixmul(step, info->dg);
	    info->b = s1->b + fixmul(step, info->db);

	    s1->r += s1->dr;
	    s1->g += s1->dg;
	    s1->b += s1->db;
	 }

	 if (flags & INTERP_FIX_UV) {
	    info->u = s1->u + fixmul(step, info->du);
	    info->v = s1->v + fixmul(step, info->dv);

	    s1->u += s1->du;
	    s1->v += s1->dv;
	 }

	 if (flags & INTERP_Z) {
	    float step_f = fixtof(step);

	    info->z = s1->z + info->dz * step_f;
	    s1->z += s1->dz;

	    if (flags & INTERP_FLOAT_UV) {
	       info->fu = s1->fu + info->dfu * step_f;
	       info->fv = s1->fv + info->dfv * step_f;

	       s1->fu += s1->dfu;
	       s1->fv += s1->dfv;
	    }
	 }
     
     int memcr = GetGDCapability(mem_dc, GDCAP_MAXX);
     /* if clipping is enabled then clip the segment */
     
     if (x < 0) {
         gap = 0 - x;
         x = 0;
         w -= gap;
         _clip_polygon_segment_f(info, gap, flags);
     }
     
     if (x+w > memcr)
         w = memcr - x;
     
     if (w > 0) {
         int dx = x * GetGDCapability(mem_dc, GDCAP_BPP);
	    
	    if (test_optim) {
	       float z1 = 1. / info->z;
	       info->u = info->fu * z1;
	       info->v = info->fv * z1;
	       info->du = info->dfu * z1;
	       info->dv = info->dfv * z1;
	       drawer = _optim_alternative_drawer;
	    }
        if (flags & INTERP_ZBUF)
            info->zbuf_addr = bmp_write_line(_zbuffer, y) + x * sizeof(float);

	    info->read_addr = bmp_read_line(mem_dc, y) + dx;
	    drawer(mem_dc, bmp_write_line(mem_dc, y) + dx, w, info);
	 }
      }

      left_edge->x += left_edge->dx;
      right_edge->x += right_edge->dx;
   }
}

/* _triangle_deltas:
 *  Triangle3d helper function to calculate the deltas. (For triangles,
 *  deltas are constant over the whole triangle).
 */
static void _triangle_deltas(HDC mem_dc, fixed w, POLYGON_SEGMENT *s1, POLYGON_SEGMENT *info, const mg3dVp *v, int flags)
{
   if (flags & INTERP_1COL)
      info->dc = fixdiv(s1->c - itofix(v->c), w);

   if (flags & INTERP_3COL) {
       Uint8 r, g, b;

      if (flags & COLOR_TO_RGB) {
	 Pixel2RGB(mem_dc, v->c, &r, &g, &b); 
      }
      else {
	 r = (v->c >> 16) & 0xFF;
	 g = (v->c >> 8) & 0xFF;
	 b = v->c & 0xFF;
      }

      info->dr = fixdiv(s1->r - itofix(r), w);
      info->dg = fixdiv(s1->g - itofix(g), w);
      info->db = fixdiv(s1->b - itofix(b), w);
   }

   if (flags & INTERP_FIX_UV) {
      info->du = fixdiv(s1->u - v->u, w);
      info->dv = fixdiv(s1->v - v->v, w);
   }

   if (flags & INTERP_Z) {
      float w1 = 65536. / w;

      /* Z (depth) interpolation */
      float z1 = 65536. / v->z;

      info->dz = (s1->z - z1) * w1;

      if (flags & INTERP_FLOAT_UV) {
	 /* floating point (perspective correct) texture interpolation */
	 float fu1 = v->u * z1;
	 float fv1 = v->v * z1;

	 info->dfu = (s1->fu - fu1) * w1;
	 info->dfv = (s1->fv - fv1) * w1;
      }
   }
}

/* _triangle_deltas_f:
 *  Floating point version of _triangle_deltas().
 */
static void _triangle_deltas_f(HDC mem_dc, fixed w, POLYGON_SEGMENT *s1, POLYGON_SEGMENT *info, const mg3dVpf *v, int flags)
{
   if (flags & INTERP_1COL)
      info->dc = fixdiv(s1->c - itofix(v->c), w);

   if (flags & INTERP_3COL) {
       Uint8 r, g, b;

      if (flags & COLOR_TO_RGB) {
         Pixel2RGB(mem_dc, v->c, &r, &g, &b);
      }
      else {
	 r = (v->c >> 16) & 0xFF;
	 g = (v->c >> 8) & 0xFF;
	 b = v->c & 0xFF;
      }

      info->dr = fixdiv(s1->r - itofix(r), w);
      info->dg = fixdiv(s1->g - itofix(g), w);
      info->db = fixdiv(s1->b - itofix(b), w);
   }

   if (flags & INTERP_FIX_UV) {
      info->du = fixdiv(s1->u - ftofix(v->u), w);
      info->dv = fixdiv(s1->v - ftofix(v->v), w);
   }

   if (flags & INTERP_Z) {
      float w1 = 65536. / w;

      /* Z (depth) interpolation */
      float z1 = 1. / v->z;

      info->dz = (s1->z - z1) * w1;

      if (flags & INTERP_FLOAT_UV) {
	 /* floating point (perspective correct) texture interpolation */
	 float fu1 = v->u * z1 * 65536.;
	 float fv1 = v->v * z1 * 65536.;

	 info->dfu = (s1->fu - fu1) * w1;
	 info->dfv = (s1->fv - fv1) * w1;
      }
   }
}

/* _clip_polygon_segment:
 *  Fixed point version of _clip_polygon_segment_f().
 */
void _clip_polygon_segment(POLYGON_SEGMENT *info, fixed gap, int flags)
{
   if (flags & INTERP_1COL)
      info->c += fixmul(info->dc, gap);

   if (flags & INTERP_3COL) {
      info->r += fixmul(info->dr, gap);
      info->g += fixmul(info->dg, gap);
      info->b += fixmul(info->db, gap);
   }

   if (flags & INTERP_FIX_UV) {
      info->u += fixmul(info->du, gap);
      info->v += fixmul(info->dv, gap);
   }

   if (flags & INTERP_Z) {
      float gap_f = fixtof(gap);

      info->z += info->dz * gap_f;

      if (flags & INTERP_FLOAT_UV) {
	 info->fu += info->dfu * gap_f;
	 info->fv += info->dfv * gap_f;
      }
   }
}

/* mg3dTriangle:
 *  Draws a 3d triangle.
 */
void mg3dTriangle(HDC mem_dc, int type, HDC texture, mg3dVp *v1, mg3dVp *v2, mg3dVp *v3)
{
   int flags;

   gal_pixel color = v1->c;
   mg3dVp *vt1, *vt2, *vt3;
   POLYGON_EDGE edge1, edge2;
   POLYGON_SEGMENT info;
   SCANLINE_FILLER drawer;

   drawer = _get_scanline_filler(type, &flags, &info, texture, mem_dc);
   if (!drawer)
      return;
   /* sort the vertices so that vt1->y <= vt2->y <= vt3->y */
   if (v1->y > v2->y) {
      vt1 = v2;
      vt2 = v1;
   }
   else {
      vt1 = v1;
      vt2 = v2; }

   if (vt1->y > v3->y) {
      vt3 = vt1;
      vt1 = v3;
   }
   else
      vt3 = v3;

   if (vt2->y > vt3->y) {
      mg3dVp* vtemp = vt2;
      vt2 = vt3;
      vt3 = vtemp;
   }

   /* do 3D triangle*/
   if (_fill_3d_edge_structure(&edge1, vt1, vt3, flags, mem_dc)) {
      /* calculate deltas */
      if (drawer != _poly_scanline_dummy) {
	 fixed w, h;
	 POLYGON_SEGMENT s1 = edge1.dat;

	 h = vt2->y - (edge1.top << 16);
	 _clip_polygon_segment(&s1, h, flags);

	 w = edge1.x + fixmul(h, edge1.dx) - vt2->x;
	 if (w) _triangle_deltas(mem_dc, w, &s1, &info, vt2, flags);
      }

      /* draws part between y1 and y2 */
      if (_fill_3d_edge_structure(&edge2, vt1, vt2, flags, mem_dc))
	 draw_triangle_part(mem_dc, texture, edge2.top, edge2.bottom, &edge1, &edge2, drawer, flags, color, &info);

      /* draws part between y2 and y3 */
      if (_fill_3d_edge_structure(&edge2, vt2, vt3, flags, mem_dc))

	 draw_triangle_part(mem_dc, texture, edge2.top, edge2.bottom, &edge1, &edge2, drawer, flags, color, &info);


   }
   
}

/* mg3dTriangleF:
 *  Draws a 3d triangle.
 */
void mg3dTriangleF(HDC mem_dc , int type, HDC texture , mg3dVpf *v1, mg3dVpf *v2, mg3dVpf *v3)
{
   int flags;

   int color = v1->c;
   mg3dVpf *vt1, *vt2, *vt3;
   POLYGON_EDGE edge1, edge2;
   POLYGON_SEGMENT info;
   SCANLINE_FILLER drawer;

   /* set up the drawing mode */
   drawer = _get_scanline_filler(type, &flags, &info, texture, mem_dc);
   if (!drawer)
      return;

   /* sort the vertices so that vt1->y <= vt2->y <= vt3->y */
   if (v1->y > v2->y) {
      vt1 = v2;
      vt2 = v1;
   }
   else {
      vt1 = v1;
      vt2 = v2;
   }

   if (vt1->y > v3->y) {
      vt3 = vt1;
      vt1 = v3;
   }
   else
      vt3 = v3;

   if (vt2->y > vt3->y) {
      mg3dVpf* vtemp = vt2;
      vt2 = vt3;
      vt3 = vtemp;
   }

   /* do 3D triangle*/
   if (_fill_3d_edge_structure_f(&edge1, vt1, vt3, flags, mem_dc)) {
      /* calculate deltas */
      if (drawer != _poly_scanline_dummy) {
	 fixed w, h;
	 POLYGON_SEGMENT s1 = edge1.dat;

	 h = ftofix(vt2->y) - (edge1.top << 16);
	 _clip_polygon_segment(&s1, h, flags);

	 w = edge1.x + fixmul(h, edge1.dx) - ftofix(vt2->x);
	 if (w) _triangle_deltas_f(mem_dc, w, &s1, &info, vt2, flags);
      }

      /* draws part between y1 and y2 */
      if (_fill_3d_edge_structure_f(&edge2, vt1, vt2, flags, mem_dc))
	 draw_triangle_part(mem_dc, texture, edge2.top, edge2.bottom, &edge1, &edge2, drawer, flags, color, &info);

      /* draws part between y2 and y3 */
      if (_fill_3d_edge_structure_f(&edge2, vt2, vt3, flags,mem_dc))
	 draw_triangle_part(mem_dc, texture, edge2.top, edge2.bottom, &edge1, &edge2, drawer, flags, color, &info);
   }

}

/* mg3dQuad:
 *  Draws a 3d quad.
 */
void mg3dQuad(HDC mem_dc, int type, HDC texture, mg3dVp *v1, mg3dVp *v2, mg3dVp *v3, mg3dVp *v4)
{
   #if (defined MINIGUI_GCC) && (defined MINIGUI_I386)
      /* dodgy assumption alert! See comments for triangle() */
     mg3dPolygon(mem_dc, type, texture, 4, &v1);

   #else
      mg3dVp *vertex[4];

      vertex[0] = v1;
      vertex[1] = v2;
      vertex[2] = v3;
      vertex[3] = v4;
      mg3dPolygon(mem_dc, type, texture, 4, vertex);
   #endif
}

/* mg3dQuadF:
 *  Draws a 3d quad.
 */
void mg3dQuadF(HDC mem_dc, int type, HDC texture, mg3dVpf *v1, mg3dVpf *v2, mg3dVpf *v3, mg3dVpf *v4)
{
   #if (defined MINIGUI_GCC) && (defined MINIGUI_I386)

      /* dodgy assumption alert! See comments for triangle() */
      mg3dPolygonF(mem_dc, type, texture, 4, &v1);

   #else

      mg3dVpf *vertex[4];

      vertex[0] = v1;
      vertex[1] = v2;
      vertex[2] = v3;
      vertex[3] = v4;
      mg3dPolygonF(mem_dc, type, texture, 4, vertex);

   #endif
}

/* mg3dCreateZbuffer:
 *  Creates a new Z-buffer the size of the given bitmap.
 */
MG3DZBUFFER mg3dCreateZbuffer(HDC mem_dc)
{
    unsigned int width = 0;
    unsigned int height = 0;

    width = GetGDCapability(mem_dc, GDCAP_MAXX)+1;
    height = GetGDCapability(mem_dc, GDCAP_MAXY)+1;	

    return CreateMemDC(width, height, 32, MEMDC_FLAG_SRCALPHA, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
}

/* mg3dClearZbuffer:
 *  Clears the given z-buffer, z is the value written in the z-buffer
 *  - it is 1/(z coordinate), z=0 meaning far away.
 */
void mg3dClearZbuffer(MG3DZBUFFER zbuffer, float z)
{
    union {
        float zf;
        long zi;
    } _zbuf_clip;

    _zbuf_clip.zf = z;

    unsigned int width = 0;
    unsigned int height = 0;
    int x,y;

    width = GetGDCapability(zbuffer, GDCAP_MAXX)+1;
    height = GetGDCapability(zbuffer, GDCAP_MAXY)+1;
    for (y = 0; y < height; y++) {
        unsigned long* d = (unsigned long*)bmp_write_line(zbuffer, y);
        for (x = width -1; x >= 0; ((d)++), x--) {
            *(d)= _zbuf_clip.zi;
        }
    }
}

/* mg3dDestroyZbuffer:
 *  Destroys the given z-buffer.
 */
void mg3dDestroyZbuffer(MG3DZBUFFER zbuf)
{
    if (zbuf) {
        if (zbuf == _zbuffer)
            _zbuffer = 0;
        DeleteMemDC(zbuf);
    }
}

/* mg3dSetZbuffer:
 *  Makes polygon drawing routines use the given BITMAP as z-buffer.
 */
void mg3dSetZbuffer(MG3DZBUFFER zbuf)
{
   _zbuffer = zbuf;
}
