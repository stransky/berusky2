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

#include "age.h"

#define TEXT_BUFFER_LEN 1000

#ifdef LINUX

void grf_print::font_create(void)
{
  return;
  
  Display *dpy;          /* Our current X display */
  XFontStruct *fontInfo = NULL; /* Our font info */
  
  /* Sotrage for 96 characters */
  base = glGenLists( 96 );
  
  /* Get our current display long enough to get the fonts */
  dpy = XOpenDisplay( NULL );
  
  /* Get the font information */
  // TODO -> Seznam fontu + vybrat
  fontInfo = XLoadQueryFont( dpy, "-adobe-helvetica-medium-r-normal--18-*-*-*-p-*-iso8859-1" );
  
  /* If the above font didn't exist try one that should */
  if ( fontInfo == NULL ) {
    fontInfo = XLoadQueryFont( dpy, "fixed" );
    /* If that font doesn't exist, something is wrong */
    if ( fontInfo == NULL ) {
      fprintf( stderr, "no X font available?\n" );
      return;
    }
  }
  
  /* generate the list */
  glXUseXFont( fontInfo->fid, 32, 96, base );
  
  /* Recover some memory */
  XFreeFont( dpy, fontInfo );
  
  /* close the display now that we're done with it */
  XCloseDisplay( dpy );    
}

#endif

void grf_print::font_destroy(void)
{
  if(base != 0) {
    glDeleteLists( base, 96 );
    base = 0;
  }
}

/* Print our GL text to the screen */
void grf_print::print_line(RGBB color, int x, int y, const char *p_text)
{   
  /* If there's no text, do nothing */
  if(p_text == NULL || base == 0)
      return;
  
  /* Set color */
  gl_diffuse::on();
  gl_diffuse::set(&color);
  
  /* Set position */
  glRasterPos2i(x,y+font_height_get());
  
  /* Pushes the Display List Bits */
  glPushAttrib( GL_LIST_BIT );
  
  /* Sets base character to 32 */
  glListBase( base - 32 );
  
  /* Draws the text */
  glCallLists( strlen( p_text ), GL_UNSIGNED_BYTE, p_text );
  
  /* Pops the Display List Bits */
  glPopAttrib( );  
}

/* Print our GL text to the screen */
void grf_print::print_line(int x, int y, const char *p_text)
{   
  print_line(color_text,x,y,p_text);
}


void grf_print::print(int x, int y, const char *fmt,...)
{
  char    text[TEXT_BUFFER_LEN];
  va_list ap;
  
  va_start(ap, fmt);
  vsnprintf(text, TEXT_BUFFER_LEN, fmt, ap);
  va_end(ap);
  
  print_line(x,y,text);
}

/*
  Text console
*/

void grf_console::print_to_buffer(char *fmt)
{
  int len;
  
  do {
    len = strlen(fmt);
    if(len > buffer_line_width) {
      len = buffer_line_width;
    }
    
    scroll_if_full();        
    strncpy(p_buffer[buffer_line_last],fmt,len);
    
    buffer_line_last = line_next(buffer_line_last);
    fmt += len;
    
  } while(len == buffer_line_width);
}

void grf_console::print_line(char *fmt)
{  
  char  newlines[] = "\n\r";
  char *p_line;
  
  p_line = strtok(fmt, newlines);
  do {
    print_to_buffer(p_line);
  } while((p_line = strtok(NULL, newlines)) != NULL);
}

void grf_console::print(const char *fmt,...)
{
  char    text[TEXT_BUFFER_LEN];
  va_list ap;
  
  va_start(ap, fmt);
  vsnprintf(text, TEXT_BUFFER_LEN, fmt, ap);
  va_end(ap);
  
  grf_console::print_line(text);
}

void grf_console::draw(void)
{  
  if(empty())
    return;
  
  mesh_material::set_default();
  
  cursor_reset();
  for(int i = buffer_line_first;
      i != buffer_line_last;
      i = line_next(i)) {
    
    grf_print::print_line(origin.x+cursor.x,
                          origin.y+cursor.y,
                          p_buffer[i]);
    cursor_new_line();
  }
}

void grf_console_interface::console_print(const char *fmt,...)
{
  char    text[TEXT_BUFFER_LEN];
  va_list ap;
  
  va_start(ap, fmt);
  vsnprintf(text, TEXT_BUFFER_LEN, fmt, ap);
  va_end(ap);
  
  console.print_line(text);
}

void grf_console_interface::console_print(int x, int y, const char *fmt,...)
{
  char    text[TEXT_BUFFER_LEN];
  va_list ap;
  
  va_start(ap, fmt);
  vsnprintf(text, TEXT_BUFFER_LEN, fmt, ap);
  va_end(ap);
 
  GRF_PRINT *p_print = &console;
  p_print->print_line(x,y,text);
}

void grf_console_interface::console_print(RGBB color, int x, int y, const char *fmt,...)
{
  char    text[TEXT_BUFFER_LEN];
  va_list ap;
  
  va_start(ap, fmt);
  vsnprintf(text, TEXT_BUFFER_LEN, fmt, ap);
  va_end(ap);
 
  GRF_PRINT *p_print = &console;
  p_print->print_line(color,x,y,text);
}
