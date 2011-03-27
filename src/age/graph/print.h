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
 * Derived from:
 */
/*
 * This code was created by Jeff Molofee '99 
 * (ported to Linux/SDL by Ti Leggett '01)
 *
 * If you've found this code useful, please let me know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 * 
 * or for port-specific comments, questions, bugreports etc. 
 * email to leggett@eecs.tulane.edu
 */
  

#ifndef __GRF_PRINT_H__
#define __GRF_PRINT_H__

#define FONT_WIDTH  12
#define FONT_HEIGHT 18

typedef class grf_print {

  GLuint  base; /* Base Display List For The Font Set */
  
  RGBB    color_text;
  RGBB    color_back;
  
public:

  void font_color_set(RGBB color)
  {
    color_text = color;
  }
  
public:

  void font_create(void);
  void font_destroy(void);

public:

  int font_width_get(void)
  {
    return(FONT_WIDTH);
  }

  int font_height_get(void)
  {
    return(FONT_HEIGHT);
  }

  int string_width_get(char *p_string)
  {
    return(strlen(p_string)*font_width_get());
  }
  
  int string_height_get(char *p_string)
  {
    return(font_height_get());
  }
  
public:
  
  void print_line(RGBB color, int x, int y, const char *p_text);
  void print_line(int x, int y, const char *fmt);
  void print(int x, int y, const char *fmt,...);

public:
  
  grf_print(void)
  {
    base = 0;
    color_text.set(0xff,0xff,0xff);
  }
  
  ~grf_print(void)
  {    
  }
  
} GRF_PRINT;

/*
 * Graphics console
 */
typedef class grf_console : public grf_print {

  /*
   * Console origin
   */
  VECT2DI origin;
  
  /*
   * Console cursor
   */
  VECT2DI cursor;
  
private:  
  
  void cursor_reset(void)
  {
    cursor.set();
  }
    
  void cursor_new_line(void)
  {
    cursor.x = 0;
    cursor.y += font_height_get();
  }
  
  /*
   * Console text buffer
   */
private:

  char  **p_buffer;
  int     buffer_line_num;
  int     buffer_line_width;

private:

  int     buffer_line_first;
  int     buffer_line_last;

private:
  
  int   line_next(int line)
  {
    return((line+1) % buffer_line_num);
  }

public:

  void  clear(void)
  {
    buffer_line_first = buffer_line_last = 0;
  }
  
  bool  empty(void)
  {
    return(buffer_line_first == buffer_line_last);
  }
  
  bool  full(void)
  {
    return(line_next(buffer_line_last) == buffer_line_first);
  }

  void  scroll(void)
  {
    if(!empty()) {
      buffer_line_first = line_next(buffer_line_first);
    }
  }
  
  void  scroll_if_full(void)
  {
    if(full()) {
      buffer_line_first = line_next(buffer_line_first);
    }
  }

private:  
  
  void  print_to_buffer(char *fmt);
  
public:  

  void  print_line(char *fmt);
  void  print(const char *fmt,...);
  
public:
  
  void  draw(void);

public:
  
  void  origin_set(VECT2DI *p_origin)
  {
    if(p_origin) {
      origin.x = p_origin->x*font_width_get();
      origin.y = p_origin->y*font_height_get();
    }
    else
      origin.set();
  }

  void  origin_set(int x, int y)
  {
    origin.x = x*font_width_get();
    origin.y = y*font_height_get();
  }
  
public:

  grf_console(int text_width = 20, int text_lines = 10, VECT2DI *p_origin = NULL)
  {
    // set up position
    origin_set(p_origin);
    cursor.set();
    
    // Set up buffer
    assert(text_lines > 0);
    
    buffer_line_num = text_lines;
    buffer_line_width = text_width;
    
    buffer_line_last = 0;
    buffer_line_first = 0;
    
    p_buffer = (char **)mmalloc(sizeof(p_buffer[0])*buffer_line_num);
    
    int i;
    for(i = 0; i < buffer_line_num; i++)
      p_buffer[i] = (char *)mmalloc(sizeof(p_buffer[0][0])*(buffer_line_width+1));
  }
  
  ~grf_console(void)
  {    
    int i;
    for(i = 0; i < buffer_line_num; i++)
      ffree(p_buffer[i]);
    
    ffree(p_buffer);
  }
  
} GRF_CONSOLE;

typedef class grf_console_interface {

  GRF_CONSOLE console;
  
public:

  void  console_font_color_set(RGBB color)
  {
    console.font_color_set(color);
  }
  
public:  
  
  void  console_clear(void)
  {
    console.clear();
  }
  
public:

  void  console_print(const char *fmt,...);
  void  console_print(int x, int y, const char *fmt,...);
  void  console_print(RGBB color, int x, int y, const char *fmt,...);
  
public:
  
  void  console_draw(void)
  {     
    console.draw();
  }
  
public:
  
  void console_origin_set(VECT2DI *p_origin)
  {
    console.origin_set(p_origin);
  }  

  void console_origin_set(int x, int y)
  {
    console.origin_set(x, y);
  }  
  
public:
  
  void console_font_create(void)
  {
    console.font_create();
  }
  
  void console_font_destroy(void)
  {
    console.font_destroy();
  }
  
public:
  
  grf_console_interface(int text_width = 20, int text_lines = 10, VECT2DI *p_origin = NULL)
    : console(text_width,text_lines, p_origin)
  {
  }
  
} GRF_CONSOLE_INTERFACE;

#endif // __GRF_PRINT_H__
