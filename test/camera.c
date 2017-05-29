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

#define SCREEN_W	640
#define SCREEN_H	480

#ifndef M_PI
   #define M_PI   3.14159
#endif


/* display a nice 8x8 chessboard grid */
#define GRID_SIZE    8


/* convert radians to degrees */
#define DEG(n)    ((n) * 180.0 / M_PI)

#define MAX_3D_POINTS     1024

mg3dVpf v3dpoint[MAX_3D_POINTS];

mg3dVpf *vtx[MAX_3D_POINTS];
/* parameters controlling the camera and projection state */
int viewport_w = 320;
int viewport_h = 240;
int fov = 48;
float aspect = 1;
float xpos = 0;
float ypos = -2;
float zpos = -4;
float heading = 0;
float pitch = 0;
float roll = 0;

/* render a tile of the grid */
void draw_square(HDC hdc, mg3dMatrixf *camera)
{

    mg3dVpf _v[4], _vout[8], _vtmp[8];
    mg3dVpf *v[4], *vout[8], *vtmp[8];
    int flags[4], out[8];
    int c, vc;
    
    for (c=0; c<4; c++)
        v[c] = &_v[c];

    for (c=0; c<8; c++) {
        vout[c] = &_vout[c];
        vtmp[c] = &_vtmp[c];
    }

    /* set up four vertices with the world-space position of the tile */
#if 1
    v[0]->x = -vtx[0]->x;
    v[0]->y = 0;
    v[0]->z = -vtx[0]->y;

    v[1]->x = vtx[1]->x;
    v[1]->y = 0;
    v[1]->z = -vtx[1]->y;

    v[2]->x = vtx[2]->x;
    v[2]->y = 0;
    v[2]->z = vtx[2]->y;

    v[3]->x = -vtx[3]->x;
    v[3]->y = 0;
    v[3]->z = vtx[3]->y;
#endif     
#if 0    
    v[0]->x = -vtx[0]->x;
    v[0]->y = 0;
    v[0]->z = -vtx[0]->y;

    v[1]->x = vtx[1]->x;
    v[1]->y = 0;
    v[1]->z = -vtx[1]->y;

    v[2]->x = vtx[2]->x;
    v[2]->y = 0;
    v[2]->z = vtx[2]->y;

    v[3]->x = -vtx[3]->x;
    v[3]->y = 0;
    v[3]->z = vtx[3]->y;

#endif 
    /* apply the camera matrix, translating world space -> view space */
    v[0]->c = RGB2Pixel(hdc, 205, 155, 55);
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
	
    if (flags[0] | flags[1] | flags[2] | flags[3]) {
		/* clip if any vertices are off the edge of the screen */
		vc = mg3dClipF (hdc,MG3D_POLYTYPE_FLAT, 0.1, 0.1, 4, (const mg3dVpf **)v, vout, vtmp, out);

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

    /* set the color */

	/* render the polygon */
   mg3dPolygonF(hdc, MG3D_POLYTYPE_FLAT, 0, vc, vout);
	
}

/* draw everything */
void render(HDC hdc)
{
    mg3dMatrixf roller, camera;
	int x, y, w, h;
	float xfront, yfront, zfront;
	float xup, yup, zup;
	char buf[80];
	
	/* clear the background */
	SetBrushColor (hdc, PIXEL_lightwhite);
	FillBox (hdc, 0, 0, SCREEN_W, SCREEN_H);


	/* set up the viewport region */
	x = (SCREEN_W - viewport_w) / 2;
	y = (SCREEN_H - viewport_h) / 2;
	w = viewport_w;
	h = viewport_h;

	mg3dSetProjectionViewport(x, y, w, h);
	
		
	/* calculate the in-front vector */
    xfront = sin(heading) * cos(pitch);
    yfront = sin(pitch);
    zfront = cos(heading) * cos(pitch);

	/* rotate the up vector around the in-front vector by the roll angle */
    mg3dGetVectorRotMatrixF(&roller, xfront, yfront, zfront, roll*128.0/M_PI);
    mg3dApplyMatrixF(&roller, 0, -1, 0, &xup, &yup, &zup);
	
	/* build the camera matrix */
    mg3dGetCameraMatrixF(&camera,
    			xpos, ypos, zpos,        /* camera position */
		        xfront, yfront, zfront,  /* in-front vector */
		        xup, yup, zup,           /* up vector */
		        fov,                     /* field of view */
		        aspect);                 /* aspect ratio */
	/* draw the grid of squares */
//	for (i=0; i<GRID_SIZE; i++)
//		for (j=0; j<GRID_SIZE; j++)
			draw_square(hdc, &camera);

	SetPenType (hdc, PT_SOLID);
	SetPenWidth (hdc, 1);
	SetPenColor(hdc, PIXEL_red);
	Rectangle(hdc, x, y, x+w, y+h);

	/* overlay some text */
	SetBkMode(hdc, BM_TRANSPARENT);
	sprintf(buf, "Viewport width: %d (w/W changes)", viewport_w);
    TextOut(hdc, 0, 0, buf);
    sprintf(buf, "Viewport height: %d (h/H changes)", viewport_h);
    TextOut(hdc, 0, 20, buf);
    sprintf(buf, "Field of view: %d (f/F changes)", fov);
    TextOut(hdc, 0, 40, buf);
    sprintf(buf, "Aspect ratio: %.2f (a/A changes)", aspect);
    TextOut(hdc, 0, 60, buf);
    sprintf(buf, "X position: %.2f (x/X changes)", xpos);
    TextOut(hdc, 0, 80, buf);
    sprintf(buf, "Y position: %.2f (y/Y changes)", ypos);
    TextOut(hdc, 0, 100, buf);
    sprintf(buf, "Z position: %.2f (z/Z changes)", zpos);
    TextOut(hdc, 0, 120, buf);
    sprintf(buf, "Heading: %.2f deg (left/right changes)", DEG(heading));
    TextOut(hdc, 0, 140, buf);
    sprintf(buf, "Pitch: %.2f deg (pgup/pgdn changes)", DEG(pitch));
    TextOut(hdc, 0, 160, buf);
    sprintf(buf, "Roll: %.2f deg (r/R changes)", DEG(roll));
    TextOut(hdc, 0, 180, buf);
    sprintf(buf, "Front vector: %.2f, %.2f, %.2f", xfront, yfront, zfront);
    TextOut(hdc, 0, 200, buf);
	sprintf(buf, "Up vector: %.2f, %.2f, %.2f", xup, yup, zup);
    TextOut(hdc, 0, 220, buf);

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

static int point_count;
static int CameraWndProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{

    static HDC hdc;
    static HDC mem_dc;
    int cur_x, cur_y;
    static int prev_x, prev_y;  
    static int first_x, first_y;
    static int first_click = TRUE;
    switch ( message ) {
        case MSG_CREATE:
            mem_dc = CreateMemDC (SCREEN_W, SCREEN_H, DEPTH,
                    MEMDC_FLAG_SWSURFACE, 0x00, 0x00, 0x00, 0x00);
       	    SetBrushColor (mem_dc, RGB2Pixel (mem_dc, 0xFF, 0xFF, 0xFF));
            FillBox (mem_dc, 0, 0, SCREEN_W, SCREEN_H);
            hdc= GetClientDC(hWnd);
            break;
        case MSG_PAINT:
	    break;
			
        case MSG_KEYDOWN:
            switch (wParam){
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
                case SCANCODE_F:
                    if (lParam & KS_SHIFT) {
                        if (fov < 96){
                            fov++;
                            InvalidateRect(hWnd, NULL, FALSE);
                        }
                    }
                    else {
                        if (fov > 16){
                            fov--;
                            InvalidateRect(hWnd, NULL, FALSE);
                        }
                    }
                    break;
                case SCANCODE_A:
                    if (lParam & KS_SHIFT) {
                        aspect += 0.05;
                        if (aspect > 2)
                            aspect = 2;
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    else{
                        aspect -= 0.05;
                        if (aspect < .1)
                            aspect = .1;
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    break;
                case SCANCODE_X:
                    if (lParam & KS_SHIFT){
                        xpos += 0.05;
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    else{
                        xpos -= 0.05;
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    break;
                case SCANCODE_Y:
                    if (lParam & KS_SHIFT){
                        ypos += 0.05;
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    else{
                        ypos -= 0.05;
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    break;
                case SCANCODE_Z:
                    if (lParam & KS_SHIFT)
                        zpos += 0.05;
                    else
                        zpos -= 0.05;
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_CURSORBLOCKLEFT:
                    heading += 0.05;
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_CURSORBLOCKRIGHT:
                    heading -= 0.05;
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_PAGEDOWN:
                    if (pitch > -M_PI/4){
                        pitch -= 0.05;
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    break;
                case SCANCODE_PAGEUP:
                    if (pitch < M_PI/4){
                        pitch += 0.05;
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    break;
                case SCANCODE_R:
                    if (lParam & KS_SHIFT) {
                        if (roll < M_PI/4){
                            roll += 0.05;
                            InvalidateRect(hWnd, NULL, FALSE);
                        }
                    }
                    else {
                        if (roll > -M_PI/4){
                            roll -= 0.05;
                            InvalidateRect(hWnd, NULL, FALSE);
                        }
                    }                        
                    break;
                case SCANCODE_CURSORBLOCKDOWN:
                    xpos += sin(heading) / 4;
                    zpos += cos(heading) / 4;
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case SCANCODE_CURSORBLOCKUP:
                    xpos -= sin(heading) / 4;
                    zpos -= cos(heading) / 4;
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
            }
	    render(mem_dc);
	    BitBlt(mem_dc, 0, 0, SCREEN_W, SCREEN_H, hdc, 0, 0, 0);
           	break;
        case MSG_LBUTTONDOWN:
	    SetCapture(hWnd);
	   
	    cur_x = LOWORD(lParam);
            cur_y = HIWORD(lParam);	     
	    v3dpoint[point_count].x = cur_x<<16;
	    v3dpoint[point_count].y = cur_y<<16; 
	    v3dpoint[point_count].z = 0; 
	    v3dpoint[point_count].u = 32<<16; 
	    v3dpoint[point_count].v = 0; 
	    v3dpoint[point_count].c = RGB2Pixel(mem_dc, rand()%255, rand()%255, rand()%255); 
	    vtx[point_count] = &v3dpoint[point_count];
	    point_count++;
	    if (first_click)
	    { 
                first_x = cur_x;
	        first_y = cur_y;	
                 
		prev_x = cur_x;
		prev_y = cur_y;

		SetPixel(hdc, first_x, first_y, PIXEL_black);
		first_click = FALSE; 
		break;
	    }

	                                                                                  
	    LineEx(hdc, prev_x, prev_y, cur_x, cur_y); 
	    prev_x = cur_x;
	    prev_y = cur_y;
	     break;

        case MSG_RBUTTONDOWN:
	     LineEx(hdc, prev_x, prev_y, first_x, first_y);
	     break;

	case MSG_LBUTTONUP:
             ReleaseCapture();
	     break;


        case MSG_CLOSE:
	    ReleaseDC(hdc); 
            DeleteMemDC (mem_dc);
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
    CreateInfo.spCaption = "Camera Window";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = CameraWndProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = SCREEN_W;
    CreateInfo.by = SCREEN_H;
    CreateInfo.iBkColor = PIXEL_lightwhite;
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

