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

#ifndef  __SURFACE_H__
#define  __SURFACE_H__

#include <assert.h>

// -------------------------------------------------------
//   the surface class
// -------------------------------------------------------

class surface;
typedef class surface SURFACE;

typedef enum
{
  SDL_SURFACE,
  SW_SURFACE,
  MASK_SURFACE,
  HEIGT_SURFACE
  
} SURFACE_CLASS;

typedef enum
{
  IMAGE_BMP = 0
  
} IMAGE_FORMAT;

typedef enum
{
  SURFACE_TEXTURE = 0,
  SURFACE_ALPHA,
  SURFACE_DISPLAY  
  
} SURFACE_FORMAT;

typedef enum
{
  
  PIXEL_TCOLOR,
  PIXEL_FLOAT,  
  PIXEL_BOOL,
  
} PIXEL_FORMAT;

typedef enum
{
  TARGET_SET = 0,
  TARGET_ADD,
  TARGET_SUB,
  TARGET_MULT
  
} BLIT_OPERATION;

/*
 * Base histogram class
 */
typedef class histogram
{
  
public:
  
  // Get the nearest color from the histogram
  virtual RGBB color_get(RGBB color)
  {
    return(RGBB(0));
  }
  
  // Get the nearest color from the histogram
  virtual RGBB color_get(float position)
  {
    return(RGBB(0));
  }

public:
  
  histogram(void) {};
  virtual ~histogram(void) {};
    
} HISTOGRAM;

/*
 * Histogram implementation
 */
typedef struct histogram_color
{
  RGBB  color;
  
  union {
    float probability;
    int   count;
  };
  
} HISTOGRAM_COLOR;

typedef struct histogram_index {

  int   index;
  float current;
  
} HISTOGRAM_INDEX;

typedef class histogram_sector_info {
  
  HISTOGRAM_COLOR *p_start;
  int              num;
  
public:

  int  color_check(int r, int g, int b);
  bool color_get(RGBB color, RGBB &color_best, float &dist_best);
  void set(HISTOGRAM_COLOR *p_start, int num);
  
} HISTOGRAM_SECTOR_INFO;

#define HISTOGRAM_CUBE_DIMENSION_SHIFT  6
#define HISTOGRAM_CUBE_DIMENSION        ((1)<<(HISTOGRAM_CUBE_DIMENSION_SHIFT))

#define COLOR_TO_CUBE_DIMENSION(c)      ((c)>>(8-HISTOGRAM_CUBE_DIMENSION_SHIFT))
#define COLOR_FROM_CUBE_DIMENSION(c)    ((c)<<(8-HISTOGRAM_CUBE_DIMENSION_SHIFT))
#define COLOR_CUBE_SIZE                 ((1)<<(8-HISTOGRAM_CUBE_DIMENSION_SHIFT))

typedef class histogram_cube {
  
  // RGB indexes
  HISTOGRAM_SECTOR_INFO cube[HISTOGRAM_CUBE_DIMENSION]
                            [HISTOGRAM_CUBE_DIMENSION]
                            [HISTOGRAM_CUBE_DIMENSION];
private:
  
  bool color_valid(int r_min, int r_max,
                   int g_min, int g_max,
                   int b_min, int b_max,
                   int r, int g, int b)
  {
    if(r < 0 || r >= HISTOGRAM_CUBE_DIMENSION)
      return(FALSE);
    if(g < 0 || g >= HISTOGRAM_CUBE_DIMENSION)
      return(FALSE);
    if(b < 0 || b >= HISTOGRAM_CUBE_DIMENSION)
      return(FALSE);
  
    // at least one color has to be equal min or max
    return((r == r_min || r == r_max) ||
           (g == g_min || g == g_max) ||
           (b == b_min || b == b_max));
  }


  bool color_get(RGBB color, int size, RGBB &color_best);

public:  
  
  int  color_check(void);
  RGBB color_get(RGBB color);
  
public:
  
  histogram_cube(HISTOGRAM_COLOR *p_hist, int hist_num);

} HISTOGRAM_CUBE;

/*
 * 'Flat' histogram model - 1D linear table
 */
#define HISTOGRAM_INDEX_TABLE_SIZE        100
#define HISTOGRAM_INDEX_TABLE_SIZE_SMALL  10

#define HISTOGRAM_CUBE_TRESHOLD  100

typedef struct histogram_table 
  : public histogram
{

  // Basic histogram table
  HISTOGRAM_COLOR *p_hist;
  int              hist_num;
  
  // Indexes for probability look-up
  HISTOGRAM_INDEX  index[HISTOGRAM_INDEX_TABLE_SIZE];
  int              index_num;
  
  // Index the histogram table
private:
  
  bool  index_create(void);

private:

  HISTOGRAM_CUBE  *p_cube;
  
private:
  
  void  cube_create(void);
  void  cube_remove(void);
  
public:

  // Get color based in the color position (probability)
  // position is from <0,1>
  RGBB color_get(float position);

  // Get the nearest color from the histogram
  RGBB color_get(RGBB color);
  
public:  
  
  histogram_table(void);
  histogram_table(SURFACE *p_surf);
  histogram_table(HISTOGRAM_COLOR *p_histogram, int histogram_num);  
  ~histogram_table(void);
  
} HISTOGRAM_TABLE;

HISTOGRAM * histogram_new(SURFACE *p_surf);

// Undefined height pixel
#define HEIGHT_UNSET          (FLOAT_UNDEFINED)

typedef class surface
{
  SURFACE_CLASS surface_class;
  
public:
  
  SURFACE_CLASS surface_class_get(void)
  {
    return(surface_class);
  }

private:
    
  PIXEL_FORMAT pixel_format;
  
public:

  PIXEL_FORMAT pixel_format_get(void)
  {
    return(pixel_format);
  }

public:
  
  virtual void * pixels_get(void) = 0;
  virtual void * pixels_get(tpos x, tpos y) = 0;

protected:

  SURFACE_FORMAT  surface_format;         // High-level surface format
  
public:
  
  virtual bool lock_required(void)
  {
    return(FALSE);
  }
  
  virtual bool locked(void) { return(TRUE); };
  virtual void lock(void)   {};
  virtual void unlock(void) {};

private:  
  
  tcount used;

public:

  tcount ref_get(void)
  {      
    return(used);
  }  
  void ref_inc(void)
  {
    assert(used >= 0);
    used++;
  }
  void ref_dec(void)
  {
    used--;
    assert(used >= 0);
  }

public:

  virtual bool loaded(void) = 0;

  virtual bool load(const char *p_file, SURFACE_FORMAT format = SURFACE_TEXTURE) = 0;
  virtual bool load_alpha(const char *p_file) = 0;  

  virtual bool save(const char *p_file, IMAGE_FORMAT format = IMAGE_BMP) = 0;

  virtual void scale(tpos nx, tpos ny) = 0;
  
public:
    
  virtual void clear(void) {};
  virtual void create(tpos width, tpos height, SURFACE_FORMAT format = SURFACE_TEXTURE) = 0;
    
public:
  
  virtual void convert_to_texture(void)        {};
  virtual void convert_to_display_format(void) {};
  virtual void convert_to_alpha(void)          {};
  virtual void convert_to_heightmap(void)      {};
  
public:

  virtual tpos width_get(void) = 0;
  virtual tpos height_get(void) = 0;
  virtual void size_get(tpos *p_dx, tpos *p_dy) = 0;
  
  RECT2DI rect_get(void)
  {
    tpos dx,dy;
    size_get(&dx, &dy);
    RECT2DI r(0, 0, dx, dy);
    return(r);
  }
    
  bool pixel_valid(tpos x, tpos y)
  {
    tpos dx,dy;
    size_get(&dx, &dy);
    return(x >= 0 && x < dx &&
           y >= 0 && y < dy);
  }
  
  bool rect_trim(tpos &x, tpos &y, tpos &dx, tpos &dy)
  {    
    tpos sdx,sdy;
    size_get(&sdx, &sdy);

    if(x >= sdx || y >= sdy) {
      return(FALSE);
    } else {
      if(x < 0) {
        dx += x;
        x = 0;      
      }
      if(y < 0) {
        dy += y;
        y = 0;
      }
    }
    
    if(x+dx >= sdx) {
      dx = sdx-x;
    }
    if(y+dy >= sdy) {
      dy = sdy-y;
    }
    
    return(dx > 0 && dy > 0);
  }
  
public:

  // Return histogram as flat table
  HISTOGRAM_COLOR * histogram_get(int *p_num);

public:

  surface(SURFACE_CLASS sclass, PIXEL_FORMAT format);
  virtual ~surface(void);

} SURFACE;

#endif // __SURFACE_H__
