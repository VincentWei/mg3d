/*
**   This program demonstrates how to view a 3d world from any position and angle.
**
** Copyright (C) 2005 Feynman Software.
**
** License: GPL
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

#ifndef M_PI
   #define M_PI   3.14159
#endif

GAL_Color pal[256];

/* lighting color mapping table */
MG3D_COLOR_MAP light_table;

/* transparency color mapping table */
MG3D_COLOR_MAP trans_table;


/* convert radians to degrees */
#define DEG(n)    ((n) * 180.0 / M_PI)

/* parameters controlling the camera and projection state */
int viewport_w = 320;
int viewport_h = 240;

enum {
    flat,
    gcol_zbuf,
    grgb_zbuf,
    atex_zbuf,
    ptex_zbuf,
    atex_mask_zbuf,
    ptex_mask_zbuf,
    atex_lit_zbuf,
    ptex_lit_zbuf,
    atex_mask_lit_zbuf,
    ptex_mask_lit_zbuf,
    atex_trans_zbuf,
    ptex_trans_zbuf,
    atex_mask_trans_zbuf,
    ptex_mask_trans_zbuf,
    last_mode
} render_mode = flat;


int render_type[] =
{
    MG3D_POLYTYPE_FLAT,
    MG3D_POLYTYPE_GCOL| MG3D_POLYTYPE_ZBUF,
    MG3D_POLYTYPE_GRGB| MG3D_POLYTYPE_ZBUF,
    MG3D_POLYTYPE_ATEX| MG3D_POLYTYPE_ZBUF,
    MG3D_POLYTYPE_PTEX| MG3D_POLYTYPE_ZBUF,
    MG3D_POLYTYPE_ATEX_MASK| MG3D_POLYTYPE_ZBUF,
    MG3D_POLYTYPE_PTEX_MASK| MG3D_POLYTYPE_ZBUF,
    MG3D_POLYTYPE_ATEX_LIT| MG3D_POLYTYPE_ZBUF,
    MG3D_POLYTYPE_PTEX_LIT| MG3D_POLYTYPE_ZBUF,
    MG3D_POLYTYPE_ATEX_MASK_LIT| MG3D_POLYTYPE_ZBUF,
    MG3D_POLYTYPE_PTEX_MASK_LIT| MG3D_POLYTYPE_ZBUF,
    MG3D_POLYTYPE_ATEX_TRANS| MG3D_POLYTYPE_ZBUF,
    MG3D_POLYTYPE_PTEX_TRANS| MG3D_POLYTYPE_ZBUF,
    MG3D_POLYTYPE_ATEX_MASK_TRANS| MG3D_POLYTYPE_ZBUF,
    MG3D_POLYTYPE_PTEX_MASK_TRANS| MG3D_POLYTYPE_ZBUF
};


char *mode_desc[] =
{
    "Flat shaded",
    "Single color gouraud shaded with zbuf",
    "Gouraud shaded with zbuf",
    "Texture mapped with zbuf",
    "Perspective correct texture mapped with zbuf",
    "Masked texture mapped with zbuf",
    "Masked persp. correct texture mapped with zbuf",
    "Lit texture map with zbuf",
    "Lit persp. correct texture map with zbuf",
    "Masked lit texture map with zbuf",
    "Masked lit persp. correct texture map with zbuf",
    "Transparent texture mapped with zbuf",
    "Transparent perspective correct texture mapped with zbuf",
    "Transparent masked texture mapped with zbuf",
    "Transparent masked persp. correct texture mapped with zbuf"
};



GAL_Color despal[16] = 
{
   { 63*4, 63*4, 63*4, 0 },   { 63*4, 0,  0,  0 },   { 0,  63*4, 0,  0 },   { 63*4, 63*4, 0,  0 },
   { 0,  0,  63*4, 0 },   { 63*4, 0,  63*4, 0 },   { 0,  63*4, 63*4, 0 },   { 16*4, 16*4, 16*4, 0 },
   { 31*4, 31*4, 31*4, 0 },   { 63*4, 31*4, 31*4, 0 },   { 31*4, 63*4, 31*4, 0 },   { 63*4, 63*4, 31*4, 0 },
   { 31*4, 31*4, 63*4, 0 },   { 63*4, 31*4, 63*4, 0 },   { 31*4, 63*4, 63*4, 0 },   { 0,  0,  0,  0 }
};


void print_progress(int pos)
{
	 printf("*");
}

mg3dVpf planez[] =
{
   { -20., -20.,  0.,  0., 64., 0},
   { -20.,  20.,  0., 64., 64., 0},
   {  20.,  20.,  0., 64.,  0., 0},
   {  20., -20.,  0.,  0.,  0., 0}
};

mg3dVpf planey[] =
{
   { -20., 0.,  20.,  0., 64., 0},
   {  20., 0.,  20., 64., 64., 0},
   {  20., 0., -20., 64.,  0., 0},
   { -20., 0., -20.,  0.,  0., 0}
};

mg3dVpf planex[] =
{
   { 0., -20.,  20.,  0., 64., 0},
   { 0.,  20.,  20., 64., 64., 0},
   { 0.,  20., -20., 64.,  0., 0},
   { 0., -20., -20.,  0.,  0., 0}
};

typedef struct FACE
{
  int v1, v2, v3, v4;
} FACE;

mg3dVpf cube[] =
{
   {  -5., -15., -15., 0., 0., 0},
   {  -5.,  -5., -15., 0., 0., 0},
   { -15.,  -5., -15., 0., 0., 0},
   { -15., -15., -15., 0., 0., 0},
   {  -5., -15.,  -5., 0., 0., 0},
   {  -5.,  -5.,  -5., 0., 0., 0},
   { -15.,  -5.,  -5., 0., 0., 0},
   { -15., -15.,-  5., 0., 0., 0}
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
/* compute rotations and speed rotation */
static float rx =0.;
static float ry =0.;
static float rz =0.;
            
static float rycube =0.;
static float scale = 2.5;
static float xoff = 0.;
static float yoff = 0.;
static float zoff = 100;

/* render a tile of the grid */
void anim_plane(HDC hdc, mg3dMatrixf *camera, mg3dVpf plane[])
{
    mg3dVpf _v[4], _vout[8], _vtmp[8];
    mg3dVpf *v[4], *vout[8], *vtmp[8];
    int flags[4], out[8];
    int c, vc;
 
    for (c=0; c<4; c++){
        v[c] = &_v[c];
    }

    for (c=0; c<8; c++) {
        vout[c] = &_vout[c];
        vtmp[c] = &_vtmp[c];
    }
    for (c=0; c<4; c++){
        *v[c] = plane[c];
    }
    /* apply the camera matrix, translating world space -> view space */
    for (c=0; c<4; c++) {
        mg3dApplyMatrixF(camera, v[c]->x, v[c]->y, v[c]->z, &v[c]->x, &v[c]->y, &v[c]->z);
        flags[c] = 0;

    /* set flags if this vertex is off the edge of the screen */
        if (v[c]->x < -v[c]->z)
            flags[c] |= 1;
        else if (v[c]->x > v[c]->z)
            flags[c] |= 2;

        if (v[c]->y < -v[c]->z)
            flags[c] |= 4;
        else if (v[c]->y > v[c]->z)
            flags[c] |= 8;

        if (v[c]->z < 0.1)
            flags[c] |= 16;
    }
   
    /* quit if all vertices are off the same edge of the screen */
    if (flags[0] & flags[1] & flags[2] & flags[3])
        return;
    
    v[0]->c = RGB2Pixel(hdc, 0xD0, 0xD0, 0xD0);
    v[1]->c = RGB2Pixel(hdc, 0x80, 0x80, 0x80);
    v[2]->c = RGB2Pixel(hdc, 0xB0, 0xB0, 0xB0);
    v[3]->c = RGB2Pixel(hdc, 0xFF, 0xFF, 0xFF);

   if (flags[0] | flags[1] | flags[2] | flags[3]) {
        /* clip if any vertices are off the edge of the screen */
		vc = mg3dClipF(hdc, MG3D_POLYTYPE_GCOL|MG3D_POLYTYPE_ZBUF, 0.01, 0.01, 4, (const mg3dVpf **)v, vout, vtmp, out);

		if (vc <= 0)
			return;
    }
	else {
        /* no need to bother clipping this one */
        vout[0] = v[0];
        vout[1] = v[1];
        vout[2] = v[2];
        vout[3] = v[3];

        vc = 4;
    }
    /* project view space -> screen space */
    for (c=0; c<vc; c++)
        mg3dPerspProjectF(vout[c]->x, vout[c]->y, vout[c]->z, &vout[c]->x, &vout[c]->y);
	
    /* render the polygon */
   mg3dPolygonF(hdc, MG3D_POLYTYPE_GCOL|MG3D_POLYTYPE_ZBUF, 0, vc, vout);
	
}

/* render a tile of the grid */
void draw_plane(HDC hdc, mg3dMatrixf *camera, int mode, HDC texture, mg3dVpf plane[])
{
    mg3dVpf _v[4], _vout[8], _vtmp[8];
    mg3dVpf *v[4], *vout[8], *vtmp[8];
    int flags[4], out[8];
    int c, vc;
 
    for (c=0; c<4; c++){
        v[c] = &_v[c];
    }

    for (c=0; c<8; c++) {
        vout[c] = &_vout[c];
        vtmp[c] = &_vtmp[c];
    }
    for (c=0; c<4; c++){
        *v[c] = plane[c];
    }
    /* apply the camera matrix, translating world space -> view space */
    for (c=0; c<4; c++) {
        mg3dApplyMatrixF(camera, v[c]->x, v[c]->y, v[c]->z, &v[c]->x, &v[c]->y, &v[c]->z);
        flags[c] = 0;

    /* set flags if this vertex is off the edge of the screen */
        if (v[c]->x < -v[c]->z)
            flags[c] |= 1;
        else if (v[c]->x > v[c]->z)
            flags[c] |= 2;

        if (v[c]->y < -v[c]->z)
            flags[c] |= 4;
        else if (v[c]->y > v[c]->z)
            flags[c] |= 8;

        if (v[c]->z < 0.1)
            flags[c] |= 16;
    }
   
    /* quit if all vertices are off the same edge of the screen */
    if (flags[0] & flags[1] & flags[2] & flags[3])
        return;
    
    /* set up the vertex color, differently for each rendering mode */
    switch (mode) {
        
        case MG3D_POLYTYPE_FLAT:
            v[0]->c = v[1]->c = v[2]->c = v[3]->c = RGB2Pixel(hdc, pal[1].r, pal[1].g, pal[1].b);
            break;
        
        case MG3D_POLYTYPE_GCOL|MG3D_POLYTYPE_ZBUF:
            v[0]->c = RGB2Pixel(hdc, 0xD0, 0xD0, 0xD0);
            v[1]->c = RGB2Pixel(hdc, 0x80, 0x80, 0x80);
            v[2]->c = RGB2Pixel(hdc, 0xB0, 0xB0, 0xB0);
            v[3]->c = RGB2Pixel(hdc, 0xFF, 0xFF, 0xFF);
            break;
        case MG3D_POLYTYPE_GRGB|MG3D_POLYTYPE_ZBUF:
            v[0]->c = 0x000000;
            v[1]->c = 0x7F0000;
            v[2]->c = 0xFF0000;
            v[3]->c = 0x7F0000;
            break;
        case MG3D_POLYTYPE_ATEX_LIT|MG3D_POLYTYPE_ZBUF:
        case MG3D_POLYTYPE_PTEX_LIT|MG3D_POLYTYPE_ZBUF:
        case MG3D_POLYTYPE_ATEX_MASK_LIT|MG3D_POLYTYPE_ZBUF:
        case MG3D_POLYTYPE_PTEX_MASK_LIT|MG3D_POLYTYPE_ZBUF:
            v[0]->c = 155;
            v[1]->c = 255;
            v[2]->c = 180;
            v[3]->c = 155;
            break;
    }

    if (flags[0] | flags[1] | flags[2] | flags[3]) {
        /* clip if any vertices are off the edge of the screen */
		vc = mg3dClipF(hdc, mode, 0.01, 0.01, 4, (const mg3dVpf **)v, vout, vtmp, out);

		if (vc <= 0)
			return;
    }
	else {
        /* no need to bother clipping this one */
        vout[0] = v[0];
        vout[1] = v[1];
        vout[2] = v[2];
        vout[3] = v[3];

        vc = 4;
    }
    /* project view space -> screen space */
    for (c=0; c<vc; c++)
        mg3dPerspProjectF(vout[c]->x, vout[c]->y, vout[c]->z, &vout[c]->x, &vout[c]->y);

	/* render the polygon */
   mg3dPolygonF(hdc, mode, texture, vc, vout);
	
}

/* draw everything */
void render(HDC hdc, HDC texture1, HDC texture2)
{
    mg3dMatrixf camera1, camera2, out;
	int x, y, w, h, i;
	char buf[100];
	
	/* clear the background */
    SetBrushColor (hdc, RGB2Pixel(hdc, pal[0].r,pal[0].g,pal[0].b));
	FillBox (hdc, 0, 0, SCREEN_W, SCREEN_H);


	/* set up the viewport region */
	x = (SCREEN_W - viewport_w) / 2;
	y = (SCREEN_H - viewport_h) / 2;
	w = viewport_w;
	h = viewport_h;
    
    /* set up the viewport for the perspective projection */
    mg3dSetProjectionViewport(x, y, w, h);
   
    /* set the transformation matrices */
    mg3dGetTransformMatrixF(&camera1, scale, rx, ry, rz, xoff, yoff, zoff);
    mg3dGetTransformMatrixF(&camera2, 1, 0, rycube, 0, 0, 0, 0);
    mg3dMatrixMulF(&camera2, &camera1, &out);
    
    /* draw the grid of squares */
    draw_plane(hdc, &camera1, render_type[render_mode], texture1, planex);
    draw_plane(hdc, &camera1, render_type[render_mode], texture2, planey);
    draw_plane(hdc, &camera1, render_type[render_mode], texture2, planez);
 
    for (i=0; i<6; i++) {
        static mg3dVpf animcube[4];
		animcube[0] = cube[faces[i].v1];
		animcube[1] = cube[faces[i].v2];
		animcube[2] = cube[faces[i].v3];
		animcube[3] = cube[faces[i].v4];

        anim_plane(hdc, &out, animcube);
    }
    
    SetPenType (hdc, PT_SOLID);
	SetPenWidth (hdc, 1);
	SetPenColor(hdc, RGB2Pixel(hdc, pal[1].r,pal[1].g, pal[1].b));
	Rectangle(hdc, x, y, x+w, y+h);

	/* overlay some text */
	SetBkMode(hdc, BM_TRANSPARENT);
	SetTextColor(hdc, RGB2Pixel(hdc, pal[3].r,pal[3].g, pal[3].b));
    TextOut(hdc, 0, 20, "Press Q/q key to change rendering mode.");
	sprintf(buf, "**  %s  **", mode_desc[render_mode]);
    TextOut(hdc, 150, 0, buf);
    
    TextOut(hdc, 0, 40, "Press W/w key to change viewport width.");
    TextOut(hdc, 0, 60, "Press H/h key to change viewport height");
    
    TextOut(hdc, 0, 80, "Press X/x key to change X position");
    TextOut(hdc, 0, 100, "Press Y/y key to change Y position.");
    TextOut(hdc, 0, 120, "Press Z/z key to change Z position.");
    TextOut(hdc, 0, 140, "Press S/s key to change scale.");
    TextOut(hdc, 0, 160, "Press up/down to rotate the X coordinate.");
    TextOut(hdc, 0, 180, "Press pgup/pgdn to rotate the Y coordinate.");
    TextOut(hdc, 0, 200, "Press left/right to rotate the Z coordinate.");
    TextOut(hdc, 0, 220, "Press R/r to rotate the cube");
    
}

static int CameraWndProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{

    HDC hdc;
    static HDC mem_dc;
    static MG3DZBUFFER zbuf;
    static HDC texture1;
    static HDC texture2;
    int c;
    switch ( message ) {
        case MSG_CREATE:
            mem_dc = CreateMemDC(SCREEN_W, SCREEN_H, DEPTH,
                    MEMDC_FLAG_SWSURFACE, 0x0000, 0x0000, 0x0000, 0x0000);
       	    texture1 = CreateMemDC(16, 16, DEPTH,
                    MEMDC_FLAG_SWSURFACE, 0x0000, 0x0000, 0x0000, 0x0000);
            texture2 = CreateMemDC(16, 16, DEPTH,
                    MEMDC_FLAG_SWSURFACE, 0x0000, 0x0000, 0x0000, 0x0000);
            zbuf = mg3dCreateZbuffer(mem_dc);
            mg3dSetZbuffer(zbuf);

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
            
            SetPalette(mem_dc, 0, 255, pal);
            SetPalette(texture1, 0, 255, pal);
            SetPalette(texture2, 0, 255, pal);
            
            mg3dCreateLightTable(&light_table, pal, 0, 0, 0, print_progress);
            color_map = &light_table;
            
            /* textures are 25% transparent (75% opaque) */
            mg3dCreateTransTable(&trans_table, pal, 192, 192, 192, print_progress);
            
            /* set up the truecolor blending functions */
            /* textures are 25% transparent (75% opaque) */
            mg3dSettransBlender(0, 0, 0, 100);
            
            int depth = GetGDCapability(mem_dc, GDCAP_DEPTH);
            switch(depth){
                case 8:
                    SetBrushColor (texture1, RGB2Pixel(texture1, pal[0].r,pal[0].g,pal[0].b));
                    SetBrushColor (texture2, RGB2Pixel(texture2, pal[0].r,pal[0].g,pal[0].b));
                    break;
                case 15:
                case 16:
                case 24:
                case 32:
                    SetBrushColor (texture1, RGB2Pixel(texture1, 0xFF, 0, 0xFF));
                    SetBrushColor (texture2, RGB2Pixel(texture2, 0xFF, 0, 0xFF));
                    break;
            }
            
            FillBox (texture1, 0, 0, 16, 16);
            FillBox (texture2, 0, 0, 16, 16);

            /* make a bitmap for use as a texture map */
            SetPenColor (texture1, RGB2Pixel(texture1, pal[2].r,pal[2].g,pal[2].b));
            LineEx(texture1, 0, 0, 15, 15);
            LineEx(texture1, 0, 15, 15, 0);
            Rectangle(texture1, 0, 0, 15, 15);
            
            /* make a bitmap for use as a texture map */
            SetPenColor (texture2, RGB2Pixel(texture2, pal[1].r,pal[1].g,pal[1].b));
            LineEx(texture2, 0, 0, 15, 15);
            LineEx(texture2, 0, 15, 15, 0);
            Rectangle(texture2, 0, 0, 15, 15);
            
            SetBrushColor (mem_dc, RGB2Pixel(mem_dc, pal[0].r, pal[0].g, pal[0].b));
            FillBox (mem_dc, 0, 0, SCREEN_W, SCREEN_H);
            
            break;
        case MSG_PAINT:
			hdc= BeginPaint(hWnd);
            mg3dClearZbuffer(zbuf, 0.);
            render(mem_dc, texture1, texture2);
			BitBlt(mem_dc, 0, 0, SCREEN_W, SCREEN_H, hdc, 0, 0, 0);
			EndPaint(hWnd,hdc);
            return 0;
        case MSG_KEYDOWN:
            switch (wParam){
                case SCANCODE_Q:
                    if (lParam & KS_SHIFT) {
                        render_mode++;
                        if (render_mode >= last_mode) {
                            render_mode = flat;
                            color_map = &light_table;
                        }
                        if (render_type[render_mode] >= (MG3D_POLYTYPE_ATEX_TRANS|MG3D_POLYTYPE_ZBUF))
                            color_map = &trans_table;
                        
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    else{
                        render_mode++;
                        if (render_mode >= last_mode) {
                            render_mode = flat;
                            color_map = &light_table;
                        }
                        if (render_type[render_mode] >= (MG3D_POLYTYPE_ATEX_TRANS|MG3D_POLYTYPE_ZBUF))
                            color_map = &trans_table;
                        
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    break;

                case SCANCODE_W:
                    if (lParam & KS_SHIFT) {
                        if (viewport_w < SCREEN_W){
                            viewport_w += 8;
                            InvalidateRect(hWnd, NULL, FALSE);
                        }
                    }
                    else {
                        if (viewport_w > 16){
                            viewport_w -= 8;
                            InvalidateRect(hWnd, NULL, FALSE);
                        }
                    }
                    break;
                case SCANCODE_H:
                    if (lParam & KS_SHIFT) {
                        if (viewport_h < SCREEN_H){
                            viewport_h += 8;
                            InvalidateRect(hWnd, NULL, FALSE);
                        }
                    }
                    else {
                        if (viewport_h > 16){
                            viewport_h -= 8;
                            InvalidateRect(hWnd, NULL, FALSE);
                        }
                    }
                    break;
                case SCANCODE_X:
                    if (lParam & KS_SHIFT) {
                        xoff += 1;
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    else {
                        xoff -= 1;
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    break;
                case SCANCODE_Y:
                    if (lParam & KS_SHIFT){
                        yoff += 1;
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    else{
                        yoff -= 1;
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    break;
                case SCANCODE_Z:
                    if (lParam & KS_SHIFT)
                        zoff += 1;
                    else
                        zoff -= 1;
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_S:
                    if (lParam & KS_SHIFT)
                        scale += 0.1;
                    else{
                        scale -= 0.1;
                        if(scale<0.1)
                            scale=0.1;
                    }
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;

                case SCANCODE_R:
                    if (lParam & KS_SHIFT){
                        rycube += 10;
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    else{
                        rycube += 10;
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    break;
                case SCANCODE_CURSORBLOCKLEFT:
                    rz -= 10;
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_CURSORBLOCKRIGHT:
                    rz += 10;
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_CURSORBLOCKDOWN:
                    rx -= 10;
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_CURSORBLOCKUP:
                    rx += 10;
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_PAGEDOWN:
                    ry -= 10;
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_PAGEUP:
                    ry += 10;
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;

            }
           	break;
        case MSG_CLOSE:
            DeleteMemDC (mem_dc);
            mg3dDestroyZbuffer(zbuf);
            DeleteMemDC (texture1);
            DeleteMemDC (texture2);
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
    CreateInfo.spCaption = "Zbuf Test Window";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = CameraWndProc;
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

