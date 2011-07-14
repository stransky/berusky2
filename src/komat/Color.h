/*
  color.h
*/
#ifndef __COLOR_H
#define __COLOR_H

inline void add_color_rgb_dword(dword * p_baze, dword r, dword g, dword b)
{                               // BGR
  dword rd, gd, bd;

  rd = ((*p_baze) & 0xff);
  gd = ((*p_baze) & 0xff00) >> 8;
  bd = ((*p_baze) & 0xff0000) >> 16;

  rd += r;
  gd += g;
  bd += b;

  if (rd > MAX_BYTE)
    rd = MAX_BYTE;
  if (gd > MAX_BYTE)
    gd = MAX_BYTE;
  if (bd > MAX_BYTE)
    bd = MAX_BYTE;

  *p_baze = rd;
  *p_baze |= (gd << 8);
  *p_baze |= (bd << 16);
}

inline void sub_color_rgb_dword(dword * p_baze, dword r, dword g, dword b)
{                               // BGR
  int rd, gd, bd;

  rd = ((*p_baze) & 0xff);
  gd = ((*p_baze) & 0xff00) >> 8;
  bd = ((*p_baze) & 0xff0000) >> 16;

  rd -= r;
  gd -= g;
  bd -= b;

  if (rd < 0)
    rd = 0;
  if (gd < 0)
    gd = 0;
  if (bd < 0)
    bd = 0;

  *p_baze = rd;
  *p_baze |= (gd << 8);
  *p_baze |= (bd << 16);
}

#endif
