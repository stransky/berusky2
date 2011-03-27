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

/* Alpha-stage function
*/

// Constants and types
#include "age.h"


ALPHA_FUNC material_alpha::alpha_functions;

/*
   Alpha-functions
*/
void alpha_func::off(void)
{
  blend_off();
  alpha_test_off();
}

void alpha_func::patch(void)
{
  blend_on();
  alpha_test_off();  
  glBlendFunc(GL_ZERO,GL_SRC_COLOR);
}

void alpha_func::smoke(void)
{
  blend_on();
  alpha_test_off();
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
}

void alpha_func::transparent(void)
{
  blend_on();
  alpha_test_on();
  glAlphaFunc(GL_GREATER,0.004f);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void alpha_func::test(void)
{  
  blend_off();
  alpha_test_on();
  glAlphaFunc(GL_GREATER,0.5f);
}
