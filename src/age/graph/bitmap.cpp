/*
 *        .þÛÛþ þ    þ þÛÛþ.     þ    þ þÛÛÛþ.  þÛÛÛþ .þÛÛþ. þ    þ
 *       .þ   Û Ûþ.  Û Û   þ.    Û    Û Û    þ  Û.    Û.   Û Ûþ.  Û
 *       Û    Û Û Û  Û Û    Û    Û   þ. Û.   Û  Û     Û    Û Û Û  Û
 *     .þþÛÛÛÛþ Û  Û Û þÛÛÛÛþþ.  þþÛÛ.  þþÛÛþ.  þÛ    Û    Û Û  Û Û
 *    .Û      Û Û  .þÛ Û      Û. Û   Û  Û    Û  Û.    þ.   Û Û  .þÛ
 *    þ.      þ þ    þ þ      .þ þ   .þ þ    .þ þÛÛÛþ .þÛÛþ. þ    þ
 *
 * AGE (C) AnakreoN
 * Martin Stransky <stransky@anakreon.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* A bitmaps routines
*/

#include "bitmap.h"

/***************************************************************** 
  RGBA Bitmaps
 *****************************************************************
*/
/* Filtrace bmp pomoci floyd-stemberga -> prevod na 16K
   do 8,8,8 -> 5,5,5
*/
/*
#define MASKA 0x7
#define PRAH  (MASKA>>1)

void bmp_pix_to_16b(byte *p_baze, int *p_er, int *p_eg, int *p_eb)
{ 
 *p_er = (p_baze[0]&MASKA)-PRAH;
 *p_eg = (p_baze[1]&MASKA)-PRAH;
 *p_eb = (p_baze[2]&MASKA)-PRAH;
  
  p_baze[0]&=~MASKA;
  p_baze[1]&=~MASKA;
  p_baze[2]&=~MASKA;
}

void add_posun_rgba(byte *p_baze, int r, int g, int b, int posun)
{
  r = (r < 0) ? p_baze[0]-((-r)>>posun) : p_baze[0]+(r>>posun);
  g = (g < 0) ? p_baze[1]-((-g)>>posun) : p_baze[1]+(g>>posun);
  b = (b < 0) ? p_baze[2]-((-b)>>posun) : p_baze[2]+(b>>posun);
  
  if(r > MAX_BYTE) r = MAX_BYTE;
  if(g > MAX_BYTE) g = MAX_BYTE;
  if(b > MAX_BYTE) b = MAX_BYTE;

  if(r < 0) r = 0;
  if(g < 0) g = 0;
  if(b < 0) b = 0;

  p_baze[0] = r;
  p_baze[1] = g;
  p_baze[2] = b;
}
*/

/*
255, 0, 255
0, 0, 0
54, 47, 45
237, 77, 0
238, 77, 0
*/
/*

#define BARVA1 (RGB(255,0,255))
#define BARVA2 (RGB(238,77,0))
#define BARVA3 (RGB(237,77,0))
#define BARVA4 (RGB(0,0,0))
#define BARVA5 (RGB(54,47,45))

#define pruhledna_barva(pruhl, barva) (barva = barva&0xffffff, (pruhl && (barva == BARVA1 || barva == BARVA2 || barva == BARVA3 || barva == BARVA4 ||barva == BARVA5)))

void bitmap::to_16bit(byte maska, int pruhl)
{
  dword *p_pix;
  dword  barva;
  int    r,g,b;
  int    tx,ty,
         dx = x,
         dy = y;

  for(ty = 0; ty < dy; ty++) {
    p_pix = data+(ty*dx);
    for(tx = 0; tx < dx; tx++, p_pix++) {
      barva = *p_pix;
      
      if(pruhledna_barva(pruhl,barva))
        continue;

      bmp_pix_to_16b((byte *)p_pix, &r, &g, &b);
            
      if(tx+1 < dx) {
        barva = p_pix[1];
        if(!pruhledna_barva(pruhl,barva))
          add_posun_rgba((byte *)(p_pix+1),r,g,b,1);
      }
      
      if(ty+1 < dy) {
        p_pix += dx;
        barva = *p_pix;
        
        if(!pruhledna_barva(pruhl,barva))
          add_posun_rgba((byte *)(p_pix),r,g,b,2);
        
        if(tx+1 < dx) {
          barva = p_pix[1];
          if(!pruhledna_barva(pruhl,barva))
            add_posun_rgba((byte *)(p_pix+1),r,g,b,4);
        }
        
        if(tx > 0) {
          barva = p_pix[-1];
          if(!pruhledna_barva(pruhl,barva))
            add_posun_rgba((byte *)(p_pix-1),r,g,b,3);
        }
        
        p_pix -= dx;
      }
    }
  }
}

void bitmap::alfa2bmp(AUX_RGBImageRec *p_alfa)
{  
  float a;
  int   i,n,j;

  n = x*y*3;
  assert(x == p_alfa->sizeX && y == p_alfa->sizeY);

  for(i = 0,j = 0; i < n; i += 3, j++) {
    a = (float)p_alfa->data[i]+(float)p_alfa->data[i+1]+(float)p_alfa->data[i+2];
    a /= 3.0f;
    data[j] = byte_a(data[j],(int)a);
  }
}
*/
/* Prevede standartni bitmapu na dot3 bitmapu
*/
/*
void bitmap::to_dot3(void)
{
  BITMAPA *p_dot = new BITMAPA(*this);
  BITMAPA_PIXEL pix;
  dword   *p_tmp;
  float    dX, dY, nX, nY, nZ, oolen;
  int      tx, ty;

  for(ty = 0; ty < y; ty++) {
    for(tx = 0; tx < x; tx++) {
      // Do Y Sobel filter
      pix = getpixel_pix((tx-1+x) % x, (ty+1) % y);
      dY  = ((float) pix.r) / 255.0f * -1.0f;
      
      pix = getpixel_pix(tx   % x, (ty+1) % y);
      dY += ((float) pix.r) / 255.0f * -2.0f;
      
      pix = getpixel_pix((tx+1) % x, (ty+1) % y);
      dY += ((float) pix.r) / 255.0f * -1.0f;
      
      pix = getpixel_pix((tx-1+x) % x, (ty-1+y) % y);
      dY += ((float) pix.r) / 255.0f *  1.0f;
      
      pix = getpixel_pix(tx   % x, (ty-1+y) % y);
      dY += ((float) pix.r) / 255.0f *  2.0f;
      
      pix = getpixel_pix((tx+1) % x, (ty-1+y) % y);
      dY += ((float) pix.r) / 255.0f *  1.0f;
      
      // Do X Sobel filter
      pix = getpixel_pix((tx-1+x) % x, (ty-1+y) % y);
      dX  = ((float) pix.r) / 255.0f * -1.0f;
      
      pix = getpixel_pix((tx-1+x) % x,   ty   % y);
      dX += ((float) pix.r) / 255.0f * -2.0f;
      
      pix = getpixel_pix((tx-1+x) % x, (ty+1) % y);
      dX += ((float) pix.r) / 255.0f * -1.0f;
      
      pix = getpixel_pix((tx+1) % x, (ty-1+y) % y);
      dX += ((float) pix.r) / 255.0f *  1.0f;
      
      pix = getpixel_pix((tx+1) % x,   ty   % y);
      dX += ((float) pix.r) / 255.0f *  2.0f;
      
      pix = getpixel_pix((tx+1) % x, (ty+1) % y);
      dX += ((float) pix.r) / 255.0f *  1.0f;
      
      
      // Cross Product of components of gradient reduces to
      nX = -dX;
      nY = -dY;
      nZ = 1;
      
      // Normalize
      oolen = 1.0f/((float) sqrt(nX*nX + nY*nY + nZ*nZ));
      nX *= oolen;
      nY *= oolen;
      nZ *= oolen;
      
      pix.r = (BYTE) PackFloatInByte(nX);
      pix.g = (BYTE) PackFloatInByte(nY);
      pix.b = (BYTE) PackFloatInByte(nZ);
      
      add_color_rgb_byte((byte *)&pix,60,60,60);
      p_dot->putpixel_pix(tx, ty, pix);
    }
  }

  p_tmp = p_dot->data;
  p_dot->data = data;
  data = p_tmp;  
  delete p_dot;
}
*/
