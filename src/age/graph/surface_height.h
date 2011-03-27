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

#ifndef  __SURFACE_HEIGHT_H__
#define  __SURFACE_HEIGHT_H__

// Kernel function for hightmap filters
typedef float FILTER_KERNEL_FUNC(int dist_max,
                                 int *p_list_dist, 
                                 float *p_list_height,
                                 int dist_num);

// Kernel filter prototypes
float surface_height_filter_interpolate(int dist_max, int *p_list_dist, float *p_list_height);
float surface_height_filter_blur(int dist_max, int *p_list_dist, float *p_list_height);

// Kernel filter defines
#define KERNEL_INTERPOLATE    surface_height_filter_interpolate
#define KERNEL_BLUR           surface_height_filter_blur

// Square types for mid-point interpolation
typedef enum 
{
  SQUARE_TYPE_1 = 0,
  SQUARE_TYPE_2
  
} SQUARE_TYPE;

typedef enum
{
  INTERPOLATION_MID_POINT,
  INTERPOLATION_LINE_MIN,
  INTERPOLATION_LINE_MAX,
  INTERPOLATION_LINE_CENTER,
  INTERPOLATION_LINE_PRIORITY_HIGH,
  INTERPOLATION_LINE_PRIORITY_LOW,
  INTERPOLATION_LINE_RANGE_HIGH,
  INTERPOLATION_LINE_RANGE_LOW,
  INTERPOLATION_LINE_RANDOM
    
} SQUARE_INTERPOLATION_TYPE;

// Configuration for mid-point generator
typedef struct mid_point_config
{
  // Hurst coeficient for fractal generator
  float generator_hurst;

  // Basic generator ranges
  float generator_delta;
  float generator_center;
  
  // Fixed values for border/center
  float correction_border;
  float correction_center;
  
  // Fixed starting corner points
  float border_start_height;

  // Perturpabion applied during interpolation
  float perturbation;
  
  // Stop generator at this distance
  // between pixels
  int   pixel_distance;

  // Filter back
  bool  pixel_filter_back;

  // Fill missing 
  bool  pixel_fill;
  
  // Heightmap filter
  bool  pixel_filter;
  int   pixel_filter_num;
 
  // Run-time values used by generator
  bool  height_overwrite;
  int   iteration_target;
  
  // interpolation methods & ranges
  SQUARE_INTERPOLATION_TYPE ifirst;
  SQUARE_INTERPOLATION_TYPE isecond;
  int                       iborder;
  
  SQUARE_INTERPOLATION_TYPE interpolation_get(tpos size)
  {
    if(size >= iborder) {
      return(ifirst);
    }
    return(isecond);
  }

  // generation methods & ranges
  int  gborder;
  
  // TRUE - point is interpolated & generated
  // FALSE - point is interpolated only
  bool generation_get(tpos size)
  {
    return(size >= gborder);
  }
  
  mid_point_config(void);

} MID_POINT_CONFIG;

// Configuration for perlin-noise generator
// Some code comes from http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
typedef struct perlin_config
{
  
public:
  
  float persistence;
  int   octaves;
  int   octaves_start;

private:

  tpos   width, height;
  float *p_array;
  
private:  
  
  int   point_norm(int p)
  {
    if(p < 0) {
      p = width-(abs(p)%width)-1;
    } else if(p > width-1) {
      p = p%width;
    }
    return(p);
  }

  float point_get(tpos x, tpos y)
  {
    x = point_norm(x);
    y = point_norm(y);
    
    return(p_array[y*width + x]);
  }
  
  float square_get(tpos x, tpos y)
  {
    
    float corners = (point_get(x-1, y-1)+ 
                     point_get(x+1, y-1)+
                     point_get(x-1, y+1)+
                     point_get(x+1, y+1))/16;
    
    float sides   = (point_get(x-1, y)+
                     point_get(x+1, y)+
                     point_get(x, y-1)+
                     point_get(x, y+1))/8;
    float center  =  point_get(x, y)/4;
    return corners + sides + center;
    
    /*
    return point_get(x,y);
    */
  }
  
  /*
    Square interpolation:
  
    *---o---*
        |
        #
        |
    *---o---*  
  */  
  float noise_get(float x, float y)
  { 
    int   integer_x    = (int)floorf(x);
    float fractional_x = x - integer_x;
    int   integer_y    = (int)floorf(y);
    float fractional_y = y - integer_y;

    float v1 = square_get(integer_x,     integer_y);
    float v2 = square_get(integer_x + 1, integer_y);
    float v3 = square_get(integer_x,     integer_y + 1);
    float v4 = square_get(integer_x + 1, integer_y + 1);

    float i1 = interpolate_cos(v1, v2, fractional_x);
    float i2 = interpolate_cos(v3, v4, fractional_x);

    return(interpolate_cos(i1, i2, fractional_y));
  }

public:
  
  float perlin_get(tpos x, tpos y)
  {
    float result = 0;
    int i;
    for(i = octaves_start; i < octaves_start+octaves; i++) {
      float frequency = powf(2,i);
      float amplitude = powf(persistence, i);
      
      // Behind texture resolution
      if(frequency > width && frequency > height)
        break;
      
      result = result + noise_get(x*(frequency/width), y*(frequency/height)) * amplitude;
    }

    return result;
  }

public:
  
  void rand(tpos width, tpos height);
  void release(void);
  
public:
    
  perlin_config(void);
  
} PERLIN_CONFIG;

class surface_height;
typedef class surface_height SURFACE_HEIGHT;

typedef void (surface_height::*HEIGHT_WRITE_FUNC)(MID_POINT_CONFIG *p_config,
                                                  int x, int y, 
                                                  float h0, float h1, float h2, float h3,
                                                  SQUARE_TYPE type,
                                                  SQUARE_INTERPOLATION_TYPE itype,
                                                  float iteration, bool generate);

typedef class surface_height
  : public surface_sw
{
  
  float pixel_height_range; // pixel ranges

public:

  tpos width_get(void)
  {
    return(surface_sw::width_get());
  }
  tpos height_get(void)
  {
    return(surface_sw::height_get());
  }

public:
  
  bool load(const char *p_file, SURFACE_FORMAT format = SURFACE_TEXTURE);
  
public:

  void copy(class surface_height *p_src);
  void move(class surface_height *p_src);
  
public:
  
  void create(tpos width, tpos height, SURFACE_FORMAT format = SURFACE_TEXTURE);
  void create(SURFACE *p_surf, SURFACE_FORMAT format = SURFACE_TEXTURE);
    
public:

  void fill(float height);
  void fill(tpos x, tpos y, tpos dx, tpos dy, float height);
  
  void blit(class surface_height *p_dst);
  void blit(class surface_height *p_dst, tpos tx, tpos ty);
  void blit(tpos sx, tpos sy, tpos dx, tpos dy, 
            class surface_height *p_dst, tpos tx, tpos ty, tpos tdx, tpos tdy);
    
public:
  
  void            scale(int nx, int ny);
  SURFACE_HEIGHT  scale_new(int nx, int ny);

public:
  
  float color_to_height(Uint8 r, Uint8 g, Uint8 b)
  {
    return((r+g+b)/(255*3.0f));
  }
  float color_to_height(tcolor color)
  {
    return(rgb_intensity(color));
  }
  tcolor height_to_color(float height)
  {    
    return(float_rgb(height, height, height));
  }
  
  /*
   * Heightmaps
   */
  
  float height_get(int x, int y)
  {
    float h = 0;
    if(pixel_valid(x,y)) {
      float *p_pixels = (float *)pixels_get();
      h = (*(p_pixels+y*width+x));
    }
    return(h);
  }
  
  float height_get(int x, int y, float default_value)
  {
    float h = default_value;
    if(pixel_valid(x,y)) {
      float *p_pixels = (float *)pixels_get();
      h = (*(p_pixels+y*width+x));
    }
    return(h);
  }
    
  void height_set(int x, int y, float height)
  {
    if(pixel_valid(x,y)) {
      if(op.op == OPERATION_SET && !op.blend) {
        height_set_simple(x,y,height);
      }
      else {
        height_set_complex(x,y,height);
      }
    }
  }
  
private:
  
  void height_set_simple(int x, int y, float height)
  {
    float *p_pixels = (float *)pixels_get();
    *(p_pixels+y*width+x) = height;
  }  
  void height_set_complex(int x, int y, float height);

public:
  
  VECT    height_normal_get(int x, int y,
                            MASK_INTERFACE *p_mask = NULL, 
                            int mask_x = 0, int mask_y = 0);

  float   height_range_get(void);
  void    height_range_set(float range);
  
  void    height_range_default(void);
  
  /*
   * Heightmaps - filters
   */
  int     height_get(int cx, int cy, int width, int height, int *p_list_dist,
                     float *p_list_height, bool center = TRUE);
  int     height_get_cross(int cx, int cy, int *p_list_dist,
                           float *p_list_height, bool center = TRUE);

  // Filter all values, HEIGHT_UNSET filter too
  void    height_filter(tpos x, tpos y, FILTER_KERNEL_FUNC *p_kernel, bool center = TRUE);
  void    height_filter(tpos x, tpos y, tpos width, tpos height,
                        FILTER_KERNEL_FUNC *p_kernel,
                        bool center = TRUE);
                        
  // Fill unset values only (HEIGHT_UNSET)
  void    height_fill(tpos x, tpos y, tpos width, tpos height,
                      FILTER_KERNEL_FUNC *p_kernel);



  void    height_normalize(void);
  void    height_normalize(tpos sx, tpos sy, tpos width, tpos height,
                           float value_min = 0.0f, float value_max = 1.0f);  

  void    height_blit(class surface_height *p_dst, tpos tx, tpos ty, BLIT_OPERATION op);  

  /*
   * Heightmaps - mid-point generator
   */
private:

  int     height_iteration_get(MID_POINT_CONFIG *p_config, int iteration);
  float   height_correction(int x, int y, float current_height, float border, float center);
  float   height_generate(MID_POINT_CONFIG *p_config, tpos x, tpos y, float height, int iteration);
  int     height_interate_cross(MID_POINT_CONFIG *p_config, int x, int y, int distance, int iteration, SQUARE_TYPE type, SQUARE_INTERPOLATION_TYPE itype, bool generate, HEIGHT_WRITE_FUNC func);
  int     height_rectangle_emit(MID_POINT_CONFIG *p_config, int x, int y, int width, int height, int iteration_act, SQUARE_TYPE type, HEIGHT_WRITE_FUNC func);

private:

  void    height_write(MID_POINT_CONFIG *p_config, int x, int y,
                       float h0, float h1, float h2, float h3,
                       SQUARE_TYPE type, SQUARE_INTERPOLATION_TYPE itype,
                       float iteration, bool generate);
  void    height_filter(MID_POINT_CONFIG *p_config, int x, int y,
                        float h0, float h1, float h2, float h3,
                        SQUARE_TYPE type, SQUARE_INTERPOLATION_TYPE itype,
                        float iteration, bool generate);
public:

  void    generate_mid_point(MID_POINT_CONFIG *p_config);
  void    generate_mid_point(MID_POINT_CONFIG *p_config, int x, int y, int width, int height);

  /*
   * Heightmaps - perlin noise generator
   */
public:

  void    generate_perlin(PERLIN_CONFIG *p_config);
  void    generate_perlin(PERLIN_CONFIG *p_config, int x, int y, int width, int height);

public:
  
  surface_height(void);
  surface_height(char *p_file, SURFACE_FORMAT format_ = SURFACE_TEXTURE);
  surface_height(tpos width, tpos height, SURFACE_FORMAT format_ = SURFACE_TEXTURE);
  surface_height(SURFACE_HEIGHT *p_src);

} SURFACE_HEIGHT;

#endif //  __SURFACE_HEIGHT_H__
