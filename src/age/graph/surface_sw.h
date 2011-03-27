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

#ifndef  __SURFACE_SW_H__
#define  __SURFACE_SW_H__

typedef class surface_sw
  : public surface
{

public:    
  // Blending operation, used by pixel_set()
  BLEND_OPERATION op;
  
protected:  
  
  tcolor *p_pixels;

public:
  
  bool loaded(void)
  {
    return(p_pixels != NULL);
  }
  
  void * pixels_get(void)
  {
    return(p_pixels);
  }

  void * pixels_get(tpos x, tpos y)
  {
    return(p_pixels ? p_pixels + width*y + x : NULL);
  }

protected:
  
  tpos    width;
  tpos    height;

public:

  tpos width_get(void)
  {
    return(width);
  }
  tpos height_get(void)
  {
    return(height);
  }
  void size_get(tpos *p_dx, tpos *p_dy)
  {
    *p_dx = width;
    *p_dy = height;
  }
  
public:

  bool load(const char *p_file, SURFACE_FORMAT format = SURFACE_TEXTURE);
  bool load_alpha(const char *p_file);

  bool save(const char *p_file, IMAGE_FORMAT format = IMAGE_BMP);

  void copy(class surface_sw *p_src);
  void move(class surface_sw *p_src);
    
public:
    
  void clear(void);
  void create(tpos width, tpos height, SURFACE_FORMAT format = SURFACE_TEXTURE);
  void create(SURFACE *p_surf, SURFACE_FORMAT format = SURFACE_TEXTURE);

public:
    
  void fill(tcolor color);
  void fill(tpos sx, tpos sy, tpos dx, tpos dy, tcolor color);
      
  void blit(class surface_sw *p_dst, tpos tx = 0, tpos ty = 0);
  void blit(tpos sx, tpos sy, tpos dx, tpos dy, class surface_sw *p_dst, 
            tpos tx = 0, tpos ty = 0);

  void scale(int nx, int ny);
      
  /*
   * Color bitmaps
   */  
  tcolor pixel_get(int x, int y)
  {
    tcolor color = 0;
    if(pixel_valid(x,y)) {
      tcolor *p_pixels = (tcolor *)pixels_get();
      color = (*(p_pixels+y*width+x));
    }
    return(color);
  }
  
  void pixel_set(int x, int y, tcolor color)
  {
    if(pixel_valid(x,y)) {
      if(op.op == OPERATION_SET && !op.blend) {
        pixel_set_simple(x,y,color);
      }
      else {
        pixel_set_complex(x,y,RGBB(color));
      }
    }
  }
    
private:
  
  void pixel_set_simple(int x, int y, tcolor color)
  {
    tcolor *p_pixels = (tcolor *)pixels_get();
    *(p_pixels+y*width+x) = color;
  }
  void pixel_set_complex(int x, int y, RGBB color);
  
public:  
  
  // look for near color
  bool    pixel_search(int x, int y, tcolor color, int range, int &px, int &py, float &distance_best);
  bool    pixel_search_square(int x, int y, tcolor color, int distance, int &px, int &py, float &distance_best);

public:
  
  surface_sw(PIXEL_FORMAT format = PIXEL_TCOLOR);
  surface_sw(char *p_file, PIXEL_FORMAT format = PIXEL_TCOLOR);
  surface_sw(tpos width, tpos height, PIXEL_FORMAT format = PIXEL_TCOLOR);
  ~surface_sw(void);

} SURFACE_SW;

#endif //  __SURFACE_SW_H__
