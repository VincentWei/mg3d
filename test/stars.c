/*
**    Example program for Minigui immigrated from example of Allegro, by Yang Yan.
**
**    This program draws a 3D starfield (depth-cued) and a polygon starship (controllable).
**
** Copyright (C) 2005 Feynman Software.
**
** License: GPL
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/fixedmath.h>

#ifdef __MG3D_LIB__
#include "mg3d.h"
#include "mg3dcolor.h"
#include "mg3dmatrix.h"
#include "mg3dbase.h"
#include "mg3dmaths.h"
#else
#include <mg3d/mg3d.h>
#include <mg3d/mg3dcolor.h>
#include <mg3d/mg3dmatrix.h>
#include <mg3d/mg3dbase.h>
#include <mg3d/mg3dmaths.h>
#endif

#ifdef MINIGUI_COLOR8
#define DEPTH            8
#endif

#ifdef MINIGUI_COLOR16
#define DEPTH           16 
#endif

#ifdef MINIGUI_COLOR24
#define DEPTH           24 
#endif

#ifdef MINIGUI_COLOR32
#define DEPTH           32 
#endif

#define SCREEN_W	640
#define SCREEN_H	480

/* starfield system */
typedef struct VECTOR 
{
   fixed x, y, z;
} VECTOR;


#define NUM_STARS          512

#define Z_NEAR             24
#define Z_FAR              1024
#define XY_CUBE            2048

#define SPEED_LIMIT        20

VECTOR stars[NUM_STARS];

fixed star_x[NUM_STARS];
fixed star_y[NUM_STARS];

VECTOR delta;


GAL_Color pal[256];
/* polygonal models */
#define NUM_VERTS          4
#define NUM_FACES          4

#define ENGINE             3     /* which face is the engine */
#define ENGINE_ON          64    /* colour index */
#define ENGINE_OFF         32

typedef struct FACE              /* for triangular models */
{
   int v1, v2, v3;
   int colour, range;
   VECTOR normal, rnormal;
} FACE;

typedef struct MODEL 
{
   VECTOR points[NUM_VERTS];
   FACE faces[NUM_FACES];
   fixed x, y, z;
   fixed rx, ry, rz;
   int minx, miny, maxx, maxy;
   VECTOR aim;
   int velocity;
} MODEL;

MODEL ship;
VECTOR direction;


/* initialises the starfield system */
void init_stars(void)
{
   int i;

   for (i=0; i<NUM_STARS; i++) {
      stars[i].x = itofix((rand() % XY_CUBE) - (XY_CUBE >> 1));
      stars[i].y = itofix((rand() % XY_CUBE) - (XY_CUBE >> 1));
      stars[i].z = itofix((rand() % (Z_FAR - Z_NEAR)) + Z_NEAR);
   }

   delta.x = 0;
   delta.y = 0;
   delta.z = 0;
}

/* draws the starfield */
void draw_stars(HDC hdc)
{
   int i, c;
   mg3dMatrix m;
   gal_pixel pixel;
   VECTOR outs[NUM_STARS];

   for (i=0; i<NUM_STARS; i++) {
      mg3dGetTranslationMatrix(&m, delta.x, delta.y, delta.z);
      mg3dApplyMatrix(&m, stars[i].x, stars[i].y, stars[i].z, &outs[i].x, &outs[i].y, &outs[i].z);
      mg3dPerspProject(outs[i].x, outs[i].y, outs[i].z, &star_x[i], &star_y[i]);
      c = (fixtoi(outs[i].z) >> 8) + 16;
      pixel = RGB2Pixel (hdc, pal[c].r, pal[c].g, pal[c].b);
      SetPixel(hdc, fixtoi(star_x[i]), fixtoi(star_y[i]), pixel);
   }
}

/* deletes the stars from the screen */
void erase_stars(HDC hdc)
{
   int i;
   gal_pixel pixel;

   for (i=0; i<NUM_STARS; i++){
      pixel = RGB2Pixel (hdc, pal[0].r, pal[0].g, pal[0].b);
      SetPixel(hdc, fixtoi(star_x[i]), fixtoi(star_y[i]), pixel);
   }
}

/* moves the stars */
void move_stars(void)
{
   int i;

   for (i=0; i<NUM_STARS; i++) {
      stars[i].x += delta.x;
      stars[i].y += delta.y;
      stars[i].z += delta.z;

      if (stars[i].x > itofix(XY_CUBE >> 1))
	 stars[i].x = itofix(-(XY_CUBE >> 1));
      else if (stars[i].x < itofix(-(XY_CUBE >> 1)))
	 stars[i].x = itofix(XY_CUBE >> 1);

      if (stars[i].y > itofix(XY_CUBE >> 1))
	 stars[i].y = itofix(-(XY_CUBE >> 1));
      else if (stars[i].y < itofix(-(XY_CUBE >> 1)))
	 stars[i].y = itofix(XY_CUBE >> 1);

      if (stars[i].z > itofix(Z_FAR))
	 stars[i].z = itofix(Z_NEAR);
      else if (stars[i].z < itofix(Z_NEAR))
	 stars[i].z = itofix(Z_FAR);
   }
}

/* initialises the ship model */
void init_ship(void)
{
   VECTOR v1, v2, *pts;
   FACE *face;
   int i;

   ship.points[0].x = itofix(0);
   ship.points[0].y = itofix(0);
   ship.points[0].z = itofix(32);

   ship.points[1].x = itofix(16);
   ship.points[1].y = itofix(-16);
   ship.points[1].z = itofix(-32);

   ship.points[2].x = itofix(-16);
   ship.points[2].y = itofix(-16);
   ship.points[2].z = itofix(-32);

   ship.points[3].x = itofix(0);
   ship.points[3].y = itofix(16);
   ship.points[3].z = itofix(-32);

   ship.faces[0].v1 = 3;
   ship.faces[0].v2 = 0;
   ship.faces[0].v3 = 1;
   pts = &ship.points[0];
   face = &ship.faces[0];
   v1.x = (pts[face->v2].x - pts[face->v1].x);
   v1.y = (pts[face->v2].y - pts[face->v1].y);
   v1.z = (pts[face->v2].z - pts[face->v1].z);
   v2.x = (pts[face->v3].x - pts[face->v1].x);
   v2.y = (pts[face->v3].y - pts[face->v1].y);
   v2.z = (pts[face->v3].z - pts[face->v1].z);
   mg3dCrossProduct(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, &(face->normal.x), &(face->normal.y), &(face->normal.z));

   ship.faces[1].v1 = 2;
   ship.faces[1].v2 = 0;
   ship.faces[1].v3 = 3;
   face = &ship.faces[1];
   v1.x = (pts[face->v2].x - pts[face->v1].x);
   v1.y = (pts[face->v2].y - pts[face->v1].y);
   v1.z = (pts[face->v2].z - pts[face->v1].z);
   v2.x = (pts[face->v3].x - pts[face->v1].x);
   v2.y = (pts[face->v3].y - pts[face->v1].y);
   v2.z = (pts[face->v3].z - pts[face->v1].z);
   mg3dCrossProduct(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, &(face->normal.x), &(face->normal.y), &(face->normal.z));

   ship.faces[2].v1 = 1;
   ship.faces[2].v2 = 0;
   ship.faces[2].v3 = 2;
   face = &ship.faces[2];
   v1.x = (pts[face->v2].x - pts[face->v1].x);
   v1.y = (pts[face->v2].y - pts[face->v1].y);
   v1.z = (pts[face->v2].z - pts[face->v1].z);
   v2.x = (pts[face->v3].x - pts[face->v1].x);
   v2.y = (pts[face->v3].y - pts[face->v1].y);
   v2.z = (pts[face->v3].z - pts[face->v1].z);
   mg3dCrossProduct(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, &(face->normal.x), &(face->normal.y), &(face->normal.z));

   ship.faces[3].v1 = 2;
   ship.faces[3].v2 = 3;
   ship.faces[3].v3 = 1;
   face = &ship.faces[3];
   v1.x = (pts[face->v2].x - pts[face->v1].x);
   v1.y = (pts[face->v2].y - pts[face->v1].y);
   v1.z = (pts[face->v2].z - pts[face->v1].z);
   v2.x = (pts[face->v3].x - pts[face->v1].x);
   v2.y = (pts[face->v3].y - pts[face->v1].y);
   v2.z = (pts[face->v3].z - pts[face->v1].z);
   mg3dCrossProduct(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, &(face->normal.x), &(face->normal.y), &(face->normal.z));

   for (i=0; i<NUM_FACES; i++) {
      ship.faces[i].colour = 32;
      ship.faces[i].range = 15;
      mg3dNormalVector(&ship.faces[i].normal.x, &ship.faces[i].normal.y, &ship.faces[i].normal.z);
      ship.faces[i].rnormal.x = ship.faces[i].normal.x;
      ship.faces[i].rnormal.y = ship.faces[i].normal.y;
      ship.faces[i].rnormal.z = ship.faces[i].normal.z;
   }

   ship.x = ship.y = 0;
   ship.z = itofix(192);
   ship.rx = ship.ry = ship.rz = 0;

   ship.aim.x = direction.x = 0;
   ship.aim.y = direction.y = 0;
   ship.aim.z = direction.z = itofix(-1);
   ship.velocity = 0;
}

/* triangle:
 *  Draws a filled triangle between the three points.
 */
void triangle(HDC hdc, int x1, int y1, int x2, int y2, int x3, int y3, int r, int g, int b)
{
    gal_pixel color;
    color = RGB2Pixel (hdc, r, g, b);
    /* portable version for other platforms */
    POINT pts[]={
        { x1, y1},
        { x2, y2},
        { x3, y3},
    };
    SetBrushColor (hdc, color);    
    FillPolygon(hdc, pts, 3);
}


/* draws the ship model */
void draw_ship(HDC hdc)
{
   VECTOR outs[NUM_VERTS];
   mg3dMatrix m;
   int i, col;
   int r, g, b;

   ship.minx = SCREEN_W;
   ship.miny = SCREEN_H;
   ship.maxx = ship.maxy = 0;

   mg3dGetRotMatrix(&m, ship.rx, ship.ry, ship.rz);
   mg3dApplyMatrix(&m, ship.aim.x, ship.aim.y, ship.aim.z, &outs[0].x, &outs[0].y, &outs[0].z);
   direction.x = outs[0].x;
   direction.y = outs[0].y;
   direction.z = outs[0].z;

   for (i=0; i<NUM_FACES; i++)
      mg3dApplyMatrix(&m, ship.faces[i].normal.x, ship.faces[i].normal.y, ship.faces[i].normal.z, &ship.faces[i].rnormal.x, &ship.faces[i].rnormal.y, &ship.faces[i].rnormal.z);

   mg3dGetTransformMatrix(&m, itofix(1), ship.rx, ship.ry, ship.rz, ship.x, ship.y, ship.z);

   for (i=0; i<NUM_VERTS; i++) {
      mg3dApplyMatrix(&m, ship.points[i].x, ship.points[i].y, ship.points[i].z, &outs[i].x, &outs[i].y, &outs[i].z);
      mg3dPerspProject(outs[i].x, outs[i].y, outs[i].z, &outs[i].x, &outs[i].y);
      if (fixtoi(outs[i].x) < ship.minx)
         ship.minx = fixtoi(outs[i].x);
      if (fixtoi(outs[i].x) > ship.maxx)
         ship.maxx = fixtoi(outs[i].x);
      if (fixtoi(outs[i].y) < ship.miny)
         ship.miny = fixtoi(outs[i].y);
      if (fixtoi(outs[i].y) > ship.maxy)
         ship.maxy = fixtoi(outs[i].y);
   }

   for (i=0; i<NUM_FACES; i++) {
      if (fixtof(ship.faces[i].rnormal.z) < 0.0) {
	 col = fixtoi(fixmul(mg3dDotProduct(ship.faces[i].rnormal.x, ship.faces[i].rnormal.y, ship.faces[i].rnormal.z, 0, 0, itofix(1)), itofix(ship.faces[i].range)));
	 if (col < 0)
	    col = -col + ship.faces[i].colour;
	 else
	    col = col + ship.faces[i].colour;
    r = pal[col].r;
    g = pal[col].g;
    b = pal[col].b;    
    triangle(hdc, fixtoi(outs[ship.faces[i].v1].x), fixtoi(outs[ship.faces[i].v1].y),
		  fixtoi(outs[ship.faces[i].v2].x), fixtoi(outs[ship.faces[i].v2].y),
		  fixtoi(outs[ship.faces[i].v3].x), fixtoi(outs[ship.faces[i].v3].y),
		  r, g, b);
      }
   }
}


/* removes the ship model from the screen */
void erase_ship(HDC hdc)
{
    gal_pixel pixel;
    pixel = RGB2Pixel (hdc, pal[0].r, pal[0].g, pal[0].b);
    SetBrushColor(hdc, pixel);
   FillBox (hdc, ship.minx, ship.miny, ship.maxx, ship.maxy);
}



static int StarsWndProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    //char buf[64];
    int i;
    HDC hdc;
    static HDC mem_dc;

	switch ( message ) {
	    case MSG_CREATE:
            mem_dc = CreateMemDC (SCREEN_W, SCREEN_H, DEPTH, 
                            MEMDC_FLAG_SWSURFACE, 0x00, 0x00, 0x00, 0x00);
            
            for (i=0; i<16; i++)
                pal[i].r = pal[i].g = pal[i].b = 0;

            /* greyscale */
            pal[16].r = pal[16].g = pal[16].b = 63*4;
            pal[17].r = pal[17].g = pal[17].b = 48*4;
            pal[18].r = pal[18].g = pal[18].b = 32*4;
            pal[19].r = pal[19].g = pal[19].b = 16*4;
            pal[20].r = pal[20].g = pal[20].b = 8*4;

            /* red range */
            for (i=0; i<16; i++) { 
                pal[i+32].r = (31 + i*2)*4;
                pal[i+32].g = 15*4;
                pal[i+32].b = 7*4;
            }

            /* a nice fire orange */
            for (i=64; i<68; i++) {
                pal[i].r = 63*4; 
                pal[i].g = (17 + (i-64)*3)*4;
                pal[i].b = 0;
            }
            
            
            SetPalette(mem_dc, 0, 255, pal);
       	    SetBrushColor (mem_dc, PIXEL_black);
            FillBox (mem_dc, 0, 0, SCREEN_W, SCREEN_H);

            mg3dSetProjectionViewport(0, 0, SCREEN_W, SCREEN_H);

            init_stars();
            draw_stars(mem_dc);
            init_ship();
            draw_ship(mem_dc);

		    break;
        case MSG_PAINT:
            hdc =BeginPaint(hWnd);
            erase_stars(mem_dc);
            erase_ship(mem_dc);
            SetBrushColor(mem_dc, PIXEL_black);
            FillBox(mem_dc, 0, 0, SCREEN_W, SCREEN_H);
            move_stars();
            draw_stars(mem_dc);
            
	        SetBkMode(mem_dc, BM_TRANSPARENT);
#if 0            
            SetTextColor (mem_dc, RGBA2Pixel (mem_dc, 0xFF, 0xFF, 0xFF, 0x80));
            sprintf(buf, "direction: [%f] [%f] [%f]", fixtof(direction.x), fixtof(direction.y), fixtof(direction.z));
            TextOut(mem_dc, 0, 0, buf);
            sprintf(buf, "delta: [%f] [%f] [%f]", fixtof(delta.x), fixtof(delta.y), fixtof(delta.z));
            TextOut(mem_dc, 0, 20, buf);
            sprintf(buf, "velocity: %d", ship.velocity);
            TextOut(mem_dc, 0, 30, buf);
#endif            
            SetTextColor (mem_dc, RGBA2Pixel (mem_dc, 0xFF, 0x00, 0x00, 0x80));
            TextOut(mem_dc, 0, 0, "Press Cursor keys to change the direction.");
            TextOut(mem_dc, 0, 15, "Press Page Up or Page Down to rotate.");
            TextOut(mem_dc, 0, 30, "Press CTRL to fire engine.");
            TextOut(mem_dc, 0, 45, "Press ESC to exit.");
            
            draw_ship(mem_dc);
            BitBlt(mem_dc, 0, 0, SCREEN_W, SCREEN_H, hdc, 0,0, 0);
            EndPaint(hWnd, hdc);
            return 0;
	    case MSG_KEYDOWN:
            switch (wParam){
                case SCANCODE_ESCAPE:
                    PostMessage(hWnd, MSG_CLOSE, 0, 0);
                    break;
                case SCANCODE_CURSORBLOCKUP:
	                ship.rx -= itofix(5);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_CURSORBLOCKDOWN:
	                ship.rx += itofix(5);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_CURSORBLOCKLEFT:
	                ship.ry -= itofix(5);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_CURSORBLOCKRIGHT:
	                ship.ry += itofix(5);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_PAGEUP:
                    ship.rz -= itofix(5);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_PAGEDOWN:
	                ship.rz += itofix(5);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
            }
            if (lParam & KS_CTRL){
                ship.faces[ENGINE].colour = ENGINE_ON;
                ship.faces[ENGINE].range = 3;
                if (ship.velocity < SPEED_LIMIT)
                    ship.velocity += 2;
            InvalidateRect(hWnd, NULL, FALSE);
            }
            else{
                ship.faces[ENGINE].colour = ENGINE_OFF;
                ship.faces[ENGINE].range = 15;
                if (ship.velocity > 0)
                    ship.velocity -= 2;
            InvalidateRect(hWnd, NULL, FALSE);
            }
		    ship.rx &= itofix(255);
            ship.ry &= itofix(255);
            ship.rz &= itofix(255);

            delta.x = fixmul(direction.x, itofix(ship.velocity));
            delta.y = fixmul(direction.y, itofix(ship.velocity));
            delta.z = fixmul(direction.z, itofix(ship.velocity));
            break;
	    case MSG_CLOSE:
		    DestroyMainWindow (hWnd);
            DeleteMemDC (mem_dc);
		    PostQuitMessage (hWnd);
		    break;
	}
		

	return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(argv[1] , "Stars" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_VISIBLE| WS_BORDER ;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Stars";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = StarsWndProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = SCREEN_W;
    CreateInfo.by = SCREEN_H;
    CreateInfo.iBkColor = PIXEL_black;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow (hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);
    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

