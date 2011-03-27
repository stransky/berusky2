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

// Age
#include "age.h"

int histogram_sector_info::color_check(int r, int g, int b)
{
  int r_min = COLOR_FROM_CUBE_DIMENSION(r);
  int g_min = COLOR_FROM_CUBE_DIMENSION(g);
  int b_min = COLOR_FROM_CUBE_DIMENSION(b);
  
  int r_max = r_min + COLOR_CUBE_SIZE;
  int g_max = g_min + COLOR_CUBE_SIZE;
  int b_max = b_min + COLOR_CUBE_SIZE;  
  
  int i;
  for(i = 0; i < num; i++) {
    RGBB color = p_start[i].color;
    assert(INSIDE_RANGE(r_min, color.r, r_max));
    assert(INSIDE_RANGE(g_min, color.g, g_max));
    assert(INSIDE_RANGE(b_min, color.b, b_max));
  }
  
  return(num);
}

bool histogram_sector_info::color_get(RGBB color, RGBB &color_best, float &dist_best)
{ 
  if(!p_start || !num)
    return(FALSE);
  
  color_best = RGBB(0);
  dist_best = FLT_MAX;
  
  HISTOGRAM_COLOR *p_current = p_start;
  int i;
  for(i = 0, p_current = p_start;
      i < num;
      i++, p_current++) 
  {
    if(p_current->color == color) {
      color_best = p_current->color;
      dist_best = 0;
      return(TRUE);
    }
    
    RGBB tmp = p_current->color - color;
    float size = VECT3DI(tmp).size();
    if(size < dist_best) {
      color_best = p_current->color;
      dist_best = size;
    }
  }
    
  return(dist_best != FLT_MAX);
}

void histogram_sector_info::set(HISTOGRAM_COLOR *p_start, int num)
{
  assert(this->p_start == NULL && this->num == 0);
  this->p_start = p_start;
  this->num = num;
}

int histogram_cube_index_get(RGBB color)
{
  int r = COLOR_TO_CUBE_DIMENSION(color.r);
  int g = COLOR_TO_CUBE_DIMENSION(color.g);
  int b = COLOR_TO_CUBE_DIMENSION(color.b);
  
  return(r*(HISTOGRAM_CUBE_DIMENSION)*(HISTOGRAM_CUBE_DIMENSION) + 
         g*(HISTOGRAM_CUBE_DIMENSION) + 
         b);
}

int histogram_cube_index_sort(const void *s1, const void *s2)
{
  HISTOGRAM_COLOR *p_s1 = (HISTOGRAM_COLOR *)s1;
  HISTOGRAM_COLOR *p_s2 = (HISTOGRAM_COLOR *)s2;
  
  int i1 = histogram_cube_index_get(p_s1->color);
  int i2 = histogram_cube_index_get(p_s2->color);
  
  return(i1 - i2);
}

bool histogram_cube::color_get(RGBB color, int size, RGBB &color_best)
{
  int center_r = COLOR_TO_CUBE_DIMENSION(color.r);
  int center_g = COLOR_TO_CUBE_DIMENSION(color.g);
  int center_b = COLOR_TO_CUBE_DIMENSION(color.b);
  
  int r_min = center_r-size,
      r_max = center_r+size;
  int g_min = center_g-size,
      g_max = center_g+size;
  int b_min = center_b-size,
      b_max = center_b+size;
  
  int r,g,b;    
  float dist_best = FLT_MAX;
  for(r = r_min; r <= r_max; r++) {
    for(g = g_min; g <= g_max; g++) {
      for(b = b_min; b <= b_max; b++) {
        if(color_valid(r_min, r_max, g_min, g_max, b_min, b_max, r, g, b)) {
          RGBB  cl;
          float dist;
          if(cube[r][g][b].color_get(color, cl, dist)) {
            if(dist < dist_best) {
              dist_best = dist;
              color_best = cl;
            }
          }
        }
      }
    }
  }    
  return(dist_best != FLT_MAX);
}

RGBB histogram_cube::color_get(RGBB color)
{
  RGBB color_best(0);
  
  int i;
  for(i = 0; i < HISTOGRAM_CUBE_DIMENSION; i++) {
    if(color_get(color, i, color_best)) {      
      return(color_best);
    }
  }
  
  pprintf("Cube hit failed for [%d, %d, %d]", color.r, color.g, color.g);
  
  // It should not happen...
  assert(0);
  return(color_best);
}

int histogram_cube::color_check(void)
{    
  int r,g,b,num = 0;
  for(r = 0; r < HISTOGRAM_CUBE_DIMENSION; r++) {
    for(g = 0; g < HISTOGRAM_CUBE_DIMENSION; g++) {
      for(b = 0; b < HISTOGRAM_CUBE_DIMENSION; b++) {        
        num += cube[r][g][b].color_check(r,g,b);
      }
    }
  }
  return(num);
}

histogram_cube::histogram_cube(HISTOGRAM_COLOR *p_hist, int hist_num)
{
  memset(cube,0,sizeof(cube));
  
  // Sort out the histogram data regards to its position
  qsort(p_hist, hist_num, sizeof(p_hist[0]), histogram_cube_index_sort);
        
  // Fill in the cube
  int index_last = histogram_cube_index_get(p_hist[0].color);;
  int start = 0;
  int i, color_num;
  
  for(i = 1, color_num = 1; i < hist_num+1; i++, color_num++) {
    int index_current = histogram_cube_index_get(p_hist[i].color);
    
    if(index_current != index_last || i == hist_num) {
      index_last = index_current;
      
      RGBB color = p_hist[start].color;
      
      int r = COLOR_TO_CUBE_DIMENSION(color.r);
      int g = COLOR_TO_CUBE_DIMENSION(color.g);
      int b = COLOR_TO_CUBE_DIMENSION(color.b);
    
      HISTOGRAM_SECTOR_INFO *p_cell = &(cube[r][g][b]);
      p_cell->set(p_hist+start, color_num);
              
      p_cell->color_check(r,g,b);
      
      start = i;
      color_num = 0;
    }    
  }
    
  int n = hist_num - color_check();
  assert(n == 0);
}

void histogram_table::cube_create(void)
{
  if(p_hist && hist_num > HISTOGRAM_CUBE_TRESHOLD) {
    cube_remove();    
    p_cube = new HISTOGRAM_CUBE(p_hist,hist_num);    
  }  
}

void histogram_table::cube_remove(void)
{
  if(p_cube) {
    delete p_cube;
    p_cube = NULL;
  }
}

RGBB histogram_table::color_get(RGBB color)
{  
  RGBB  best(0);
  
  if(p_cube) {
    best = p_cube->color_get(color);
  }
  else {  
    float dist_min = FLT_MAX;
    HISTOGRAM_COLOR *p_current;
    int              i;  
    for(i = 0, p_current = p_hist; i < hist_num; i++, p_current++) {
      if(p_current->color == color) {
        return(color);
      }
      RGBB tmp = p_current->color - color;
      float size = VECT3DI(tmp).size();
      if(size < dist_min) {
        best = p_current->color;
        dist_min = size;
      }
    }
  }
    
  return(best);
}

bool histogram_table::index_create(void)
{
  index_num = (hist_num < HISTOGRAM_INDEX_TABLE_SIZE) ? 
              HISTOGRAM_INDEX_TABLE_SIZE_SMALL : 
              HISTOGRAM_INDEX_TABLE_SIZE;
    
  if(index_num < hist_num) {
    // histogram is too small for index table - don't use it
    index_num = 0;
  }
  
  if(!index_num)
    return(TRUE);
  
  assert(index_num < hist_num);
  
  float step = (1.0f)/index_num;
   
  float border_lower =-step;
  float border_upper = 0;
  
  float current;
  int hist_index,
      index_index;
  for(hist_index = 0, index_index = 0, current = 0.0f;
      hist_index < hist_num; 
      current += p_hist[hist_index++].probability) 
  {
    if(border_lower <= current && current < border_upper) {
      // in range -> continue
      continue;
    }
    
    // move to next range
    if(current >= border_upper) {
      if(index_index >= index_num) {
        break;
      }
      
      border_lower += step;
      border_upper += step;
      
      // Save first histogram in this range
      index[index_index].index = hist_index;
      index[index_index].current = current;
      index_index++;
    }
  }
  
  assert(index_index == index_num);
  
  return(TRUE);
}

RGBB histogram_table::color_get(float position)
{
  if(position >= 1) {
    return(p_hist[hist_num-1].color);
  }
  if(position <= 0) {
    return(p_hist[0].color);
  }

  float current;
  int index_start;
  
  if(index_num == 0) {
    current = 0;
    index_start = 0;
  }
  else {  
    int hash = floorf(position*index_num);
    assert(hash < index_num);
    current = index[hash].current;
    if(current > position) {
      hash--;
      current = index[hash].current;
      assert(current <= position);
    }      
    index_start = index[hash].index;
  }
    
  int i;
  for(i = index_start; i < hist_num; i++) {
    if(current <= position && position < current+p_hist[i].probability) {
      return(p_hist[i].color);
    }
    current += p_hist[i].probability;
  }
    
  return(p_hist[hist_num-1].color);
}

histogram_table::histogram_table(void)
{
  p_hist = NULL;
  hist_num = 0;
  index_num = 0;
  p_cube = NULL;
}

histogram_table::histogram_table(SURFACE *p_surf)
{
  p_cube = NULL;
  p_hist = p_surf->histogram_get(&hist_num);  
  cube_create();
}

histogram_table::histogram_table(HISTOGRAM_COLOR *p_histogram, int histogram_num)
{
  p_cube = NULL;
  p_hist = p_histogram;
  hist_num = histogram_num;
  cube_create();
}
  
histogram_table::~histogram_table(void)
{
  cube_remove();
}

HISTOGRAM * histogram_new(SURFACE *p_surf)
{
  int hist_num;
  HISTOGRAM_COLOR *p_table = p_surf->histogram_get(&hist_num);
  HISTOGRAM *p_hist = NULL;
  
  // Only table for now
  p_hist = new HISTOGRAM_TABLE(p_table, hist_num);
  return(p_hist);
}

int histogram_sort_intensity(const void *s1, const void *s2);
int histogram_sort_color(const void *s1, const void *s2);
int histogram_sort_color_delta(const void *s1, const void *s2);

// R,G,B order
int histogram_sort_color(const void *s1, const void *s2)
{
  HISTOGRAM_COLOR *p_s1 = (HISTOGRAM_COLOR *)s1;
  HISTOGRAM_COLOR *p_s2 = (HISTOGRAM_COLOR *)s2;  
  
  if(p_s1->color.r != p_s2->color.r)
    return((int)(p_s1->color.r) - (int)(p_s2->color.r));
  if(p_s1->color.g != p_s2->color.g)
    return((int)(p_s1->color.g) - (int)(p_s2->color.g));

  return((int)(p_s1->color.b) - (int)(p_s2->color.b));
}

/*
static int histogram_sort_num(const void *s1, const void *s2)
{
  HISTOGRAM_COLOR *p_s1 = (HISTOGRAM_COLOR *)s1;
  HISTOGRAM_COLOR *p_s2 = (HISTOGRAM_COLOR *)s2;  
  
  RGBF c1(p_s1->color);
  RGBF c2(p_s2->color);

  float I1 = 0.299f*c1.r + 0.587f*c1.g + 0.114f*c1.b;
  float I2 = 0.299f*c2.r + 0.587f*c2.g + 0.114f*c2.b;
    
  return(int((I1-I2)*100));
}
*/

/*
Color mapping:
  - 
*/

#define C_RANGE 0.30f
#define R_MIN   0
#define G_MIN   0.30f
#define B_MIN   0.60f
#define O_MIN   0.90f

// calculates the color index
float color_index(int r, int g, int b)
{
  if(r > g && r > b) {
    return(R_MIN + color_intensity(r,g,b)*C_RANGE);
  }
  if(g > r && g > b) {
    return(G_MIN + color_intensity(r,g,b)*C_RANGE);
  }
  if(b > r && b > g) {
    return(B_MIN + color_intensity(r,g,b)*C_RANGE);
  }
  return(O_MIN + color_intensity(r,g,b)*0.1f);
}

/*
  int r1_gain = MAX(p_s1->color.r-p_s1->color.g, p_s1->color.r-p_s1->color.b);
  int g1_gain = MAX(p_s1->color.g-p_s1->color.r, p_s1->color.g-p_s1->color.b);
  int b1_gain = MAX(p_s1->color.b-p_s1->color.r, p_s1->color.b-p_s1->color.g);
  
  int r2_gain = MAX(p_s2->color.r-p_s2->color.g, p_s2->color.r-p_s2->color.b);
  int g2_gain = MAX(p_s2->color.g-p_s2->color.r, p_s2->color.g-p_s2->color.b);
  int b2_gain = MAX(p_s2->color.b-p_s2->color.r, p_s2->color.b-p_s2->color.g);
*/

// ---------------------------
// What is the dominant color?
// ---------------------------
// r - g - b order
int histogram_sort_color_delta(const void *s1, const void *s2)
{
  HISTOGRAM_COLOR *p_s1 = (HISTOGRAM_COLOR *)s1;
  HISTOGRAM_COLOR *p_s2 = (HISTOGRAM_COLOR *)s2;
  
  float i1 = color_index(p_s1->color.r, p_s1->color.g, p_s1->color.b);
  float i2 = color_index(p_s2->color.r, p_s2->color.g, p_s2->color.b);  
  
  return((i1-i2)*INT_MAX);
}

int histogram_sort_intensity(const void *s1, const void *s2)
{
  HISTOGRAM_COLOR *p_s1 = (HISTOGRAM_COLOR *)s1;
  HISTOGRAM_COLOR *p_s2 = (HISTOGRAM_COLOR *)s2;  
  
  RGBF c1(p_s1->color);
  RGBF c2(p_s2->color);

  float I1 = color_intensity(c1.r,c1.g,c1.b);
  float I2 = color_intensity(c2.r,c2.g,c2.b);
  
  int intensity = (I1-I2)*1000;
  
  return(intensity);
}

int histogram_sort_hue(const void *sr1, const void *sr2)
{
  HISTOGRAM_COLOR *p_s1 = (HISTOGRAM_COLOR *)sr1;
  HISTOGRAM_COLOR *p_s2 = (HISTOGRAM_COLOR *)sr2;
  
  RGBF c1(p_s1->color);
  RGBF c2(p_s2->color);
  
  float h1,s1,v1;
  float h2,s2,v2;
  
  rgb_to_hsv(c1.r, c1.g, c1.b, h1, s1, v1);
  rgb_to_hsv(c2.r, c2.g, c2.b, h2, s2, v2);
  
  int diff = (h1-h2)*INT_MAX;
  return(diff);
}

HISTOGRAM_COLOR * surface::histogram_get(int *p_num)
{
  if(surface_format != SURFACE_TEXTURE) {
    *p_num = 0;
    return(NULL);
  }
  
  tpos width = width_get();
  tpos height = height_get();
  
  assert(width > 0 && height > 0);
  
  tpos pixel_num = width*height;
  HISTOGRAM_COLOR *p_list = (HISTOGRAM_COLOR *)mmalloc(sizeof(HISTOGRAM_COLOR)*pixel_num);
  tpos x,y;
  
  int i = 0;
  for(y = 0; y < height; y++) {
    for(x = 0; x < width; x++) {
      tcolor color = *(tcolor *)pixels_get(x, y);
      p_list[i].color = RGBB(color);
      i++;
    }
  }
  
  // Sort by color
  qsort(p_list, pixel_num, sizeof(HISTOGRAM_COLOR), histogram_sort_color);
  
  HISTOGRAM_COLOR *p_final = (HISTOGRAM_COLOR *)mmalloc(sizeof(HISTOGRAM_COLOR)*pixel_num);
  
  int index = 0;
  
  p_final[index] = p_list[0];
  p_final[index].count = 1;
  
  for(i = 1; i < pixel_num; i++) {
    if(p_final[index].color != p_list[i].color) {
      // Move current index to another one
      index++;
      p_final[index].color = p_list[i].color;
    }
    p_final[index].count++;
  }
  
  ffree(p_list);
  
  // index+1 is number of unique colors
  int final_num = *p_num = index+1;
  
  pprintf("Histogram contains %d colors...", final_num);
  
  // Sort by count
  qsort(p_final, final_num, sizeof(HISTOGRAM_COLOR), histogram_sort_hue);
  
  for(i = 0; i < final_num; i++) {
    p_final[i].probability = (float)p_final[i].count / pixel_num;
  }
  
  return(p_final);
}

surface::surface(SURFACE_CLASS sclass, PIXEL_FORMAT format)
: surface_class(sclass),
  pixel_format(format),
  surface_format(SURFACE_TEXTURE),
  used(0)
{
}

surface::~surface(void)
{
  clear();
}
