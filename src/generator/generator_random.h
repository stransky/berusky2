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

#ifndef __GENERATOR_RANDOM_H__
#define __GENERATOR_RANDOM_H__

/*
 * Generator type
 */
typedef enum { 
 
  GENERATOR_GAUSS = 0,
  GENERATOR_RAND  = 1

} RANDOM_GENERATOR_TYPE;
  
/*
 * Generator class
 */
typedef class random_generator
{

public:
  
  /*
    Random number generator    
    range is 0...1
  */
  static float generator_rand(void);

  /*
    Random number generator    
    range is -1...1
  */
  static float generator_rand_0(void);

  /*
    Gauss random number generator

    center is    0.5
    range is  +- 0.5
  */
  static float generator_gauss(void);

  /*
    Gauss random number generator
    
    center is    0.0
    range is  +- 1.0
  */
  static float generator_gauss_0(void);
  
  /*
    Gauss random number generator
    
    center (and maximum) is 1.0
    range is                1.0 - 0.0
  */
  static float generator_gauss_1(void);
  
  /*
   * Generate scattering
   */  
  static float generate_scattering(RANDOM_GENERATOR_TYPE generator_type);
  
} RANDOM_GENERATOR;

/*
 * Random generator fractal
 */
typedef class random_generator_fractal
  : public random_generator
{
  /*
   * Fractal generator params
   */
public:
  /*
    <0,1>
    0 - maximax disertion
    1 - minimal disertion
  */
  float generator_hurst;

public:

  float generator_delta;
  float generator_center;
  
public:

  void  generator_range_set(float min, float max)
  {
    generator_delta = max - min;
    generator_center = min + generator_delta/2;
  }
  
  void  generator_center_set(float center, float delta)
  {
    generator_delta = delta;
    generator_center = center;
  }

  void  generator_hurst_set(float hurst)
  {
    generator_hurst = hurst;
  }

public:
  
  /*
   * Generate height
   */
  float generate_height(int iteration);
  static float generate_height(int iteration, float hurst, float center, float delta);
    
public:

  random_generator_fractal(void);
  
} RANDOM_GENERATOR_FRACTAL;

#endif // __GENERATOR_RANDOM_H__
