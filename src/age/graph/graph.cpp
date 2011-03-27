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

/*
  2D Graphics library
*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <ctype.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "age.h"

// -------------------------------------------------------
//   the graph 2d store class
// -------------------------------------------------------
void graph::check(void)
{  
  int i;

  SURFACE *p_srf;
  p_srf = p_screen->surf_get();

  pprintf("Screen sprite %p, p_surf %p, HW surf %p", p_screen,
         p_srf, p_srf->surf_get());

  pprintf("Stored surfaces:");
  SURFACE *p_srlist = store.get_surface_interface();
  
  for(i = 0; i < store.get_surface_num(); i++) {
    p_srf = p_srlist+i;
    pprintf("%d Surface %p, HW surface %p, used %d", i, p_srf, p_srf->surf_get(), p_srf->used_get());
  }

  pprintf("Stored sprites:");
  SPRITE *p_splist = store.sprite_interface_get();
  SPRITE *p_sp;  
  
  for(i = 0; i < store.sprite_num_get(); i++) {
    p_sp = p_splist+i;
    if(p_sp->surf_get()) {
      RECT *p_rec = p_sp->rect_get();
      pprintf("%d Sprite %p, Surface %p, flag %d, rec = {%d, %d, %d, %d}", 
      i, p_sp, p_sp->surf_get(), p_sp->flag_get(), p_rec->x, p_rec->y,
      p_rec->w, p_rec->h);
    }
  }
  pprintf("--- ALL ---");
}

// -------------------------------------------------------
//   the font table interface
// -------------------------------------------------------

bool font_lookup_table::load(char *p_file)
{  
  FFILE f(surface::graphics_dir_get(),p_file,"r");
  
  memset(position,0,sizeof(position));
  
  int pos = 0;
  char line[10];
  while(fgets(line,10,f)) {
    position[toupper(line[0])] = position[tolower(line[0])] = pos;
    pos++;
  }

  fclose(f);
  return(TRUE);
}

// -------------------------------------------------------
//   the font_info interface
// -------------------------------------------------------
GRAPH * font_info::p_grf = NULL;

font_info::font_info(void)
{
  font_sprite_first = font_sprite_num = 0;
}

font_info::~font_info(void)
{
  p_grf->sprite_delete(font_sprite_first, font_sprite_num);
  font_sprite_first = font_sprite_num = 0;
}

bool font_info::load(int font_index, int first, int num)
{
  assert(!font_sprite_num && font_index < FONT_NUM);

  char  tmp[2000];
  sprintf(tmp, "font%d.spr", font_index);
  
  int i = p_grf->sprite_insert(tmp, first, num, NULL);
  if(i != num)
    return(FALSE);
  
  font_sprite_first = first;
  font_sprite_num = num;
  
  return(ftable.load(tail_change(tmp,".tab")));
}

void font_info::free(void)
{ 
  p_grf->sprite_delete(font_sprite_first, font_sprite_num);
}
// -------------------------------------------------------
//   the font_info interface
// -------------------------------------------------------
font::font(GRAPH *p_graph)
{
  memset(this,0,sizeof(*this));

  font_info::p_grf = p_graph;
  align = LEFT;
}

font::~font(void)
{  
}

void font::print(char *p_string, RECT *p_res, int lines)
{
  if(!p_string)
    return;

  char *p_start = sstrdup(p_string);
  char *p_tmp = p_start;
  char *p_nl;

  FONT_INFO *p_font = finfo+font_selected;

  if(p_res) {
    p_res->x = ax;
    p_res->y = ay;
    p_res->w = p_res->h = 0;
  }

  while(*p_tmp) {
    p_nl = strchr(p_tmp, '\n');
    if(p_nl)
      *p_nl = '\0';
    
    tpos width_string = p_font->width_get(p_tmp);
    tpos height_string = p_font->height_get(p_tmp);
    tpos width_screen = p_font->screen_width_get();
    tpos px = ax,
         py = ay;
    
    switch(align) {
      case LEFT:
      case LEFT_NO_ARROW:
        break;
      case CENTER:
      case CENTER_NO_ARROW:
        px = (width_screen - width_string) / 2;
        break;
      case RIGHT:
      case RIGHT_NO_ARROW:
        px -= width_string;
        break;
    }

    if(p_res) {
      //pprintf("print - [%d %d] -> [%d %d]",px, py, width_string, height_string);
      rect_adjust(p_res, px, py, width_string, height_string);
      //pprintf("print - [%d %d] -> [%d %d]",p_res->x,p_res->y,p_res->w,p_res->h);
    }
  
    while(*p_tmp) {
      px += p_font->print(*p_tmp,px,py);
      p_tmp++;
    }
 
    if(p_nl) {
      new_line();
      p_tmp = p_nl+1;
    }
  
    if(lines > 0) {
      lines--;
      if(!lines)
        break;
    }
  }

  ::free(p_start);
}

void font::print(RECT *p_res, tpos x, tpos y, char *p_text,...)
{
  char      text[2000];
  va_list   arguments;  

  if(!p_text)
    return;

  va_start(arguments,p_text);
  vsnprintf(text,2000,p_text,arguments);
  va_end(arguments);
  
  start_set(x,y);
  print(text,p_res);
}

void font::print(RECT *p_res, tpos x, tpos y, int lines, char *p_text,...)
{
  char      text[2000];
  va_list   arguments;  

  if(!p_text)
    return;
  
  va_start(arguments,p_text);
  vsnprintf(text,2000,p_text,arguments);
  va_end(arguments);
  
  start_set(x,y);
  print(text,p_res,lines);
}

void rect_adjust(RECT *p_r, tpos x, tpos y, tpos w, tpos h)
{
  assert(p_r);

  if(x < p_r->x) {
    p_r->w += p_r->x - x;
    p_r->x = x;
  }
  if(y < p_r->y) {
    p_r->h += p_r->y - y;
    p_r->y = y;
  }
  if(p_r->x + p_r->w < x + w) {
    p_r->w = x + w - p_r->x;
  }
  if(p_r->y + p_r->h < y + h) {
    p_r->h = y + h - p_r->y;
  }
}

void rect_adjust(RECT *p_r, RECT *p_s)
{
  rect_adjust(p_r, p_s->x, p_s->y, p_s->w, p_s->h);
}
