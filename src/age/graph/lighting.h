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

#ifndef __LIGHTING_H__
#define __LIGHTING_H__

/*
 * Basic phong illumination model
 */
typedef class phong_lighting_intensity {

public:  
  
  // Light angle = (angle_min + I*angle_scale);
  float light_angle_min;  
  float light_angle_scale;

public:

  void light_intensity_set(float min, float scale)
  {
    light_angle_min = min;
    light_angle_scale = scale;
  }
  void light_intensity_range_set(float min, float max)
  {
    light_angle_min = min;
    light_angle_scale = max-min;
  }

  // Ignore back-faces
public:
  
  bool  light_bidirectional;

private:
  
  bool  light_range_active;
  float light_range;

  bool  light_fade_active;
  float light_fade_constant;
  float light_fade_linear;
  float light_fade_cubic;

public:

  // Calculate simple light intensity (w/o distance)
  float light_intensity_get(VECT *p_light_vector, VECT *p_normal)
  {
    float I = vect_dot_product(p_light_vector,p_normal);
    
    if(light_bidirectional)
      I = fabs(I);
    
    if(I < 0.0f)
      I = 0.0f;
          
    I = light_angle_min + I*light_angle_scale;
    
    if(I < 0)
      I = 0;    
    if(I > 1)
      I = 1;
    
    return(I);
  }

  // Calculate simple light intensity (with distance)
  float light_intensity_get(VECT *p_light_vector, VECT *p_light_position,
                            VECT *p_normal, VECT *p_normal_position)
  {
    float I = vect_dot_product(p_light_vector,p_normal);
    
    if(light_bidirectional)
      I = fabs(I);
    
    if(I < 0.0f)
      I = 0.0f;
    
    I = light_angle_min + I*light_angle_scale;
    
    if(I < 0)
      I = 0;
    if(I > 1)
      I = 1;
    
    if(light_range_active || light_fade_active) {
      float distance = point_dist(p_light_position,p_normal_position);
      
      if(light_range_active && distance > light_range)
        return(0.0f);
        
      if(light_fade_active) {
        I *= 1.0f/(light_fade_constant+
                   light_fade_linear*distance+
                   light_fade_cubic*distance*distance);
      }
    }
    
    return(I);
  }
  
public:
  
  phong_lighting_intensity(void);
  
} PHONG_LIGHTING_INTENSITY;


typedef struct phong_lighting_color
{
  RGBAF  phong_light_color_ambient;

  bool   phong_light_diffuse;
  RGBAF  phong_light_color_diffuse;

  bool   phong_light_specular;
  RGBAF  phong_light_color_specular;
  float  phong_light_color_specular_shine;

public:

  RGBAF * phong_lighting_calc(RGBAF *p_material_color, RGBAF *p_final_color, float Intensity)
  { 
    RGBAF tmp = *p_material_color;
    
    if(phong_light_diffuse) {
      tmp *= (phong_light_color_ambient + phong_light_color_diffuse*Intensity);
    }
    
    if(phong_light_specular) {
      // Shine parameters - which part of the range is exposed
      float specular_intensity = Intensity - (1.0f - phong_light_color_specular_shine);
      if(specular_intensity > 0) {
        specular_intensity *= (1.0f / phong_light_color_specular_shine);
        tmp += phong_light_color_specular*specular_intensity;
      }
    }
    
    *p_final_color = tmp;
    
    return(p_final_color);
  }
  
public:

  phong_lighting_color(void);
  
} PHONG_LIGHTING_COLOR;

typedef class phong_lighting : 
  public phong_lighting_intensity,
  public phong_lighting_color
{

public:
  
  RGBAF * phong_lighting_get(RGBAF *p_material_color, RGBAF *p_final_color,
                             VECT *p_light_vector, VECT *p_normal)
  {
    float I = light_intensity_get(p_light_vector, p_normal);    
    return(phong_lighting_calc(p_material_color, p_final_color, I));
  }
  
  RGBAF * phong_lighting_get(RGBAF *p_material_color, RGBAF *p_final_color, 
                             VECT *p_light_vector, VECT *p_light_position,
                             VECT *p_normal, VECT *p_normal_position)
  {
    float I = light_intensity_get(p_light_vector, p_light_position, p_normal, p_normal_position);
    return(phong_lighting_calc(p_material_color, p_final_color, I));
  }  
  
public:

  phong_lighting(void);
  
} PHONG_LIGHTING;


#endif // __LIGHTING_H__
