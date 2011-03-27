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

#ifndef __ALPHA_STAGE_H__
#define __ALPHA_STAGE_H__

#define STATE_BLOCK_TRANSPARENT   50
#define STAGE_ALPHA_FUNC_NUM      5

#define STAGE_ALPHA_MASKA         4 

typedef enum {
  
  ALPHA_OFF         = 0,
  ALPHA_PATCH       = 1,
  ALPHA_SMOKE       = 2,
  ALPHA_TRANSPARENT = 3,
  ALPHA_TEST        = 4
  
} ALPHA_TYPE;

// blending settings
typedef struct _STATE_ALPHA_BLOCK {

  char   name[MAX_NAME];
  int    alpha_transparent;
  int    func;

} STATE_ALPHA_BLOCK;

typedef class alpha_func {

  // Table of blending functions
  STATE_ALPHA_BLOCK state_block[STAGE_ALPHA_FUNC_NUM+STATE_BLOCK_TRANSPARENT+1];
  
  // Table of alpha functions
  void (alpha_func::*p_func[STAGE_ALPHA_FUNC_NUM])(void);

  int alpha_block;       // an active alpha block
  int alpha_test;        // alpha-test on-off
  int blending;          // blending on-off

private:  
  
  void blend_set_one_zero(void)
  {
    if(blending) {
      glBlendFunc(GL_ONE,GL_ZERO);
    }
  }
  
  void blend_get_default(GLenum *p_src, GLenum *p_dest)
  {
    *p_src = GL_ONE;
    *p_dest = GL_ZERO;
  }
  
  void blend_set(GLenum src, GLenum dest)
  {
    if(blending) {
      glBlendFunc(src,dest);
    }
  }
  
  void blend_off(bool force = FALSE)
  {
    if(blending || force) {
      blending = FALSE;
      glDisable(GL_BLEND);
    }
  }
  
  void blend_on(bool force = FALSE)
  {
    if(!blending || force) {
      blending = TRUE;
      glEnable(GL_BLEND);
    }
  }
  
  void blend_param(int stav, bool force = FALSE)
  {
    if(stav) {
      blend_on(force);
    }
    else {
      blend_off(force);
    }
  }

  void alpha_test_off(bool force = FALSE)
  {
    if(alpha_test || force) {
      alpha_test = FALSE;
      glDisable(GL_ALPHA_TEST);
    }
  }
  
  void alpha_test_on(bool force = FALSE)
  {
    if(!alpha_test || force) {
      alpha_test = TRUE;
      glEnable(GL_ALPHA_TEST);
    }
  }
  
private:
  
  void off(void);
  void patch(void);
  void smoke(void);
  void transparent(void);
  void test(void);

public:

  void set(ALPHA_TYPE alpha_function, bool force = FALSE)
  {
    if(alpha_block != alpha_function) {
      alpha_block = alpha_function;
      (this->*p_func[state_block[alpha_function].func])();
    }
  }

  void reset(bool force = FALSE)
  {
    alpha_block = ERROR;
    off();
  }

  alpha_func(void)
  {
    p_func[ALPHA_OFF] = &alpha_func::off;
    p_func[ALPHA_PATCH] = &alpha_func::patch;
    p_func[ALPHA_SMOKE] = &alpha_func::smoke;
    p_func[ALPHA_TRANSPARENT] = &alpha_func::transparent;
    p_func[ALPHA_TEST] = &alpha_func::test;

    strcpy(state_block[ALPHA_OFF].name, "Default");
    state_block[ALPHA_OFF].alpha_transparent = FALSE;
    state_block[ALPHA_OFF].func = 0;
    
    strcpy(state_block[ALPHA_PATCH].name, "Patch");
    state_block[ALPHA_PATCH].alpha_transparent = TRUE;
    state_block[ALPHA_PATCH].func = 1;
    
    strcpy(state_block[ALPHA_SMOKE].name, "Smoke");
    state_block[ALPHA_SMOKE].alpha_transparent = TRUE;
    state_block[ALPHA_SMOKE].func = 2;
    
    strcpy(state_block[ALPHA_TRANSPARENT].name, "Transparent");
    state_block[ALPHA_TRANSPARENT].alpha_transparent = TRUE;
    state_block[ALPHA_TRANSPARENT].func = 3;
    
    strcpy(state_block[ALPHA_TEST].name, "Mask");
    state_block[ALPHA_TEST].alpha_transparent = FALSE;
    state_block[ALPHA_TEST].func = 4;
  
    alpha_block = 0;
    alpha_test = 0;
    blending = 0;
  }  
  
} ALPHA_FUNC;


typedef class material_alpha {

public:
  
  static ALPHA_FUNC   alpha_functions;

private:

  ALPHA_TYPE          alpha_type;

public:  

  void set(void)
  {
    alpha_functions.set(alpha_type);
  }
  
public:

  void alpha_func_set(ALPHA_TYPE function)
  {
    alpha_type = function;
  }
  
  ALPHA_TYPE alpha_func_get(void)
  {
    return(alpha_type);
  }

public:
  
  material_alpha(void) 
  {    
    alpha_type = ALPHA_OFF;
  }
  
} MATERIAL_ALPHA;

#endif // __ALPHA_STAGE_H__
