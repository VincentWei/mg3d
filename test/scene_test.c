#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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

#define CUBEVERTRICES  8
#define CUBEPLANES     6
#define PLANEVERTRICES 4
#define CLIPBUFFSIZE   12
#define MAXOBJECTS     13
#define SQRT2 1.414213562

const static int WNDWIDTH=640,WNDHEIGHT=480;
const static int SCREEN_W=640,SCREEN_H=480; 

typedef struct QUAD
{
    int v[4];
} QUAD;

static QUAD cube[] =
{
    {{ 2, 1, 0, 3 }},
    {{ 4, 5, 6, 7 }},
    {{ 0, 1, 5, 4 }},
    {{ 2, 3, 7, 6 }},
    {{ 4, 7, 3, 0 }},
    {{ 1, 2, 6, 5 }}
};

static mg3dVpf cubeVertrices[] =
{
    /* A small cube user can control */
    { -8., -8., -8., 0., 0., 72 },
    { -8.,  8., -8., 0., 0., 80 },
    {  8.,  8., -8., 0., 0., 95 },
    {  8., -8., -8., 0., 0., 88 },
    { -8., -8.,  8., 0., 0., 72 },
    { -8.,  8.,  8., 0., 0., 80 },
    {  8.,  8.,  8., 0., 0., 95 },
    {  8., -8.,  8., 0., 0., 88 },
};

static mg3dVpf coreVertrices[] = 
{
    /* A big cube, lie on the center of screen */
    { -32., -32., -32., 0., 0., 72},
    { -32.,  32., -32., 0., 0., 80},
    {  32.,  32., -32., 0., 0., 95},
    {  32., -32., -32., 0., 0., 88},
    { -32., -32.,  32., 0., 0., 72},
    { -32.,  32.,  32., 0., 0., 80},
    {  32.,  32.,  32., 0., 0., 95},
    {  32., -32.,  32., 0., 0., 88}
};

static mg3dVpf tetrahedronVertrices[] = 
{
    {          0.,        -10.,  0., 0., 0., 72},
    { -8.66025404, 4.142135624, -5., 0., 0., 80},
    {  8.66025404, 4.142135624, -5., 0., 0., 95},
    {           0.,4.142135624, 10., 0., 0., 88},
};
static int tetrahedron[4][3] = 
{
    {1,0,3},{0,2,1},{0,3,2},{3,2,1}
};

static mg3dVpf tetrahedronVertricesBuf[4];
static mg3dVpf *pTetrahedronPlanes[4][3];
static mg3dVpf tetrahedronClipOutVer[CLIPBUFFSIZE], *pTetrahedronClipOutVer[CLIPBUFFSIZE];
static mg3dVpf tetrahedronClipTmp[CLIPBUFFSIZE], *pTetrahedronClipTmp[CLIPBUFFSIZE];
 
static mg3dVpf cubeVertricesBuf[CUBEVERTRICES]; 
static mg3dVpf coreVertricesBuf[CUBEVERTRICES];
static mg3dVpf *pCubePlanes[CUBEPLANES][PLANEVERTRICES]; 
static mg3dVpf *pCorePlanes[CUBEPLANES][PLANEVERTRICES];
static mg3dVpf cubeClipOutVer[CLIPBUFFSIZE], *pCubeClipOutVer[CLIPBUFFSIZE];
static mg3dVpf cubeClipTmp[CLIPBUFFSIZE], *pCubeClipTmp[CLIPBUFFSIZE];
static mg3dVpf coreClipOutVer[CLIPBUFFSIZE], *pCoreClipOutVer[CLIPBUFFSIZE];
static mg3dVpf coreClipTmp[CLIPBUFFSIZE], *pCoreClipTmp[CLIPBUFFSIZE];
static HDC hWindowDC, hSceneDC;
volatile unsigned lastKey = 0; 
static float offsetX=0, offsetY=0, offsetZ=0;
static float rotaX=0.,rotaY=0.,rotaZ=0.;


static void initTetrahedronPlanes(void)
{
    int i;
    memcpy(&tetrahedronVertricesBuf, &tetrahedronVertrices, 
           sizeof(tetrahedronVertrices));
    for (i = 0; i < 4; i++) {
        pTetrahedronPlanes[i][0] = &(tetrahedronVertricesBuf[tetrahedron[i][0]]);
        pTetrahedronPlanes[i][1] = &(tetrahedronVertricesBuf[tetrahedron[i][1]]);
        pTetrahedronPlanes[i][2] = &(tetrahedronVertricesBuf[tetrahedron[i][2]]);
    }
    for (i = 0; i < CLIPBUFFSIZE; i++) {
        pTetrahedronClipTmp[i]    = &tetrahedronClipTmp[i];
        pTetrahedronClipOutVer[i] = &tetrahedronClipOutVer[i];
    }
}

static void 
initCubePlanes(void)
{
    int i;

    memcpy(&cubeVertricesBuf, &cubeVertrices, sizeof(cubeVertrices));
    memcpy(&coreVertricesBuf, &coreVertrices, sizeof(coreVertrices));
    
    for (i = 0; i < CUBEPLANES; i++) {
        pCubePlanes[i][0] = &(cubeVertricesBuf[cube[i].v[0]]);
        pCubePlanes[i][1] = &(cubeVertricesBuf[cube[i].v[1]]);
        pCubePlanes[i][2] = &(cubeVertricesBuf[cube[i].v[2]]);
        pCubePlanes[i][3] = &(cubeVertricesBuf[cube[i].v[3]]);
        pCorePlanes[i][0] = &(coreVertricesBuf[cube[i].v[0]]);
        pCorePlanes[i][1] = &(coreVertricesBuf[cube[i].v[1]]);
        pCorePlanes[i][2] = &(coreVertricesBuf[cube[i].v[2]]);
        pCorePlanes[i][3] = &(coreVertricesBuf[cube[i].v[3]]);
    }
    for (i = 0; i < CLIPBUFFSIZE; i++) {
        pCubeClipTmp[i]    = &cubeClipTmp[i];
        pCubeClipOutVer[i] = &cubeClipOutVer[i];
        pCoreClipTmp[i]    = &coreClipTmp[i];
        pCoreClipOutVer[i] = &coreClipOutVer[i];
    }
} 

#if 0
static void 
drawTetrahedron(mg3dMatrixf *pMatrix)
{
    int i, j, nv, out[CLIPBUFFSIZE];
    
    for (i = 0; i < 4; i++) {
        mg3dApplyMatrixF(pMatrix, 
            tetrahedronVertrices[i].x, tetrahedronVertrices[i].y, tetrahedronVertrices[i].z, 
            &(tetrahedronVertricesBuf[i].x), &(tetrahedronVertricesBuf[i].y), &(tetrahedronVertricesBuf[i].z)); 
    }
    
    for (i = 0; i < 4; i++) {
        nv = mg3dClipF(hSceneDC, MG3D_POLYTYPE_GCOL, 0.1, 1000., 
                       3, (const mg3dVpf**)pTetrahedronPlanes[i], 
                       pTetrahedronClipOutVer, pTetrahedronClipTmp, out);
        if (nv) {
            for (j = 0; j < nv; j++) {
                mg3dPerspProjectF(tetrahedronClipOutVer[j].x, tetrahedronClipOutVer[j].y, tetrahedronClipOutVer[j].z,
                                  &(tetrahedronClipOutVer[j].x), &(tetrahedronClipOutVer[j].y));
            }
            if (mg3dPolygonZNormalF(&(tetrahedronClipOutVer[0]),&(tetrahedronClipOutVer[1]), 
                                    &(tetrahedronClipOutVer[2])) > 0) { 
                mg3dScenePolygonF(MG3D_POLYTYPE_GCOL, 0, nv, pTetrahedronClipOutVer);
            }
        }
    }    
}
#endif

static void 
drawCore(mg3dMatrixf *pMatrix)
{
    int i, j, nv, out[CLIPBUFFSIZE];

    for (i = 0; i < CUBEVERTRICES; i++) {
        mg3dApplyMatrixF(pMatrix, 
            coreVertrices[i].x, coreVertrices[i].y, coreVertrices[i].z, 
            &(coreVertricesBuf[i].x), &(coreVertricesBuf[i].y), &(coreVertricesBuf[i].z)); 
    }
    
    for (i = 0; i < CUBEPLANES; i++) {
        nv = mg3dClipF(hSceneDC, MG3D_POLYTYPE_GCOL, 0.1, 1000., 
                       PLANEVERTRICES, (const mg3dVpf**)pCorePlanes[i], 
                       pCoreClipOutVer, pCoreClipTmp, out);
        if (nv) {
            for (j = 0; j < nv; j++) {
                mg3dPerspProjectF(coreClipOutVer[j].x, coreClipOutVer[j].y, coreClipOutVer[j].z,
                                  &(coreClipOutVer[j].x), &(coreClipOutVer[j].y));
            }
            if (mg3dPolygonZNormalF(&(coreClipOutVer[0]),&(coreClipOutVer[1]), 
                                    &(coreClipOutVer[2])) > 0) { 
                mg3dScenePolygonF(MG3D_POLYTYPE_GCOL, 0, nv, pCoreClipOutVer);
            }
        }
    }    
}

static void 
drawCube(mg3dMatrixf *pMatrix)
{
    int i, j, nv, out[CLIPBUFFSIZE];
    
    for (i = 0; i < CUBEVERTRICES; i++) {
        mg3dApplyMatrixF(pMatrix, cubeVertrices[i].x, 
            cubeVertrices[i].y, cubeVertrices[i].z, &(cubeVertricesBuf[i].x), 
            &(cubeVertricesBuf[i].y), &(cubeVertricesBuf[i].z)); 
    }    

    for (i = 0; i < CUBEPLANES; i++) {
        
        nv = mg3dClipF(hSceneDC, MG3D_POLYTYPE_GCOL, 0.1, 1000., 
                 PLANEVERTRICES, (const mg3dVpf**)pCubePlanes[i], 
                 pCubeClipOutVer, pCubeClipTmp, out);
        if (nv) {

            for (j = 0; j < nv; j++) {
                mg3dPerspProjectF(cubeClipOutVer[j].x, 
                    cubeClipOutVer[j].y, cubeClipOutVer[j].z,
                    &(cubeClipOutVer[j].x), &(cubeClipOutVer[j].y));
            }
            if (mg3dPolygonZNormalF(&(cubeClipOutVer[0]), 
                    &(cubeClipOutVer[1]), &(cubeClipOutVer[2])) > 0) { 
                mg3dScenePolygonF(MG3D_POLYTYPE_GCOL, 0, nv, pCubeClipOutVer);
            }
        }
    }
}

static void 
getCoreMatrix(mg3dMatrixf *pMatrix)
{
    

    switch (lastKey) 
    {
    case SCANCODE_CURSORBLOCKLEFT:
        offsetX -= 1.;
        break;
    case SCANCODE_CURSORBLOCKRIGHT:
        offsetX += 1.;
        break;
    case SCANCODE_CURSORBLOCKUP: 
        offsetY -= 1.;
        break;
    case SCANCODE_CURSORBLOCKDOWN: 
        offsetY += 1.;
        break;
    case SCANCODE_Z:
        offsetZ += 1.;
        break;
    case SCANCODE_X:
        offsetZ -= 1.;
        break;
    case SCANCODE_D:
        rotaY += 4.;
        break;
    case SCANCODE_A:
        rotaY -= 4.;
        break;
    case SCANCODE_S:
        rotaX -= 4.;
        break;
    case SCANCODE_W:
        rotaX += 4.;
        break;
    case SCANCODE_Q:
        rotaZ += 4.;
        break;
    case SCANCODE_E:
        rotaZ -= 4.;
        break;
    default:
        break;
    }
    lastKey = 0;
    mg3dGetTransformMatrixF(pMatrix, 1., rotaX, rotaY, rotaZ, 
                            offsetX, offsetY, offsetZ+100.);
}

static void 
getCube1Matrix(mg3dMatrixf *pMatrix, float angle)
{
    static float r1 = 0;
    r1+=0.01;
    
    mg3dGetTranslationMatrixF(pMatrix, offsetX+SQRT2*64.*cos(r1+angle), 
        offsetY+SQRT2*64.*sin(r1+angle), offsetZ+100);
}


static void 
getCube2Matrix(mg3dMatrixf *pMatrix, float angle)
{
    static float r2 = 0;
    r2+=0.01;
    
    mg3dGetTranslationMatrixF(pMatrix, offsetX, 
        offsetY+SQRT2*64.*sin(r2+angle), offsetZ+SQRT2*64.*cos(r2+angle)+100);
}


static void 
getCube3Matrix(mg3dMatrixf *pMatrix, float angle)
{
    static float r3 = 0;
    r3+=0.01;
    
    mg3dGetTranslationMatrixF(pMatrix, offsetX+SQRT2*64.*cos(r3+angle), 
        offsetY, offsetZ+100+SQRT2*64.*sin(r3+angle));
}

static void *
drawScreenThreadFunc(void *pdata)
{
    mg3dMatrixf cube1Matrix, cube2Matrix, cube3Matrix, coreMatrix;

    mg3dCreateScene(24 * MAXOBJECTS, CUBEPLANES * MAXOBJECTS);
    mg3dSetProjectionViewport(0, 0, SCREEN_W, SCREEN_H);
    
    initCubePlanes();
    initTetrahedronPlanes();
    for ( ; ; ) {
        getCube1Matrix(&(cube1Matrix), 0.);
        getCube2Matrix(&(cube2Matrix), 45.);
        getCube3Matrix(&(cube3Matrix), 90.);
        getCoreMatrix(&coreMatrix);
        //getTetrahedronMatrix(&tmatrix);
        SetBrushColor(hSceneDC, PIXEL_black);
        FillBox(hSceneDC, 0, 0, WNDWIDTH, WNDHEIGHT);
        mg3dClearScene(hSceneDC);
        drawCube(&cube1Matrix);
        drawCube(&cube2Matrix);
        drawCube(&cube3Matrix);
        drawCore(&coreMatrix);
        //drawTetrahedron(&coreMatrix);
        mg3dRenderScene();
        BitBlt(hSceneDC, 0, 0, WNDWIDTH, WNDHEIGHT, hWindowDC, 0, 0, 0);                  
    }
    return NULL;
}




static int 
mg3dSceneTestProc(HWND hWnd, int msg, WPARAM wParam, LPARAM lParam)
{
    static pthread_t tid;
    
    switch (msg)
    {
    case MSG_CREATE: 
      {   
          hSceneDC = CreateMemDC(SCREEN_W, SCREEN_H, DEPTH, 
                               MEMDC_FLAG_SWSURFACE,
                                 0, 0, 0, 0);
          hWindowDC = GetClientDC(hWnd);
          
          pthread_create(&tid, NULL, drawScreenThreadFunc, NULL);
          
          break;    
      } 
    case MSG_KEYDOWN:             
        switch (wParam) {
        case SCANCODE_CURSORBLOCKLEFT:
        case SCANCODE_CURSORBLOCKRIGHT:
        case SCANCODE_CURSORBLOCKUP:
        case SCANCODE_CURSORBLOCKDOWN:
        case SCANCODE_Z:
        case SCANCODE_X:
        case SCANCODE_A:
        case SCANCODE_D:
        case SCANCODE_W:
        case SCANCODE_S:
        case SCANCODE_Q:
        case SCANCODE_E:
            lastKey = (unsigned)(wParam);
            break;
        default:
            break;
        }
        break;
    case MSG_CLOSE:
        printf("End Game!!\n");
        pthread_cancel(tid);
        sleep(1);
        DeleteMemDC(hSceneDC);
        //ReleaseDC(hSceneDC);
        ReleaseDC(hWindowDC);
        DestroyMainWindow (hWnd);
        PostQuitMessage (hWnd);
		
    }
  
    return DefaultMainWinProc(hWnd, msg, wParam, lParam);
}






int 
MiniGUIMain(int argc, const char *argv[])
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
    if (JoinLayer (layer, argv[0], 0, 0) == INV_LAYER_HANDLE) {
        printf ("JoinLayer: invalid layer handle.\n");
        exit (1);
    }
#endif

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "A 'Best' 3D Simple All Over the World!    -_-b";
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
