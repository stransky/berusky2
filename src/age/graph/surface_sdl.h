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

#ifndef  __SURFACE_SDL_H__
#define  __SURFACE_SDL_H__

typedef class surface_sdl
  : public surface
{
  SDL_Surface * p_surf;         // surface pix-map

public:

  SDL_Surface * surf_get(void)
  {
    return(p_surf);
  }
  
  tpos pitch_get(void)
  {
    return(p_surf->pitch>>2);
  }
  
public:  
  
  tpos width_get(void)
  {
    return(p_surf ? p_surf->w : 0);
  }
  
  tpos height_get(void)
  {
    return(p_surf ? p_surf->h : 0);
  }
  
  void size_get(tpos *p_dx, tpos *p_dy)
  {
    if(p_surf) {
      *p_dx = p_surf->w;
      *p_dy = p_surf->h;
    }
    else {
      *p_dx = 0;
      *p_dy = 0;
    }
  }
  
private:

  int locks;

public:

  static bool lock_required(SDL_Surface *p_surf)
  {
    return(p_surf ? SDL_MUSTLOCK(p_surf) : FALSE);
  }

  bool lock_required(void)
  {
    return(p_surf ? SDL_MUSTLOCK(p_surf) : FALSE);
  }

  bool locked(void)
  {
    return(locks > 0);
  }

  void lock(void)
  {
    if(!locked() && p_surf) {
      SDL_LockSurface(p_surf);
    }
    locks++;
  }

  void unlock(void)
  {
    if(locked()) {
      locks--;
    }
    if(!locked() && p_surf) {
      SDL_UnlockSurface(p_surf);
    }
  }

public:

  bool loaded(void)
  {
    return(p_surf != NULL);
  }
  
  bool load(const char *p_file, SURFACE_FORMAT format = SURFACE_TEXTURE);
  bool load_alpha(const char *p_file);  

  bool save(const char *p_file, IMAGE_FORMAT format = IMAGE_BMP);

  void copy(class surface_sdl *p_src);
  void move(class surface_sdl *p_src);
  
public:
    
  void clear(void);
  void create(tpos width, tpos height, SURFACE_FORMAT format = SURFACE_TEXTURE);
  void remove(void);

public:
  
  void ckey_set(trgbcomp r, trgbcomp g, trgbcomp b);
  void ckey_set(tcolor color);
  
  void fill(tcolor color);
  void fill(tpos x, tpos y, tpos dx, tpos dy, tcolor color);
  
  void blit(class surface_sdl *p_dst, tpos tx = 0, tpos ty = 0);
  void blit(tpos sx, tpos sy, tpos dx, tpos dy, class surface_sdl *p_dst,
            tpos tx = 0, tpos ty = 0);
    
  void scale(int nx, int ny);

  static SDL_Surface * convert_to_texture(SDL_Surface *p_surf);
  static SDL_Surface * convert_to_display_format(SDL_Surface *p_surf);
  static SDL_Surface * convert_to_alpha(SDL_Surface *p_surf);
  static SDL_Surface * merge(SDL_Surface *p_dest, SDL_Surface *p_src);
  
  void convert_to_texture(void);
  void convert_to_display_format(void); 
  void convert_to_alpha(void);
  
  tcolor color_map(Uint8 r, Uint8 g, Uint8 b)
  {    
    return(p_surf ? SDL_MapRGB(p_surf->format, r, g, b) : 0);
  }
  
  void * pixels_get(void)
  {
    if(lock_required() && !locked()) {
      assert(0);
      return(NULL);
    }
    return(p_surf ? p_surf->pixels : NULL);
  }

  void * pixels_get(tpos x, tpos y)
  {
    dword *p_pix = (dword *)pixels_get();
    if(!p_pix) {
      return(NULL);
    }
    
    return(p_pix + y*(p_surf->pitch>>2) + x);
  }
  
  bool    pixels_set(tcolor *p_pixels, tpos width, tpos height);
  
  // Pixel operations (need lock/unlock)
  tcolor  pixel_get(int x, int y);
  void    pixel_set(int x, int y, tcolor color);

  tcolor  single_pixel_get(int x, int y);
  void    single_pixel_set(int x, int y, tcolor color);
  
public:
  
  surface_sdl(void);
  surface_sdl(char *p_file, SURFACE_FORMAT format_ = SURFACE_TEXTURE);
  surface_sdl(SDL_Surface *p_surf_, SURFACE_FORMAT format_ = SURFACE_TEXTURE);
  surface_sdl(tpos width, tpos height, SURFACE_FORMAT format_ = SURFACE_TEXTURE);

} SURFACE_SDL;

#endif //  __SURFACE_SDL_H__
