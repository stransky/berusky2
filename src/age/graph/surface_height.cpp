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
// -------------------------------------------------------
//   the surface class
// -------------------------------------------------------
// Constants and types

#include "age.h"
#include "generator_random.h"

#define DEBUG_HEIGHT 0

float surface_height_filter_interpolate(int dist_max, int *p_list_dist,
                                        float *p_list_height, int dist_num);
float surface_height_filter_interpolate2(int dist_max, int *p_list_dist,
                                         float *p_list_height, int dist_num);


void perlin_config::rand(tpos width, tpos height)
{
  if(!p_array || this->width != width || this->height != height) {
    release();
    
    this->width = width;
    this->height = height;    
    p_array = (float *)mmalloc(sizeof(float)*width*height);
  }
  
  // Fill with random numbers from -1 to 1
  int i;
  for(i = 0; i < width*height; i++) {
    p_array[i] = random_generator::generator_rand_0();
  }
}

void perlin_config::release(void)
{
  ffree(p_array);
  width = 0;
  height = 0;
}

perlin_config::perlin_config(void)
{ 
  p_array = NULL;
  
  // Some perlin config
  octaves_start = 0;
  octaves = 30;
  persistence = 0.8f;
}

void surface_height::create(tpos width, tpos height, SURFACE_FORMAT format)
{ 
  surface_sw::create(width, height, format);
  height_range_default();
}

void surface_height::create(SURFACE *p_surf, SURFACE_FORMAT format)
{ 
  surface_sw::create(p_surf, format);
  height_range_default();
}

bool surface_height::load(const char *p_file, SURFACE_FORMAT format)
{
  if(surface_sw::load(p_file,format)) {
    
    tpos width = width_get();
    tpos height = height_get();    
    tpos x,y;
    
    for(y = 0; y < height; y++) {
      for(x = 0; x < width; x++) {
        tcolor *p_src = (tcolor *)pixels_get(x,y);
        float  *p_dst = (float *)p_src;
        *p_dst = color_to_height(*p_src);
      }
    }
    
    height_range_default();
    
    return(TRUE);
  }
  return(FALSE);
}

void surface_height::copy(class surface_height *p_src)
{
  surface_sw::copy(p_src);
  pixel_height_range = p_src->pixel_height_range;
}

void surface_height::move(class surface_height *p_src)
{  
  surface_sw::move(p_src);
  pixel_height_range = p_src->pixel_height_range;
}

surface_height::surface_height(void)
: surface_sw(PIXEL_FLOAT)
{
  pixel_height_range = 1.0f;
}

surface_height::surface_height(char *p_file, SURFACE_FORMAT format_)
: surface_sw(PIXEL_FLOAT)
{
  load(p_file, format_);
}

surface_height::surface_height(tpos width, tpos height, SURFACE_FORMAT format_)
: surface_sw(PIXEL_FLOAT)
{
  create(width,height);
}

surface_height::surface_height(SURFACE_HEIGHT *p_src)
: surface_sw(PIXEL_FLOAT)
{
  copy(p_src);
}

// blit whole source surtace to destination surface
void surface_height::blit(class surface_height *p_dst)
{
  tpos sdx = width_get();
  tpos sdy = height_get();

  tpos tdx = p_dst->width_get();
  tpos tdy = p_dst->height_get();
  
  if(sdx == tdx && sdy == tdy)
  {
    p_dst->copy(this);
  }
  else {
    blit(0, 0, sdx, sdy, p_dst, 0, 0, tdx, tdy);
  }
}

// blit whole source surtace to destination surface_height
void surface_height::blit(class surface_height *p_dst, tpos tx, tpos ty)
{
  tpos dx = width_get();
  tpos dy = height_get();
  
  blit(0, 0, dx, dy, p_dst, tx, ty, dx, dy);
}

// HEIGHT_UNSET -> ignore them
// blit part of source surface_height to destination surface_height
void surface_height::blit(tpos sx, tpos sy, tpos dx, tpos dy, 
                          class surface_height *p_dst,
                          tpos tx, tpos ty, tpos tdx, tpos tdy)
{
  if(dx == tdx && dy == tdy)
  {
    int x,y;
    for(y = 0; y < tdy; y++) {
      for(x = 0; x < tdx; x++) {
        float height = height_get(sx+x, sy+y);
        p_dst->height_set(tx+x, ty+y, height);
      }
    }
  }
  else {
    // Scale down
    if(dx >= tdx && dy >= tdy) {
      float scale_x = (float)dx / (float)tdx;
      float scale_y = (float)dy / (float)tdy;
      
      tpos kernel_size_x = ceilf(scale_x);
      tpos kernel_size_y = ceilf(scale_y);
    
      // We want odd kernel size
      if(!(kernel_size_x&0x1)) {
        kernel_size_x++;
      }
      if(!(kernel_size_y&0x1)) {
        kernel_size_y++;
      }
  
      int   *p_distances = (int *)mmalloc(sizeof(int)*kernel_size_x*kernel_size_y);
      float *p_heights = (float *)mmalloc(sizeof(float)*kernel_size_x*kernel_size_y);
      FILTER_KERNEL_FUNC *p_kernel = surface_height_filter_interpolate2;
            
      int x,y;
      for(y = ty; y < ty+tdy; y++) {
        for(x = tx; x < tx+tdx; x++) {
            int src_x = roundf(scale_x*x);
            int src_y = roundf(scale_y*y);
  
            int dist_values = height_get(src_x+kernel_size_x, src_y+kernel_size_y, 
                                         kernel_size_x, kernel_size_y,
                                         p_distances, p_heights, TRUE);
            p_dst->height_set(x, y, p_kernel(0, p_distances, p_heights, 
                              dist_values));
        }
      }
      
      ffree(p_distances);
      ffree(p_heights);
    }
    else if(dx < tdx && dy < tdy) { // Scale up
      float scale_x = (float)tdx/(float)dx;
      float scale_y = (float)tdy/(float)dy;
      
      tpos rect_dx = ceilf(scale_x);
      tpos rect_dy = ceilf(scale_y);
            
      int x,y;
      for(y = sy; y < sy+dy; y++) {
        for(x = sx; x < sx+dx; x++) {
          float hg = height_get(x,y);
          p_dst->fill(x*scale_x,
                      y*scale_y,
                      rect_dx,
                      rect_dy,
                      hg);
        }
      }      
    }
    else {
      // Hm?
      assert(0);
    }
  }
}

SURFACE_HEIGHT surface_height::scale_new(int nx, int ny)
{
  assert(nx > 0 && ny > 0);
  
  SURFACE_HEIGHT tmp(nx,ny);
  
  tpos width = width_get();
  tpos height = height_get();  
  
  float fx = ((float)nx)/width,
        fy = ((float)ny)/height;
  
  tpos x,y;
  
  for(y = 0; y < ny; y++) {
    for(x = 0; x < nx; x++) {
      tmp.height_set(x,y,height_get(x*fx,y*fy));
    }
  }
  
  tmp.height_range_default();
  return(tmp);
}

void surface_height::scale(int nx, int ny)
{
  SURFACE_HEIGHT tmp = scale_new(nx, ny);  
  clear();
  *this = tmp;
}

void surface_height::height_set_complex(int x, int y, float height)
{
  float target = height_get(x,y);
  height_set_simple(x,y,blend(op,target,height));
}

VECT surface_height::height_normal_get(int x, int y, MASK_INTERFACE *p_mask,
                                       int mask_x, int mask_y)
{
  VECT2DI list[] = 
  { 
    VECT2DI( 0, 1),
    VECT2DI( 1, 1),
    VECT2DI( 1, 0),
    VECT2DI( 1,-1),
    VECT2DI( 0,-1),
    VECT2DI(-1,-1),
    VECT2DI(-1, 0),
    VECT2DI(-1, 1),
    VECT2DI( 0, 1)
  };
  VECT vects[] = 
  {
    VECT3DF( 0, 0, 1),
    VECT3DF( 1, 0, 1),
    VECT3DF( 1, 0, 0),
    VECT3DF( 1, 0,-1),
    VECT3DF( 0, 0,-1),
    VECT3DF(-1, 0,-1),
    VECT3DF(-1, 0, 0),
    VECT3DF(-1, 0, 1),
    VECT3DF( 0, 0, 1)
  };
  
  int   points = (int)(sizeof(list)/sizeof(list[0]));
  
  float height_current = height_get(x, y);
  
  int i;
  for(i = 0; i < points; i++) {
    tpos ax = x+list[i].x;
    tpos ay = y+list[i].y;
    if(pixel_valid(ax, ay) && (!p_mask || p_mask->mask_get(mask_x+ax,mask_y+ay))) {
      float height = height_get(ax, ay);
      vects[i].y = (height - height_current)*pixel_height_range;
      vects[i].norm();
    }
    else {
      vects[i].set(FLOAT_UNDEFINED);
    }
  }
  
  VECT last(0);
  for(i = 0; i < points; i++) {    
    if(vects[i].x != FLOAT_UNDEFINED) {
      last = vects[i];      
      break;
    }
  }
  
  if(i == points) {
    // return some default normal
    return(VECT3DF(0,-1,0));
  }
  
  VECT normal(0);
  for(i = i+1; i < points; i++) {
    if(vects[i].x != FLOAT_UNDEFINED) {
      VECT3DF tmp = vect_mult(last, vects[i]);
      tmp.norm();
      normal += tmp;
      last = vects[i];
    }
  }
  
  normal.norm();
  return(normal);
}

float surface_height::height_range_get(void)
{
  return(pixel_height_range);
}

void  surface_height::height_range_set(float range)
{
  pixel_height_range = range;
}

void  surface_height::height_range_default(void)
{
  tpos width = width_get();
  tpos height = height_get();
  pixel_height_range = width > height ? width : height;
}

void  surface_height::fill(float height)
{   
  tpos dx = width_get();
  tpos dy = height_get();
  
  fill(0,0,dx,dy,height);
}

void  surface_height::fill(tpos x, tpos y, tpos dx, tpos dy, float height)
{ 
  if(!pixels_get())
    return;
  
  if(rect_trim(x, y, dx, dy)) {
    int cx, cy;
    for(cy = y; cy < y+dy; cy++) {
      float *p_pixels = (float *)pixels_get(x,cy);
      for(cx = x; cx < x+dx; cx++, p_pixels++)
        *p_pixels = height;
    }
  }
}

/*
  Height filters  
  width and height should be odd numbers (3,5,7...)
*/
int surface_height::height_get(int cx, int cy, int width, int height,
                               int *p_list_dist, float *p_list_height,
                               bool center)
{
  tpos x,y;
  tpos dx = width/2,
       dy = height/2;
  
  int  values = 0;
  
  for(y = cy-dy; y <= cy+dy; y++) {
    for(x = cx-dx; x <= cx+dx; x++) {
  
      if(!pixel_valid(x, y)) {
        continue;
      }

      if(!center && x == cx && y == dy) {
        continue;
      }
      
      float height_current = height_get(x, y);
      if(height_current == HEIGHT_UNSET) {
        continue;
      }
        
      p_list_height[values] = height_current;
      p_list_dist[values] = square_dist(x, y, cx, cy);      
      values++;
    }
  }

  return(values);
}


VECT2DI cross_list[] = 
{ 
  VECT2DI( 0, 0),
  VECT2DI( 0, 1),
  VECT2DI( 0,-1),
  VECT2DI( 1, 0),
  VECT2DI(-1, 0)
};

int surface_height::height_get_cross(int cx, int cy,
                                     int *p_list_dist, float *p_list_height,
                                     bool center)
{ 
  int  i;
  int  values = 0;
  
  for(i = center ? 0 : 1; 
      i < (int)(sizeof(cross_list)/sizeof(cross_list[0])); 
      i++) 
  {
    int x = cx+cross_list[i].x,
        y = cy+cross_list[i].y;
  
    if(!pixel_valid(x, y)) {
      continue;
    }

    float height_current = height_get(x, y);
    if(height_current == HEIGHT_UNSET) {
      continue;
    }
      
    p_list_height[values] = height_current;
    p_list_dist[values] = square_dist(x, y, cx, cy);
    values++;
  }

  return(values);
}

#define MAX_DISTANCES 5

float surface_height_filter_interpolate(int dist_max,
                                        int *p_list_dist,
                                        float *p_list_height,
                                        int dist_num)
{
  float height_current = 0;
  
  float heights[MAX_DISTANCES] = {0,0,0,0,0};
  int   heights_num[MAX_DISTANCES] = {0,0,0,0,0};
  
  assert(dist_max < MAX_DISTANCES);

  // Save all values
  int i;
  for(i = 0; i < dist_num; i++) {
    int dist = p_list_dist[i];
    heights[dist] += p_list_height[i];
    heights_num[dist]++;
  }

  // Interpolate them
  int h_num = 0;
  for(i = 0; i < MAX_DISTANCES; i++) {
    if(heights_num[i] > 0) {
      height_current += (heights[i] / heights_num[i]);
      h_num++;
    }
  }
  
  height_current /= h_num;
  
  return(height_current);
}

float surface_height_filter_interpolate2(int dist_max,
                                         int *p_list_dist,
                                         float *p_list_height,
                                         int dist_num)
{
  float height_current = 0;
   
  // Interpolate them
  int i, num = 0;
  for(i = 0; i < dist_num; i++) {
    height_current += p_list_height[i];
    num++;
  }
  
  height_current /= num;
  
  return(height_current);
}

float surface_height_filter_blur(int dist_max,
                                 int *p_list_dist,
                                 float *p_list_height,
                                 int dist_num)
{
  float height_current = 0;
  /*
  int   i,pixels = 0;
  float diff_min = 1;
  float diff_max = 0;
  
  for(i = 0; i < FILTER_VECTORS-1; i++) {
    if(p_list_dist[i] != 0) {
      
      if(diff_min > fabs(p_list_height[i]))
        diff_min = fabs(p_list_height[i]);
      if(diff_max < fabs(p_list_height[i]))
        diff_max = fabs(p_list_height[i]);
      
      height_current += p_list_height[i];
      pixels++;
    }
  }
  
  height_current /= pixels;  
  //height_current += random_generator::generator_rand_0()*(diff_max-diff_min);
  //height_current += random_generator::generator_rand_0();  
  */
  return(height_current);
}

/*
  height min/max  - values which are replaced in the heighmap
  valid min/max   - values which are valid (considered as ok)
*/
void surface_height::height_filter(tpos x, tpos y, FILTER_KERNEL_FUNC *p_kernel,
                                   bool center)
{  
  #define KERNEL_WIDTH 3
  
  int   distances[KERNEL_WIDTH*KERNEL_WIDTH];
  float heights[KERNEL_WIDTH*KERNEL_WIDTH];

  int dist_values = height_get(x, y, KERNEL_WIDTH, KERNEL_WIDTH,
                               distances, heights, center);
  height_set(x, y, p_kernel(KERNEL_WIDTH/2, distances, heights, dist_values));
}

void surface_height::height_filter(tpos x, tpos y,
                                   tpos width, tpos height,
                                   FILTER_KERNEL_FUNC *p_kernel,
                                   bool center)
{  
  if(rect_trim(x, y, width, height)) {
    int sx,sy;
    for(sy = 0; sy < height; sy++) {
      for(sx = 0; sx < width; sx++) {
        height_filter(sx+x,sy+y,p_kernel,center);
      }
    }
  }
}

void surface_height::height_fill(tpos x, tpos y,
                                 tpos width, tpos height,
                                 FILTER_KERNEL_FUNC *p_kernel)
{
  if(rect_trim(x, y, width, height)) {  
    int sx,sy;
    for(sy = 0; sy < height; sy++) {
      for(sx = 0; sx < width; sx++) {
        
        tpos cx = sx + x;
        tpos cy = sy + y;
        
        if(height_get(cx,cy) == HEIGHT_UNSET) {        
          height_filter(cx,cy,p_kernel);
        }
      }
    }
  }
}

/*
 * Normalize the height-map to 0...1 range
 */
void surface_height::height_normalize(tpos sx, tpos sy, tpos width, tpos height,
                                      float value_min, float value_max)
{  
  float min = FLT_MIN;
  float max =-FLT_MAX;
  
  if(!rect_trim(sx, sy, width, height))
    return;
  
  int x,y;  
  for(y = sy; y < height; y++) {
    for(x = sx; x < width; x++) {
      float height = height_get(x,y);
      if(height == HEIGHT_UNSET) {
        pdebug(DEBUG_HEIGHT,"surface_height::height_normalize(), height == HEIGHT_UNSET, x = %d, y = %d",
                height,x,y);
        continue;
      }
      min = MIN(height,min);
      max = MAX(height,max);
    }
  }
  
  pdebug(DEBUG_HEIGHT,"heightmap_normalize: min = %f, max = %f",min,max);
  
  float range = max - min;
  float new_range = value_max - value_min;
  for(y = sy; y < height; y++) {
    for(x = sx; x < width; x++) {
      float height = height_get(x,y);
      if(height == HEIGHT_UNSET) {
        continue;
      }
            
      // <0,1> range
      height = (height - min) / range;
      
      // <value_min, value_max> range
      height = value_min + height*new_range;
      
      pdebug(DEBUG_HEIGHT,"N: [%d, %d] -> %f",x,y,height);      
      
      height_set(x,y,height);
    }
  }
}

void surface_height::height_normalize(void)
{
  int width = width_get();
  int height = height_get();

  height_normalize(0,0,width,height);
}

/*
 * Height operations
 */
// blit whole source surtace to destination surface_height
void surface_height::height_blit(class surface_height *p_dst, tpos tx, tpos ty, BLIT_OPERATION op)
{
  tpos source_width = width_get();
  tpos source_height = height_get();
  
  tpos target_width = p_dst->width_get();
  tpos target_height = p_dst->height_get();
  
  if(tx+source_width > target_width)
    source_width = target_width-tx;
    
  if(ty+source_height <= target_height)
    source_height = target_height-ty;
  
  tpos x,y;
  for(y = 0; y < source_height; y++) {
    
    float *p_source_line = (float *)pixels_get(0,y);
    float *p_dest_line = (float *)p_dst->pixels_get(tx,ty+y);
    
    for(x = 0; x < source_width; x++) {
      *p_dest_line = *p_source_line;
      
      // TODO
      assert(0);
      
      p_dest_line++;
      p_source_line++;
    }
  }
}

/*
 * Fractal generator
 */
mid_point_config::mid_point_config(void)
{
  generator_hurst = 0.6;

  generator_delta = 1;
  generator_center = 0;
  
  correction_border = FLOAT_UNDEFINED;
  correction_center = FLOAT_UNDEFINED;
  
  border_start_height = 0;
  perturbation = FLOAT_UNDEFINED;
  
  pixel_fill = TRUE;
  pixel_distance = 1;
  
  pixel_filter = FALSE;
  pixel_filter_num = 1;

  pixel_filter_back = FALSE;
  
  height_overwrite = FALSE;
  
  ifirst = INTERPOLATION_MID_POINT;
  isecond = INTERPOLATION_MID_POINT;
  iborder = 3;
  
  gborder = 0;
}

inline static float interpolate_float(float v1, float v2, float v3, float v4)
{
  float result = 0;
  int   num = 0;
  
  if(v1 != HEIGHT_UNSET) {
    result += v1;
    num++;
  }
  if(v2 != HEIGHT_UNSET) {
    result += v2;
    num++;
  }
  if(v3 != HEIGHT_UNSET) {
    result += v3;
    num++;
  }
  if(v4 != HEIGHT_UNSET) {
    result += v4;
    num++;
  }
    
  return(result / num);
}

inline static void range_float(float v1, float v2, float v3, float v4,
                               float &min, float &max)
{
  min =  FLT_MAX;
  max = -FLT_MAX;  
  
  if(v1 != HEIGHT_UNSET) {
    min = MIN(min,v1);
    max = MAX(max,v1);
  }
  if(v2 != HEIGHT_UNSET) {
    min = MIN(min,v2);
    max = MAX(max,v2);
  }
  if(v3 != HEIGHT_UNSET) {
    min = MIN(min,v3);
    max = MAX(max,v3);
  }
  if(v4 != HEIGHT_UNSET) {
    min = MIN(min,v4);
    max = MAX(max,v4);
  }
}

inline static bool interpolate_axe(float v1, float v2, float &min, float &max, float &result)
{
  result = 0;
  int num = 0;
  
  if(v1 != HEIGHT_UNSET) {
    min = MIN(v1,min);
    max = MAX(v1,max);
    result += v1;
    num++;
  }
  
  if(v2 != HEIGHT_UNSET) {
    min = MIN(v2,min);
    max = MAX(v2,max);
    result += v2;
    num++;
  }
  
  if(num == 2) {
    result /= num;
    return(TRUE);
  }
  
  return(FALSE);
}

inline static bool choose_one(float v1, float v2, float v3, float v4, float &result)
{
  if(v1 != HEIGHT_UNSET) {
    result = v1;    
    return(TRUE);
  }
  if(v2 != HEIGHT_UNSET) {
    result = v2;    
    return(TRUE);
  }
  if(v3 != HEIGHT_UNSET) {
    result = v3;
    return(TRUE);
  }
  if(v4 != HEIGHT_UNSET) {
    result = v4;
    return(TRUE);
  }
  
  return(FALSE);
}

/*
  Interpolate in two axes:
  
  v11 -> v12
  v21 -> v22
*/ 
// v11 -> v12 is more important
inline static float interpolate_square(MID_POINT_CONFIG *p_config,
                                       float v11, float v12,
                                       float v21, float v22,
                                       SQUARE_INTERPOLATION_TYPE interpolation)
{
  float result = 0;
  
  switch(interpolation) {
    case INTERPOLATION_MID_POINT:
      result = interpolate_float(v11, v12, v21, v22);
      break;
    
    case INTERPOLATION_LINE_MIN:
    case INTERPOLATION_LINE_MAX:
    case INTERPOLATION_LINE_CENTER:
    case INTERPOLATION_LINE_RANGE_HIGH:
    case INTERPOLATION_LINE_RANGE_LOW:
    case INTERPOLATION_LINE_PRIORITY_HIGH:
    case INTERPOLATION_LINE_PRIORITY_LOW:
    case INTERPOLATION_LINE_RANDOM:
    {
      float min1 =  FLT_MAX;
      float max1 = -FLT_MAX;
      float min2 =  FLT_MAX;
      float max2 = -FLT_MAX;
      
      float a1_height;
      bool  a1 = interpolate_axe(v11, v12, min1, max1, a1_height);
    
      float a2_height;
      bool  a2 = interpolate_axe(v21, v22, min2, max2, a2_height);
      
      if(!a1 && !a2) {
        bool a = choose_one(v11, v12, v21, v22, result);
        assert(a);
      } 
      else if(a1 && a2) {
        switch(interpolation) {
          case INTERPOLATION_LINE_MIN:
            result = MIN(a1_height, a2_height);
            break;
          case INTERPOLATION_LINE_MAX:
            result = MAX(a1_height, a2_height);
            break;
          case INTERPOLATION_LINE_CENTER:
          {
            float center = interpolate_float(v11, v12, v21, v22);
          
            if(abs(center-a1_height) <= abs(center-a2_height)) {
              result = a1_height;
            }
            else {
              result = a2_height;
            }
            break;
          }
          case INTERPOLATION_LINE_PRIORITY_HIGH:
            result = a1_height;
            break;
          case INTERPOLATION_LINE_PRIORITY_LOW:
            result = a2_height;
            break;
          case INTERPOLATION_LINE_RANGE_HIGH:
            result = (abs(max1-min1) > abs(max2-min2)) ? a1_height : a2_height;
            break;
          case INTERPOLATION_LINE_RANGE_LOW:
            result = (abs(max1-min1) < abs(max2-min2)) ? a1_height : a2_height;
            break;
          case INTERPOLATION_LINE_RANDOM:
            result = (random_generator::generator_rand_0() > 0) ? a1_height : a2_height;
            break;
          default:
            break;
        }
        
        pdebug(DEBUG_HEIGHT,"interpolation = %d, a1_height = %f, a2_height = %f, result = %f",
                interpolation,a1_height,a2_height,result);
      }
      else if(a1) {
        result = a1_height;
      }
      else if(a2) {
        result = a2_height;
      }
      break;
    }    
  } 

  if(p_config->perturbation != FLOAT_UNDEFINED) {
    float min;
    float max;
    range_float(v11, v12, v21, v22, min, max);
    float range = (max - min);
    result += random_generator::generator_rand_0()*range*p_config->perturbation;
  }

  pdebug(DEBUG_HEIGHT,"[%f, %f, %f, %f] -> %f", v11, v12, v21, v22, result);
  
  return(result);
}

float surface_height::height_correction(int x, int y, float current_height, float border, float center)
{
  int width = width_get();
  int height = height_get();
  
  if(center != FLOAT_UNDEFINED) {
    tpos  sx = (width-1)/2,
          sy = (height-1)/2;
    
    if(x == sx && y == sy)
      return(center);
  }
  
  if(border != FLOAT_UNDEFINED) {
    tpos  ex = width-1,
          ey = height-1;
    
    if(x == 0 || x == ex || y == 0 || y == ey) {
      return(border);
    }
  }
  
  return(current_height);
}

float surface_height::height_generate(MID_POINT_CONFIG *p_config, tpos x, tpos y, 
                                      float height, int iteration)
{
  height += random_generator_fractal::generate_height(iteration,
                                      p_config->generator_hurst,
                                      p_config->generator_center,
                                      p_config->generator_delta);
  height = height_correction(x, y, height,
                             p_config->correction_border, 
                             p_config->correction_center);  
  return(height);
}

/*
  pdebug(DEBUG_HEIGHT,"R1: [%d, %d] [%d, %d] [%d, %d] [%d, %d]",
         x, y-distance, x, y+distance,
         x-distance, y, x+distance, y);
*/
void surface_height::height_write(MID_POINT_CONFIG *p_config,
                                  int x, int y,
                                  float h0, float h1, float h2, float h3,
                                  SQUARE_TYPE type,
                                  SQUARE_INTERPOLATION_TYPE itype,
                                  float iteration,
                                  bool generate)
{
  float height_center = 0;
  
  if(type == SQUARE_TYPE_1) {
    height_center = interpolate_square(p_config,h0,h1,h2,h3,itype);
  }
  else {
    height_center = interpolate_square(p_config,h2,h3,h0,h1,itype);
  }
  
  pdebug(DEBUG_HEIGHT,"R1: [%d, %d] -> %f", x, y, height_center);    
  if(generate)
    height_center = height_generate(p_config, x, y, height_center, iteration);
  pdebug(DEBUG_HEIGHT,"R2: [%d, %d] -> %f", x, y, height_center);
  
  height_set(x,y,height_center);
}

void surface_height::height_filter(MID_POINT_CONFIG *p_config,
                                   int x, int y,
                                   float h0, float h1, float h2, float h3,
                                   SQUARE_TYPE type,
                                   SQUARE_INTERPOLATION_TYPE itype,
                                   float iteration,
                                   bool generate)
{ 
  height_filter(x,y,surface_height_filter_interpolate2,FALSE);
}

/*
  SQUARE_TYPE (cross)

  Generate:

  *---1---*
  |   |   |
  2---*---2
  |   |   |
  *---1---*
*/

// Iterate height with given distance
int surface_height::height_interate_cross(MID_POINT_CONFIG *p_config,
                                          int x, int y,
                                          int distance,
                                          int iteration,
                                          SQUARE_TYPE type,
                                          SQUARE_INTERPOLATION_TYPE itype,
                                          bool generate,
                                          HEIGHT_WRITE_FUNC func)
{
  float height_center = height_get(x,y);
  
  if(p_config->height_overwrite || height_center == HEIGHT_UNSET) {
    int distance2 = distance+1;
    
    float y0 = height_get(x, y-distance, HEIGHT_UNSET);
    if(y0 == HEIGHT_UNSET)
       y0 = height_get(x, y-distance2, HEIGHT_UNSET);
    
    float y1 = height_get(x, y+distance, HEIGHT_UNSET);
    if(y1 == HEIGHT_UNSET)
       y1 = height_get(x, y+distance2, HEIGHT_UNSET);
    
    float x0 = height_get(x-distance, y, HEIGHT_UNSET);
    if(x0 == HEIGHT_UNSET)
       x0 = height_get(x-distance2, y, HEIGHT_UNSET);
    
    float x1 = height_get(x+distance, y, HEIGHT_UNSET);
    if(x1 == HEIGHT_UNSET)
       x1 = height_get(x+distance2, y, HEIGHT_UNSET);
    
    pdebug(DEBUG_HEIGHT,"R1: [%d, %d] [%d, %d] [%d, %d] [%d, %d]",
            x, y-distance, x, y+distance,
            x-distance, y, x+distance, y);
    
    (this->*func)(p_config, x, y, x0, x1, y0, y1, type, itype, 
                  iteration, generate);
    
    return(1);
  }
  return(0);
}


/*
  SQUARE_TYPE:

  *-------*
  | 1 | 2 |
  |---o---|
  | 2 | 1 |
  *-------*
*/

// Emit rectangle, width and height are inclusive
int surface_height::height_rectangle_emit(MID_POINT_CONFIG *p_config,
                                          int x, int y, int width, int height,
                                          int iteration,
                                          SQUARE_TYPE type,
                                          HEIGHT_WRITE_FUNC func)
{   
  int iteration_target = p_config->iteration_target;
  int pixels = 0;
  
  int max_distance = p_config->pixel_distance;
  if(max_distance < 0) {
    max_distance = 0;
  }
  
  // 3x3 and bigger
  if(width >= max_distance && height >= max_distance) {
    
    tpos  mdx = width/2,
          mdy = height/2;
          
    int   mdx_correction = (width&0x1)  ? mdx+1 : mdx;
    int   mdy_correction = (height&0x1) ? mdy+1 : mdy;
    
    tpos  mx = x + mdx,
          my = y + mdy;
            
    tpos  dist = MIN(mdx,mdy);

    /*
      Generate corner points
    
      *-------*
      |       |
      |       |
      |       |
      *-------*
    */
    
    // Generate only when those points are undefined
    if(!p_config->height_overwrite && 
        iteration == 0 && iteration_target == iteration)
    {
      float height_current = p_config->border_start_height;
      float height_base = p_config->border_start_height;
      
      if(p_config->border_start_height == FLOAT_UNDEFINED) {
        height_current = height_generate(p_config, x, y, height_base,
                                         iteration);
      }
      height_set(x,y, height_current);
      pixels++;

      if(p_config->border_start_height == FLOAT_UNDEFINED) {
        height_current = height_generate(p_config, x+width, y, height_base,
                                         iteration);
      }
      height_set(x+width,y, height_current);
      pixels++;
      
      if(p_config->border_start_height == FLOAT_UNDEFINED) {
        height_current = height_generate(p_config, x, y+height, height_base,
                                         iteration);
      }
      height_set(x,y+height, height_current);
      pixels++;
      
      if(p_config->border_start_height == FLOAT_UNDEFINED) {
        height_current = height_generate(p_config, x+width, y+height,
                                         height_base, iteration);
      }
      height_set(x+width,y+height, height_current);
      pixels++;
    }
    
    /*
      Generate center of the rectangle
    
      *-------*
      |   |   |
      |---o---|
      |   |   |
      *-------*
    
    */
    if(iteration == iteration_target) {

      // Always overwrite this one -> it clears artifacts generated by 
      // cross-interpolator
      if(1) {
        // Interpolate midle of the given rectangle
        float h0 = height_get(x,y);
        float h1 = height_get(x+width,y+height);
        float h2 = height_get(x+width,y);
        float h3 = height_get(x,y+height);
        pdebug(DEBUG_HEIGHT,"C1: [%d, %d], [%d, %d], [%d, %d], [%d, %d]",
                x,y, x+width,y+height, x+width,y, x,y+height);
        
        SQUARE_INTERPOLATION_TYPE itype = p_config->interpolation_get(MIN(width,height));
        bool generate = p_config->generation_get(MIN(width,height));
        
        (this->*func)(p_config, mx, my, h0, h1, h2, h3, type, itype, iteration, generate);
        pixels++;
      }
      
      return(pixels);
    }
    
    iteration++;
      
    /*
      Generate 
    
      *---2---*
      |   |   |
      1---*---3
      |   |   |
      *---4---*
    
      Types:
    
      *---1---*
      |   |   |
      2---*---2
      |   |   |
      *---1---* 
    */
    if(iteration == iteration_target) {
      SQUARE_INTERPOLATION_TYPE itype = p_config->interpolation_get(MIN(width,height));
      bool generate = p_config->generation_get(MIN(width,height));
      
      // 1
      pixels += height_interate_cross(p_config, x,  my, dist, iteration,
                                      SQUARE_TYPE_2, itype, generate, func);
      // 2    
      pixels += height_interate_cross(p_config, mx,  y, dist, iteration,
                                      SQUARE_TYPE_1, itype, generate, func);
      // 3    
      pixels += height_interate_cross(p_config, x+width, my, dist, iteration,
                                      SQUARE_TYPE_2, itype, generate, func);
      // 4
      pixels += height_interate_cross(p_config, mx, y+height, dist, iteration,
                                      SQUARE_TYPE_1, itype, generate, func);      
      return(pixels);
    }
    
    iteration++;
    
    /*  
      Target areas:
    
      *-------*
      | 1 | 2 |
      |---*---|
      | 3 | 4 |
      *-------*
    
      Types:
    
      *-------*
      | 1 | 2 |
      |---*---|
      | 2 | 1 |
      *-------*
    */
    if(iteration <= iteration_target && mdx > 0 && mdy > 0) {
      // 1
      pixels += height_rectangle_emit(p_config, x,  y,  mdx, mdy, 
                                      iteration, SQUARE_TYPE_1, func);
      // 2
      pixels += height_rectangle_emit(p_config, mx, y,  mdx_correction, mdy,
                                      iteration, SQUARE_TYPE_2, func);
      // 3
      pixels += height_rectangle_emit(p_config, x,  my, mdx, mdy_correction,
                                      iteration, SQUARE_TYPE_2, func);
      // 4
      pixels += height_rectangle_emit(p_config, mx, my, mdx_correction, mdy_correction,
                                      iteration, SQUARE_TYPE_1, func);
    }
  }
  
  return(pixels);
}

void surface_height::generate_mid_point(MID_POINT_CONFIG *p_config,
                                        int x, int y, int width, int height)
{
  fill(x, y, width, height, HEIGHT_UNSET);
  
  // Generate the heightmap
  p_config->height_overwrite = FALSE;
  int j;
  for(j = 0; ; j++) {
    p_config->iteration_target = j;
    if(!height_rectangle_emit(p_config, x, y, width-1, height-1, 0,
                              SQUARE_TYPE_1, &surface_height::height_write))
      break;
  }

  // Filter back some patterns, if required
  if(p_config->pixel_filter_back) {
    p_config->height_overwrite = TRUE;
    for(j = 0; ; j++) {
      p_config->iteration_target = j;
      if(!height_rectangle_emit(p_config, x, y, width-1, height-1, 0,
                                SQUARE_TYPE_1, &surface_height::height_filter))
        break;
    }
  }

  // Fill missing pixels
  if(p_config->pixel_fill) {
    height_fill(x, y, width, height, surface_height_filter_interpolate2);
  }
  
  // Filter
  if(p_config->pixel_filter) {
    int i;
    for(i = 0; i < p_config->pixel_filter_num; i++) {
      height_filter(x, y, width, height, surface_height_filter_interpolate2);
    }
  }
  
  // Normalize the result
  height_normalize(x, y, width, height);
}

void surface_height::generate_mid_point(MID_POINT_CONFIG *p_config)
{    
  int width = width_get();
  int height = height_get();
  
  generate_mid_point(p_config, 0, 0, width, height);
}


void surface_height::generate_perlin(PERLIN_CONFIG *p_config,
                                     int sx, int sy, int width, int height)
{
  int x,y;
  for(y = sy; y < height; y++) {
    for(x = sx; x < width; x++) {
      height_set(x,y,p_config->perlin_get(x, y));
    }
  }
  
  // Normalize the result
  height_normalize(sx, sy, width, height);
}

void surface_height::generate_perlin(PERLIN_CONFIG *p_config)
{
  int width = width_get();
  int height = height_get();
  
  generate_perlin(p_config, 0, 0, width, height);
}
