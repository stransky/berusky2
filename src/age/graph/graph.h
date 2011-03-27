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

  TODO:
  - remove free from descructor!! -> only one copy in the system!!


  - stand-alone versus in-list surfaces / sprites
  - reference counting/loading
  - some simple sprites with included surfaces?? (just one surface like texture)

  - surface list
  - sprite list
  - graf2d class
  - sprite class -> reference to surface list
  
*/

#ifndef  __GRAPH_H__
#define  __GRAPH_H__

#include <assert.h>
#include <limits.h>

void rect_adjust(RECT *p_r, tpos x, tpos y, tpos w, tpos h);
void rect_adjust(RECT *p_r, RECT *p_s);

// -------------------------------------------------------
//   the graph_2d store class
// -------------------------------------------------------

#define SURFACES 100
#define SPRITES  10000
#define RECT_NUM      (2*800)
#define RECT_NUM_ALERT 800

typedef class graph
{

private:

  int          redraw;

  SURFACE     *p_screen_surface;
  SPRITE      *p_screen;

  // Array of surfaces
  SPRITE_STORE store;

  // Update rectangles (max number is X_cells * Y_cells)
  RECT         rects[RECT_NUM];
  int          rect_last;
  bool         rect_whole;

public:
  
  static void graphics_dir_set(char *p_dir)
  {
    surface::graphics_dir_set(p_dir);
  }

  char * graphics_dir_get(void)
  {
    return(surface::graphics_dir_get());
  }

public:

  SPRITE_STORE * store_get(void)
  {
    return(&store);
  }

  SPRITE * sprite_get(spr_handle spr = 0)
  {    
    return(spr ? store.sprite_get(spr) : p_screen);
  }

  tpos sprite_get_width(spr_handle spr = 0)
  {    
    return(spr ? store.sprite_get_width(spr) : p_screen->get_width());
  }

  tpos sprite_get_height(spr_handle spr = 0)
  {
    return(spr ? store.sprite_get_height(spr) : p_screen->get_height());
  }

  tpos sprite_get_width_center(spr_handle spr)
  {
    assert(spr);    
    return((p_screen->get_width() - store.sprite_get_width(spr)) / 2);
  }

  tpos sprite_get_height_center(spr_handle spr)
  {
    assert(spr);    
    return((p_screen->get_height() - store.sprite_get_height(spr)) / 2);
  }

  spr_handle sprite_insert(char *p_file, spr_handle first, spr_handle num = 1, spr_handle *p_last = NULL)
  {
    return(store.sprite_insert(p_file,first,num,p_last));
  }

  spr_handle sprite_insert(SPRITE *p_spr, int num = 1, spr_handle first = INSERT_APPEND)
  {
    return(store.sprite_insert(p_spr,num,first));
  }

  spr_handle sprite_copy(spr_handle dst_handle, spr_handle src_handle, bool surface_copy = false)
  {
    return(store.sprite_copy(dst_handle, src_handle, surface_copy));
  }

  void sprite_delete(spr_handle handle, int num = 1)
  {
    store.sprite_delete(handle, num);
  }

  void fill(tcolor color, spr_handle dst = 0)
  {      
    SPRITE *p_dst = dst ? store.sprite_get(dst) : p_screen;
    if(p_dst)
       p_dst->fill(color);
  }

  void fill(tpos x, tpos y, tpos dx, tpos dy, tcolor color, spr_handle dst = 0)
  {        
    SPRITE *p_dst = dst ? store.sprite_get(dst) : p_screen;
    if(p_dst)
       p_dst->fill(x,y,dx,dy,color);
  }

  void fill(RECT *p_rect, tcolor color, spr_handle dst = 0)
  {
    fill(p_rect->x, p_rect->y, p_rect->w, p_rect->h, color, dst);
  }

  void fill(spr_handle dst = 0)
  {
    fill(0, 0, p_screen->get_width(), p_screen->get_height(), 0, dst);
  }

  void clear(spr_handle dst = 0)
  {
    fill(dst);
    if(dst == 0)
      redraw_add();
  }

  tcolor color_map(Uint8 r, Uint8 g, Uint8 b, spr_handle dst = 0)
  {
    SPRITE *p_dst = dst ? store.sprite_get(dst) : p_screen;
    return(p_dst->color_map(r,g,b));
  }

  void draw(spr_handle spr, tpos tx, tpos ty, spr_handle dst = 0)
  {
    SPRITE *p_spr = store.sprite_get(spr);
    SPRITE *p_dst = dst ? store.sprite_get(dst) : p_screen;
    if(p_spr && p_dst)
       p_spr->blit(p_dst,tx,ty);
  }

  void draw(spr_handle spr, tpos sx, tpos sy, tpos dx, tpos dy, 
            spr_handle dst, tpos tx, tpos ty)
  {
    SPRITE *p_spr = store.sprite_get(spr);
    SPRITE *p_dst = dst ? store.sprite_get(dst) : p_screen;
    if(p_spr && p_dst)
       p_spr->blit(sx, sy, dx, dy, p_dst, tx, ty);
  }

  SPRITE * screen_get(void)
  {
    return(p_screen);
  }
  
  void redraw_set(int red_) 
  {
    redraw = red_; 
  }

  int  redraw_get(void) 
  { 
    return redraw; 
  }

  void redraw_add(trec x, trec y, trec dx, trec dy)
  {
    assert(rect_last < RECT_NUM);
  
    RECT *p_tmp = rects+rect_last++;
  
    p_tmp->x = x;
    p_tmp->y = y;
  
    p_tmp->w = dx;
    p_tmp->h = dy;
  }

  void redraw_add(spr_handle spr, trec x, trec y)
  {
    assert(rect_last < RECT_NUM);
  
    RECT *p_tmp = rects+rect_last++;
  
    p_tmp->x = x;
    p_tmp->y = y;
  
    p_tmp->w = sprite_get_width(spr);
    p_tmp->h = sprite_get_height(spr);
  }

  void redraw_add(RECT *p_rect)
  {
    assert(rect_last < RECT_NUM);
    rects[rect_last++] = *p_rect;
  }

  void redraw_add(void)
  {
    rect_whole = TRUE;
  }

  void redraw_reset(void)
  {
    rect_last = 0;
    redraw = false;  
  }

  int flip(void)
  {
    if(rect_last || rect_whole) {
      if(rect_last > RECT_NUM_ALERT)
        pprintf("RECT_NUM_ALERT: %d slots.", rect_last);
      if(rect_whole) {
        SDL_UpdateRect(p_screen_surface->surf_get(), 0, 0, 0, 0);
        rect_whole = FALSE;
      } else {
        SDL_UpdateRects(p_screen_surface->surf_get(), rect_last, rects);
        redraw_reset();
      }
    }
    return (TRUE);
  }  

  void check(void);
  
  /* Graphics configuration settings
   */
public:
  
  int  graphics_flag;
  tpos graphics_width;
  tpos graphics_height;
  int  graphics_bpp;
  bool fullscreen;

public:
  
  SDL_Surface * create_screen(void)
  {
  
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
      fprintf(stderr, "unable to init SDL: %s", SDL_GetError());
      exit(0);
    }      
          
    int flag = graphics_flag|SDL_HWSURFACE;
      
    pprintf("Init video surface...\n");
    SDL_Surface *p_hwscreen = SDL_SetVideoMode(graphics_width, graphics_height, graphics_bpp, flag);
    
    if(!p_hwscreen) {
      fprintf (stderr, "Unable to set the video mode: %s", SDL_GetError());
      exit(0);
    }
    
    p_screen_surface = new SURFACE(p_hwscreen);
    p_screen = new SPRITE(p_screen_surface, SDL_SPRITE_SEPARATE_SURFACE, NULL);
  
    redraw_reset();
    rect_whole = FALSE;
  
    return(p_hwscreen);
  }

  graph(tpos dx, tpos dy, int screen_depth, int flag)
  : store(SURFACES, SPRITES), rect_last(0)
  {
    graphics_flag = flag;
    graphics_width = dx;
    graphics_height = dy;
    graphics_bpp = screen_depth;      
    fullscreen = flag|GRF_FULSCREEN;
  
    /* SDL init */
    pprintf("SDL Init...");
    SDL_Init(SDL_INIT_VIDEO);

     /* Create new screen */
    pprintf("Screen resolution %dx%d\nColor depth %d bits\nFullscreen = %d\n",
    graphics_width, graphics_height, graphics_bpp, fullscreen);
  
    create_screen();
  }

  ~graph(void)
  {
    SDL_Quit();
  }

} GRAPH;

// -------------------------------------------------------
//   Font interface
// -------------------------------------------------------

#define FONT_NUM                  3

typedef class font_lookup_table {

  int position[256];

public:
  bool load(char *p_file);
  int  lookup(int c)
  {
    assert(c >= 0 && c < 256);
    return(position[c]);
  }

} FONT_LOOKUP_TABLE;

/*
#define FALIGN_LEFT             0
#define FALIGN_CENTER           1
#define FALIGN_RIGHT            2

#define FALIGN_LEFT_NO_ARROW    3
#define FALIGN_CENTER_NO_ARROW  4
#define FALIGN_RIGHT_NO_ARROW   5
*/

typedef enum _ALIGNMENT {

  LEFT,
  CENTER,
  RIGHT,

  LEFT_NO_ARROW,
  CENTER_NO_ARROW,
  RIGHT_NO_ARROW

} ALIGNMENT;

#define DEFAULT_NEW_LINE_SHIFT  4

typedef class font_info {

  spr_handle        font_sprite_first;
  spr_handle        font_sprite_num;

  FONT_LOOKUP_TABLE ftable;

  int               extra_height;

public:

  static GRAPH  *p_grf;

public:
  
  font_info(void);
  ~font_info(void);

  bool load(int font_index, int first, int num);
  void free(void);

  int  width_get(char c)
  {
    return(p_grf->sprite_get_width(font_sprite_first+ftable.lookup(c)));
  }

  int  width_get(char *p_string)
  { 
    tpos width = 0;  
  
    while(*p_string) {
      width += width_get(*p_string++);
    }
  
    return(width);
  }

  int  height_get(char c = 'A')
  {
    return(p_grf->sprite_get_height(font_sprite_first+ftable.lookup(c)));
  }

  int  height_get(char *p_string)
  {
    return(height_get(p_string[0]));
  }

  int  height_get_new_line(char c)
  {
    return(p_grf->sprite_get_height(font_sprite_first+ftable.lookup(c))+extra_height);
  }
    
  int  height_get_new_line(char *p_string)
  {
    return(height_get_new_line(p_string[0]));
  }

  void height_set_new_line(int height = -1)
  {
    if(height == -1)
      extra_height = DEFAULT_NEW_LINE_SHIFT;
    else
      extra_height = height;
  }

  int  height_get_new_line(void)
  {
    return extra_height;
  }

  tpos print(int c, tpos px, tpos py)
  {
    spr_handle spr = font_sprite_first+ftable.lookup(c);
    p_grf->draw(spr, px, py);
    return(p_grf->sprite_get_width(spr));
  }

  tpos screen_width_get(void)
  {
    return(p_grf->sprite_get_width());
  }

  tpos screen_height_get(void)
  {
    return(p_grf->sprite_get_height());
  }

} FONT_INFO;

#define FONT_DEFAULT      0
#define FONT_SELECTED     1

typedef class font {

  int               font_selected;
  FONT_INFO         finfo[FONT_NUM];

  ALIGNMENT         align;

  // Start x,y
  tpos              sx,sy;
  // Actuall x,y
  tpos              ax,ay;

public:

  font(GRAPH *p_graph);
  ~font(void);

public:
  
  int width_get(char c)
  {
    return(finfo[font_selected].width_get(c));
  }
  
  int height_get(char c = 'A')
  {
    return(finfo[font_selected].height_get(c));
  }

  int height_get_new_line(char c)
  {
    return(finfo[font_selected].height_get_new_line(c));
  }

  void extra_height_set(int height = -1, int font_index = -1)
  {
    if(font_index == -1)
      font_index = font_selected;
    finfo[font_index].height_set_new_line(height);
  }

  int  extra_height_get(int font_index = -1)
  {
    if(font_index == -1)
      font_index = font_selected;
    return(finfo[font_index].height_get_new_line());
  }

  int width_get(char *p_string)
  { 
    return(finfo[font_selected].width_get(p_string));
  }
  
  int height_get(char *p_string)
  { 
    return(finfo[font_selected].height_get(p_string));
  }

  int height_get_new_line(char *p_string)
  { 
    return(finfo[font_selected].height_get_new_line(p_string));
  }
  
  void start_set(tpos x, tpos y)
  {
    ax = sx = x; ay = sy = y;
  }
  
  void alignment_set(ALIGNMENT al)
  {
    align = al;  
  }
  
  void new_line(void)
  {
    ax = sx; ay += height_get_new_line('A');
  }

  void select(int font_index)
  {
    assert(font_index >= 0 && font_index < FONT_NUM);
    font_selected = font_index;
  }
  
  bool load(int font_index, int first, int num)
  {
    return(finfo[font_index].load(font_index,first,num));
  }  

  void free(int font_index)
  {
    finfo[font_index].free();
  }

  void print(char *p_string, RECT *p_res = NULL, int lines = 0);
  void print(RECT *p_res, tpos x, tpos y, char *p_text,...);
  void print(RECT *p_res, tpos x, tpos y, int lines, char *p_text,...);  

} FONT;

#define COLOR_MAP(r,g,b)  (p_grf->color_map(r,g,b))

#endif //__GRAPH_H__
