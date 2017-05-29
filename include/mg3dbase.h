/**
 * \file mg3dbase.h
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
** $Id: mg3dbase.h,v 1.4 2006-01-14 05:07:51 xwyan Exp $
**
** mgbase.h: base header, defines basic stuff needed by pretty much everything else.
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

#ifndef __mg3D_BASE_H__
#define __mg3D_BASE_H__

#ifdef __cplusplus
   extern "C" {
#endif

/*******************************************/
/************ Some global stuff ************/
/*******************************************/

#ifndef TRUE 
   #define TRUE         -1
   #define FALSE        0
#endif

#undef MIN
#undef MAX
#undef MID
     
#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#define MID(x,y,z)   MAX((x), MIN((y), (z)))
     
#undef ABS
#define ABS(x)       (((x) >= 0) ? (x) : (-(x)))
     
#undef SGN
#define SGN(x)       (((x) >= 0) ? 1 : -1)

#ifdef __cplusplus
   }
#endif

#endif          /*__mg3D_BASE_H__ */


