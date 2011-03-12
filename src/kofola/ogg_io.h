#ifndef __ogg_h__
#define __ogg_h__

#include "al.h"
#include "ogg.h"
#include "vorbisfile.h"

#define BUFFER_SIZE (4096 * 10)

#ifdef __cplusplus
	extern "C" {
#endif

   int ogg_open(char *file, float gain);
   void ogg_release();
   char ogg_playback();
   char ogg_playing();
   char ogg_update();
   void ogg_gain(float gain);

#ifdef __cplusplus
	}
#endif

#endif // __ogg_h__
