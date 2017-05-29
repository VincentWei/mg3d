/*
** Copyright (C) 2005 Feynman Software.
**
** License: GPL
**
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

GAL_Color pal[256];

typedef struct FACE
{
  int v1, v2, v3, v4;
} FACE;


mg3dVpf cube1[] =
{
   { -32., -32., -32., 0., 0., 72},
   { -32.,  32., -32., 0., 0., 80},
   {  32.,  32., -32., 0., 0., 95},
   {  32., -32., -32., 0., 0., 88},
   { -32., -32.,  32., 0., 0., 72},
   { -32.,  32.,  32., 0., 0., 80},
   {  32.,  32.,  32., 0., 0., 95},
   {  32., -32.,  32., 0., 0., 88}
};


mg3dVpf cube2[] =
{
   { -32., -32., -32., 0., 0., 104},
   { -32.,  32., -32., 0., 0., 112},
   {  32.,  32., -32., 0., 0., 127},
   {  32., -32., -32., 0., 0., 120},
   { -32., -32.,  32., 0., 0., 104},
   { -32.,  32.,  32., 0., 0., 112},
   {  32.,  32.,  32., 0., 0., 127},
   {  32., -32.,  32., 0., 0., 120}
};


FACE faces[] =
{
   { 2, 1, 0, 3 },
   { 4, 5, 6, 7 },
   { 0, 1, 5, 4 },
   { 2, 3, 7, 6 },
   { 4, 7, 3, 0 },
   { 1, 2, 6, 5 }
};


/* update cube positions */
void anim_cube(mg3dMatrixf* matrix1, mg3dMatrixf* matrix2, mg3dVpf x1[], mg3dVpf x2[])
{
   int i;

   for (i=0; i<8; i++) {
      mg3dApplyMatrixF(matrix1, cube1[i].x, cube1[i].y, cube1[i].z, &(x1[i].x), &(x1[i].y), &(x1[i].z));
      mg3dApplyMatrixF(matrix2, cube2[i].x, cube2[i].y, cube2[i].z, &(x2[i].x), &(x2[i].y), &(x2[i].z));
      mg3dPerspProjectF(x1[i].x, x1[i].y, x1[i].z, &(x1[i].x), &(x1[i].y));
      mg3dPerspProjectF(x2[i].x, x2[i].y, x2[i].z, &(x2[i].x), &(x2[i].y));
   }
}


/* cull backfaces and draw cubes */
void draw_cube(HDC buffer, mg3dVpf x1[], mg3dVpf x2[])
{
	int i;
	
	for (i=0; i<6; i++) {
		mg3dVpf vtx1, vtx2, vtx3, vtx4;
		
		vtx1 = x1[faces[i].v1];
		vtx2 = x1[faces[i].v2];
		vtx3 = x1[faces[i].v3];
		vtx4 = x1[faces[i].v4];
        
        if (mg3dPolygonZNormalF(&vtx1, &vtx2, &vtx3) > 0)
            mg3dQuadF(buffer, MG3D_POLYTYPE_GCOL | MG3D_POLYTYPE_ZBUF, 0, &vtx1, &vtx2, &vtx3, &vtx4);
		
		vtx1 = x2[faces[i].v1];
		vtx2 = x2[faces[i].v2];
		vtx3 = x2[faces[i].v3];
		vtx4 = x2[faces[i].v4];
		
        if (mg3dPolygonZNormalF(&vtx1, &vtx2, &vtx3) > 0)
			mg3dQuadF(buffer, MG3D_POLYTYPE_GCOL| MG3D_POLYTYPE_ZBUF, 0, &vtx1, &vtx2, &vtx3, &vtx4);
	}
}



static int ZBufferWndProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    static MG3DZBUFFER zbuf;
    static HDC buffer;
    HDC hdc;
    static mg3dMatrixf matrix1;
    static mg3dMatrixf matrix2;
    static mg3dVpf x1[8], x2[8];
    
    static int t;
    int i;
   
    int frame = 0;
    static float fps = 0.;
    
    static float rx1, ry1, rz1;		/* cube #1 rotations */
    static float drx1, dry1, drz1;	/* cube #1 rotation speed */
    static float rx2, ry2, rz2;		/* cube #2 rotations */
    static float drx2, dry2, drz2;	/* cube #1 rotation speed */
    float tx = 16.;		/* x shift between cubes */
    float tz1 = 100.;		/* cube #1 z coordinate */
    float tz2 = 105.;		/* cube #2 z coordinate */

    switch ( message ) {
	    case MSG_CREATE:
            /* double buffer the animation and create the Z-buffer */
            buffer = CreateMemDC (SCREEN_W, SCREEN_H, DEPTH,
                    MEMDC_FLAG_SWSURFACE, 0x00, 0x00, 0x00, 0x00);
            zbuf = mg3dCreateZbuffer(buffer);
            mg3dSetZbuffer(zbuf);
            
            /* color 0 = black */
            pal[0].r = pal[0].g = pal[0].b = 0;
            /* color 1 = red */
            pal[1].r = 255;
            pal[1].g = pal[1].b = 0;
            /* copy the desktop palette */
            for (i=2; i<64; i++)
                pal[i].r = pal[i].g = pal[i].b = i*4;
            /* make a blue gradient */
            for (i=64; i<96; i++) {
                pal[i].b = (i-64)*8;
                pal[i].g = pal[i].r = 0;
            }
            
            /* make a green gradient */
            for (i=96; i<128; i++) {
                pal[i].g = (i-96)*8;
                pal[i].r = pal[i].b = 0;
            }
            
            SetPalette(buffer, 0, 255, pal);
            SetPalette(zbuf, 0, 255, pal);
            
            /* set up the viewport for the perspective projection */
            mg3dSetProjectionViewport(0, 0, SCREEN_W, SCREEN_H);
            
            /* compute rotations and speed rotation */
            rx1 = ry1 = rz1 = 0.;
            rx2 = ry2 = rz2 = 0.;
            
            drx1 = ((rand() & 31) - 16) / 4.;
            dry1 = ((rand() & 31) - 16) / 4.;
            drz1 = ((rand() & 31) - 16) / 4.;
            
            drx2 = ((rand() & 31) - 16) / 4.;
            dry2 = ((rand() & 31) - 16) / 4.;
            drz2 = ((rand() & 31) - 16) / 4.;

            /* set the transformation matrices */
            mg3dGetTransformMatrixF(&matrix1, 1., rx1, ry1, rz1, tx, 0., tz1);
            mg3dGetTransformMatrixF(&matrix2, 1., rx2, ry2, rz2, -tx, 0., tz2);
            
            /* set colors */
            for (i=0; i<8; i++) {
                x1[i].c = RGB2Pixel (buffer, pal[cube1[i].c].r,pal[cube1[i].c].g,pal[cube1[i].c].b);
                x2[i].c = RGB2Pixel (buffer, pal[cube2[i].c].r,pal[cube2[i].c].g,pal[cube2[i].c].b);
            }
           SetTimer(hWnd, 100, 10);
            break;

        case MSG_TIMER:
            SetBrushColor(buffer, RGB2Pixel (buffer, pal[0].r, pal[0].g, pal[0].b));
            FillBox(buffer, 0, 0, SCREEN_W, SCREEN_H);
            mg3dClearZbuffer(zbuf, 0.);
            anim_cube(&matrix1, &matrix2, x1, x2);
            draw_cube(buffer, x1, x2);
            
            /* update transformation matrices */
            rx1 += drx1;
            ry1 += dry1;
            rz1 += drz1;
            rx2 += drx2;
            ry2 += dry2;
            rz2 += drz2;
            
            mg3dGetTransformMatrixF(&matrix1, 1., rx1, ry1, rz1, tx, 0., tz1);
            mg3dGetTransformMatrixF(&matrix2, 1., rx2, ry2, rz2, -tx, 0., tz2);
            
            SetBkMode(buffer, BM_TRANSPARENT);
            SetTextColor (buffer, RGB2Pixel (buffer, pal[1].r, pal[1].g, pal[1].b));
#if 0
            sprintf(buf, "Z-buffered polygons (%.1f fps)", fps);
            TextOut(buffer, 0, 0, buf);
#endif       
            hdc=GetClientDC(hWnd);
            BitBlt(buffer, 0, 0, SCREEN_W, SCREEN_H, hdc, 0, 0, 0);
            
            if (t > 100) {
                fps = (100. * frame) / t;
                t = 0;
                frame = 0;
            }
            t++;
            ReleaseDC(hdc);
           break;
	    case MSG_CLOSE:
            mg3dDestroyZbuffer(zbuf);
		    DeleteMemDC (buffer);
		    DestroyMainWindow (hWnd);
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
    JoinLayer(NAME_DEF_LAYER , "MultiWnd" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "ZBuffer Window";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = ZBufferWndProc;
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

