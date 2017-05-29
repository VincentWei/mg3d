#include <stdio.h>


#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

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

#define WNDWIDTH  640 
#define WNDHEIGHT 480 
#define SCREEN_W  640 
#define SCREEN_H  480 

#define MAX_CUBES 4

typedef struct QUAD
{
    int v[4];
} QUAD;

GAL_Color pal[256];

mg3dVpf vertex[] =
{
    { -8., -8., -8., 0., 0., 72 },
    { -8.,  8., -8., 0., 0., 80 },
    {  8.,  8., -8., 0., 0., 95 },
    {  8., -8., -8., 0., 0., 88 },
    { -8., -8.,  8., 0., 0., 72 },
    { -8.,  8.,  8., 0., 0., 80 },
    {  8.,  8.,  8., 0., 0., 95 },
    {  8., -8.,  8., 0., 0., 88 }
};

GAL_Color despal[16] = 
{
   { 63*4, 63*4, 63*4, 0 },   { 63*4, 0,  0,  0 },   { 0,  63*4, 0,  0 },   { 63*4, 63*4, 0,  0 },
   { 0,  0,  63*4, 0 },   { 63*4, 0,  63*4, 0 },   { 0,  63*4, 63*4, 0 },   { 16*4, 16*4, 16*4, 0 },
   { 31*4, 31*4, 31*4, 0 },   { 63*4, 31*4, 31*4, 0 },   { 31*4, 63*4, 31*4, 0 },   { 63*4, 63*4, 31*4, 0 },
   { 31*4, 31*4, 63*4, 0 },   { 63*4, 31*4, 63*4, 0 },   { 31*4, 63*4, 63*4, 0 },   { 0,  0,  0,  0 }
};



QUAD cube[] =
{
    {{ 2, 1, 0, 3 }},
    {{ 4, 5, 6, 7 }},
    {{ 0, 1, 5, 4 }},
    {{ 2, 3, 7, 6 }},
    {{ 4, 7, 3, 0 }},
    {{ 1, 2, 6, 5 }}
};


mg3dVpf v[4], vout[12], vtmp[12];
mg3dVpf *pv[4], *pvout[12], *pvtmp[12];


static HDC hdc, hSceneDC, hTextureDC = 0;
static mg3dMatrixf matrix, matrix1, matrix2, matrix3;
static int rx = 0, ry = 0, tz = 40;
static int rot = 0, inc = 1;
static int i, j, k;

static int cur_render_type = MG3D_POLYTYPE_FLAT;

const char *note[15] =
{
    "flat",
    "gcol",
    "grgb",
    "atex",
    "ptex",
    "atex-mask",
    "ptex-mask",
    "atex-light",
    "ptex-light",
    "atex-mask-light",
    "ptex-mask-light",
    "atex-trans",
    "ptex-trans",
    "atex-mask-trans",
    "ptex-mask-trans"
};

/* draw_cube:
 *  Translates, rotates, clips, projects, culls backfaces and draws a cube.
 */
void draw_cube(mg3dMatrixf *matrix, int num_poly)
{
    int i, j, nv;
    int out[12];
   
    for (i =0; i<num_poly; i++) 
    {
        for (j=0; j<4; j++) 
        {
            v[j] = vertex[cube[i].v[j]];
            mg3dApplyMatrixF(matrix, v[j].x, v[j].y, v[j].z,
                           &v[j].x, &v[j].y, &v[j].z);
        }
        /* four vertices */
        v[0].u =  0;
        v[0].v =  0;
        v[1].u =  16;
        v[1].v =  0;
        v[2].u =  16;
        v[2].v =  16;
        v[3].u =  0;
        v[3].v =  16;
     
        /* nv: number of vertices after clipping is done */
        nv = mg3dClipF(hSceneDC, cur_render_type, 
                      0.1, 1000., 4, 
                      (const mg3dVpf**)pv, pvout,
                      pvtmp, out);
        
        if (nv)  
        {
            for (j = 0; j < nv; j++)
            {
                mg3dPerspProjectF(vout[j].x, vout[j].y, vout[j].z, 
                                &(vout[j].x), &(vout[j].y));
            }

            if (mg3dPolygonZNormalF(&vout[0], &vout[1], &vout[2]) > 0)
            { 
               mg3dScenePolygonF(cur_render_type, hTextureDC, nv, pvout);
            }
        
        }
    }
}


int init_palette(GAL_Color *pal)
{
   int i;
   
   /* color 0 = black */
   pal[0].r = pal[0].g = pal[0].b = 0;
   /* color 1 = blue*/
   pal[1].b = 63*4;
   pal[1].g = pal[1].r = 0;

   /* copy the desktop palette */
   for (i=2; i<64; i++)
      pal[i] = despal[i];

   /* make a blue gradient */
   for (i=64; i<96; i++) {
      pal[i].b = (i-64) * 8;
      pal[i].g = pal[i].r = 0;
   }

   for (i=96; i<256; i++)
      pal[i].r = pal[i].g = pal[i].b = 0; 

   SetPalette(hSceneDC, 0, 255, pal);
   SetPalette(hTextureDC, 0, 255, pal);

   return 0;
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

int mg3dSceneTestProc(HWND hWnd, int msg, WPARAM wParam, LPARAM lParam)
{
    
    static int ret;

    switch (msg)
    {
    case MSG_CREATE:
      {   
      
          hSceneDC = CreateMemDC(SCREEN_W, SCREEN_H, DEPTH, 
                                 MEMDC_FLAG_SWSURFACE,
                                 0, 0, 0, 0);
          hTextureDC = CreateMemDC(16, 16, 16,
                MEMDC_FLAG_SWSURFACE, 0x00, 0x00, 0x00, 0x00);

          init_palette(pal);
           
          ret = mg3dCreateScene(24 * MAX_CUBES * MAX_CUBES * MAX_CUBES,
                             6 * MAX_CUBES * MAX_CUBES * MAX_CUBES);


          SetBrushColor(hTextureDC, RGB2Pixel(hTextureDC, pal[0].r,pal[0].g,pal[0].b));
          FillBox(hTextureDC, 0, 0, 16, 16);

          /* make a bitmap for use as a hTextureDC map */
          SetPenColor (hTextureDC, RGB2Pixel(hTextureDC, pal[1].r,pal[1].g,pal[1].b));
          SetBkMode(hTextureDC, BM_TRANSPARENT);

          LineEx(hTextureDC, 0, 0, 15, 15);
          LineEx(hTextureDC, 0, 15, 15, 0);
          Rectangle(hTextureDC, 0, 0, 15, 15);

          SetTextColor (hTextureDC, RGB2Pixel (hTextureDC, 63*4, 63*4, 63*4));
          TextOut(hTextureDC, 0, 0, "text");

          if (!ret)
          {
              mg3dSetProjectionViewport(0, 0, SCREEN_W, SCREEN_H);
              
              /* initialize pointers */
              for (j=0; j<4; j++) 
              {
                  pv[j] = &v[j];
              }
              
              for (j=0; j<12; j++) 
              {
                  pvtmp[j] = &vtmp[j];
                  pvout[j] = &vout[j];
              }
          }
          SetTimer(hWnd, 100, 1);
      } 
      break;

    case MSG_KEYDOWN:
    {
        if (wParam == SCANCODE_CURSORBLOCKLEFT)
        {
            cur_render_type++; 
            if (cur_render_type >= 7){
                cur_render_type = 0;
            }
        }
        break;
    }
    case MSG_TIMER:
      {                      
          if (!ret)
          {
              hdc = GetClientDC(hWnd);
              SetBrushColor(hSceneDC, PIXEL_black);
              FillBox(hSceneDC, 0, 0, WNDWIDTH, WNDHEIGHT);

              mg3dClearScene(hSceneDC);
          
              mg3dGetRotMatrixF(&matrix2, rx, ry, 0);
              mg3dGetRotMatrixF(&matrix3, 0, rot, 0);
          
              for (k = MAX_CUBES - 1; k >= 0; k--) { 
                  for (j = 0; j < MAX_CUBES; j++) {
                      for (i = 0; i < MAX_CUBES; i++) 
                      {
                          mg3dGetTranslationMatrixF(&matrix1, j*40-MAX_CUBES*20+20,
                                                   i*40-MAX_CUBES*20+20, tz+k*40);
                          mg3dMatrixMulF(&matrix2, &matrix1, &matrix);
                          mg3dMatrixMulF(&matrix, &matrix3, &matrix);
                          draw_cube(&matrix, 6);
                      }
                  }
              }
          
              mg3dRenderScene();

              SetBkMode(hSceneDC, BM_TRANSPARENT);
              SetTextColor (hSceneDC, RGB2Pixel (hTextureDC, 255, 255, 0));
              TextOut(hSceneDC, 5, 0, "Press left key to change render mode");
              TextOut(hSceneDC, 300, 0, note[cur_render_type]);
              
              BitBlt(hSceneDC, 0, 0, WNDWIDTH, WNDHEIGHT, hdc, 0, 0, 0);                  

          
              /* manage cubes movement */
              tz -= 2;
              if (!tz) tz = 40;
              rx += 4;
              ry += 4;
              rot += inc;

              if ((rot >= 25) || (rot <= -25)) inc = -inc;
              ReleaseDC(hdc);
          }
          break;
      }

    case MSG_CLOSE:
        mg3dDestroyScene();
		DeleteMemDC (hSceneDC);
		DeleteMemDC (hTextureDC);
		PostQuitMessage (hWnd);
        DestroyMainWindow (hWnd);
        break;
    }
    
    return DefaultMainWinProc(hWnd, msg, wParam, lParam);
}


int MiniGUIMain(int argc, const char *argv[])
{
    
    MSG msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    int i;
    const char* layer = NULL;

    for (i = 1; i < argc; i++) {
        if (strcmp (argv[i], "-layer") == 0) {
            layer = argv[i + 1];
            break;
        }
    }
    GetLayerInfo (layer, NULL, NULL, NULL);
    //if (JoinLayer ("same","same"/* argv[0]*/, 0, 0) == INV_LAYER_HANDLE) {
    if (JoinLayer (layer, argv[0], 0, 0) == INV_LAYER_HANDLE) {
        printf ("JoinLayer: invalid layer handle.\n");
        exit (1);
    }
#endif

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "3D";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = mg3dSceneTestProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = WNDWIDTH;
    CreateInfo.by = WNDHEIGHT;
    CreateInfo.iBkColor = PIXEL_black;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    hMainWnd = CreateMainWindow(&CreateInfo);

    if (hMainWnd == HWND_INVALID)
    {
        return -1;
    }
								     
    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&msg, hMainWnd))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    } 
									         
    MainWindowThreadCleanup(hMainWnd);
    return 0;
}

#ifndef _LITE_BERSION
#include <minigui/dti.c>
#endif
