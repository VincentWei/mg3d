/*     
** $Id: mg3dgfx.h,v 1.3 2006-01-14 05:49:25 xwyan Exp $
**
** Port to MiniGUI by Feynman from allegro.
**
** Copyright (C) 2006 Feynman Software.
**
** Graphics inline functions (generic C).

** Allegro is a gift-ware.
*/


#ifndef __mg3D_GFX_H__
#define __mg3D_GFX_H__

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>



#define DRAW_MODE_SOLID             0        /* flags for drawing_mode() */
#define DRAW_MODE_XOR               1
#define DRAW_MODE_COPY_PATTERN      2
#define DRAW_MODE_SOLID_PATTERN     3
#define DRAW_MODE_MASKED_PATTERN    4
#define DRAW_MODE_TRANS             5


#ifdef __cplusplus
   extern "C" {
#endif

/* info about the current graphics drawing mode */
extern int _drawing_mode;
extern HDC _drawing_pattern;
extern int _drawing_x_anchor;
extern int _drawing_y_anchor;
extern unsigned int _drawing_x_mask ;
extern unsigned int _drawing_y_mask ;


unsigned long bmp_write_line(HDC mem_dc, int line);


unsigned long bmp_read_line(HDC mem_dc, int line);


#ifdef __cplusplus
   }
#endif

#endif          /* ifndef __mg3D_GFX_H__ */


