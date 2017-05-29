/**
 * \file mg3dmatrix.h
 * \author Feynman Software
 * \date 2006/01/06
 * 
 * This file includes 3D Matrix math routines. 
 *
 \verbatim
    Copyright (C) 2006 ~ 2008 Feynman Software.

    This file is part of mg3d which is one of components of MiniGUI.
\endverbatim
*/

/*
** $Id: mg3dmatrix.h,v 1.7 2006-01-14 05:07:51 xwyan Exp $
**
** mg3dmatrix.h: mg3d Matrix math routines. 
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

#ifndef __mg3D_MATRIX_H__
#define __mg3D_MATRIX_H__

#ifdef __cplusplus
   extern "C" {
#endif

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/fixedmath.h>

#include "mg3d.h"       

/**
 * \defgroup matrix_fns matrix math routines
 * @{
 */


/**
 * Structure that describes 4x4 matrix information. It can be used for
 * translation, rotation, scaling ...
 */

typedef struct _mg3dMatrix       /* transformation matrix (fixed point) */
{
   fixed v[3][3];                /* scaling and rotation */
   fixed t[3];                   /* translation */
} mg3dMatrix;


/**
 * It is floating point version of mg3dMatrix.
 */
typedef struct _mg3dMatrixf      /* transformation matrix (floating point) */
{
   float v[3][3];                /* scaling and rotation */
   float t[3];                   /* translation */
} mg3dMatrixf;


mg3dMatrix identity_matrix;
mg3dMatrixf identity_matrix_f;

/**
 * \fn void mg3dGetTranslationMatrix(mg3dMatrix *m, fixed x, fixed y, fixed z)
 * \brief Constructs a 3d translation matrix. When applied to the vector 
 *  *  (vx, vy, vx), this will produce (vx+x, vy+y, vz+z).
 *
 * \param m The translation matrix.
 * \param x The x translation value. 
 * \param y The y translation value. 
 * \param z The z translation value. 
 *
 */
void mg3dGetTranslationMatrix(mg3dMatrix *m, fixed x, fixed y, fixed z);

/**
 * \fn void mg3dGetTranslationMatrixF(mg3dMatrixf *m, float x, float y, float z)
 * \brief Floating point version of mg3dGetTranslationMatrix().
 *
 * \param m The translation matrix.
 * \param x The x translation value. 
 * \param y The y translation value. 
 * \param z The z translation value. 
 *
 */
void mg3dGetTranslationMatrixF(mg3dMatrixf *m, float x, float y, float z);

/**
 * \fn void mg3dGetScalingMatrix(mg3dMatrix *m, fixed x, fixed y, fixed z)
 * \brief Constructs a 3d scaling matrix. When applied to the vector 
 *  *  (vx, vy, vx), this will produce (vx*x, vy*y, vz*z).
 *
 * \param m The scaling matrix.
 * \param x The x scaling value. 
 * \param y The y scaling value. 
 * \param z The z scaling value. 
 *
 */
void mg3dGetScalingMatrix(mg3dMatrix *m, fixed x, fixed y, fixed z);

/**
 * \fn void mg3dGetScalingMatrixF(mg3dMatrixf *m, float x, float y, float z)
 * \brief Floating point version of mg3dGetScalingMatrix().
 *
 * \param m The scaling matrix.
 * \param x The x scaling value. 
 * \param y The y scaling value. 
 * \param z The z scaling value. 
 *
 */
void mg3dGetScalingMatrixF(mg3dMatrixf *m, float x, float y, float z);

/**
 * \fn void mg3dGetXRotMatrix(mg3dMatrix *m, fixed r)
 * \brief Constructs a 3d transformation matrix, which will rotate points around 
 *  the x axis by the specified amount (given in the mG3d fixed point, 
 *  256 degrees to a circle format).
 *
 *
 * \param m The transformation matrix.
 * \param r The rotating angle. 
 *
 */
void mg3dGetXRotMatrix(mg3dMatrix *m, fixed r);

/**
 * \fn void mg3dGetXRotMatrixF(mg3dMatrixf *m, float r)
 * \brief Floating point version of mg3dGetXRotMatrix().
 *
 * \param m The transformation matrix.
 * \param r The rotating angle. 
 * 
 * */ 
void mg3dGetXRotMatrixF(mg3dMatrixf *m, float r);

/**
 * \fn void mg3dGetYRotMatrix(mg3dMatrix *m, fixed r)
 * \brief Constructs a 3d transformation matrix, which will rotate points around 
 *  the y axis by the specified amount (given in the mG3d fixed point, 
 *  256 degrees to a circle format).
 *
 * \param m The transformation matrix.
 * \param r The rotating angle. 
 *
 */
void mg3dGetYRotMatrix(mg3dMatrix *m, fixed r);

/**
 * \fn void mg3dGetYRotMatrixF(mg3dMatrixf *m, float r)
 * \brief Floating point version of mg3dGetYRotMatrix().
 *
 * \param m The transformation matrix.
 * \param r The rotating angle. 
 *
 */
void mg3dGetYRotMatrixF(mg3dMatrixf *m, float r); 
/**
 * \fn void mg3dGetZRotMatrix(mg3dMatrix *m, fixed r)
 * \brief Constructs a 3d transformation matrix, which will rotate points around 
 *  the z axis by the specified amount (given in the mG3d fixed point, 
 *  256 degrees to a circle format).
 *
 * \param m The transformation matrix.
 * \param r The rotating angle. 
 *
 */
void mg3dGetZRotMatrix(mg3dMatrix *m, fixed r);

/**
 * \fn void mg3dGetZRotMatrixF(mg3dMatrix *m, float r)
 * \brief Floating point version of mg3dGetZRotMatrix().
 *
 * \param m The transformation matrix.
 * \param r The rotating angle. 
 *
 */
void mg3dGetZRotMatrixF(mg3dMatrix *m, float r);

/**
 * \fn void mg3dGetRotMatrix(mg3dMatrix *m, fixed x, fixed y, fixed z)
 * \brief Constructs a 3d transformation matrix, which will rotate points around
 *  all three axis by the specified amounts (given in the mG3d fixed 
 *  point, 256 degrees to a circle format).
 *
 * \param m The transformation matrix.
 * \param x The rotating angle around x axis.
 * \param y The rotating angle around y axis.
 * \param z The rotating angle around z axis.
 *
 */
void mg3dGetRotMatrix(mg3dMatrix *m, fixed x, fixed y, fixed z);

/**
 * \fn void mg3dGetRotMatrixF(mg3dMatrixf *m, float x, float y, floatz)
 * \brief Floating point version of mg3dGetRotMatrix().
 *
 * \param m The transformation matrix.
 * \param x The rotating angle around x axis.
 * \param y The rotating angle around y axis.
 * \param z The rotating angle around z axis.
 *
 */
void mg3dGetRotMatrixF(mg3dMatrixf *m, float x, float y, float z);

/**
 * \fn void mg3dGetAlignMatrix(mg3dMatrix *m, fixed xfront, fixed yfront, fixed zfront, fixed xup, fixed yup, fixed zup)
 * \brief  Aligns a matrix along an arbitrary coordinate system.
 *
 * \param m The transformation matrix.
 * \param xfront The x coordinate of an 'in front' vector.
 * \param yfront The y coordinate of an 'in front' vector.
 * \param zfront The z coordinate of an 'in front' vector.
 * \param xup The x coordinate of an 'up' direction vector.
 * \param yup The y coordinate of an 'up' direction vector.
 * \param zup The z coordinate of an 'up' direction vector.
 *
 */
void mg3dGetAlignMatrix(mg3dMatrix *m, fixed xfront, fixed yfront, fixed zfront, fixed xup, fixed yup, fixed zup);

/**
 * \fn void mg3dGetAlignMatrixF(mg3dMatrixf *m, float xfront, float yfront, float zfront, float xup, float yup, float zup)
 * \brief Floating point version of mg3dgetAlignMatrix().
 *
 * \param m The transformation matrix.
 * \param xfront The x coordinate of an 'in front' vector.
 * \param yfront The y coordinate of an 'in front' vector.
 * \param zfront The z coordinate of an 'in front' vector.
 * \param xup The x coordinate of an 'up' direction vector.
 * \param yup The y coordinate of an 'up' direction vector.
 * \param zup The z coordinate of an 'up' direction vector.
 *
 */
void mg3dGetAlignMatrixF(mg3dMatrixf *m, float xfront, float yfront, float zfront, float xup, float yup, float zup);


/**
 * \fn void mg3dGetVectorRotMatrix(mg3dMatrix *m, fixed x, fixed y, fixed z, fixed a)
 * \brief  Constructs a 3d transformation matrix, which will rotate points around
 *  *  the specified x,y,z vector by the specified angle (given in the mG3d 
 *   *  fixed point, 256 degrees to a circle format), in a clockwise direction.
 *
 * \param m The transformation matrix.
 * \param x The x coordinate of rotating vector.
 * \param y The y coordinate of rotating vector.
 * \param z The z coordinate of rotating vector.
 * \param a The rotating angle. 
 *
 */
void mg3dGetVectorRotMatrix(mg3dMatrix *m, fixed x, fixed y, fixed z, fixed a);

/**
 * \fn void mg3dGetVectorRotMatrixF(mg3dMatrixf *m, float x, float y, float z, float a)
 * \brief Floating point version of mg3dGetVectorRotMatrix().
 *
 * \param m The transformation matrix.
 * \param x The x coordinate of rotating vector.
 * \param y The y coordinate of rotating vector.
 * \param z The z coordinate of rotating vector.
 * \param a The rotating angle. 
 *
 */
void mg3dGetVectorRotMatrixF(mg3dMatrixf *m, float x, float y, float z, float a);

/**
 * \fn void mg3dGetTransformMatrix(mg3dMatrix *m, fixed scale, fixed xrot, fixed yrot, fixed zrot, fixed x, fixed y, fixed z)
 * \brief Constructs a 3d transformation matrix, which will rotate points around
 *  all three axis by the specified amounts (given in the mG3d fixed 
 *  point, 256 degrees to a circle format), scale the result by the
 *  specified amount (itofix(1) for no change of scale), and then translate
 *  to the requested x, y, z position.
 *
 * \param m The transformation matrix.
 * \param xrot The rotating angle around x axis.
 * \param yrot The rotating angle around y axis.
 * \param zrot The rotating angle around z axis.
 * \param scale The scaling value. 
 * \param x The x translation value. 
 * \param y The y translation value. 
 * \param z The z translation value. 
 *
 */
void mg3dGetTransformMatrix(mg3dMatrix *m, fixed scale, fixed xrot, fixed yrot, fixed zrot, fixed x, fixed y, fixed z);

/**
 * \fn void mg3dGetTransformMatrixF(mg3dMatrixf *m, float scale, float xrot, float yrot, float zrot, float x, float y, float z)
 * \brief Floating point version of mg3dGetTransformMatrix().
 *
 * \param m The transformation matrix.
 * \param xrot The rotating angle around x axis.
 * \param yrot The rotating angle around y axis.
 * \param zrot The rotating angle around z axis.
 * \param scale The scaling value. 
 * \param x The x translation value. 
 * \param y The y translation value. 
 * \param z The z translation value. 
 *
 */
void mg3dGetTransformMatrixF(mg3dMatrixf *m, float scale, float xrot, float yrot, float zrot, float x, float y, float z);

/**
 * \fn void mg3dGetCameraMatrix(mg3dMatrix *m, fixed x, fixed y, fixed z, fixed xfront, fixed yfront, fixed zfront, fixed xup, fixed yup, fixed zup, fixed fov, fixed aspect)
 * \brief  Constructs a camera matrix for translating world-space objects into
 *  a normalised view space, ready for the perspective projection. 
 *
 * \param m The transformation matrix.
 * \param x The x coordinate of the camera position. 
 * \param y The y coordinate of the camera position. 
 * \param z The z coordinate of the camera position. 
 * \param xfront The x coordinate of an 'in front' vector specifying which way the camera is facing.
 * \param yfront The y coordinate of an 'in front' vector specifying which way the camera is facing.
 * \param zfront The z coordinate of an 'in front' vector specifying which way the camera is facing.
 * \param xup The x coordinate of an 'up' direction vector.
 * \param yup The y coordinate of an 'up' direction vector.
 * \param zup The z coordinate of an 'up' direction vector.
 * \param fov The filed of view in fixed point, 256 degrees to the circle format.For typical projections,
 *            a field of view in the region 32-48 will work well.
 * \param aspect The ratio which is used to scale the Y dimensions of the image relative to the X axis.
 *
 */
void mg3dGetCameraMatrix(mg3dMatrix *m, fixed x, fixed y, fixed z, fixed xfront, fixed yfront, fixed zfront, fixed xup, fixed yup, fixed zup, fixed fov, fixed aspect);

/**
 * \fn void mg3dGetCameraMatrixF(mg3dMatrixf *m, float x, float y, float z, float xfront, float yfront, float zfront, float xup, float yup, float zup, float fov, float aspect)
 * \brief Floating point version of mg3dGetCameraMatrix().
 * \brief 
 *
 * \param m The transformation matrix.
 * \param x The x coordinate of the camera position. 
 * \param y The y coordinate of the camera position. 
 * \param z The z coordinate of the camera position. 
 * \param xfront The x coordinate of an 'in front' vector specifying which way the camera is facing.
 * \param yfront The y coordinate of an 'in front' vector specifying which way the camera is facing.
 * \param zfront The z coordinate of an 'in front' vector specifying which way the camera is facing.
 * \param xup The x coordinate of an 'up' direction vector.
 * \param yup The y coordinate of an 'up' direction vector.
 * \param zup The z coordinate of an 'up' direction vector.
 * \param fov The filed of view in fixed point, 256 degrees to the circle format.For typical projections,
 *            a field of view in the region 32-48 will work well.
 * \param aspect The ratio which is used to scale the Y dimensions of the image relative to the X axis.
 *
 */
void mg3dGetCameraMatrixF(mg3dMatrixf *m, float x, float y, float z, float xfront, float yfront, float zfront, float xup, float yup, float zup, float fov, float aspect);

/**
 * \fn void mg3dQTranslateMatrix(mg3dMatrix *m, fixed x, fixed y, fixed z)
 * \brief Adds a position offset to an existing matrix.
 *
 * \param m The transformation matrix.
 * \param x The x component offset value. 
 * \param y The y component offset value. 
 * \param z The z component offset value. 
 *
 */

void mg3dQTranslateMatrix(mg3dMatrix *m, fixed x, fixed y, fixed z);

/**
 * \fn void mg3dQTranslateMatrixF(mg3dMatrixf *m, float x, float y, float z)
 * \brief Floating point version of mg3dqTranslateMatrix().
 *
 * \param m The transformation matrix.
 * \param x The x component offset value. 
 * \param y The y component offset value. 
 * \param z The z component offset value. 
 *
 */
void mg3dQTranslateMatrixF(mg3dMatrixf *m, float x, float y, float z);

/**
 * \fn void mg3dQscaleMatrix(mg3dMatrix *m, fixed scale)
 * \brief  Adds a scaling factor to an existing matrix.
 *
 * \param m The transformation matrix.
 * \param scale The scaling value.
 *
 */
void mg3dQscaleMatrix(mg3dMatrix *m, fixed scale);

/**
 * \fn void mg3dQscaleMatrixF(mg3dMatrixf *m, float scale)
 * \brief Floating point version of mg3dQscaleMatrix().
 *
 * \param m The transformation matrix.
 * \param scale The scaling value.
 *
 */
void mg3dQscaleMatrixF(mg3dMatrixf *m, float scale);

/**
 * \fn void mg3dMatrixMul(const mg3dMatrix *m1, const mg3dMatrix *m2, mg3dMatrix *out)
 * \brief Multiplies two matrices, storing the result in out (this must be
 *  different from the two input matrices). The resulting matrix will
 *  have the same effect as the combination of m1 and m2, ie. when
 *  applied to a vector v, (v * out) = ((v * m1) * m2). Any number of
 *  transformations can be concatenated in this way. 
 *
 *
 * \param m1 The first transformation matrix.
 * \param m2 The second transformation matrix.
 * \param out The result matrix.
 *
 */
void mg3dMatrixMul(const mg3dMatrix *m1, const mg3dMatrix *m2, mg3dMatrix *out);

/**
 * \fn void mg3dMatrixMulF(const mg3dMatrixf *m1, const mg3dMatrixf *m2, mg3dMatrixf *out)
 * \brief Floating point version of mg3dMatrixMul().
 * \brief 
 *
 * \param m1 The first transformation matrix.
 * \param m2 The second transformation matrix.
 * \param out The result matrix.
 *
 */
void mg3dMatrixMulF(const mg3dMatrixf *m1, const mg3dMatrixf *m2, mg3dMatrixf *out);

/**
 * \fn void mg3dApplyMatrix(mg3dMatrix *m, fixed x, fixed y, fixed z, fixed *xout, fixed *yout, fixed *zout)
 * \brief Multiplies the point (x, y, z) by the transformation matrix m, storing the result in (*xout, *yout, *zout).
 *
 * \param m The transformation matrix.
 * \param x The x coordinate of 3d point(x, y, z).
 * \param y The y coordinate of 3d point(x, y, z).
 * \param z The z coordinate of 3d point(x, y, z).
 * \param xout The xout coordinate of the result point.
 * \param yout The yout coordinate of the result point.
 * \param zout The zout coordinate of the result point.
 *
 */
void mg3dApplyMatrix(mg3dMatrix *m, fixed x, fixed y, fixed z, fixed *xout, fixed *yout, fixed *zout);

/**
 * \fn void mg3dApplyMatrixF(const mg3dMatrixf *m, float x, float y, float z, float *xout, float *yout, float *zout)
 * \brief Floating point version of mg3dApplyMatrix().
 *
 * \param m The transformation matrix.
 * \param x The x coordinate of 3d point(x, y, z).
 * \param y The y coordinate of 3d point(x, y, z).
 * \param z The z coordinate of 3d point(x, y, z).
 * \param xout The xout coordinate of the result point.
 * \param yout The yout coordinate of the result point.
 * \param zout The zout coordinate of the result point.
 *
 */
void mg3dApplyMatrixF(const mg3dMatrixf *m, float x, float y, float z, float *xout, float *yout, float *zout);

#ifdef __cplusplus
   }
#endif

    /** @} */
#endif          /* ifndef __MG3D_MATRIX_H__ */


