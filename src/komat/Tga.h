/******************************************************************************
 * filename: tga.h                                                            *
 *                                                                            *
 * author:   Christopher Oat                                                  *
 *           ATI Research, Inc.                                               *
 *           3D Application Research Group                                    *
 *           email: coat@ati.com                                              *
 *                                                                            *
 * description: routines to load .tga image files.                            *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 $Header::                                                                    $
 ******************************************************************************
 *   (C) 2001 ATI Research, Inc.  All rights reserved.                        *
 ******************************************************************************/

#ifndef _TGA_H
#define _TGA_H

unsigned int tgaLoad(APAK_HANDLE * pHandle, char *filename, int mipmap,
  GLenum format);

#endif
