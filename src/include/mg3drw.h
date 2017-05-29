/*     
** $Id: mg3drw.h,v 1.2 2006-01-14 05:49:25 xwyan Exp $
**
** Port to MiniGUI by Feynman from allegro.
**
** Copyright (C) 2006 Feynman Software.
**
** Configuration defines.

** Allegro is a gift-ware.
*/

#ifndef __mg3D_RW_H__
#define __mg3D_RW_H__
#include "mg3dconfig.h"
/* the rest of this file fills in some default definitions of language
 * features and helper functions, which are conditionalised so they will
 * only be included if none of the above headers defined custom versions.
 */

#define MINIGUI_LITTLE_ENDIAN 1
/* endian-independent 3-byte accessor macros */
#ifdef MINIGUI_LITTLE_ENDIAN

   #define READ3BYTES(p)  (((int) *(p))               \
                           | ((int) *((p) + 1) << 8)  \
                           | ((int) *((p) + 2) << 16))

   #define WRITE3BYTES(p,c)  ((*(p) = (c)),             \
                              (*((p) + 1) = (c) >> 8),  \
                              (*((p) + 2) = (c) >> 16))

#elif defined MINIGUI_BIG_ENDIAN

   #define READ3BYTES(p)  (((int) *(p) << 16)         \
                           | ((int) *((p) + 1) << 8)  \
                           | ((int) *((p) + 2)))

   #define WRITE3BYTES(p,c)  ((*(p) = (c) >> 16),       \
                              (*((p) + 1) = (c) >> 8),  \
                              (*((p) + 2) = (c)))

#else
   #error endianess not defined
#endif


/* generic versions of the video memory access helpers */
#ifndef bmp_write8
   #define bmp_write8(addr, c)         (*((unsigned char  *)(addr)) = (c))
   #define bmp_write15(addr, c)        (*((unsigned short *)(addr)) = (c))
   #define bmp_write16(addr, c)        (*((unsigned short *)(addr)) = (c))
   #define bmp_write32(addr, c)        (*((unsigned long  *)(addr)) = (c))

   #define bmp_read8(addr)             (*((unsigned char  *)(addr)))
   #define bmp_read15(addr)            (*((unsigned short *)(addr)))
   #define bmp_read16(addr)            (*((unsigned short *)(addr)))
   #define bmp_read32(addr)            (*((unsigned long  *)(addr)))
#endif
#endif  /* ifndef __mg3d_RW_H__*/
