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

#include <stdlib.h>
#include <math.h>
#include "generator_random.h"

/*
  Random number generator    
  range is 0...1
*/
float random_generator::generator_rand(void)
{
  return((float)rand()/(float)RAND_MAX);
}

/*
  Random number generator    
  center is    0.0
  range is  +- 1.0
*/
float random_generator::generator_rand_0(void)
{
  return(generator_rand()*2.0f-1.0f);
}

/*
  Gauss random number generator
  
  center is    0.5
  range is  +- 0.5
*/
float random_generator::generator_gauss(void)
{ 
  #define ITERATIONS 10
  float sum = 0;
  
  int i;
  for(i = 0; i < ITERATIONS; i++)
    sum += (float)rand() / (float)RAND_MAX;
  
  return(sum / ITERATIONS);
}

/*
  Gauss random number generator
  
  center is    0.0
  range is  +- 1.0
*/
float random_generator::generator_gauss_0(void)
{ 
  return(2.0f*(generator_gauss() - 0.5f));
}

/*
  Gauss random number generator
  
  center (and maximum) is 1.0
  range is                1.0 - 0.0
*/
float random_generator::generator_gauss_1(void)
{
  return(1.0f - fabs(generator_gauss_0()));
}

float random_generator::generate_scattering(RANDOM_GENERATOR_TYPE generator_type)
{
  // Calculate a scattering
  float scattering = 0.0f;
  
  switch(generator_type) {
    case GENERATOR_RAND:
      scattering = random_generator::generator_rand_0();
      break;
    case GENERATOR_GAUSS:
      scattering = random_generator::generator_gauss_0();
      break;
  }
  
  return(scattering);
}

/*
 * Random generator fractal
 */
random_generator_fractal::random_generator_fractal(void)
{
  generator_center = 0.0f;
  generator_delta = 1.0f;
  generator_hurst = 0.6f;
}

float random_generator_fractal::generate_height(int iteration)
{  
  float d = generator_delta/(powf(2.0f, 2.0f*generator_hurst*iteration));
  float height_change = generator_gauss()*d - d / 2.0f + generator_center;
  return(height_change);
}

float random_generator_fractal::generate_height(int iteration, float hurst, float center, float delta)
{  
  float d = delta/(powf(2.0f, 2.0f*hurst*iteration));
  float height_change = generator_gauss()*d - d / 2.0f + center;
/*  
  if(iteration == 0)
    return(1);
  else
    return(0);
*/  
  return(height_change);
}
