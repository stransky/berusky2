#ifndef __CAMERA_
#define __CAMERA_

typedef struct
{
	float	Position[3];// pozice
	float	Target[3];	// cil
	float	Roll;
	float	fi;
	float	r;
	float	Distance;
} CAMERA_STRUCT;


typedef struct
{
	CAMERA_STRUCT	init;
	CAMERA_STRUCT	real;
	CAMERA_STRUCT	finish;
	int				flag;
	int				bChange;
	char			anim_type;
	int				last_move;
} CAMERA_ANIMATION;

void camera_Animate(CAMERA_ANIMATION *cam);
void camera_Animate_Jumping(CAMERA_ANIMATION *cam);
void camera_Fast_Turn(int pressed_key, char left, int *flag, float fpq);
void camera_Zoom(int pressed_key, char out, int *flag, float fpq);
void camera_Turn(int pressed_key, int left, int down, CAMERA_ANIMATION *cam, float fpq);
void camera_Move(int pressed_key, int left, int back, CAMERA_ANIMATION *cam, float fpq);
void camera_Center(int *pcameraflag, void *v_Level, int framenum, char bTopView, char bLastCam);

#endif
