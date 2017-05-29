/**
 * \file mg3d.h
 * \author Feynman Software
 * \date 2006/01/06
 * 
 * This file includes 3D polygon drawing routines.
 *
 \verbatim
    Copyright (C) 2006 ~ 2008 Feynman Software.

    This file is part of mg3d which is one of components of MiniGUI.
 \endverbatim
*/

/*
** $Id: mg3d.h,v 1.18 2006-01-14 09:48:09 xwyan Exp $
**
** mg3d.h: mg3d polygon drawing routines. 
**
** Copyright (C) 2006 ~ 2008 Feynman Software, all rights reserved.
**
** Use of this source package is subject to specific license terms
** from Beijing Feynman Software Technology Co., Ltd.
**
** URL: http://www.minigui.com
**
** Author: Feynman Software
**
*/


#ifndef __mg3D_H__
#define __mg3D_H__

#include <minigui/fixedmath.h>       
#include <minigui/common.h>
#include <minigui/gdi.h>
#include <minigui/minigui.h>       

#ifdef __MG3D_LIB__
#include "../mg3dconfig.h"
#else
#include "mg3dconfig.h"
#endif

/**
 * \defgroup 3d_fns polygon drawing routines 
 * @{
 */

/**
 * Structure that describes a vertice point in fixed point.
 *
 * The x item describes the x position of a 3d vertice.
 * The y item describes the y position of a 3d vertice.
 * The z item describes the z position of a 3d vertice.
 * The u and v items describes the coordinates in texture map. 
 * The c item describes the 3d vertice color. 
 */
typedef struct _mg3dVp               /* a 3d point (fixed point version) */
{
   fixed x, y, z;                   /* position */
   fixed u, v;                      /* texture map coordinates */
   gal_pixel c;                     /*color*/
} mg3dVp;


/**
 * Structure that describes a vertice point in floating point.
 * See also mg3dVp.
 */
typedef struct _mg3dVpf                /* a 3d point (floating point version) */
{
   float x, y, z;                   /* position */
   float u, v;                      /* texture map coordinates */
   gal_pixel c;                     /*color*/
} mg3dVpf;

/*===================== Polygon rendering mode type  =====================*/

/**
 * \def MG3D_POLYTYPE_FLAT
 * \brief A simple flat shaded polygon. Taking the color from the `c' value 
 *        of the first vertex.
 */
#define MG3D_POLYTYPE_FLAT               0

/**
 * \def MG3D_POLYTYPE_GCOL
 * \brief  A single-color gouraud shaded polygon. The colors for each vertex
 *         are taken from the `c' value, and interpolated across the polygon.  
 */
#define MG3D_POLYTYPE_GCOL               1

/**
 * \def MG3D_POLYTYPE_GRGB
 * \brief  A gouraud shaded polygon which interpolates RGB triplets rather 
 *         than a single color. 
 */
#define MG3D_POLYTYPE_GRGB               2

/**
 * \def MG3D_POLYTYPE_ATEX
 * \brief  An affine texture mapped polygon. This stretches the texture across 
 *         the polygon with a simple 2d linear interpolation, 
 */
#define MG3D_POLYTYPE_ATEX               3

/**
 * \def MG3D_POLYTYPE_PTEX
 * \brief  A perspective-correct texture mapped polygon. This uses the `z' 
 *         value from the vertex structure as well as the u/v coordinates, 
 */
#define MG3D_POLYTYPE_PTEX               4

/**
 * \def MG3D_POLYTYPE_ATEX_MASK
 * \brief  Like MG3D_POLYTYPE_ATEX, but zero texture map pixels are skipped, 
 *         allowing parts of the texture map to be transparent.
 */
#define MG3D_POLYTYPE_ATEX_MASK          5

/**
 * \def MG3D_POLYTYPE_PTEX_MASK
 * \brief  Like MG3D_POLYTYPE_PTEX, but zero texture map pixels are skipped, 
 *         allowing parts of the texture map to be transparent.
 */
#define MG3D_POLYTYPE_PTEX_MASK          6

/**
 * \def MG3D_POLYTYPE_ATEX_LIT
 * \brief  Like MG3D_POLYTYPE_ATEX , but the global color_map table (for
 *         256-color modes) or blender function (for non-MMX truecolor
 *         modes) is used to blend the texture with a light level taken 
 *         from the `c' value in the vertex structure. This must only be 
 *         used after you have set up the color mapping table or blender
 *         functions!
 */
#define MG3D_POLYTYPE_ATEX_LIT           7

/**
 * \def MG3D_POLYTYPE_PTEX_LIT
 * \brief  Like MG3D_POLYTYPE_PTEX , but the global color_map table (for
 *         256-color modes) or blender function (for non-MMX truecolor
 *         modes) is used to blend the texture with a light level taken 
 *         from the `c' value in the vertex structure. This must only be 
 *         used after you have set up the color mapping table or blender
 *         functions!
 */
#define MG3D_POLYTYPE_PTEX_LIT           8

/**
 * \def MG3D_POLYTYPE_ATEX_MASK_LIT
 * \brief  Like MG3D_POLYTYPE_ATEX_LIT, but zero texture 
 *         map pixels are skipped, allowing parts of the texture map to be 
 *         transparent.
 */
#define MG3D_POLYTYPE_ATEX_MASK_LIT      9

/**
 * \def MG3D_POLYTYPE_PTEX_MASK_LIT
 * \brief  Like MG3D_POLYTYPE_PTEX_LIT, but zero texture 
 *         map pixels are skipped, allowing parts of the texture map to be 
 *         transparent.
 */
#define MG3D_POLYTYPE_PTEX_MASK_LIT      10

/**
 * \def MG3D_POLYTYPE_ATEX_TRANS
 * \brief Render translucent textures. 
 */
#define MG3D_POLYTYPE_ATEX_TRANS         11

/**
 * \def MG3D_POLYTYPE_PTEX_TRANS
 * \brief Render translucent textures. 
 */
#define MG3D_POLYTYPE_PTEX_TRANS         12

/**
 * \def MG3D_POLYTYPE_ATEX_MASK_TRANS
 * \brief  Like MG3D_POLYTYPE_ATEX_TRANS, but zero 
 *         texture map pixels are skipped.
 */
#define MG3D_POLYTYPE_ATEX_MASK_TRANS    13

/**
 * \def MG3D_POLYTYPE_PTEX_MASK_TRANS
 * \brief  Like MG3D_POLYTYPE_PTEX_TRANS, but zero 
 *         texture map pixels are skipped.
 */
#define MG3D_POLYTYPE_PTEX_MASK_TRANS    14

/**
 * \def MG3D_POLYTYPE_MAX
 * \brief The maximum of polygen type.
 */
#define MG3D_POLYTYPE_MAX                15

/**
 * \def MG3D_POLYTYPE_ZBUF
 * \brief Zbuffer. 
 */
#define MG3D_POLYTYPE_ZBUF               16


/**
 * \fn void mg3dPolygon(HDC mem_dc, int type, HDC texture, int vc, mg3dVp *vtx[])
 * \brief Draw 3d polygons onto the specified memory DC, using the specified 
 *        rendering mode. These routines don't support concave or 
 *        self-intersecting shapes. The vertices parameter should be followed 
 *        by that many pointers to mg3dVp structures, which describe each vertex 
 *        of the polygon.
 *
 * \param mem_dc The specified memory DC.
 * \param type The specified rendering mode.
 * \param texture The width and height of the texture bitmap must be powers 
 *                of two, but can be different.
 * \param vc The vertex count parameter.
 * \param vtx An array containing the appropriate number of pointers to 
 *               the fixed point mg3dVp vertex structures.
 *
 */
void mg3dPolygon(HDC mem_dc, int type, HDC texture, int vc, mg3dVp *vtx[]);

/**
 * \fn void mg3dPolygonF(HDC mem_dc, int type, HDC texture, int vc, mg3dVpf *vtx[])
 * \brief Floating point version of mg3dPolygon().
 *
 * \param mem_dc The specified memory DC.
 * \param type The specified rendering mode.
 * \param texture The width and height of the texture bitmap must be powers 
 *                of two, but can be different.
 * \param vc The vertex count parameter.
 * \param vtx An array containing the appropriate number of pointers to 
 *               the floating point mg3dVpf vertex structures.
 *
 */
void mg3dPolygonF(HDC mem_dc, int type, HDC texture, int vc, mg3dVpf *vtx[]);

/**
 * \fn void mg3dTriangle(HDC mem_dc, int type, HDC texture, mg3dVp *v1, mg3dVp *v2, mg3dVp *v3)
 * \brief Draw 3d triangles, using fixed point vertex structures. Unlike mg3dQuad(), mg3dTriangle() 
 *        functions are not wrappers of mg3dPolygon(). The mg3dTriangle() functions use their own 
 *        routines taking into account the constantness of the gradients. Therefore mg3dTriangle()
 *        is faster than mg3dPolygon().
 *
 * \param mem_dc The specified memory DC.
 * \param type The specified rendering mode.
 * \param texture The width and height of the texture bitmap must be powers 
 *                of two, but can be different.
 * \param v1 The pointer to the fixed point mg3dVp vertex structure.
 * \param v2 The pointer to the fixed point mg3dVp vertex structure.
 * \param v3 The pointer to the fixed point mg3dVp vertex structure.
 *
 */
void mg3dTriangle(HDC mem_dc, int type, HDC texture, mg3dVp *v1, mg3dVp *v2, mg3dVp *v3);

/**
 * \fn void mg3dTriangleF(HDC mem_dc, int type, HDC texture, mg3dVpf *v1, mg3dVpf *v2, mg3dVpf *v3)
 * \brief Draw 3d triangles, using floating point vertex structures. Unlike mg3dQuadF(), 
 *        mg3dTriangleF functions are not wrappers of mg3dPolygonF. The mg3dTriangleF functions 
 *        use their own routines taking into account the constantness of the gradients. 
 *        Therefore mg3dTriangleF is faster than mg3dPolygonF.
 *
 * \param mem_dc The specified memory DC.
 * \param type The specified rendering mode.
 * \param texture The width and height of the texture bitmap must be powers 
 *                of two, but can be different.
 * \param v1 The pointer to the floating point mg3dVpf vertex structure.
 * \param v2 The pointer to the floating point mg3dVpf vertex structure.
 * \param v3 The pointer to the floating point mg3dVpf vertex structure.
 *
 */
void mg3dTriangleF(HDC mem_dc, int type, HDC texture, mg3dVpf *v1, mg3dVpf *v2, mg3dVpf *v3);

/**
 * \fn void mg3dQuad(HDC mem_dc, int type, HDC texture, mg3dVp *v1, mg3dVp *v2, mg3dVp *v3, mg3dVp *v4)
 * \brief Draw 3d quads, using fixed point vertex structures. These are equivalent to 
 *        calling mg3dPolygon.
 *
 * \param mem_dc The specified memory DC.
 * \param type The specified rendering mode.
 * \param texture The width and height of the texture bitmap must be powers 
 *                of two, but can be different.
 * \param v1 The pointer to the fixed point mg3dVp vertex structure.
 * \param v2 The pointer to the fixed point mg3dVp vertex structure.
 * \param v3 The pointer to the fixed point mg3dVp vertex structure.
 * \param v4 The pointer to the fixed point mg3dVp vertex structure.
 *
 */
void mg3dQuad(HDC mem_dc, int type, HDC texture, mg3dVp *v1, mg3dVp *v2, mg3dVp *v3, mg3dVp *v4);

/**
 * \fn void mg3dQuadF(HDC mem_dc, int type, HDC texture, mg3dVpf *v1, mg3dVpf *v2, mg3dVpf *v3, mg3dVpf *v4)
 * \brief Draw 3d quads, using floating point vertex structures. These are equivalent to calling 
 *        mg3dPolygonF.
 *
 * \param mem_dc The specified memory DC.
 * \param type The specified rendering mode.
 * \param texture The width and height of the texture bitmap must be powers 
 *                of two, but can be different.
 * \param v1 The pointer to the floating point mg3dVpf vertex structure.
 * \param v2 The pointer to the floating point mg3dVpf vertex structure.
 * \param v3 The pointer to the floating point mg3dVpf vertex structure.
 * \param v4 The pointer to the floating point mg3dVpf vertex structure.
 *
 */
void mg3dQuadF(HDC mem_dc, int type, HDC texture, mg3dVpf *v1, mg3dVpf *v2, mg3dVpf *v3, mg3dVpf *v4);

/**
 * \fn int mg3dClip(HDC mem_dc, int type, fixed min_z, fixed max_z, int vc, const mg3dVp *vtx[], mg3dVp *vout[], mg3dVp *vtmp[], int out[])
 * \brief Clips a 3D polygon against planes x = -z, x = z, y = -z, y = z. It is fixed point version. 
 *
 * \param mem_dc The memeory device context.
 * \param type The render type. 
 * \param min_z The minimum z value. 
 * \param max_z The maximum z value. 
 * \param vc The number of vertices. 
 * \param vtx The input vertices.
 * \param vout The array of receiving the result vertices. 
 * \param vtmp It is for temporary storage.
 * \param out It is for temporary storage.
 *
 * \return The number of vertices. 
 */
int mg3dClip(HDC mem_dc, int type, fixed min_z, fixed max_z, int vc, const mg3dVp *vtx[], mg3dVp *vout[], mg3dVp *vtmp[], int out[]);

/**
 * \fn int mg3dClipF(HDC mem_dc, int type, float min_z, float max_z, int vc, const mg3dVpf *vtx[], mg3dVpf *vout[], mg3dVpf *vtmp[], int out[])
 * \brief Clips a 3D polygon against planes x = -z, x = z, y = -z, y = z. It is float point version.
 *
 * \param mem_dc The memeory device context.
 * \param type The render type. 
 * \param min_z The minimum z value. 
 * \param max_z The maximum z value. 
 * \param vc The number of vertices. 
 * \param vtx The input vertices.
 * \param vout The array of receiving the result vertices. 
 * \param vtmp It is for temporary storage.
 * \param out It is for temporary storage.
 *
 * \return The number of vertices. 
 */
int mg3dClipF(HDC mem_dc, int type, float min_z, float max_z, int vc, const mg3dVpf *vtx[], mg3dVpf *vout[], mg3dVpf *vtmp[], int out[]);

/**
 * \fn fixed mg3dPolygonZNormal(const mg3dVp *v1, const mg3dVp *v2, const mg3dVp *v3)
 * \brief Helper function for backface culling: returns the z component of the
 *  normal vector to the polygon formed from the three vertices.
 *
 * \param v1 The first vertex of three vertices. 
 * \param v2 The second vertex of three vertices. 
 * \param v3 The third vertex of three vertices. 
 *
 * \return The z component of the normal vector. 
 */
fixed mg3dPolygonZNormal(const mg3dVp *v1, const mg3dVp *v2, const mg3dVp *v3);

/**
 * \fn float mg3dPolygonZNormalF(const mg3dVpf *v1, const mg3dVpf *v2, const mg3dVpf *v3)
 * \brief Floating point version of mg3dPolygongZNormal().
 *
 * \param v1 The first vertex of three vertices. 
 * \param v2 The second vertex of three vertices. 
 * \param v3 The third vertex of three vertices. 
 *
 * \return The z component of the normal vector. 
 */
float mg3dPolygonZNormalF(const mg3dVpf *v1, const mg3dVpf *v2, const mg3dVpf *v3);


/* Note: You are not supposed to mix MG3DZBUFFER with HDC even though it is
 * currently possible. This is just the internal representation, and it may
 * change in the future.
 *
 * A Z-buffer stores the depth of each pixel that is drawn on a viewport.
 * When a 3D object is rendered, the depth of each of its pixels is compared  
 * against the value stored into the Z-buffer: if the pixel is closer it is 
 * drawn, otherwise it is skipped. Z-buffered polygons are designed as an
 * extension of the normal MG3D_POLYTYPE_* rendering styles. Just OR the 
 * MG3D_POLYTYPE with the value MG3D_POLYTYPE_ZBUF, and the normal mg3dPolygon(),
 * mg3dPolygonF(),mg3dQuad(),etc. functions will render z-buffered polygons. 
 * Of couse, the z coordinates have to be valid regardless of rendering style.
 * 
 * A Z-buffered rendering procedure looks like a double-buffered rendering 
 * procedure. You should follow four steps: create a Z-buffer at the beginning
 * of the program and make the library use it by calling set_zbuffer(). Then,
 * for each frame, clear the Z-buffer and draw polygons with MG3D_POLYTYPE_* |
 * MG3D_POLYTYPE_ZBUF and finally destroy the Z-buffer when leaving the program. 
 * All the Z-buffered routines are much slower than their normal counterparts.
 */
typedef HDC MG3DZBUFFER;

/**
 * \fn MG3DZBUFFER mg3dCreateZbuffer(HDC mem_dc)
 * \brief Creates a Z-buffer using the size of the memory DC you are planning 
 *        to draw on. Several Z-buffers can be defined but only one can be used 
 *        at the same time, so you must call mg3dSetZbuffer() to make this Z-buffer 
 *        active.
 *
 * \param mem_dc The memory DC you are planning to draw on.
 *
 * \return Returns the pointer to the MG3DZBUFFER or NULL if there was an error.
 *         Remember to destroy the ZBUFFER once you are done with it, to avoid 
 *         having memory leaks.
 */
MG3DZBUFFER mg3dCreateZbuffer(HDC mem_dc);

/**
 * \fn void mg3dSetZbuffer(MG3DZBUFFER zbuf)
 * \brief Makes the given Z-buffer be the active one. This should have been previously 
 *        created with mg3dCreateZbuffer().
 *
 * \param zbuf The given Z-buffer.
 *
 */
void mg3dSetZbuffer(MG3DZBUFFER zbuf);

/**
 * \fn void mg3dClearZbuffer(MG3DZBUFFER zbuffer, float z)
 * \brief Writes z into the given Z-buffer(0 means far away). This function 
 *        should be used to initialize the Z-buffer before each frame. Actually,
 *        low-level routines compare depth of the current pixel with 1/z: for 
 *        example, if you want to clip polygons farther than 10, you must call 
 *        clear_zbuffer(zbuf, 0.1).
 *
 * \param zbuffer The given Z-buffer.
 * \param z Depth of the pixel.
 *
 */
void mg3dClearZbuffer(MG3DZBUFFER zbuffer, float z);

/**
 * \fn void mg3dDestroyZbuffer(MG3DZBUFFER zbuf)
 * \brief Destroys the Z-buffer when you are finished with it. Use this to avoid 
 *        memory leaks in you program. 
 *
 * \param zbuf The Z-buffer you want to destroy.
 *
 */
void mg3dDestroyZbuffer(MG3DZBUFFER zbuf);

/**
 * \fn int mg3dCreateScene(int nedge, int npoly)
 * \brief Allocates memory for a scene. 
 *
 * \param nedge The number of edges
 * \param npoly The number of polygons
 *
 * \return Zero on success, or negative number on failure.
 */
int mg3dCreateScene(int nedge, int npoly);

/**
 * \fn void mg3dClearScene(HDC mem_dc)
 * \brief Initializes a scene.
 *
 * \param mem_dc The mem_dc will be rendered eventually.
 *
 */
void mg3dClearScene(HDC mem_dc);

/**
 * \fn void mg3dDestroyScene(void)
 * \brief Deallocate memory previously allocated by mg3dCreateScene.
 *
 */
void mg3dDestroyScene(void);

/**
 * \fn int mg3dScenePolygon(int type, HDC texture, int vx, mg3dVp *vtx[])
 * \brief Put a polygon in the rendering list(fixed point operation).
 *        Nothing is really rendered at this moment. It should be called
 *        between mg3dCleanScene() and mg3dRenderScene(). It stores texture
 *        as pointers to use in mg3dRenderScene().
 *
 * \param type The rendering mode.
 * \param texture The memory dc, its width and height must be powers of two.
 * \param vx The vertex count parameter. 
 * \param vtx The array containing the appropriate number of pointers to 
 *             vertex structures. It uses fixed point mg3dVp structure.
 *
 * \return Zero on success, or a negative number if it won't be rendered
 *         for lack of a rendering routine.
 */
int mg3dScenePolygon(int type, HDC texture, int vx, mg3dVp *vtx[]);

/**
 * \fn int mg3dScenePolygonF(int type, HDC texture, int vx, mg3dVpf *vtx[])
 * \brief Put a polygon in the rendering list.(floting point operation)
 *        Nothing is really rendered at this moment. It should be called
 *        between mg3dCleanScene() and mg3dRenderScene(). It stores texture
 *        as pointers to use in mg3dRenderScene().
 *
 * \param type The rendering mode.
 * \param texture The memory dc, its width and height must be powers of two.
 * \param vx The vertex count parameter. 
 * \param vtx The array containing the appropriate number of pointers to 
 *             vertex structures. It uses floating point mg3dVp structure.
 *
 * \return Zero on success, or a negative number if it won't be rendered
 *         for lack of a rendering routine.
 */
int mg3dScenePolygonF(int type, HDC texture, int vx, mg3dVpf *vtx[]);

/**
 * \fn void mg3dRenderScene(void)
 * \brief Render all specified mg3dScenePolygon()'s on the mem_dc passed to 
 *        mg3dClearScene(). Rendering is done one scanline at a time.
 *
 */
void mg3dRenderScene(void);

    /** @} */

#endif          /* ifndef __mg3D_H__ */


