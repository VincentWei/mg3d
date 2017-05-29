/*
** $Id: color.c,v 1.5 2006-01-14 05:26:07 xwyan Exp $
**
** Port to MiniGUI by Feynman from allegro.
**
** Copyright (C) 2006 Feynman Software.
 *
 * Color manipulation routines (blending, format conversion, lighting
 * table construction, etc).
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
#include <string.h>
#include <math.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/fixedmath.h>

#include "mg3drw.h"
#include "mg3dintern.h"

/* 1.5k lookup table for color matching */
static unsigned int col_diff[3*128]; 

/* bestfit_init:
 *  Color matching is done with weighted squares, which are much faster
 *  if we pregenerate a little lookup table...
 */
static void bestfit_init(void)
{
    int i;
    
    for (i=1; i<64; i++) {
        int k = i * i;
        
        col_diff[0  +i] = col_diff[0  +128-i] = k * (59 * 59)*4;
        col_diff[128+i] = col_diff[128+128-i] = k * (30 * 30)*4;
        col_diff[256+i] = col_diff[256+128-i] = k * (11 * 11)*4;
    }
}

/* bestfit_color:
 *  Searches a palette for the color closest to the requested R, G, B value.
 */
int bestfit_color(const PALETTE pal, int r, int g, int b)
{
    int i, coldiff, lowest, bestfit;
    
    if (col_diff[1] == 0)
        bestfit_init();
    
    bestfit = 0;
    lowest = INT_MAX;
    
    /* only the transparent (pink) color can be mapped to index 0 */
    if ((r == 63) && (g == 0) && (b == 63))
        i = 0;
    else
        i = 1;
    
    while (i<PAL_SIZE) {
        const GAL_Color *rgb = &pal[i];
        coldiff = (col_diff + 0) [ ((rgb->g)/4 - g) & 0x7F ];
        
        if (coldiff < lowest) {
            coldiff += (col_diff + 128) [ ((rgb->r)/4 - r) & 0x7F ];
            if (coldiff < lowest) {
                coldiff += (col_diff + 256) [ ((rgb->b)/4 - b) & 0x7F ];
                if (coldiff < lowest) {
                    bestfit = rgb - pal;    /* faster than `bestfit = i;' */
                    if (coldiff == 0)
                        return bestfit;
                    lowest = coldiff;
                }
            }
        }
        i++;
    }
    return bestfit;
}

/* mg3dCreateRgbTable:
 *  Fills an MG3D_RGB_MAP lookup table with conversion data for the specified
 *  palette. This is the faster version by Jan Hubicka.
 *
 *  Uses alg. similar to floodfill - it adds one seed per every color in
 *  palette to its best position. Then areas around seed are filled by
 *  same color because it is best approximation for them, and then areas
 *  about them etc...
 *
 *  It does just about 80000 tests for distances and this is about 100
 *  times better than normal 256*32000 tests so the calculation time
 *  is now less than one second at all computers I tested.
 */
void mg3dCreateRgbTable(MG3D_RGB_MAP *table, const PALETTE pal, void (*callback)(int pos))
{
   #define UNUSED 65535
   #define LAST 65532

   /* macro add adds to single linked list */
   #define add(i)    (next[(i)] == UNUSED ? (next[(i)] = LAST, \
		     (first != LAST ? (next[last] = (i)) : (first = (i))), \
		     (last = (i))) : 0)

   /* same but w/o checking for first element */
   #define add1(i)   (next[(i)] == UNUSED ? (next[(i)] = LAST, \
		     next[last] = (i), \
		     (last = (i))) : 0)

   /* calculates distance between two colors */
   #define dist(a1, a2, a3, b1, b2, b3) \
		     (col_diff[ ((a2) - (b2)) & 0x7F] + \
		     (col_diff + 128)[((a1) - (b1)) & 0x7F] + \
		     (col_diff + 256)[((a3) - (b3)) & 0x7F])

   /* converts r,g,b to position in array and back */
   #define pos(r, g, b) \
		     (((r) / 2) * 32 * 32 + ((g) / 2) * 32 + ((b) / 2))

   #define depos(pal, r, g, b) \
		     ((b) = ((pal) & 31) * 2, \
		      (g) = (((pal) >> 5) & 31) * 2, \
		      (r) = (((pal) >> 10) & 31) * 2)

   /* is current color better than pal1? */
   #define better(r1, g1, b1, pal1) \
		     (((int)dist((r1), (g1), (b1), \
				 (pal1).r, (pal1).g, (pal1).b)) > (int)dist2)

   /* checking of position */
   #define dopos(rp, gp, bp, ts) \
      if ((rp > -1 || r > 0) && (rp < 1 || r < 61) && \
	  (gp > -1 || g > 0) && (gp < 1 || g < 61) && \
	  (bp > -1 || b > 0) && (bp < 1 || b < 61)) { \
	 i = first + rp * 32 * 32 + gp * 32 + bp; \
         if (!data[i]) { \
	    data[i] = val; \
	    add1(i); \
	 } \
	 else if ((ts) && (data[i] != val)) { \
	    dist2 = (rp ? (col_diff+128)[(r+2*rp-pal[val].r) & 0x7F] : r2) + \
		    (gp ? (col_diff    )[(g+2*gp-pal[val].g) & 0x7F] : g2) + \
		    (bp ? (col_diff+256)[(b+2*bp-pal[val].b) & 0x7F] : b2); \
	    if (better((r+2*rp), (g+2*gp), (b+2*bp), pal[data[i]])) { \
	       data[i] = val; \
	       add1(i); \
	    } \
	 } \
      }

   int i, curr, r, g, b, val, dist2;
   unsigned int r2, g2, b2;
   unsigned short next[32*32*32];
   unsigned char *data;
   int first = LAST;
   int last = LAST;
   int count = 0;
   int cbcount = 0;

   #define AVERAGE_COUNT   18000

   if (col_diff[1] == 0)
      bestfit_init();

   memset(next, 255, sizeof(next));
   memset(table->data, 0, sizeof(char)*32*32*32);

   data = (unsigned char *)table->data;

   /* add starting seeds for floodfill */
   for (i=1; i<PAL_SIZE; i++) { 
      curr = pos(pal[i].r, pal[i].g, pal[i].b);
      if (next[curr] == UNUSED) {
	 data[curr] = i;
	 add(curr);
      }
   }

   /* main floodfill: two versions of loop for faster growing in blue axis */
   while (first != LAST) { 
      depos(first, r, g, b);

      /* calculate distance of current color */
      val = data[first];
      r2 = (col_diff+128)[((pal[val].r)-(r)) & 0x7F];
      g2 = (col_diff    )[((pal[val].g)-(g)) & 0x7F];
      b2 = (col_diff+256)[((pal[val].b)-(b)) & 0x7F];

      /* try to grow to all directions */
      dopos( 0, 0, 1, 1); 
      dopos( 0, 0,-1, 1);
      dopos( 1, 0, 0, 1);
      dopos(-1, 0, 0, 1);
      dopos( 0, 1, 0, 1);
      dopos( 0,-1, 0, 1);

      /* faster growing of blue direction */
      if ((b > 0) && (data[first-1] == val)) { 
	 b -= 2;
	 first--;
	 b2 = (col_diff+256)[((pal[val].b)-(b)) & 0x7F];

	 dopos(-1, 0, 0, 0);
	 dopos( 1, 0, 0, 0);
	 dopos( 0,-1, 0, 0);
	 dopos( 0, 1, 0, 0);

	 first++;
      }

      /* get next from list */
      i = first; 
      first = next[first];
      next[i] = UNUSED;

      /* second version of loop */
      if (first != LAST) { 
	 depos(first, r, g, b);

	 val = data[first];
	 r2 = (col_diff+128)[((pal[val].r)-(r)) & 0x7F];
	 g2 = (col_diff    )[((pal[val].g)-(g)) & 0x7F];
	 b2 = (col_diff+256)[((pal[val].b)-(b)) & 0x7F];

	 dopos( 0, 0, 1, 1);
	 dopos( 0, 0,-1, 1);
	 dopos( 1, 0, 0, 1);
	 dopos(-1, 0, 0, 1);
	 dopos( 0, 1, 0, 1);
	 dopos( 0,-1, 0, 1);

	 if ((b < 61) && (data[first + 1] == val)) {
	    b += 2;
	    first++;
	    b2 = (col_diff+256)[((pal[val].b)-(b)) & 0x7f];

	    dopos(-1, 0, 0, 0);
	    dopos( 1, 0, 0, 0);
	    dopos( 0,-1, 0, 0);
	    dopos( 0, 1, 0, 0);

	    first--;
	 }

	 i = first;
	 first = next[first];
	 next[i] = UNUSED;
      }

      count++;
      if (count == (cbcount+1)*AVERAGE_COUNT/256) {
	 if (cbcount < 256) {
	    if (callback)
	       callback(cbcount);
	    cbcount++;
	 }
      }
   }

   /* only the transparent (pink) color can be mapped to index 0 */
   if ((pal[0].r == 63) && (pal[0].g == 0) && (pal[0].b == 63))
      table->data[31][0][31] = 0;

   if (callback)
      while (cbcount < 256)
	 callback(cbcount++);
}

/*  mg3dCreateLightTable:
 *  Constructs a lighting color table for the specified palette. At light
 *  intensity 255 the table will produce the palette colors directly, and
 *  at level 0 it will produce the specified R, G, B value for all colors
 *  (this is specified in 0-63 VGA format). If the callback function is 
 *  not NULL, it will be called 256 times during the calculation, allowing
 *  you to display a progress indicator.
 */
void mg3dCreateLightTable(MG3D_COLOR_MAP *table, const PALETTE pal, int r, int g, int b, void (*callback)(int pos))
{
    int r1, g1, b1, r2, g2, b2, x, y;
    unsigned int tmp;
    r =r/4;
    g =g/4;
    b =b/4;
    if (rgb_map) {
        for (x=0; x<PAL_SIZE-1; x++) {
            tmp = (255 - x) * 65793;
            r1 = r * tmp + (1 << 24);
            g1 = g * tmp + (1 << 24);
            b1 = b * tmp + (1 << 24);
            
            tmp = (1 << 24) - tmp;
            for (y=0; y<PAL_SIZE; y++) {
                r2 = (r1 + (unsigned int)pal[y].r * tmp) >> 25;
                g2 = (g1 + (unsigned int)pal[y].g * tmp) >> 25;
                b2 = (b1 + (unsigned int)pal[y].b * tmp) >> 25;
                
                table->data[x][y] = rgb_map->data[r2][g2][b2];
            }
        }
        
        if (callback)
            (*callback)(x);
    }
    else {
        for (x=0; x<PAL_SIZE-1; x++) {
            tmp = (255 - x) * 65793;
            r1 = r * tmp + (1 << 23);
            g1 = g * tmp + (1 << 23);
            b1 = b * tmp + (1 << 23);
            
            tmp = (1 << 24) - tmp;
            for (y=0; y<PAL_SIZE; y++) {
                r2 = (r1 + ((unsigned int)pal[y].r)/4 * tmp) >> 24;
                g2 = (g1 + ((unsigned int)pal[y].g)/4 * tmp) >> 24;
                b2 = (b1 + ((unsigned int)pal[y].b)/4 * tmp) >> 24;
                
                table->data[x][y] = bestfit_color(pal, r2, g2, b2);
            }
        }
        if (callback)
            (*callback)(x);
    }
    
    for (y=0; y<PAL_SIZE; y++)
        table->data[255][y] = y;
}

/*  mg3dCreateTransTable:
 *  Constructs a translucency color table for the specified palette. The
 *  r, g, and b parameters specifiy the solidity of each color component,
 *  ranging from 0 (totally transparent) to 255 (totally solid). Source
 *  color #0 is a special case, and is set to leave the destination 
 *  unchanged, so that masked sprites will draw correctly. If the callback 
 *  function is not NULL, it will be called 256 times during the calculation, 
 *  allowing you to display a progress indicator.
 */
void mg3dCreateTransTable(MG3D_COLOR_MAP *table, const PALETTE pal, int r, int g, int b, void (*callback)(int pos))
{
   int tmp[768], *q;
   int x, y, i, j, k;
   unsigned char *p;
   GAL_Color c;

   for (x=0; x<256; x++) {
      tmp[x*3]   = pal[x].r * (255-r) / 255/4;
      tmp[x*3+1] = pal[x].g * (255-g) / 255/4;
      tmp[x*3+2] = pal[x].b * (255-b) / 255/4;
   }

   for (y=0; y<PAL_SIZE; y++)
      table->data[0][y] = y;

   if (callback)
      (*callback)(0);

   for (x=1; x<PAL_SIZE; x++) {
      i = pal[x].r * r / 255/4;
      j = pal[x].g * g / 255/4;
      k = pal[x].b * b / 255/4;

      p = table->data[x];
      q = tmp;

      if (rgb_map) {
	 for (y=0; y<PAL_SIZE; y++) {
	    c.r = (i + *(q++))*4;
	    c.g = (j + *(q++))*4;
	    c.b = (k + *(q++))*4;
	    p[y] = rgb_map->data[c.r>>1][c.g>>1][c.b>>1];
	 }
      }
      else {
	 for (y=0; y<PAL_SIZE; y++) {
	    c.r = (i + *(q++))*4; 
	    c.g = (j + *(q++))*4; 
	    c.b = (k + *(q++))*4;
	    p[y] = bestfit_color(pal, c.r, c.g, c.b);
	 }
      }

      if (callback)
	 (*callback)(x);
   }
}

/*  mg3dCreateColorTable:
 *  Creates a color mapping table, using a user-supplied callback to blend
 *  each pair of colors. Your blend routine will be passed a pointer to the
 *  palette and the two colors to be blended (x is the source color, y is
 *  the destination), and should return the desired output RGB for this
 *  combination. If the callback function is not NULL, it will be called 
 *  256 times during the calculation, allowing you to display a progress 
 *  indicator.
 */
void mg3dCreateColorTable(MG3D_COLOR_MAP *table, const PALETTE pal, void (*blend)(const PALETTE pal, int x, int y, GAL_Color *rgb), void (*callback)(int pos))
{
   int x, y;
   GAL_Color c;

   for (x=0; x<PAL_SIZE; x++) {
      for (y=0; y<PAL_SIZE; y++) {
	 blend(pal, x, y, &c);

	 if (rgb_map)
	    table->data[x][y] = rgb_map->data[c.r>>1][c.g>>1][c.b>>1];
	 else
	    table->data[x][y] = bestfit_color(pal, c.r, c.g, c.b);
      }

      if (callback)
	 (*callback)(x);
   }
}



