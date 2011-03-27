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

#ifndef  __SURFACE_MASK_H__
#define  __SURFACE_MASK_H__

// General mask interface - it's provided by various class
typedef class mask_interface
{  
  
public:
  
  virtual tpos width_get(void) = 0;
  virtual tpos height_get(void) = 0; 
    
  virtual bool mask_get(int x, int y) = 0;
  virtual void mask_set(int x, int y, bool mask) = 0;  
  
} MASK_INTERFACE;

/*
 * Basic surface mask class (boolean)
 */
typedef class surface_mask
  : public surface,
    public mask_interface
{
  
protected:  
  
  BIT_ARRAY pixels;

public:
  
  bool loaded(void)
  {
    return(pixels.loaded());
  }
  
  void * pixels_get(void)
  {
    return(NULL);
  }

  void * pixels_get(tpos x, tpos y)
  {
    return(NULL);
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
  
  int index_get(tpos x, tpos y)
  {
    return(y*width + x);
  }
  
public:

  bool load(const char *p_file, SURFACE_FORMAT format = SURFACE_TEXTURE)
  {
    return(FALSE);
  }
  bool load_alpha(const char *p_file)
  {
    return(FALSE);
  }

  bool save(const char *p_file, IMAGE_FORMAT format = IMAGE_BMP)
  {
    return(FALSE);
  }

  void copy(class surface_mask *p_src)
  {
    assert(0);
  }
  void move(class surface_mask *p_src)
  {
    assert(0);
  }
    
public:
    
  void clear(void)
  {
    pixels.clear();
  }
  void create(tpos width, tpos height, SURFACE_FORMAT format = SURFACE_TEXTURE)
  {
    this->width = width;
    this->height = height;
    pixels.create(width*height);
  }
  void create(SURFACE *p_surf, SURFACE_FORMAT format = SURFACE_TEXTURE)
  {
    assert(0);
  }

public:
    
  void fill(tcolor mask)
  {
    pixels.set(mask);
  }
  
  void fill(tpos sx, tpos sy, tpos dx, tpos dy, tcolor mask)
  {
    if(loaded()) {
      int x,y;
      for(y = 0; y < dy; y++) {
        for(x = 0; x < dx; x++) {
          if(pixel_valid(sx+x,sy+y)) {
            pixels.set(index_get(sx+x,sy+y), (bool)mask);
          }
        }
      }
    }
  }
  
  void blit(class surface_mask *p_dst, tpos tx = 0, tpos ty = 0)
  {
    assert(0);
  }
  
  void blit(tpos sx, tpos sy, tpos dx, tpos dy, class surface_mask *p_dst, 
            tpos tx = 0, tpos ty = 0)
  {
    assert(0);
  }
  
  void scale(int nx, int ny)
  {
    assert(0);
  }
  
  /*
   * Mask bitmaps
   */
  bool pixel_get(int x, int y)
  {
    if(loaded() && pixel_valid(x,y)) {
      return(pixels.get(index_get(x,y)));
    }    
    return(FALSE);
  }
  void pixel_set(int x, int y, bool mask)
  {
    if(loaded() && pixel_valid(x,y)) {
      pixels.set(index_get(x,y), mask);
    }
  }

  /*
   * Mask interface
   */
  bool mask_get(int x, int y)
  {
    return(pixel_get(x,y));
  }
  
  void mask_set(int x, int y, bool mask)
  {
    pixel_set(x,y,mask);
  }  
  
public:

  surface_mask(void)
  : surface(MASK_SURFACE, PIXEL_BOOL),    
    width(0),
    height(0)
  {
  }
  
  surface_mask(tpos width, tpos height)
  : surface(MASK_SURFACE, PIXEL_BOOL),
    width(0),
    height(0)
  {
    create(width,height);
  }

  ~surface_mask(void)
  {
    clear();
  }
        
} SURFACE_MASK;

#endif //  __SURFACE_MASK_H__
