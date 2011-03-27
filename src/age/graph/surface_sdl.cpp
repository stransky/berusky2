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

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// Constants and types
#include "age.h"

// -------------------------------------------------------
//   the surface class
// -------------------------------------------------------
SDL_Surface * surface_sdl::convert_to_texture(SDL_Surface *p_surf)
{
  assert(p_surf);
  assert(!(p_surf->flags&SDL_SWSURFACE));

  SDL_PixelFormat myformat;  

  memset(&myformat,0,sizeof(myformat));
  myformat.BitsPerPixel = 32;
  myformat.BytesPerPixel = 4;
  
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  myformat.Rmask = 0xff000000;
  myformat.Gmask = 0x00ff0000;
  myformat.Bmask = 0x0000ff00;
  myformat.Amask = 0x000000ff;
#else
  myformat.Rmask = 0x000000ff;
  myformat.Gmask = 0x0000ff00;
  myformat.Bmask = 0x00ff0000;
  myformat.Amask = 0xff000000;
#endif
  
  myformat.Rshift = 0;
  myformat.Gshift = 8;
  myformat.Bshift = 16;
  myformat.Ashift = 24;  
  
  // Create a new-one
  SDL_Surface *p_new = SDL_ConvertSurface(p_surf, &myformat, SDL_SWSURFACE);
  if(!p_new) {
    PERROR(TRUE, "Can't convert surface %p to texture format!",p_surf);  
  }
  // Ensure there aren't any extra bytes in a end of scan line
  if(p_new->pitch != p_new->w*myformat.BytesPerPixel) {
    PERROR(TRUE, "Error in texture conversion!");
  }

  return(p_new);
}

void surface_sdl::convert_to_texture(void)
{
  SDL_Surface *p_tmp = convert_to_texture(p_surf);
  SDL_FreeSurface(p_surf);
  p_surf = p_tmp;
  surface_format = SURFACE_TEXTURE;
}

SDL_Surface * surface_sdl::convert_to_display_format(SDL_Surface *p_surf)
{
  assert(p_surf);
  SDL_Surface *p_new = SDL_DisplayFormat(p_surf);  
  if(!p_new) {
    PERROR(TRUE, "Can't convert surface %p to display format!",p_surf);
  }
  return(p_new);
}

void surface_sdl::convert_to_display_format(void)
{
  SDL_Surface *p_tmp = convert_to_display_format(p_surf);  
  SDL_FreeSurface(p_surf);
  p_surf = p_tmp;
  surface_format = SURFACE_DISPLAY;
}

SDL_Surface * surface_sdl::convert_to_alpha(SDL_Surface *p_surf)
{
  assert(p_surf);
  SDL_Surface *p_tmp = convert_to_texture(p_surf);
  
  // Convert only valid surfaces in texture format
  assert(p_tmp && p_tmp->format->BytesPerPixel == 4);
  assert(!lock_required(p_tmp));

  dword *p_pix = (dword *)p_tmp->pixels;  
  assert(p_pix);
  int l, lenght = p_tmp->w*p_tmp->h;

  PIXELFORMAT *p_format = p_tmp->format;
  
  int Rmask = p_format->Rmask;
  int Gmask = p_format->Gmask;
  int Bmask = p_format->Bmask;
  int Amask = p_format->Amask;
  
  int Rshift = p_format->Rshift;
  int Gshift = p_format->Gshift;
  int Bshift = p_format->Bshift;
  int Ashift = p_format->Ashift;
  
  for(l = 0; l < lenght; l++, p_pix++) {
    tcolor pixel = *p_pix;
    int alpha = ((pixel&Rmask)>>Rshift) + 
                ((pixel&Gmask)>>Gshift) + 
                ((pixel&Bmask)>>Bshift);
    alpha /= 3;
    *p_pix = (alpha<<Ashift)&Amask;
  }

  return(p_tmp);
}

void surface_sdl::convert_to_alpha(void)
{
  SDL_Surface *p_tmp = convert_to_alpha(p_surf);
  SDL_FreeSurface(p_surf);
  p_surf = p_tmp;
  surface_format = SURFACE_ALPHA;
}

SDL_Surface * surface_sdl::merge(SDL_Surface *p_dest, SDL_Surface *p_src)
{
  assert(p_dest && p_src);
  
  // Convert only valid surfaces in texture format
  assert(p_dest->format->BytesPerPixel == 4);
  assert(!lock_required(p_dest));
  assert(p_src->format->BytesPerPixel == 4);
  assert(!lock_required(p_src));

  dword *p_psrc = (dword *)p_src->pixels;
  dword *p_pdst = (dword *)p_dest->pixels;
  assert(p_psrc && p_pdst);
  int l, lenght = p_src->w*p_src->h;

  for(l = 0; l < lenght; l++, p_psrc++, p_pdst++) {
    *p_pdst |= *p_psrc;
  }

  return(p_dest);
}

void surface_sdl::create(tpos width, tpos height, SURFACE_FORMAT format)
{    
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  Uint32 Rmask = 0xff000000;
  Uint32 Gmask = 0x00ff0000;
  Uint32 Bmask = 0x0000ff00;
  Uint32 Amask = 0x000000ff;
#else
  Uint32 Rmask = 0x000000ff;
  Uint32 Gmask = 0x0000ff00;
  Uint32 Bmask = 0x00ff0000;
  Uint32 Amask = 0xff000000;
#endif
  
  p_surf = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
                                Rmask, Gmask, Bmask, Amask);
  if(p_surf == NULL) {
    pprintf("SDL_CreateRGBSurface failed: %s\n", SDL_GetError());    
    assert(p_surf != NULL);
  }
  
  SDL_SetAlpha(p_surf,0,SDL_ALPHA_OPAQUE);
  
  locks = 0;
  
  surface_format = format;
}

bool surface_sdl::load(const char *p_file, SURFACE_FORMAT format)
{
  assert(p_surf == NULL);
  p_surf = IMG_Load(p_file);
  if(p_surf) {
    if(format == SURFACE_TEXTURE) {
      convert_to_texture();
    }
    else {
      convert_to_display_format();
    }
  }

  return(p_surf != NULL);
}

bool surface_sdl::load_alpha(const char *p_file)
{
  if(p_surf == NULL)
    return(FALSE);
  
  SDL_Surface *p_tmp = IMG_Load(p_file);
  if(p_tmp) {
    SDL_Surface *p_alpa = convert_to_alpha(p_tmp);
    merge(p_surf, p_alpa);
    SDL_FreeSurface(p_tmp);
    SDL_FreeSurface(p_alpa);
  } 
  return(p_tmp != NULL);
}

bool surface_sdl::save(const char *p_file, IMAGE_FORMAT format)
{ 
  if(pixels_get()) {
    SDL_SaveBMP(p_surf, p_file);
  }
  return(TRUE);
}

void surface_sdl::copy(class surface_sdl *p_src)
{
  clear();  
  
  // Source surface is valid - copy pixels
  if(p_src->p_surf) {
    p_surf = SDL_ConvertSurface(p_src->p_surf, p_src->p_surf->format, p_src->p_surf->flags);
    assert(p_surf);
  } else {
    *this = *p_src;
  }
  surface_format = p_src->surface_format;
  locks = 0;
}

void surface_sdl::move(class surface_sdl *p_src)
{
  clear();
  
  *this = *p_src;
  p_src->p_surf = NULL;  
  
  locks = 0;
}

void surface_sdl::clear(void)
{
  assert(ref_get() == 0);

  if(p_surf) {
    SDL_FreeSurface(p_surf);
    p_surf = NULL;
  }

  locks = 0;
}

surface_sdl::surface_sdl(void)
: surface(SDL_SURFACE, PIXEL_TCOLOR),
  p_surf(NULL)
{
}

surface_sdl::surface_sdl(char *p_file, SURFACE_FORMAT format_)
: surface(SDL_SURFACE, PIXEL_TCOLOR),
  p_surf(NULL)
{
  load(p_file, format_);
}

surface_sdl::surface_sdl(SDL_Surface *p_surf_, SURFACE_FORMAT format_)
: surface(SDL_SURFACE, PIXEL_TCOLOR),
  p_surf(p_surf_)
{
  surface_format = format_;
}

surface_sdl::surface_sdl(tpos width, tpos height, SURFACE_FORMAT format_)
: surface(SDL_SURFACE, PIXEL_TCOLOR),
  p_surf(NULL)
{
  create(width,height,format_);
}

// Color-key set
void surface_sdl::ckey_set(trgbcomp r, trgbcomp g, trgbcomp b)
{
  if(p_surf) {
    int ret = SDL_SetColorKey(p_surf, SDL_SRCCOLORKEY, SDL_MapRGB(p_surf->format, r, g, b));
    assert(ret != -1);
  }
}

void surface_sdl::ckey_set(tcolor color)
{
  if(p_surf) {
    int ret = SDL_SetColorKey(p_surf, SDL_SRCCOLORKEY, color);
    assert(ret != -1);
  }
}

void surface_sdl::fill(tcolor color)
{ 
  if(p_surf) {
    SDL_FillRect(p_surf, NULL, color);
  }
}

void surface_sdl::fill(tpos x, tpos y, tpos dx, tpos dy, tcolor color)
{ 
  if(p_surf) {
    SDL_Rect rec = { x, y, dx, dy };
    SDL_FillRect(p_surf, &rec, color);
  }
}

// blit whole source surtace to destination surface
void surface_sdl::blit(class surface_sdl *p_dst, tpos tx, tpos ty)
{
  if(p_surf) {
    SDL_Rect dst_rec = {tx,ty,p_surf->w,p_surf->h};
    SDL_BlitSurface(p_surf, NULL, p_dst->p_surf, &dst_rec);
  }
}

// blit part of source surface to destination surface
void surface_sdl::blit(tpos sx, tpos sy, tpos dx, tpos dy, class surface_sdl *p_dst, tpos tx, tpos ty)
{
  if(p_surf) {
    SDL_Rect src_rec = {sx,sy,dx,dy};
    SDL_Rect dst_rec = {tx,ty,dx,dy};
    SDL_BlitSurface(p_surf, &src_rec, p_dst->p_surf, &dst_rec);  
  }
}

void surface_sdl::scale(int nx, int ny)
{   
  if(p_surf)
    return;
  
  // Scale only valid surfaces in texture format
  assert(surface_format == SURFACE_TEXTURE && p_surf->format->BytesPerPixel == 4);
  assert(!lock_required());

  if(nx != p_surf->w || ny != p_surf->h) {  
    if(nx < 1)
      nx = 1;
    if(ny < 1)
      ny = 1;

    SDL_Surface *p_new = SDL_CreateRGBSurface(p_surf->flags, nx, ny, 4,
                         p_surf->format->Rmask, p_surf->format->Gmask,
                         p_surf->format->Bmask, p_surf->format->Amask);   
    
    dword *p_src_mem = (dword *)p_surf->pixels;
    dword *p_dest_mem = (dword *)p_new->pixels;
    dword *p_dest = NULL;
    float  px,py,dx,dy;
    int    tx,ty,ys;
    
    dx = (float)p_surf->w/(float)nx;
    dy = (float)p_surf->h/(float)ny;
    py = 0;
    
    for(ty = 0; ty < ny; ty++) {
      ys = (int)((py)*(p_surf->w));
      px = 0;
      p_dest = p_dest_mem+ty*nx;
      for(tx = 0; tx < nx; tx++) {
        p_dest[tx] = p_src_mem[ys+(int)px];
        px += dx;
      }
      py += dy;
    }
    
    SDL_FreeSurface(p_surf);
    p_surf = p_new;
  }
}

tcolor surface_sdl::single_pixel_get(int x, int y)
{
  tcolor color = 0;
  
  if(pixel_valid(x,y)) {
    assert(p_surf->format->BytesPerPixel == 4);
    lock();
    dword *p_pixels = (dword *)pixels_get();
    color = *(p_pixels+y*(p_surf->pitch>>2)+x);
    unlock();
  }
  
  return(color);
}

void surface_sdl::single_pixel_set(int x, int y, tcolor color)
{
  assert(x >= 0 && y >= 0 && x < width_get() && y < height_get());
  
  if(pixel_valid(x,y)) {
    assert(p_surf->format->BytesPerPixel == 4);
    
    bool locked = FALSE;
    if(lock_required()) {
      lock();
      locked = TRUE;
    }
    
    dword *p_pixels = (dword *)pixels_get();
    *(p_pixels+y*(p_surf->pitch>>2)+x) = color;
    
    if(locked)
      unlock();
  }
}

tcolor surface_sdl::pixel_get(int x, int y)
{
  tcolor color = 0;
  if(pixel_valid(x,y)) {
    dword *p_pixels = (dword *)pixels_get();
    color = (*(p_pixels+y*(p_surf->pitch>>2)+x));
  }
  return(color);
}

void surface_sdl::pixel_set(int x, int y, tcolor color)
{
  assert(x >= 0 && y >= 0 && x < width_get() && y < height_get());
  
  if(pixel_valid(x,y)) {
    dword *p_pixels = (dword *)pixels_get();
    *(p_pixels+y*(p_surf->pitch>>2)+x) = color;
  }
}

bool surface_sdl::pixels_set(tcolor *p_pixels, tpos width, tpos height)
{  
  assert(surface_format == SURFACE_TEXTURE && p_surf->format->BytesPerPixel == 4);
  assert(!lock_required());
    
  tpos target_width = width_get();
  tpos target_height = height_get();
  
  assert(width <= target_width);
  assert(height <= target_height);
  
  int y;
  for(y = 0; y < height; y++) {
    tcolor *p_pix = (tcolor *)pixels_get(0,y);
    assert(p_pix);
    memcpy(p_pix, p_pixels+(width*y), sizeof(tcolor)*width);
  }
  
  return(TRUE);
}
