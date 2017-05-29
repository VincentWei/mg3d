/**
 * \file mg3dmaths.h
 * \author Feynman Software
 * \date 2006/01/06
 * 
 * This file includes 3D oriented math routines.
 *
 \verbatim
    Copyright (C) 2006 ~ 2008 Feynman Software.

    This file is part of mg3d which is one of components of MiniGUI.
\endverbatim
*/

/*
** $Id: mg3dmaths.h,v 1.10 2006-01-14 05:07:51 xwyan Exp $
**
** mg3dmaths.h: mg3d oriented math routines.
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

#ifndef __MG3D_MATHS_H__
#define __MG3D_MATHS_H__

#ifdef __cplusplus
extern "C" {
#endif
   
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/fixedmath.h>

/**
 * \defgroup maths_fns oriented math routines
 * @{
 */
extern fixed _persp_xscale;
extern fixed _persp_yscale;
extern fixed _persp_xoffset;
extern fixed _persp_yoffset;

extern float _persp_xscale_f;
extern float _persp_yscale_f;
extern float _persp_xoffset_f;
extern float _persp_yoffset_f;


/**
 * \fn fixed mg3dVectorLength(fixed x, fixed y, fixed z)
 * \brief  Computes the length of a vector, using the son of the squaw...
 *
 * \param x The x coordinate of vector. 
 * \param y The y coordinate of vector. 
 * \param z The z coordinate of vector. 
 *
 * \return The length of the vector. 
 */
fixed mg3dVectorLength(fixed x, fixed y, fixed z);

/**
 * \fn float mg3dVectorLengthF(float x, float y, float z)
 * \brief Floating point version of mg3dVectorLength().
 *
 * \param x The x coordinate of vector. 
 * \param y The y coordinate of vector. 
 * \param z The z coordinate of vector. 
 *
 * \return The length of the vector. 
 */
float mg3dVectorLengthF(float x, float y, float z);

/**
 * \fn void mg3dNormalVector(fixed *x, fixed *y, fixed *z)
 * \brief Converts the specified vector to a unit vector, which has the same
 *  orientation but a length of one.
 *
 * \param x The x coordinate of vector. 
 * \param y The y coordinate of vector. 
 * \param z The z coordinate of vector. 
 *
 */
void mg3dNormalVector(fixed *x, fixed *y, fixed *z);

/**
 * \fn void mg3dNormalVectorF(float *x, float *y, float *z)
 * \brief Floating point version of mg3dNormalVector().
 *
 * \param x The x coordinate of vector. 
 * \param y The y coordinate of vector. 
 * \param z The z coordinate of vector. 
 *
 */
void mg3dNormalVectorF(float *x, float *y, float *z);

/**
 * \fn void mg3dCrossProduct(fixed x1, fixed y1, fixed z1, fixed x2, fixed y2, fixed z2, fixed *xout, fixed *yout, fixed *zout)
 * \brief  Calculates the cross product of two vectors.
 *
 * \param x1 The x coordinate of first vector.
 * \param y1 The y coordinate of first vector.
 * \param z1 The z coordinate of first vector.
 * \param x2 The x coordinate of second vector.
 * \param y2 The y coordinate of second vector.
 * \param z2 The z coordinate of second vector.
 * \param xout The x coordinate of cross vector.
 * \param yout The y coordinate of cross vector.
 * \param zout The z coordinate of cross vector.
 *
 */
void mg3dCrossProduct(fixed x1, fixed y1, fixed z1, fixed x2, fixed y2, fixed z2, fixed *xout, fixed *yout, fixed *zout);

/**
 * \fn void mg3dCrossProductF(float x1, float y1, float z1, float x2, float y2, float z2, float *xout, float *yout, float *zout)
 * \brief Floating point version of mg3dCrossProduct().
 *
 * \param x1 The x coordinate of first vector.
 * \param y1 The y coordinate of first vector.
 * \param z1 The z coordinate of first vector.
 * \param x2 The x coordinate of second vector.
 * \param y2 The y coordinate of second vector.
 * \param z2 The z coordinate of second vector.
 * \param xout The x coordinate of cross vector.
 * \param yout The y coordinate of cross vector.
 * \param zout The z coordinate of cross vector.
 *
 */
void mg3dCrossProductF(float x1, float y1, float z1, float x2, float y2, float z2, float *xout, float *yout, float *zout);

/**
 * \fn fixed mg3dDotProduct(fixed x1, fixed y1, fixed z1, fixed x2, fixed y2, fixed z2)
 * \brief  Calculates the dot product of two vectors.
 *
 * \param x1 The x coordinate of first vector.
 * \param y1 The y coordinate of first vector.
 * \param z1 The z coordinate of first vector.
 * \param x2 The x coordinate of second vector.
 * \param y2 The y coordinate of second vector.
 * \param z2 The z coordinate of second vector.
 *
 * \return The result value.
 */
fixed mg3dDotProduct(fixed x1, fixed y1, fixed z1, fixed x2, fixed y2, fixed z2);
                                                                                         
/**
 * \fn float mg3dDotProductF(float x1, float y1, float z1, float x2, float y2, float z2)
 * \brief Floating point version of mg3dDotProduct().
 *
 * \param x1 The x coordinate of first vector.
 * \param y1 The y coordinate of first vector.
 * \param z1 The z coordinate of first vector.
 * \param x2 The x coordinate of second vector.
 * \param y2 The y coordinate of second vector.
 * \param z2 The z coordinate of second vector.
 *
 * \return The result value.
 */
float mg3dDotProductF(float x1, float y1, float z1, float x2, float y2, float z2); 

/**
 * \fn void mg3dSetProjectionViewport(int x, int y, int w, int h)
 * \brief Sets the viewport used to scale the output of the mg3dPerspProject() 
 *  function.
 *
 * \param x The x coordinate of viewport starting point in screen. 
 * \param y The y coordinate of viewport starting point in screen. 
 * \param w The width of viewport.
 * \param h The height of viewport. 
 *
 */
void mg3dSetProjectionViewport(int x, int y, int w, int h);
                                                                                         
/**
 * \fn void mg3dPerspProject(fixed x, fixed y, fixed z, fixed *xout, fixed *yout)
 * \brief  Projects the 3d point (x, y, z) into 2d screen space.
 *
 * \param x The x coordinate of 3d point(x, y, z).
 * \param y The y coordinate of 3d point(x, y, z).
 * \param z The z coordinate of 3d point(x, y, z).
 * \param xout The x coordinate of 2d point in screen space.
 * \param yout The y coordinate of 2d point in screen space.
 *
 */
void mg3dPerspProject(fixed x, fixed y, fixed z, fixed *xout, fixed *yout);

/**
 * \fn void mg3dPerspProjectF (float x, float y, float z, float *xout, float *yout)
 * \brief Floating point version of mg3dPerspProject().
 *
 * \param x The x coordinate of 3d point(x, y, z).
 * \param y The y coordinate of 3d point(x, y, z).
 * \param z The z coordinate of 3d point(x, y, z).
 * \param xout The x coordinate of 2d point in screen space.
 * \param yout The y coordinate of 2d point in screen space.
 *
 */
void mg3dPerspProjectF (float x, float y, float z, float *xout, float *yout);
   
#ifdef __cplusplus
}
#endif

    /** @} */
#endif          /* _MG3D_MATHS_H__ */


