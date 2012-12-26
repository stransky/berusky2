/******************************************************************************
 * filename: tga.cpp                                                          *
 *                                                                            *
 * author:   Christopher Oat                                                  *
 *           ATI Research, Inc.                                               *
 *           3D Application Research Group                                    *
 *           email: coat@ati.com                                              *
 *                                                                            *
 * description: Loads a 24-bit TGA image                                      *
 *                                                                            *
 *                                                                            *
 ******************************************************************************
 $Header::                                                                    $
 ******************************************************************************
 *   (C) 2001 ATI Research, Inc.  All rights reserved.                        *
 ******************************************************************************/

#include "3d_all.h"
#include "Tga.h"

#pragma pack(push,x1)
#pragma pack(1)

typedef struct
{
  unsigned char idSize, mapType, imageType;
  unsigned short paletteStart, paletteSize;
  unsigned char paletteEntryDepth;
  unsigned short x, y, width, height;
  unsigned char colorDepth, descriptor;

} TGAHeader;

typedef struct
{
  unsigned int id;
  unsigned int width;
  unsigned int height;
  float scaleS;
  float scaleT;
  unsigned char channels;
  unsigned char depth;
  void *buffer;
} ImageData;

#pragma pack(pop,x1)

#define TGA_CACHE_SIZE 1024
static ImageData tgaImageCache[TGA_CACHE_SIZE] = { 0, 0, 0, 0.0f, 0.0f, 0, 0, NULL};

static unsigned short tgaMakePowerOfTwo(unsigned short number);
static unsigned int tgaFindFreeCacheIndex(void);

unsigned int tgaLoad(APAK_HANDLE * pHandle, char *filename, int mipmap,
  GLenum format)
{
  KFILE *fp;
  TGAHeader header;
  int dudv = FALSE;
  unsigned int currentIndex = tgaFindFreeCacheIndex();
  int x, y;

  if (format == GL_DUDV_ATI) {
    dudv = TRUE;
  }

  if ((fp = kopen(pHandle, filename, "rb")) == NULL) {
    return (0);
  }

  if (kread(&header, sizeof(TGAHeader), 1, fp) == 0) {
    return (0);
  }

  tgaImageCache[currentIndex].depth = header.colorDepth;
  tgaImageCache[currentIndex].channels = header.colorDepth / 8;

  tgaImageCache[currentIndex].height = header.height;
  tgaImageCache[currentIndex].height = header.width;

  if (tgaImageCache[currentIndex].channels == 0) {
    tgaImageCache[currentIndex].id = 0;
    return (0);
  }

  if (tgaImageCache[currentIndex].channels == 1) {
    tgaImageCache[currentIndex].buffer =
      (GLubyte *) mmalloc(sizeof(GLubyte) * (header.width) * (header.height));
    if (tgaImageCache[currentIndex].buffer == NULL) {
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    if (kread(tgaImageCache[currentIndex].buffer, sizeof(GLubyte),
        (header.width) * (header.height), fp) == 0) {
      free(tgaImageCache[currentIndex].buffer);
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    kclose(fp);

    glGenTextures(1, &tgaImageCache[currentIndex].id);
    if (tgaImageCache[currentIndex].id == 0) {
      free(tgaImageCache[currentIndex].buffer);
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    tgaImageCache[currentIndex].scaleS =
      (float) (((double) header.width) /
      ((double) tgaMakePowerOfTwo(header.width)));
    tgaImageCache[currentIndex].scaleT =
      (float) (((double) header.height) /
      ((double) tgaMakePowerOfTwo(header.height)));

    glBindTexture(GL_TEXTURE_2D, tgaImageCache[currentIndex].id);
    glEnable(GL_TEXTURE_2D);

    if (mipmap) {
      glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    }

    // make an empty "padded" texture then copy the data into in (this avoids the power of 2 limitation)
    glTexImage2D(GL_TEXTURE_2D, 0, 1, tgaMakePowerOfTwo(header.width),
      tgaMakePowerOfTwo(header.height), 0, format, GL_UNSIGNED_BYTE, NULL);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, header.width, header.height,
      format, GL_UNSIGNED_BYTE, tgaImageCache[currentIndex].buffer);

    return (tgaImageCache[currentIndex].id);
  }

  if (tgaImageCache[currentIndex].channels == 2) {
    unsigned char *buffer16 = NULL;

    buffer16 =
      (unsigned char *) malloc(sizeof(unsigned char) * (header.width) *
      (header.height) * 2);
    if (buffer16 == NULL) {
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    if (kread(buffer16, sizeof(unsigned char),
        (header.width) * (header.height) * 2, fp) == 0) {
      free(buffer16);
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    tgaImageCache[currentIndex].buffer =
      (GLubyte *) malloc(sizeof(GLubyte) * (header.width) * (header.height) *
      2);
    if (tgaImageCache[currentIndex].buffer == NULL) {
      free(buffer16);
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    for (y = 0; y < header.height; y++) {
      for (x = 0; x < header.width; x++) {
        int i = (y * header.width * 2) + (x * 2);

        ((GLubyte *) tgaImageCache[currentIndex].buffer)[i + 1] =
          buffer16[i + 0];
        ((GLubyte *) tgaImageCache[currentIndex].buffer)[i + 0] =
          buffer16[i + 1];

      }
    }

    kclose(fp);
    free(buffer16);

    glGenTextures(1, &tgaImageCache[currentIndex].id);
    if (tgaImageCache[currentIndex].id == 0) {
      free(tgaImageCache[currentIndex].buffer);
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    tgaImageCache[currentIndex].scaleS =
      (float) (((double) header.width) /
      ((double) tgaMakePowerOfTwo(header.width)));
    tgaImageCache[currentIndex].scaleT =
      (float) (((double) header.height) /
      ((double) tgaMakePowerOfTwo(header.height)));

    glBindTexture(GL_TEXTURE_2D, tgaImageCache[currentIndex].id);
    glEnable(GL_TEXTURE_2D);

    if (mipmap) {
      glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    }

    // make an empty "padded" texture then copy the data into in (this avoids the power of 2 limitation)
    glTexImage2D(GL_TEXTURE_2D, 0, 2, tgaMakePowerOfTwo(header.width),
      tgaMakePowerOfTwo(header.height), 0, format, GL_UNSIGNED_BYTE, NULL);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, header.width, header.height,
      format, GL_UNSIGNED_BYTE, tgaImageCache[currentIndex].buffer);

    return (tgaImageCache[currentIndex].id);
  }

  if (tgaImageCache[currentIndex].channels == 3) {
    unsigned char *buffer24 = NULL;

    buffer24 =
      (unsigned char *) malloc(sizeof(unsigned char) * (header.width) *
      (header.height) * 3);
    if (buffer24 == NULL) {
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    if (kread(buffer24, sizeof(unsigned char),
        (header.width) * (header.height) * 3, fp) == 0) {
      free(buffer24);
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    if (!(dudv)) {
      tgaImageCache[currentIndex].buffer =
        (GLubyte *) malloc(sizeof(GLubyte) * (header.width) *
        (header.height) * 3);
    }
    else {
      tgaImageCache[currentIndex].buffer =
        (GLfloat *) malloc(sizeof(GLfloat) * (header.width) *
        (header.height) * 2);
    }

    if (tgaImageCache[currentIndex].buffer == NULL) {
      free(buffer24);
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    for (y = 0; y < header.height; y++) {
      for (x = 0; x < header.width; x++) {
        if (!(dudv)) {
          int i = (y * header.width * 3) + (x * 3);

          ((GLubyte *) tgaImageCache[currentIndex].buffer)[i + 2] =
            buffer24[i + 0];
          ((GLubyte *) tgaImageCache[currentIndex].buffer)[i + 1] =
            buffer24[i + 1];
          ((GLubyte *) tgaImageCache[currentIndex].buffer)[i + 0] =
            buffer24[i + 2];
        }
        else {
          int i = (y * header.width * 3) + (x * 3);
          int j = (y * header.width * 2) + (x * 2);

          ((GLfloat *) tgaImageCache[currentIndex].buffer)[j + 1] =
            (((((float) buffer24[i + 1]) * 2.0f) / 255.0f)) - 1.0f;
          ((GLfloat *) tgaImageCache[currentIndex].buffer)[j + 0] =
            (((((float) buffer24[i + 2]) * 2.0f) / 255.0f)) - 1.0f;
        }
      }
    }

    kclose(fp);
    free(buffer24);

    glGenTextures(1, &tgaImageCache[currentIndex].id);
    if (tgaImageCache[currentIndex].id == 0) {
      free(tgaImageCache[currentIndex].buffer);
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    tgaImageCache[currentIndex].scaleS =
      (float) (((double) header.width) /
      ((double) tgaMakePowerOfTwo(header.width)));
    tgaImageCache[currentIndex].scaleT =
      (float) (((double) header.height) /
      ((double) tgaMakePowerOfTwo(header.height)));

    glBindTexture(GL_TEXTURE_2D, tgaImageCache[currentIndex].id);
    glEnable(GL_TEXTURE_2D);

    if (mipmap) {
      glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    }

    // make an empty "padded" texture then copy the data into in (this avoids the power of 2 limitation)
    if (!(dudv)) {
      glTexImage2D(GL_TEXTURE_2D, 0, 3, tgaMakePowerOfTwo(header.width),
        tgaMakePowerOfTwo(header.height), 0, format, GL_UNSIGNED_BYTE, NULL);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, header.width, header.height,
        format, GL_UNSIGNED_BYTE, tgaImageCache[currentIndex].buffer);
    }
    else {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DU8DV8_ATI,
        tgaMakePowerOfTwo(header.width), tgaMakePowerOfTwo(header.height), 0,
        format, GL_FLOAT, NULL);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, header.width, header.height,
        format, GL_FLOAT, tgaImageCache[currentIndex].buffer);
    }

    return (tgaImageCache[currentIndex].id);
  }

  if (tgaImageCache[currentIndex].channels == 4) {
    unsigned char *buffer32 = NULL;

    buffer32 =
      (unsigned char *) malloc(sizeof(unsigned char) * (header.width) *
      (header.height) * 4);
    if (buffer32 == NULL) {
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    if (kread(buffer32, sizeof(unsigned char),
        (header.width) * (header.height) * 4, fp) == 0) {
      free(buffer32);
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    if (!(dudv)) {
      tgaImageCache[currentIndex].buffer =
        (GLubyte *) malloc(sizeof(GLubyte) * (header.width) *
        (header.height) * 4);
    }
    else {
      tgaImageCache[currentIndex].buffer =
        (GLfloat *) malloc(sizeof(GLfloat) * (header.width) *
        (header.height) * 2);
    }

    if (tgaImageCache[currentIndex].buffer == NULL) {
      free(buffer32);
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    for (y = 0; y < header.height; y++) {
      for (x = 0; x < header.width; x++) {
        if (!(dudv)) {
          int i = (y * header.width * 4) + (x * 4);

          ((GLubyte *) tgaImageCache[currentIndex].buffer)[i + 2] =
            buffer32[i + 0];
          ((GLubyte *) tgaImageCache[currentIndex].buffer)[i + 1] =
            buffer32[i + 1];
          ((GLubyte *) tgaImageCache[currentIndex].buffer)[i + 0] =
            buffer32[i + 2];
          ((GLubyte *) tgaImageCache[currentIndex].buffer)[i + 3] =
            buffer32[i + 3];
        }
        else {
          int i = (y * header.width * 4) + (x * 4);
          int j = (y * header.width * 2) + (x * 2);

          ((GLfloat *) tgaImageCache[currentIndex].buffer)[j + 1] =
            (((((float) buffer32[i + 0]) * 2.0f) / 255.0f)) - 1.0f;
          ((GLfloat *) tgaImageCache[currentIndex].buffer)[j + 0] =
            (((((float) buffer32[i + 1]) * 2.0f) / 255.0f)) - 1.0f;
        }

      }
    }

    kclose(fp);
    free(buffer32);

    glGenTextures(1, &tgaImageCache[currentIndex].id);
    if (tgaImageCache[currentIndex].id == 0) {
      free(tgaImageCache[currentIndex].buffer);
      tgaImageCache[currentIndex].id = 0;
      return (0);
    }

    tgaImageCache[currentIndex].scaleS =
      (float) (((double) header.width) /
      ((double) tgaMakePowerOfTwo(header.width)));
    tgaImageCache[currentIndex].scaleT =
      (float) (((double) header.height) /
      ((double) tgaMakePowerOfTwo(header.height)));

    glBindTexture(GL_TEXTURE_2D, tgaImageCache[currentIndex].id);
    glEnable(GL_TEXTURE_2D);

    if (mipmap) {
      glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    }

    // make an empty "padded" texture then copy the data into in (this avoids the power of 2 limitation)
    if (!(dudv)) {
      glTexImage2D(GL_TEXTURE_2D, 0, 4, tgaMakePowerOfTwo(header.width),
        tgaMakePowerOfTwo(header.height), 0, format, GL_UNSIGNED_BYTE, NULL);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, header.width, header.height,
        format, GL_UNSIGNED_BYTE, tgaImageCache[currentIndex].buffer);
    }
    else {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DUDV_ATI,
        tgaMakePowerOfTwo(header.width), tgaMakePowerOfTwo(header.height), 0,
        format, GL_FLOAT, NULL);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, header.width, header.height,
        format, GL_FLOAT, tgaImageCache[currentIndex].buffer);

    }

    return (tgaImageCache[currentIndex].id);
  }

  return (0);

}


static unsigned short tgaMakePowerOfTwo(unsigned short number)
{
  unsigned short power2 = 1;

  while (number > power2) {
    power2 = power2 << 1;
  }

  return (power2);
}

static unsigned int tgaFindFreeCacheIndex(void)
{
  int n;

  for (n = 0; n < TGA_CACHE_SIZE; ++n) {
    if (tgaImageCache[n].id == 0)
      return (n);
  }

  return (0);
}
