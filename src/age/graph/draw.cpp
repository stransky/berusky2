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

void graph3d::draw_rect_line_2d(int x1, int y1, int x2, int y2, RGBB color)
{ 
 glColor3f(color.r,color.g,color.b);
 glBegin(GL_LINE_STRIP);
   glVertex2d(x1,y1);
   glVertex2d(x2,y1);
   glVertex2d(x2,y2);
   glVertex2d(x1,y2);
   glVertex2d(x1,y1);
 glEnd();
}

void graph3d::draw_rect_solid_2d(int x1, int y1, int x2, int y2, RGBB color)
{  
 glColor3f(color.r,color.g,color.b);
 glBegin(GL_QUADS);
   glVertex2d(x1,y1);
   glVertex2d(x1,y2);
   glVertex2d(x2,y2);
   glVertex2d(x2,y1);
 glEnd();
}

void graph3d::draw_line_3d(VECT *p_s, VECT *p_k, RGBB color)
{
 glColor3ub(color.r,color.g,color.b);
 glBegin(GL_LINES);
   glVertex3f(p_s->x,p_s->y,p_s->z);
   glVertex3f(p_k->x,p_k->y,p_k->z);
 glEnd();
}

void graph3d::draw_line_3d(float sx, float sy, float sz, float ex, float ey, float ez, RGBB color)
{
 glColor3ub(color.r,color.g,color.b);
 glBegin(GL_LINES);
   glVertex3f(sx,sy,sz);
   glVertex3f(ex,ey,ez);
 glEnd();
}

void graph3d::draw_cursor_normal(int mx, int my, int sx, int sy, int xres, int yres, RGBB color)
{     
 glColor3f(color.r,color.g,color.b); 
 glBegin(GL_LINES);
   glVertex2i(sx+mx,sy);
   glVertex2i(sx+mx,sy+yres);
   glVertex2i(sx,sy+my);
   glVertex2i(sx+xres,sy+my);
 glEnd(); 
}

#define MOUSE_CURSOR_RECT_SELECT_SIZE 20

void graph3d::draw_cursor_rect_select(int mx, int my, RGBB color)
{     
 glColor3f(color.r,color.g,color.b); 
 glBegin(GL_LINES);
   glVertex2i(mx-MOUSE_CURSOR_RECT_SELECT_SIZE,my);
   glVertex2i(mx+MOUSE_CURSOR_RECT_SELECT_SIZE,my);
   glVertex2i(mx,my-MOUSE_CURSOR_RECT_SELECT_SIZE);
   glVertex2i(mx,my+MOUSE_CURSOR_RECT_SELECT_SIZE);
 glEnd(); 
}

void graph3d::draw_cube_line(float sx, float sy, float sz,
                            float dx, float dy, float dz,
                            RGBB color)
{
  glColor3ub(color.r,color.g,color.b);
  
  glBegin(GL_LINE_STRIP);  
    glVertex3f(sx-dx,sy-dy,sz-dz);
    glVertex3f(sx+dx,sy-dy,sz-dz);
    glVertex3f(sx+dx,sy-dy,sz+dz);
    glVertex3f(sx-dx,sy-dy,sz+dz);
    glVertex3f(sx-dx,sy-dy,sz-dz);  
  glEnd();
  
  glBegin(GL_LINE_STRIP);  
    glVertex3f(sx-dx,sy+dy,sz-dz);
    glVertex3f(sx+dx,sy+dy,sz-dz);
    glVertex3f(sx+dx,sy+dy,sz+dz);
    glVertex3f(sx-dx,sy+dy,sz+dz);
    glVertex3f(sx-dx,sy+dy,sz-dz);  
  glEnd();
  
  glBegin(GL_LINES);
    glVertex3f(sx-dx,sy-dy,sz-dz);
    glVertex3f(sx-dx,sy+dy,sz-dz);
    
    glVertex3f(sx+dx,sy-dy,sz-dz);
    glVertex3f(sx+dx,sy+dy,sz-dz);
    
    glVertex3f(sx+dx,sy-dy,sz+dz);
    glVertex3f(sx+dx,sy+dy,sz+dz);
    
    glVertex3f(sx-dx,sy-dy,sz+dz);
    glVertex3f(sx-dx,sy+dy,sz+dz);
  glEnd();
}

void graph3d::draw_cube_line(VECT *p_center, VECT *p_size, RGBB color)
{
  draw_cube_line(p_center->x, p_center->y, p_center->z,
                 p_size->x, p_size->y, p_size->z,
                 color);
}

void graph3d::draw_cube_solid(float sx, float sy, float sz, 
                              float dx, float dy, float dz, 
                              RGBB color)
{
  VECT vertex[8];
  
  vertex[0].x =-dx;
  vertex[0].y =-dy;
  vertex[0].z =-dz;
  vertex[1].x = dx;
  vertex[1].y =-dy;
  vertex[1].z =-dz;
  vertex[2].x = dx;
  vertex[2].y = dy;
  vertex[2].z =-dz;
  vertex[3].x =-dx;
  vertex[3].y = dy;
  vertex[3].z =-dz;
  vertex[4].x =-dx;
  vertex[4].y =-dy;
  vertex[4].z = dz;
  vertex[5].x = dx;
  vertex[5].y =-dy;
  vertex[5].z = dz;
  vertex[6].x = dx;
  vertex[6].y = dy;
  vertex[6].z = dz;
  vertex[7].x =-dx;
  vertex[7].y = dy;
  vertex[7].z = dz;
  
  int i;
  for(i = 0; i < 8; i++) {
   vertex[i].x += sx;
   vertex[i].y += sy;
   vertex[i].z += sz;
  }
  
  glColor3ub(color.r,color.g,color.b);  
  glBegin(GL_TRIANGLES);  
    
    //front side
    glVertex3fv((float *)(vertex+0));
    glVertex3fv((float *)(vertex+1));
    glVertex3fv((float *)(vertex+2));
    glVertex3fv((float *)(vertex+2));
    glVertex3fv((float *)(vertex+3));
    glVertex3fv((float *)(vertex+0));
    
    //back side
    glVertex3fv((float *)(vertex+5));
    glVertex3fv((float *)(vertex+6));
    glVertex3fv((float *)(vertex+7));
    glVertex3fv((float *)(vertex+7));
    glVertex3fv((float *)(vertex+4));
    glVertex3fv((float *)(vertex+5));
    
    // right side
    glVertex3fv((float *)(vertex+1));
    glVertex3fv((float *)(vertex+5));
    glVertex3fv((float *)(vertex+6));
    glVertex3fv((float *)(vertex+6));
    glVertex3fv((float *)(vertex+2));
    glVertex3fv((float *)(vertex+1));
    
    // left side
    glVertex3fv((float *)(vertex+4));
    glVertex3fv((float *)(vertex+0));
    glVertex3fv((float *)(vertex+3));
    glVertex3fv((float *)(vertex+3));
    glVertex3fv((float *)(vertex+7));
    glVertex3fv((float *)(vertex+4));
    
    // top
    glVertex3fv((float *)(vertex+3));
    glVertex3fv((float *)(vertex+2));
    glVertex3fv((float *)(vertex+6));
    glVertex3fv((float *)(vertex+6));
    glVertex3fv((float *)(vertex+7));
    glVertex3fv((float *)(vertex+3));
    
    // bottom
    glVertex3fv((float *)(vertex+4));
    glVertex3fv((float *)(vertex+0));
    glVertex3fv((float *)(vertex+1));
    glVertex3fv((float *)(vertex+1));
    glVertex3fv((float *)(vertex+5));
    glVertex3fv((float *)(vertex+4));
  
  glEnd();
}

void graph3d::draw_cube_solid(VECT *p_center, VECT *p_size, RGBB color)
{
  draw_cube_solid(p_center->x, p_center->y, p_center->z,
                 p_size->x, p_size->y, p_size->z,
                 color);  
}

void graph3d::draw_label(VECT *p_position, const char *p_label, RGBB color)
{ 
  GPIPE *p_pipe = gpipe_get();
  
  VECT    tmp = *p_position;
  VECT2DI tmp2d;
  
  if(p_pipe->world_to_screen(&tmp,&tmp2d)) {
    p_pipe->matrix_2d_mode_set();
    console_print(color,tmp2d.x,tmp2d.y,p_label);
  }
}

void graph3d::draw_label(float x, float y, float z, const char *p_label, RGBB color)
{ 
  VECT tmp(x,y,z);
  draw_label(&tmp,p_label,color);
}

void graph3d::draw_cross(VECT *p_position, float size, tflag flags)
{
  VECT  p = *p_position;
  RGBB  color;

  GPIPE *p_pipe = gpipe_get();
    
  p_pipe->world_to_screen(&p);
  size *= p.z*0.1f;

  mesh_material::set_default();
  
  glBegin(GL_LINES);
    color = (flags&CROSS_COLOR_AXIS_X) ? RGBB(255,0,0) : RGBB(0,0,0);
    glColor3ubv((byte *)&color);
    glVertex3fv((float *)p_position);
    glVertex3f(p_position->x+size,p_position->y,p_position->z);

    color = (flags&CROSS_COLOR_AXIS_Y) ? RGBB(0,0,255) : RGBB(0,0,0);
    glColor3ubv((byte *)&color);
    glVertex3fv((float *)p_position);
    glVertex3f(p_position->x,p_position->y+size,p_position->z);

    color = (flags&CROSS_COLOR_AXIS_Z) ? RGBB(0,255,0) : RGBB(0,0,0);
    glColor3ubv((byte *)&color);
    glVertex3fv((float *)p_position);
    glVertex3f(p_position->x,p_position->y,p_position->z+size);
  glEnd();
  
  if(flags&CROSS_AXIS_NAMES) {
    
    p_pipe->matrix_2d_mode_set();
    
    draw_label(p_position->x+size,p_position->y,p_position->z,"X");
    draw_label(p_position->x,p_position->y+size,p_position->z,"Y");
    draw_label(p_position->x,p_position->y,p_position->z+size,"Z");
    
    p_pipe->matrix_3d_mode_set();
  }
}

/*
void oe_kresli_editacni_stred(K_EDITOR *p_cnf, BOD *p_stred, int flag)
{
  text_off(GL_TEXTURE_1D);
  text_off(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  oe_test_kriz_stred(p_cnf,VELIKOST_STREDU,p_stred->x,p_stred->y,p_stred->z,
                     POPIS_OS_ON,STRED_ON,flag);
  glEnable(GL_DEPTH_TEST);
}
*/

// Draws grid 
/*
  Calc differences
*/
void graph3d::draw_floor_line(VECT *p_center, float size, int lines_min, int lines_max)
{
  float half_size = size/2;
  float step = 1.0f;
  
  while(1) {
    int lines = ftoi(size/step);
    if(lines > lines_max) {
      step *= 2.0f;
    } else if(lines < lines_min) {
      step *= 0.2f;
    } else {
      break;
    }
  }
  
  // step is a distance between lines
  float sx = ceilf((p_center->x-half_size)/step)*step;
  float sy = p_center->y;
  float sz = ceilf((p_center->z-half_size)/step)*step;
  
  int   nx = (int)ceilf(fabs(size)/step);
  int   nz = (int)ceilf(fabs(size)/step);
  
  float dx = step;
  float dz = step;
  
  int x,xm,z,zm;
  
  #define MINOR_LINES 5
  
  RGBB color_major(200,200,200);
  RGBB color_minor(140,140,140);
  
  // X axis
  for(x = 0; x <= nx; x++) {
    draw_line_3d(sx+dx*x,sy,sz, sx+dx*x,sy,sz+dz*nz, color_major);
    for(xm = 1; xm < MINOR_LINES; xm++) {
      float mdx = (dx/((float)MINOR_LINES))*xm;
      draw_line_3d(sx+dx*x+mdx,sy,sz, sx+dx*x+mdx,sy,sz+dz*nz, color_minor);
    }
  }
  
  // Z axis
  for(z = 0; z <= nz; z++) {
    draw_line_3d(sx,sy,sz+dz*z, sx+dx*nx,sy,sz+dz*z, color_major);
    for(zm = 1; zm < MINOR_LINES; zm++) {
      float mdz = (dz/((float)MINOR_LINES))*zm;
      draw_line_3d(sx,sy,sz+dz*z+mdz, sx+dx*nx,sy,sz+dz*z+mdz, color_minor);
    }
  }  
  
  // Center cross
  #define CENTER_SIZE 20
  draw_line_3d(0,0,-CENTER_SIZE,
               0,0, CENTER_SIZE,
               RGBB(255,0,0));
  draw_line_3d(-CENTER_SIZE,0,0,
                CENTER_SIZE,0,0,
               RGBB(0,255,0));
}

void graph3d::draw_camera(VECT *p_position, VECT *p_target, float fov, RGBB color)
{  
  VECT p = *p_target - *p_position;
  
  float z = p.size();
  float s = (float)tan(fov)*z/2;
  
  VECT v[5];
  
  v[0].x = 0.0f;
  v[0].y = 0.0f;
  v[0].z = 0.0f; 
  
  v[1].x = s;
  v[1].y =-s;
  v[1].z = z; 
  
  v[2].x = s;
  v[2].y = s;
  v[2].z = z; 
  
  v[3].x =-s;
  v[3].y = s;
  v[3].z = z; 
  
  v[4].x =-s;
  v[4].y =-s;
  v[4].z = z;   
  
  glColor3ub(color.r,color.g,color.b);
  glBegin(GL_LINES);  
    glVertex3fv((float *)(v+0));
    glVertex3fv((float *)(v+1));
    glVertex3fv((float *)(v+0));
    glVertex3fv((float *)(v+2));
    glVertex3fv((float *)(v+0));
    glVertex3fv((float *)(v+3));
    glVertex3fv((float *)(v+0));
    glVertex3fv((float *)(v+4));
    glVertex3fv((float *)(v+4));
    glVertex3fv((float *)(v+1));
    glVertex3fv((float *)(v+1));
    glVertex3fv((float *)(v+2));
    glVertex3fv((float *)(v+2));
    glVertex3fv((float *)(v+3));
    glVertex3fv((float *)(v+3));
    glVertex3fv((float *)(v+4));
  glEnd();
}

void graph3d::draw_camera(CAMERA_INTERFACE *p_camera, float size, RGBB color)
{
 VECT  position;
 VECT  direction;
 
 p_camera->position_get(&position);
 p_camera->direction_get(&direction);
 direction.norm(size);
  
 VECT  target(position); 
 target += direction;

 draw_camera(&position, &target, p_camera->projection_fov_get(), color);
}
