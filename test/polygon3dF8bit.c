
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

#undef DEPTH
#define DEPTH          8 

#define SCREEN_W	640
#define SCREEN_H	480
#define MAX_3D_POINTS     1024


enum {
    flat,
    grgb,
    gcol,
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
} render_mode = flat;

int render_type[] =
{
    MG3D_POLYTYPE_FLAT,
    MG3D_POLYTYPE_GRGB,
    MG3D_POLYTYPE_GCOL,
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


GAL_Color pal[256];

GAL_Color despal[16] = 
{
   { 63*4, 63*4, 63*4, 0 },   { 63*4, 0,  0,  0 },   { 0,  63*4, 0,  0 },   { 63*4, 63*4, 0,  0 },
   { 0,  0,  63*4, 0 },   { 63*4, 0,  63*4, 0 },   { 0,  63*4, 63*4, 0 },   { 16*4, 16*4, 16*4, 0 },
   { 31*4, 31*4, 31*4, 0 },   { 63*4, 31*4, 31*4, 0 },   { 31*4, 63*4, 31*4, 0 },   { 63*4, 63*4, 31*4, 0 },
   { 31*4, 31*4, 63*4, 0 },   { 63*4, 31*4, 63*4, 0 },   { 31*4, 63*4, 63*4, 0 },   { 0,  0,  0,  0 }
};

/* lighting color mapping table */
MG3D_COLOR_MAP light_table;

MG3D_RGB_MAP rgb_table;

/* transparency color mapping table */
MG3D_COLOR_MAP trans_table;

void print_progress(int pos)
{
    ;
}

struct poly_info
{
    int type;
    int vc;
    mg3dVpf v3d[1024];
    mg3dVpf *vtx[1024];
}poly_info[100];

gal_pixel color[100];
gal_pixel crgb[100];

static int MG3DWndProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    static HDC texture;
    int c;
    int cur_x, cur_y;
    static int prev_x, prev_y;  
    static int first_x, first_y;
    static int first_click = TRUE;
    static HDC hdc;
    static int point_count;
    static HDC mem_dc;
    static int poly_count;
    static BITMAP bmp;
    int i = 0;
    
    switch ( message ) {
        case MSG_CREATE:
            mem_dc = CreateMemDC(SCREEN_W, SCREEN_H, 8,
                    MEMDC_FLAG_SWSURFACE, 0x00, 0x00, 0x00, 0x00);
            texture = CreateMemDC(32, 32, 8,
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
            
            SetPalette(mem_dc, 0, 255, pal);
            SetPalette(texture, 0, 255, pal);
	    for (i=0; i<100;)
	    {  
                color[i++] = RGB2Pixel(mem_dc,0xD0, 0xD0, 0xD0);
                color[i++] = RGB2Pixel(mem_dc, 0xf0, 0xf0, 0xf0);
                color[i++] = RGB2Pixel(mem_dc, 0xB0, 0xB0, 0xB0);
                color[i++] = RGB2Pixel(mem_dc, 0x0F, 0x0F, 0x0F);
	    }

	    for (i=0; i<100;)
	    {  
                crgb[i++] = 0x000000;
                crgb[i++] = 0x7F0000;
                crgb[i++] = 0xFF0000;
                crgb[i++] = 0x7F0000;
	    }

            mg3dCreateLightTable(&light_table, pal, 0, 0, 0, print_progress);
            color_map = &light_table;
            
            /* textures are 25% transparent (75% opaque) */
            mg3dCreateTransTable(&trans_table, pal, 128, 128, 128, print_progress);

            
            /* set up the truecolor blending functions */
            /* textures are 25% transparent (75% opaque) */
            mg3dSettransBlender(100, 100, 100, 192);

	    SetBkMode(mem_dc, BM_TRANSPARENT);
	    SetTextColor(mem_dc, RGB2Pixel(mem_dc, pal[0].r, pal[0].g, pal[0].b));
	    SetBrushColor(mem_dc, RGB2Pixel(mem_dc, pal[255].r, pal[255].g, pal[255].b));
	    FillBox(mem_dc, 0, 0, SCREEN_W, SCREEN_H);
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

	    hdc = GetClientDC(hWnd);
            break;

        case MSG_TIMER:
            break;

	case MSG_PAINT:
	    FillBox(mem_dc, 0, 0, SCREEN_W, SCREEN_H);
            switch (render_type[render_mode])
	   {
		case MG3D_POLYTYPE_FLAT:
		    TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_FLAT");
		    break;

		case MG3D_POLYTYPE_GCOL:
		    TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_GCOL");
		    break;
			   
		case MG3D_POLYTYPE_GRGB:
		    TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_GRGB");
		    break;

		case MG3D_POLYTYPE_ATEX:
		    TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_ATEX");
		    break;

		case MG3D_POLYTYPE_PTEX:
		    TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_PTEX");
		    break;

		case MG3D_POLYTYPE_ATEX_MASK:
		    TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_ATEX_MASK");
		    break;

		case MG3D_POLYTYPE_PTEX_MASK:
		    TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_PTEX_MASK");
		    break;

		case MG3D_POLYTYPE_ATEX_LIT:
		   TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_ATEX_LIT");
		    break;

		case MG3D_POLYTYPE_PTEX_LIT:
		    TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_PTEX_LIT");
		    break;
		    
		case MG3D_POLYTYPE_ATEX_MASK_LIT:
		    TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_ATEX_MASK_LIT");
		    break;

		case MG3D_POLYTYPE_PTEX_MASK_LIT:
		    TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_PTEX_MASK_LIT");
		    break;

		case MG3D_POLYTYPE_ATEX_TRANS:
		    TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_ATEX_TRANS");
		    break;

		case MG3D_POLYTYPE_PTEX_TRANS:
		    TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_PTEX_TRANS");
		    break;

		case MG3D_POLYTYPE_ATEX_MASK_TRANS:
		    TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_ATEX_MASK_TRANS");
		    break;

		case MG3D_POLYTYPE_PTEX_MASK_TRANS:
		    TextOut(mem_dc, 10, 10, "MG3D_POLYTYPE_PTEX_MASK_TRANS");
		    break;
 
	    }
	        mg3dPolygonF(mem_dc,  render_type[poly_info[poly_count-1].type], texture, poly_info[poly_count-1].vc, poly_info[poly_count-1].vtx);
	    BitBlt(mem_dc, 0, 0, SCREEN_W, SCREEN_H, hdc, 0, 0, 0);
	    BitBlt(texture, 0, 0, 31, 31, hdc, 600, 0, 0);
	    break;

        case MSG_KEYDOWN:
            switch (wParam){
                case SCANCODE_ESCAPE:
                    PostMessage(hWnd, MSG_CLOSE, 0, 0);
                    break;

                case SCANCODE_CURSORBLOCKLEFT:
                    render_mode++;
                    if (render_mode >= last_mode) {
                        render_mode = flat;
                        color_map = &light_table;
                    }
                    
                    if (render_type[render_mode] >= MG3D_POLYTYPE_ATEX_TRANS)
                       color_map = &trans_table;
		    
		    if (render_type[render_mode] == MG3D_POLYTYPE_GRGB)
		    {
			for (i=0; i<poly_info[poly_count-1].vc; i++)
			    poly_info[poly_count-1].v3d[i].c = crgb[rand()%99];
		    }

		    
		    if (render_type[render_mode] == MG3D_POLYTYPE_GCOL)
		    {
			for (i=0; i<poly_info[poly_count-1].vc; i++)
			    poly_info[poly_count-1].v3d[i].c = color[i]; 
		    }
		    
		    poly_info[poly_count-1].type = render_mode;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;

		case SCANCODE_CURSORBLOCKRIGHT:
		    break;

		case SCANCODE_CURSORBLOCKDOWN:
                    InvalidateRect(hWnd, NULL, TRUE);
		    break;
            }
           break;

	case MSG_LBUTTONDOWN:
	    SetCapture(hWnd);
	   
	    cur_x = LOWORD(lParam);
            cur_y = HIWORD(lParam);	     
            poly_info[poly_count].v3d[point_count].x = cur_x;
            poly_info[poly_count].v3d[point_count].y = cur_y;
            poly_info[poly_count].v3d[point_count].z = 6.;
            poly_info[poly_count].v3d[point_count].u = rand()%32;
            poly_info[poly_count].v3d[point_count].v = rand()%32;
	    poly_info[poly_count].v3d[point_count].c = RGB2Pixel(mem_dc, rand()%255, rand()%255, rand()%255); 
            poly_info[poly_count].vtx[point_count] = &poly_info[poly_count].v3d[point_count];
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
	     first_click = TRUE;
             poly_info[poly_count].vc = point_count;
	     point_count = 0;
	     poly_count++;
             //InvalidateRect(hWnd, NULL, TRUE);
	     break;

	case MSG_LBUTTONUP:
             ReleaseCapture();
	     break;

        case MSG_CLOSE:
	    UnloadBitmap(&bmp); 
	    DeleteMemDC(mem_dc);
	    DeleteMemDC(texture);
            ReleaseDC(hdc);
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
