#ifndef _ADAS_TYPES_H_
#define _ADAS_TYPES_H_

//------------------------------------------------------------------------------------------------
// constants declarations
//------------------------------------------------------------------------------------------------
#define ALL_SOUND_SOURCES		0xFFFFFFF
#define	PARTICULAR_SOUND_SOURCE	0xFFFFFFE
#define	ALL_TYPES				0x0000000
#define NORMAL_TYPE				0x0000001
#define	LOOPING_TYPE			0x0000002
#define	SCREAM_TYPE				0x0000003
#define HDD						0x0000004
#define RAM						0x0000005
#define BOUNDING_BOX			0x0000006
#define	BOUNDING_SPHERE			0x0000007
#define UNDEFINED_VALUE		   -0xFFFFFFF

//------------------------------------------------------------------------------------------------
// types definitions
//------------------------------------------------------------------------------------------------
typedef struct
{
	char			Channels;			// number of channels. One sound can be played in one
										// channel. 32 channels = max 32 sounds in one time
	char			*Implementation;	// string defined in OpenAL
	char			Sound_Dir[256];	// directory with waves and index.dat file
	float			Scale_Factor;		// factor of distance scaling
} ADAS_INIT_DATA;

typedef struct
{
	float	LBF_point[3];				// Left, Bottom, Front point
	float	RTB_point[3];				// Right, Top, Back point
	float	Radius;						// Radius of Sound Source (for Bounding sphere)
	int		Type;						// Type of bounding object (see definitions)
} BOUNDING_OBJECT;

typedef struct
{
	int					Wave_Index;			// index of wave
	int					Owner;				// owner of sound source
	char				Type;				// type of sound source
	float				Pos[3];				// starting position
	float				Velocity[3];		// velocity
	float				Gain;				// gain (volume)
	float				Pitch;				// pitch
	char				Source_Relative;	// position will be interpreted as relative to 
											// listener
	long				Obstruction;
	float				ObstructionLF;
	long				wall_occlusion;
	float				wall_occlusionLF;
	float				wall_occlusion_room;
	long				out_room;
	long				out_roomHF;
	float				out_room_rolloff;
	float				out_air_absorbtion;
	BOUNDING_OBJECT		Bounding_Object;
} ADAS_SOUND_SOURCE_DATA;

#endif
