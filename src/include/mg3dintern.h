/*     
** $Id: mg3dintern.h,v 1.6 2006-01-14 05:49:25 xwyan Exp $
**
** Port to MiniGUI by Feynman from allegro.
**
** Copyright (C) 2006 Feynman Software.
**
 * Some definitions for internal use by the library code.
 *
** Allegro is a gift-ware.
*/

#ifndef __mg3D_AINTERN_H__
#define __mg3D_AINTERN_H__

#include "mg3drw.h"
#include "mg3d.h"
#include "mg3dcolor.h"


#ifdef MINIGUI_I386
   #define BYTES_PER_PIXEL(bpp)     (((int)(bpp) + 7) / 8)
#else
   #ifdef MINIGUI_MPW 
      /* in Mac 24 bit is a unsigned long */
      #define BYTES_PER_PIXEL(bpp)  (((bpp) <= 8) ? 1                                    \
				     : (((bpp) <= 16) ? sizeof (unsigned short)          \
					: sizeof (unsigned long)))
   #else
      #define BYTES_PER_PIXEL(bpp)  (((bpp) <= 8) ? 1                                    \
				     : (((bpp) <= 16) ? sizeof (unsigned short)          \
					: (((bpp) <= 24) ? 3 : sizeof (unsigned long))))
   #endif
#endif



/* default truecolor pixel format */
#define DEFAULT_RGB_R_SHIFT_15  0
#define DEFAULT_RGB_G_SHIFT_15  5
#define DEFAULT_RGB_B_SHIFT_15  10
#define DEFAULT_RGB_R_SHIFT_16  0
#define DEFAULT_RGB_G_SHIFT_16  5
#define DEFAULT_RGB_B_SHIFT_16  11
#define DEFAULT_RGB_R_SHIFT_24  0
#define DEFAULT_RGB_G_SHIFT_24  8
#define DEFAULT_RGB_B_SHIFT_24  16
#define DEFAULT_RGB_R_SHIFT_32  0
#define DEFAULT_RGB_G_SHIFT_32  8
#define DEFAULT_RGB_B_SHIFT_32  16
#define DEFAULT_RGB_A_SHIFT_32  24



/* current drawing mode */
//extern int _drawing_mode;
//extern HDC  _drawing_pattern;
//extern int _drawing_x_anchor;
//extern int _drawing_y_anchor;
//extern unsigned int _drawing_x_mask;
//extern unsigned int _drawing_y_mask;

extern int _textmode;

extern int (* _palette_expansion_table)(int bpp);

extern int _color_depth;

extern int _current_palette_changed;

extern PALETTE _prev_current_palette;

extern int _got_prev_current_palette;

extern int _palette_color8[];
extern int _palette_color15[];
extern int _palette_color16[];
extern int _palette_color24[];
extern int _palette_color32[];

/* truecolor blending functions */
extern BLENDER_FUNC _blender_func15;
extern BLENDER_FUNC _blender_func16;
extern BLENDER_FUNC _blender_func24;
extern BLENDER_FUNC _blender_func32;

extern BLENDER_FUNC _blender_func15x;
extern BLENDER_FUNC _blender_func16x;
extern BLENDER_FUNC _blender_func24x;

extern int _blender_col_15;
extern int _blender_col_16;
extern int _blender_col_24;
extern int _blender_col_32;

extern int _blender_alpha;

unsigned long _blender_black (unsigned long x, unsigned long y, unsigned long n);

#ifdef MINIGUI_COLOR16

unsigned long _blender_trans15(unsigned long x, unsigned long y, unsigned long n);
unsigned long _blender_color15(unsigned long x, unsigned long y, unsigned long n);

unsigned long _blender_trans16(unsigned long x, unsigned long y, unsigned long n);
unsigned long _blender_screen16(unsigned long x, unsigned long y, unsigned long n);

#endif

#if (defined MINIGUI_COLOR24) || (defined MINIGUI_COLOR32)

unsigned long _blender_trans24(unsigned long x, unsigned long y, unsigned long n);
unsigned long _blender_color24(unsigned long x, unsigned long y, unsigned long n);
unsigned long _blender_screen24(unsigned long x, unsigned long y, unsigned long n);

#endif

unsigned long _blender_alpha15(unsigned long x, unsigned long y, unsigned long n);
unsigned long _blender_alpha16(unsigned long x, unsigned long y, unsigned long n);
unsigned long _blender_alpha24(unsigned long x, unsigned long y, unsigned long n);
unsigned long _blender_alpha32(unsigned long x, unsigned long y, unsigned long n);

unsigned long _blender_write_alpha(unsigned long x, unsigned long y, unsigned long n);

/* bitfield specifying which polygon attributes need interpolating */
#define INTERP_FLAT           1      /* no interpolation */
#define INTERP_1COL           2      /* gcol or alpha */
#define INTERP_3COL           4      /* grgb */
#define INTERP_FIX_UV         8     /* atex */
#define INTERP_Z              16     /* always in scene3d */
#define INTERP_FLOAT_UV       32     /* ptex */
#define OPT_FLOAT_UV_TO_FIX   64     /* translate ptex to atex */
#define COLOR_TO_RGB          128   /* grgb to gcol for truecolor */
#define INTERP_ZBUF           256    /* z-buffered */
#define INTERP_THRU           512    /* any kind of transparent */
#define INTERP_NOSOLID        1024   /* non-solid modes for 8-bit flat */
#define INTERP_BLEND          2048  /* lit for truecolor */
#define INTERP_TRANS          4096   /* trans for truecolor */


/* information for polygon scanline fillers */
typedef struct POLYGON_SEGMENT
{
   fixed u, v, du, dv;              /* fixed point u/v coordinates */
   fixed c, dc;                     /* single color gouraud shade values */
   fixed r, g, b, dr, dg, db;       /* RGB gouraud shade values */
   float z, dz;                     /* polygon depth (1/z) */
   float fu, fv, dfu, dfv;          /* floating point u/v coordinates */
   unsigned char *texture;          /* the texture map */
   int umask, vmask, vshift;        /* texture map size information */
   int seg;                         /* destination bitmap selector */
   unsigned long zbuf_addr;	    /* Z-buffer address */
   unsigned long read_addr;	    /* reading address for transparency modes */
} POLYGON_SEGMENT;


/* prototype for the scanline filler functions */
typedef void (*SCANLINE_FILLER)(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);


/* an active polygon edge */
typedef struct POLYGON_EDGE 
{
   int top;                         /* top y position */
   int bottom;                      /* bottom y position */
   fixed x, dx;                     /* fixed point x position and gradient */
   fixed w;                         /* width of line segment */
   POLYGON_SEGMENT dat;             /* texture/gouraud information */
   struct POLYGON_EDGE *prev;       /* doubly linked list */
   struct POLYGON_EDGE *next;
   struct POLYGON_INFO *poly;	    /* father polygon */
} POLYGON_EDGE;


typedef struct POLYGON_INFO         /* a polygon waiting rendering */
{
   struct POLYGON_INFO *next, *prev;/* double linked list */
   int inside;                      /* flag for "scanlining" */
   int flags;                       /* INTERP_* flags */
   int color;                       /* vtx[0]->c */
   float a, b, c;                   /* plane's coefficients -a/d, -b/d, -c/d */
   int dmode;                       /* drawing mode */
   HDC dpat;                         /* drawing pattern */
   int xanchor, yanchor;            /* for dpat */
   int alpha;                       /* blender alpha */
   int b15, b16, b24, b32;          /* blender colors */
   MG3D_COLOR_MAP *cmap;                 /* trans color map */
   SCANLINE_FILLER drawer;	    /* scanline drawing functions */
   SCANLINE_FILLER alt_drawer; 
   POLYGON_EDGE *left_edge;	    /* true edges used in interpolation */
   POLYGON_EDGE *right_edge;
   POLYGON_SEGMENT info;            /* base information for scanline functions */
} POLYGON_INFO;


/* global variable for z-buffer */
extern HDC _zbuffer;

/* polygon helper functions */
extern SCANLINE_FILLER _optim_alternative_drawer;
POLYGON_EDGE * _add_edge(POLYGON_EDGE *list, POLYGON_EDGE *edge, int sort_by_x);
POLYGON_EDGE * _remove_edge(POLYGON_EDGE *list, POLYGON_EDGE *edge);

int _fill_3d_edge_structure(POLYGON_EDGE *edge, const mg3dVp *v1, const mg3dVp *v2, int flags, HDC mem_dc);

int _fill_3d_edge_structure_f(POLYGON_EDGE *edge, const mg3dVpf *v1, const mg3dVpf *v2, int flags,HDC mem_hdc);

SCANLINE_FILLER _get_scanline_filler(int type, int *flags, POLYGON_SEGMENT *info, HDC texture, HDC mem_hdc );

void _clip_polygon_segment(POLYGON_SEGMENT *info, fixed gap, int flags);

void _clip_polygon_segment_f(POLYGON_SEGMENT *info, int gap, int flags);


/* polygon scanline filler functions */
void _poly_scanline_dummy(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);

#ifdef MINIGUI_COLOR8

void _poly_scanline_gcol8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_grgb8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_lit8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_lit8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask_lit8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask_lit8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_trans8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_trans8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask_trans8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask_trans8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);


void _poly_zbuf_flat8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_gcol8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_grgb8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_lit8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_lit8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask_lit8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask_lit8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_trans8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_trans8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask_trans8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask_trans8(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);

#endif

#ifdef MINIGUI_COLOR16

void _poly_scanline_grgb15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_lit15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_lit15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask_lit15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask_lit15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_trans15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_trans15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask_trans15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask_trans15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);

void _poly_zbuf_grgb15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_lit15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_lit15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask_lit15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask_lit15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_trans15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_trans15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask_trans15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask_trans15(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);

void _poly_scanline_grgb16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_lit16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_lit16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask_lit16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask_lit16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_trans16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_trans16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask_trans16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask_trans16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);

void _poly_zbuf_flat16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_grgb16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_lit16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_lit16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask_lit16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask_lit16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_trans16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_trans16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask_trans16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask_trans16(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);

#endif

#ifdef MINIGUI_COLOR24

void _poly_scanline_grgb24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_lit24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_lit24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask_lit24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask_lit24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_trans24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_trans24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask_trans24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask_trans24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);


void _poly_zbuf_flat24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_grgb24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_lit24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_lit24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask_lit24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask_lit24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_trans24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_trans24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask_trans24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask_trans24(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);

#endif

#ifdef MINIGUI_COLOR32

void _poly_scanline_grgb32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_lit32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_lit32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask_lit32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask_lit32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_trans32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_trans32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_atex_mask_trans32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_scanline_ptex_mask_trans32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);


void _poly_zbuf_flat32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_grgb32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_lit32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_lit32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask_lit32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask_lit32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_trans32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_trans32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_atex_mask_trans32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);
void _poly_zbuf_ptex_mask_trans32(HDC mem_dc, unsigned long addr, int w, POLYGON_SEGMENT *info);

#endif

#ifdef __cplusplus
   }
#endif

#endif          /* ifndef __mg3D_AINTERN_H__ */

