//------------------------------------------------------------------------------------------------
// 0.0.1
//------------------------------------------------------------------------------------------------
#include "camera.h"
#include "3d_math.h"
#include "Object.h"
#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"

#define MAXZOOMSPEED 20
#define MAXRSPEED    80
#define MAXFISPEED   60

//------------------------------------------------------------------------------------------------
// spocte procenta
//------------------------------------------------------------------------------------------------
float f_Count_Percentage(float init, float real, float finish)
{
  if ((finish - init) == 0)
    return 100;

  return ((100.0f * (real - init)) / (finish - init));
}

float vf_Count_Percentage(float *init, float *real, float *finish)
{
  float vr[3], dr, vf[3], df;

  Make_Vector(init, finish, vf);
  df = Vector_Length(vf);

  Make_Vector(init, real, vr);
  dr = Vector_Length(vr);

  if (df == 0)
    return 100;

  return ((100.0f * dr) / df);
}

//------------------------------------------------------------------------------------------------
// spocte minimum
//------------------------------------------------------------------------------------------------
float Min(float *vfArray, int size)
{
  int i;
  float min = vfArray[0];

  for (i = 1; i < size; i++)
    if (vfArray[i] < min)
      min = vfArray[i];

  return min;
}

//------------------------------------------------------------------------------------------------
// aniamce kamery "jumping"
//------------------------------------------------------------------------------------------------
void camera_Animate_Jumping(CAMERA_ANIMATION * cam)
{
  float v[3], r, fi, dist, fpq, perc[4], min_p;

  if (!cam->flag)
    return;

  fpq = kom_get_framerate() / 4.0f;

  kam_pol_get((BOD *) & cam->real.Position, &cam->real.r, &cam->real.fi,
    &cam->real.Distance);
  Make_Vector(cam->real.Position, cam->finish.Position, v);

  Multiple_Vector(v, (1.0f / fpq));
  r = (cam->finish.r - cam->real.r) / fpq;
  fi = (cam->finish.fi - cam->real.fi) / fpq;
  dist = (cam->finish.Distance - cam->real.Distance) / fpq;

  Add_Vector(cam->real.Position, v, cam->real.Position);
  cam->real.r += r;
  cam->real.fi += fi;
  cam->real.Distance += dist;

  perc[0] =
    f_Count_Percentage(cam->init.Distance, cam->real.Distance,
    cam->finish.Distance);
  perc[1] = f_Count_Percentage(cam->init.r, cam->real.r, cam->finish.r);
  perc[2] = f_Count_Percentage(cam->init.fi, cam->real.fi, cam->finish.fi);
  perc[3] =
    vf_Count_Percentage(cam->init.Position, cam->real.Position,
    cam->finish.Position);

  min_p = Min(perc, 4);

  if (min_p > 60)
    cam->bChange = 1;

  if (fabs(r) < 0.1f &&
    fabs(fi) < 0.1f &&
    fabs(dist) < 0.1f &&
    fabs(v[0]) < 0.1f && fabs(v[1]) < 0.1f && fabs(v[2]) < 0.1f)
    cam->flag = 0;


  kam_pol_set((BOD *) & cam->real.Position, cam->real.r, cam->real.fi,
    cam->real.Distance, 1);
}

//------------------------------------------------------------------------------------------------
// init kamery
//------------------------------------------------------------------------------------------------
void camera_Init(CAMERA_ANIMATION * cam)
{
  kam_pol_get((BOD *) & cam->init.Position, &cam->init.r, &cam->init.fi,
    &cam->init.Distance);
}

//------------------------------------------------------------------------------------------------
// animace kamery
//------------------------------------------------------------------------------------------------
void camera_Animate(CAMERA_ANIMATION * cam)
{
  float v[3], r, fi, dist, perc[4], min_p, fpq;

  if (!cam->flag)
    return;

  fpq = kom_get_framerate() / 4.0f;

  kam_pol_get((BOD *) & cam->real.Position, &cam->real.r, &cam->real.fi,
    &cam->real.Distance);

  perc[0] =
    f_Count_Percentage(cam->init.Distance, cam->real.Distance,
    cam->finish.Distance);
  perc[1] = f_Count_Percentage(cam->init.r, cam->real.r, cam->finish.r);
  perc[2] = f_Count_Percentage(cam->init.fi, cam->real.fi, cam->finish.fi);
  perc[3] =
    vf_Count_Percentage(cam->init.Position, cam->real.Position,
    cam->finish.Position);

  min_p = Min(perc, 4);

  if (min_p > 75) {
    Make_Vector(cam->real.Position, cam->finish.Position, v);
    Multiple_Vector(v, (1.0f / fpq));
    r = (cam->finish.r - cam->real.r) / fpq;
    fi = (cam->finish.fi - cam->real.fi) / fpq;
    dist = (cam->finish.Distance - cam->real.Distance) / fpq;
  }
  else {
    Make_Vector(cam->init.Position, cam->finish.Position, v);
    Normalize_Vector(v);
    Multiple_Vector(v, (1.0f / fpq));

    r = (cam->finish.r - cam->init.r) / fpq;
    if (fabs(r) > (MAXRSPEED / (fpq * 4.0f))) {
      if (r < 0)
        r = MAXRSPEED / (fpq * -4.0f);
      else
        r = MAXRSPEED / (fpq * 4.0f);
    }

    fi = (cam->finish.fi - cam->init.fi) / fpq;
    if (fabs(fi) > (MAXFISPEED / (fpq * 4.0f))) {
      if (fi < 0)
        fi = MAXFISPEED / (fpq * -4.0f);
      else
        fi = MAXFISPEED / (fpq * 4.0f);
    }


    dist = (cam->finish.Distance - cam->init.Distance) / (fpq * 4.0f);
    if (fabs(dist) > (MAXZOOMSPEED / (fpq * 4.0f))) {
      if (dist < 0)
        dist = MAXZOOMSPEED / (fpq * -4.0f);
      else
        dist = MAXZOOMSPEED / (fpq * 4.0f);
    }
  }

  if (min_p > 60)
    cam->bChange = 1;

  Add_Vector(cam->real.Position, v, cam->real.Position);
  cam->real.r += r;
  cam->real.fi += fi;
  cam->real.Distance += dist;

  if (fabs(r) < 0.1f &&
    fabs(fi) < 0.1f &&
    fabs(dist) < 0.1f &&
    fabs(v[0]) < 0.1f && fabs(v[1]) < 0.1f && fabs(v[2]) < 0.1f)
    cam->flag = 0;

  kam_pol_set((BOD *) & cam->real.Position, cam->real.r, cam->real.fi,
    cam->real.Distance, 1);
}

//------------------------------------------------------------------------------------------------
// spusti kameru
//------------------------------------------------------------------------------------------------
void camera_Go(CAMERA_ANIMATION * cam, float fpq)
{
  cam->flag = 1;
  cam->bChange = 0;
  camera_Init(cam);
}

void camera_Fast_Turn(int pressed_key, char left, int *flag, float fpq)
{
  CAMERA_ANIMATION cam;
  float lr;
  int f, i;

  key[pressed_key] = 0;

  kam_pol_get((BOD *) & cam.finish.Position, &cam.finish.r, &cam.finish.fi,
    &cam.finish.Distance);

  lr = cam.finish.r;

  if (left)
    cam.finish.r -= 90.0f;
  else
    cam.finish.r += 90.0f;

  f = (int) floor(cam.finish.r / 90.0f);
  i = (int) floor(cam.finish.r) % 90;

  if (abs(i) < 45) {
    if (left)
      f--;
    else
      f++;
  }

  cam.finish.r = f * 90.0f;

  while (fabs(cam.finish.r - lr) > 135) {
    if (left)
      f++;
    else
      f--;

    cam.finish.r = f * 90.0f;
  }

  cam.last_move = left;

  kam_pol_anim((BOD *) cam.finish.Position, cam.finish.r, cam.finish.fi,
    cam.finish.Distance, flag, GK_REMOVE, 4, 1);
  //camera_Go(cam, fpq);
}

void camera_Zoom(int pressed_key, char out, int *flag, float fpq)
{
  CAMERA_ANIMATION cam;

  key[pressed_key] = 0;

  kam_pol_get((BOD *) & cam.finish.Position, &cam.finish.r, &cam.finish.fi,
    &cam.finish.Distance);

  if (!out)
    cam.finish.Distance -= 10.0f;
  else
    cam.finish.Distance += 10.0f;

//      camera_Go(cam, fpq);

  kam_pol_anim((BOD *) cam.finish.Position, cam.finish.r, cam.finish.fi,
    cam.finish.Distance, flag, GK_REMOVE, 4, 1);
}

void camera_Turn(int pressed_key, int left, int down, CAMERA_ANIMATION * cam,
  float fpq)
{
  key[pressed_key] = 0;

  kam_pol_get((BOD *) & cam->finish.Position, &cam->finish.r, &cam->finish.fi,
    &cam->finish.Distance);

  if (!left)
    cam->finish.r += 40.0f;
  else if (left == 1)
    cam->finish.r -= 40.0f;

  if (!down)
    cam->finish.fi += 20.0f;
  else if (down == 1)
    cam->finish.fi -= 20.0f;

  camera_Go(cam, fpq);
}

void camera_Move(int pressed_key, int left, int back, CAMERA_ANIMATION * cam,
  float fpq)
{
  float v[3], vR[3];

  key[pressed_key] = 0;

  kam_pol_get((BOD *) & cam->finish.Position, &cam->finish.r, &cam->finish.fi,
    &cam->finish.Distance);
  kam_3ds_get((BOD *) & cam->finish.Position, (BOD *) & cam->finish.Target,
    &cam->finish.Roll);

  cam->finish.Position[1] = cam->finish.Target[1];
  Make_Vector(cam->finish.Target, cam->finish.Position, v);

  Normalize_Vector(v);

  if (!left) {
    Normal_Vector(v, vR);
    Multiple_Vector(vR, 4);
    Add_Vector(cam->finish.Target, vR, cam->finish.Position);
  }
  else if (left == 1) {
    Normal_Vector(v, vR);
    Multiple_Vector(vR, 4);
    Inverse_Vector(vR, vR, 3);
    Add_Vector(cam->finish.Target, vR, cam->finish.Position);
  }

  if (!back) {
    Multiple_Vector(v, 4);
    Inverse_Vector(v, v, 3);
    Add_Vector(cam->finish.Target, v, cam->finish.Position);
  }
  else if (back == 1) {
    Multiple_Vector(v, 4);
    Add_Vector(cam->finish.Target, v, cam->finish.Position);
  }

  camera_Go(cam, fpq);
}

void camera_Center(int *pcameraflag, void *v_Level, int framenum,
  char bTopView, char bLastCam)
{
  LEVELINFO *p_Level = (LEVELINFO *) v_Level;
  float pos[3] = {0,0,0};
  int r;

  BOD p;
  float fr, ffi, dist;

  if (bLastCam) {
    kam_pol_anim((BOD *) p_Level->lCamera.Position,
      p_Level->lCamera.r, p_Level->lCamera.fi,
      p_Level->lCamera.Distance, pcameraflag, GK_REMOVE, framenum, 1);

    *pcameraflag = 0;
    return;
  }

  kam_pol_get(&p, &fr, &ffi, &dist);

  if(p_Level->Actual_Item != -1) {
    kom_mesh_get_float(p_Level->Level[p_Level->Actual_Item]->Index_Of_Game_Mesh,
                       &pos[0], &pos[1], &pos[2], &r);
  }

  if (p_Level->Actual_Item != -1) {
/*		switch(p_Level->Level[p_Level->Actual_Item]->Rotation)
		{
		case 0:
			if(!bTopView)
				kam_pol_anim((BOD *)pos, 0, 45, 15, pcameraflag,GK_REMOVE,framenum,1);
			else
				kam_pol_anim((BOD *)pos, 0, 89, 15, pcameraflag,GK_REMOVE,framenum,1);
			break;
		case 1:
			if(!bTopView)
				kam_pol_anim((BOD *)pos, 270, 45, 15, pcameraflag,GK_REMOVE,framenum,1);
			else
				kam_pol_anim((BOD *)pos, 270, 89, 15, pcameraflag,GK_REMOVE,framenum,1);
			break;
		case 2:
			if(!bTopView)
				kam_pol_anim((BOD *)pos, 180, 45, 15, pcameraflag,GK_REMOVE,framenum,1);
			else
				kam_pol_anim((BOD *)pos, 270, 89, 15, pcameraflag,GK_REMOVE,framenum,1);
			break;
		case 3:
			if(!bTopView)
				kam_pol_anim((BOD *)pos, 90, 45, 15, pcameraflag,GK_REMOVE,framenum,1);
			else
				kam_pol_anim((BOD *)pos, 270, 89, 15, pcameraflag,GK_REMOVE,framenum,1);
			break;
		}*/

    kam_pol_anim((BOD *) pos, fr, ffi, 25, pcameraflag, GK_REMOVE, framenum,
      1);

    *pcameraflag = 0;
  }
}
