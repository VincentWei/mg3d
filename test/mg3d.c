/*
**
** Copyright (C) 2006 Feynman Software.
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

#define SCREEN_W	640
#define SCREEN_H	480

#define NUM_SHAPES         8     /* number of bouncing cubes */

#define NUM_VERTICES       8     /* a cube has eight corners */
#define NUM_FACES          6     /* a cube has six faces */

GAL_Color pal[256];

int retrace_count;

typedef struct VTX
{
    fixed x, y, z;
} VTX;

typedef struct QUAD              /* four vertices makes a quad */
{
    VTX *vtxlist;
    int v1, v2, v3, v4;
} QUAD;

typedef struct SHAPE             /* store position of a shape */
{
    fixed x, y, z;                /* x, y, z position */
    fixed rx, ry, rz;             /* rotations */
    fixed dz;                     /* speed of movement */
    fixed drx, dry, drz;          /* speed of rotation */
} SHAPE;


VTX points[] =                   /* a cube, centered on the origin */
{
    /* vertices of the cube */
    { -32 << 16, -32 << 16, -32 << 16 },
    { -32 << 16,  32 << 16, -32 << 16 },
    {  32 << 16,  32 << 16, -32 << 16 },
    {  32 << 16, -32 << 16, -32 << 16 },
    { -32 << 16, -32 << 16,  32 << 16 },
    { -32 << 16,  32 << 16,  32 << 16 },
    {  32 << 16,  32 << 16,  32 << 16 },
    {  32 << 16, -32 << 16,  32 << 16 },
};



QUAD faces[] =                   /* group the vertices into polygons */
{
    { points, 0, 3, 2, 1 },
    { points, 4, 5, 6, 7 },
    { points, 0, 1, 5, 4 },
    { points, 2, 3, 7, 6 },
    { points, 0, 4, 7, 3 },
    { points, 1, 2, 6, 5 }
};


GAL_Color despal[16] = 
{
   { 63*4, 63*4, 63*4, 0 },   { 63*4, 0,  0,  0 },   { 0,  63*4, 0,  0 },   { 63*4, 63*4, 0,  0 },
   { 0,  0,  63*4, 0 },   { 63*4, 0,  63*4, 0 },   { 0,  63*4, 63*4, 0 },   { 16*4, 16*4, 16*4, 0 },
   { 31*4, 31*4, 31*4, 0 },   { 63*4, 31*4, 31*4, 0 },   { 31*4, 63*4, 31*4, 0 },   { 63*4, 63*4, 31*4, 0 },
   { 31*4, 31*4, 63*4, 0 },   { 63*4, 31*4, 63*4, 0 },   { 31*4, 63*4, 63*4, 0 },   { 0,  0,  0,  0 }
};


SHAPE shapes[NUM_SHAPES];        /* a list of shapes */


/* somewhere to put translated vertices */
VTX output_points[NUM_VERTICES * NUM_SHAPES];
QUAD output_faces[NUM_FACES * NUM_SHAPES];


enum {
    wireframe,
    flat,
    gcol,
    grgb,
    atex,
    ptex,
    atex_mask,
    ptex_mask,
    atex_lit,
    ptex_lit,
    atex_mask_lit,
    ptex_mask_lit,
    atex_trans,
    ptex_trans,
    atex_mask_trans,
    ptex_mask_trans,
    last_mode
} render_mode = wireframe;


int render_type[] =
{
    0,
    MG3D_POLYTYPE_FLAT,
    MG3D_POLYTYPE_GCOL,
    MG3D_POLYTYPE_GRGB,
    MG3D_POLYTYPE_ATEX,
    MG3D_POLYTYPE_PTEX,
    MG3D_POLYTYPE_ATEX_MASK,
    MG3D_POLYTYPE_PTEX_MASK,
    MG3D_POLYTYPE_ATEX_LIT,
    MG3D_POLYTYPE_PTEX_LIT,
    MG3D_POLYTYPE_ATEX_MASK_LIT,
    MG3D_POLYTYPE_PTEX_MASK_LIT,
    MG3D_POLYTYPE_ATEX_TRANS,
    MG3D_POLYTYPE_PTEX_TRANS,
    MG3D_POLYTYPE_ATEX_MASK_TRANS,
    MG3D_POLYTYPE_PTEX_MASK_TRANS
};


char *mode_desc[] =
{
    "Wireframe",
    "Flat shaded",
    "Single color gouraud shaded",
    "Gouraud shaded",
    "Texture mapped",
    "Perspective correct texture mapped",
    "Masked texture mapped",
    "Masked persp. correct texture mapped",
    "Lit texture map",
    "Lit persp. correct texture map",
    "Masked lit texture map",
    "Masked lit persp. correct texture map",
    "Transparent texture mapped",
    "Transparent perspective correct texture mapped",
    "Transparent masked texture mapped",
    "Transparent masked persp. correct texture mapped",
};

HDC texture;

/* initialise shape positions */
void init_shapes(void)
{
    int c;
    for (c=0; c<NUM_SHAPES; c++) {
        shapes[c].x = ((rand() & 255) - 128) << 16;
        shapes[c].y = ((rand() & 255) - 128) << 16;
        shapes[c].z = 768 << 16;
        shapes[c].rx = 0;
        shapes[c].ry = 0;
        shapes[c].rz = 0;
        shapes[c].dz =  ((rand() & 255) - 8) << 12;
        shapes[c].drx = ((rand() & 31) - 16) << 12;
        shapes[c].dry = ((rand() & 31) - 16) << 12;
        shapes[c].drz = ((rand() & 31) - 16) << 12;
    }
}


/* update shape positions */
void animate_shapes(void)
{
    int c;
    
    for (c=0; c<NUM_SHAPES; c++) {
        shapes[c].z += shapes[c].dz;
        
        if ((shapes[c].z > itofix(1024)) ||
                (shapes[c].z < itofix(192)))
            shapes[c].dz = -shapes[c].dz;
        
        shapes[c].rx += shapes[c].drx;
        shapes[c].ry += shapes[c].dry;
        shapes[c].rz += shapes[c].drz;
    }
}



/* translate shapes from 3d world space to 2d screen space */
void translate_shapes(void)
{
    int c, d;
    mg3dMatrix matrix;
    VTX *outpoint = output_points;
    QUAD *outface = output_faces;
    
    for (c=0; c<NUM_SHAPES; c++) {
        /* build a transformation matrix */
        mg3dGetTransformMatrix(&matrix, itofix(1),
                shapes[c].rx, shapes[c].ry, shapes[c].rz,
                shapes[c].x, shapes[c].y, shapes[c].z);
        
        /* output the vertices */
        for (d=0; d<NUM_VERTICES; d++) {
            mg3dApplyMatrix(&matrix, points[d].x, points[d].y, points[d].z, &outpoint[d].x, &outpoint[d].y, &outpoint[d].z);
            mg3dPerspProject(outpoint[d].x, outpoint[d].y, outpoint[d].z, &outpoint[d].x, &outpoint[d].y);
        }
        
        /* output the faces */
        for (d=0; d<NUM_FACES; d++) {
            outface[d] = faces[d];
            outface[d].vtxlist = outpoint;
        }
        outpoint += NUM_VERTICES;
        outface += NUM_FACES;
   }
}



/* draw a line (for wireframe display) */
void wire(HDC b, VTX *v1, VTX *v2)
{
    int col = MID(128, 255 - fixtoi(v1->z+v2->z) / 16, 255);
    SetPenColor (b, RGB2Pixel(b, col,col,col));
    LineEx(b, fixtoi(v1->x), fixtoi(v1->y), fixtoi(v2->x), fixtoi(v2->y));
}



/* draw a quad */
void draw_quad(HDC b, VTX *v1, VTX *v2, VTX *v3, VTX *v4, int mode)
{
    int col;
    
    /* four vertices */
    mg3dVp vtx1 = { 0, 0, 0, 0,      0,      0 };
    mg3dVp vtx2 = { 0, 0, 0, 32<<16, 0,      0 };
    mg3dVp vtx3 = { 0, 0, 0, 32<<16, 32<<16, 0 };
    mg3dVp vtx4 = { 0, 0, 0, 0,      32<<16, 0 };
    
    vtx1.x = v1->x;   vtx1.y = v1->y;   vtx1.z = v1->z;
    vtx2.x = v2->x;   vtx2.y = v2->y;   vtx2.z = v2->z;
    vtx3.x = v3->x;   vtx3.y = v3->y;   vtx3.z = v3->z;
    vtx4.x = v4->x;   vtx4.y = v4->y;   vtx4.z = v4->z;

    /* cull backfaces */
    if ((mode != MG3D_POLYTYPE_ATEX_MASK) && (mode != MG3D_POLYTYPE_PTEX_MASK) &&
            (mode != MG3D_POLYTYPE_ATEX_MASK_LIT) && (mode != MG3D_POLYTYPE_PTEX_MASK_LIT) &&
            (mg3dPolygonZNormal(&vtx1, &vtx2, &vtx3) < 0))
        return;
    
    /* set up the vertex color, differently for each rendering mode */
    switch (mode) {
        
        case MG3D_POLYTYPE_FLAT:
            col = MID(128, 255 - fixtoi(v1->z+v2->z) / 16, 255);
            vtx1.c = vtx2.c = vtx3.c = vtx4.c = RGB2Pixel(b, col, col, col);
            break;
        
        case MG3D_POLYTYPE_GCOL:
            vtx1.c = RGB2Pixel(b,0xD0, 0xD0, 0xD0);
            vtx2.c = RGB2Pixel(b, 0x80, 0x80, 0x80);
            vtx3.c = RGB2Pixel(b, 0xB0, 0xB0, 0xB0);
            vtx4.c = RGB2Pixel(b, 0xFF, 0xFF, 0xFF);
            break;
        case MG3D_POLYTYPE_GRGB:
            vtx1.c = 0x000000;
            vtx2.c = 0x7F0000;
            vtx3.c = 0xFF0000;
            vtx4.c = 0x7F0000;
            break;
        case MG3D_POLYTYPE_ATEX_LIT:
        case MG3D_POLYTYPE_PTEX_LIT:
        case MG3D_POLYTYPE_ATEX_MASK_LIT:
        case MG3D_POLYTYPE_PTEX_MASK_LIT:
            vtx1.c = MID(0, 255 - fixtoi(v1->z) / 4, 255);
            vtx2.c = MID(0, 255 - fixtoi(v2->z) / 4, 255);
            vtx3.c = MID(0, 255 - fixtoi(v3->z) / 4, 255);
            vtx4.c = MID(0, 255 - fixtoi(v4->z) / 4, 255);
            break;
    }
    
    /* draw the quad */
    mg3dQuad(b, mode, texture, &vtx1, &vtx2, &vtx3, &vtx4);
}



/* callback for qsort() */
int quad_cmp(const void *e1, const void *e2)
{
    QUAD *q1 = (QUAD *)e1;
    QUAD *q2 = (QUAD *)e2;
    
    fixed d1 = q1->vtxlist[q1->v1].z + q1->vtxlist[q1->v2].z +
        q1->vtxlist[q1->v3].z + q1->vtxlist[q1->v4].z;
    
    fixed d2 = q2->vtxlist[q2->v1].z + q2->vtxlist[q2->v2].z +
        q2->vtxlist[q2->v3].z + q2->vtxlist[q2->v4].z;
    
    return d2 - d1;
}



/* draw the shapes calculated by translate_shapes() */
void draw_shapes(HDC b)
{
    int c;
    QUAD *face = output_faces;
    VTX *v1, *v2, *v3, *v4;

    /* depth sort */
    qsort(output_faces, NUM_FACES * NUM_SHAPES, sizeof(QUAD), quad_cmp);

    for (c=0; c < NUM_FACES * NUM_SHAPES; c++) {
        /* find the vertices used by the face */
        v1 = face->vtxlist + face->v1;
        v2 = face->vtxlist + face->v2;
        v3 = face->vtxlist + face->v3;
        v4 = face->vtxlist + face->v4;
        
        /* draw the face */
        if (render_mode == wireframe) {
            wire(b, v1, v2);
            wire(b, v2, v3);
            wire(b, v3, v4);
            wire(b, v4, v1);
        }
        else {
            draw_quad(b, v1, v2, v3, v4, render_type[render_mode]);
        }
        face++;
    }
}

/* lighting color mapping table */
MG3D_COLOR_MAP light_table;

/* transparency color mapping table */
MG3D_COLOR_MAP trans_table;




void print_progress(int pos)
{
        ;
}


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

static int MG3DWndProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    char buf[64];
    static HDC buffer;
    HDC hdc;
    int c;
    int last_retrace_count;
    
    switch ( message ) {
        case MSG_CREATE:
            buffer = CreateMemDC(SCREEN_W, SCREEN_H, DEPTH,
                    MEMDC_FLAG_SWSURFACE, 0x00, 0x00, 0x00, 0x00);
            texture = CreateMemDC(32, 32, DEPTH,
                    MEMDC_FLAG_SWSURFACE, 0x00, 0x00, 0x00, 0x00);
            /* color 0 = black */
            pal[0].r = pal[0].g = pal[0].b = 0;
            
            /* copy the desktop palette */
            for (c=1; c<64; c++)
                pal[c] = despal[c];

           
            /* make a red gradient */
            for (c=64; c<96; c++) {
                pal[c].r = (c-64)*8;
                pal[c].g = pal[c].b = 0;
            }
            
            /* make a green gradient */
            for (c=96; c<128; c++) {
                pal[c].g = (c-96)*8;
                pal[c].r = pal[c].b = 0;
            }
            
            /* set up a greyscale in the top half of the palette */
            for (c=128; c<256; c++)
                pal[c].r = pal[c].g = pal[c].b = (c-128)*2;
            
            SetPalette(buffer, 0, 255, pal);
            SetPalette(texture, 0, 255, pal);
            
            mg3dCreateLightTable(&light_table, pal, 0, 0, 0, print_progress);
            color_map = &light_table;
            
            /* textures are 25% transparent (75% opaque) */
            mg3dCreateTransTable(&trans_table, pal, 192, 192, 192, print_progress);
            
            /* set up the truecolor blending functions */
            /* textures are 25% transparent (75% opaque) */
            mg3dSettransBlender(0, 0, 0, 192);
            
            SetBrushColor (texture, RGB2Pixel(texture, pal[0].r,pal[0].g,pal[0].b));
            FillBox (texture, 0, 0, SCREEN_W, SCREEN_H);

            /* make a bitmap for use as a texture map */
            SetPenColor (texture, RGB2Pixel(texture, pal[1].r,pal[1].g,pal[1].b));
            LineEx(texture, 0, 0, 31, 31);
            LineEx(texture, 0, 31, 31, 0);
            Rectangle(texture, 0, 0, 31, 31);
            
            SetBkMode(texture, BM_TRANSPARENT);
            SetTextColor (texture, RGB2Pixel (texture, pal[2].r, pal[2].g, pal[2].b));
            
            TextOut(texture, 0, 0, "dead");
            TextOut(texture, 0, 8, "pigs");
            TextOut(texture, 0, 16, "cant");
            TextOut(texture, 0, 24, "fly.");
            
            /* set up the viewport for the perspective projection */
            mg3dSetProjectionViewport(0, 0, SCREEN_W, SCREEN_H);
            
            /* initialise the bouncing shapes */
            init_shapes();
            
            last_retrace_count = retrace_count;
            SetTimer(hWnd, 100, 1);
            break;

        case MSG_TIMER:
            SetBrushColor (buffer, RGB2Pixel(buffer, pal[0].r,pal[0].g,pal[0].b));
            FillBox (buffer, 0, 0, SCREEN_W, SCREEN_H);
            animate_shapes();
#if 0           
            while (last_retrace_count < retrace_count) {
                animate_shapes();
                last_retrace_count++;
            }
#endif       
            translate_shapes();
            draw_shapes(buffer);
            
            SetBkMode(buffer, BM_TRANSPARENT);
            SetTextColor (buffer, RGB2Pixel (buffer, pal[1].r, pal[1].g, pal[1].b));
            sprintf(buf, "%s", mode_desc[render_mode]);
            TextOut(buffer, 0, 20, buf);
            TextOut(buffer, 0, 40, "Press left key to change");
            
            hdc=GetClientDC(hWnd);

            BitBlt(buffer, 0, 0, SCREEN_W, SCREEN_H, hdc, 0 , 0, 0); 
#if 0
            retrace_count++;
            if (retrace_count > 25)
            {
                retrace_count = 0;
            }
#endif
            ReleaseDC(hdc);
            break;
        case MSG_KEYDOWN:
            switch (wParam){
                case SCANCODE_ESCAPE:
                    PostMessage(hWnd, MSG_CLOSE, 0, 0);
                    break;

                case SCANCODE_CURSORBLOCKLEFT:
                    render_mode++;
                    if (render_mode >= last_mode) {
                        render_mode = wireframe;
                        color_map = &light_table;
                    }
                    
                    if (render_type[render_mode] >= MG3D_POLYTYPE_ATEX_TRANS)
                        color_map = &trans_table;
                    break;
            }
           break;

        case MSG_CLOSE:
		    DeleteMemDC (buffer);
		    DeleteMemDC (texture);
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
    CreateInfo.spCaption = "MG3D Window";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = MG3DWndProc;
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

