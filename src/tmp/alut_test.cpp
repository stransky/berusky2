/*
 *        .þÛÛþ þ    þ þÛÛþ.     þ    þ þÛÛÛþ.  þÛÛÛþ .þÛÛþ. þ    þ
 *       .þ   Û Ûþ.  Û Û   þ.    Û    Û Û    þ  Û.    Û.   Û Ûþ.  Û
 *       Û    Û Û Û  Û Û    Û    Û   þ. Û.   Û  Û     Û    Û Û Û  Û
 *     .þþÛÛÛÛþ Û  Û Û þÛÛÛÛþþ.  þþÛÛ.  þþÛÛþ.  þÛ    Û    Û Û  Û Û
 *    .Û      Û Û  .þÛ Û      Û. Û   Û  Û    Û  Û.    þ.   Û Û  .þÛ
 *    þ.      þ þ    þ þ      .þ þ   .þ þ    .þ þÛÛÛþ .þÛÛþ. þ    þ
 *
 * Berusky (C) AnakreoN
 * Martin Stransky <stransky@anakreon.cz> 
 * Michal Simonik <simonik@anakreon.cz> 
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
#include <stdio.h>
#include <stdlib.h>
#include <AL/alut.h>
#include "compat_mini.h"

#define NUM_BUFFERS 3
#define NUM_SOURCES 3
#define NUM_ENVIRONMENTS 1

static ALfloat listenerPos[]={0.0,0.0,0.0};
static ALfloat listenerVel[]={0.0,0.0,0.0};
static ALfloat listenerOri[]={0.0,0.0,1.0, 0.0,1.0,0.0};

ALfloat pos[]={ 0.0, 0.0, 0.0};
ALfloat vel[]={ 0.0, 0.0, 0.0};

static ALuint	buffer[NUM_BUFFERS];
static ALuint	source[NUM_SOURCES];
static ALuint environment[NUM_ENVIRONMENTS];

static ALsizei size,freq;
static ALenum 	format;
static ALvoid 	*data;
static int 	ch;

#define HOME_DIR "/home/komat/Projekty/berusky2/data/"

void init(int ini)
{
  if(ini) {
    alutInit(0, NULL);
  }

  alListenerfv(AL_POSITION,listenerPos);
  alListenerfv(AL_VELOCITY,listenerVel);
  alListenerfv(AL_ORIENTATION,listenerOri);
  
  alGetError(); // clear any error messages

  // Generate buffers, or else no sound will happen!
  alGenBuffers(NUM_BUFFERS, buffer);

  if(alGetError() != AL_NO_ERROR) {
      printf("- Error creating buffers !!\n");
      exit(1);
  }
  
  alutLoadWAVFile(HOME_DIR"1.wav",&format,&data,&size,&freq,0);
  alBufferData(buffer[0],format,data,size,freq);
  alutUnloadWAV(format,data,size,freq);

  alutLoadWAVFile(HOME_DIR"2.wav",&format,&data,&size,&freq,0);
  alBufferData(buffer[1],format,data,size,freq);
  alutUnloadWAV(format,data,size,freq);

  alutLoadWAVFile(HOME_DIR"3.wav",&format,&data,&size,&freq,0);
  alBufferData(buffer[2],format,data,size,freq);
  alutUnloadWAV(format,data,size,freq);

  alGetError(); /* clear error */
  alGenSources(NUM_SOURCES, source);

  if(alGetError() != AL_NO_ERROR) {
      printf("- Error creating sources !!\n");
      exit(2);
  }

  int i;
  for(i = 0; i < NUM_SOURCES; i++) {
    alSourcef(source[i],AL_PITCH,1.0f);
    alSourcef(source[i],AL_GAIN,1.0f);
    alSourcefv(source[i],AL_POSITION,pos);
    alSourcefv(source[i],AL_VELOCITY,vel);
    alSourcei(source[i],AL_BUFFER,buffer[i]);
    alSourcei(source[i],AL_LOOPING,AL_TRUE);
  }
}

void alut_test(int ini)
{
  kprintf(TRUE,"Alut_test...");

	init(ini);

  alSourcePlay(source[0]);
  alSourcePlay(source[1]);
  alSourcePlay(source[2]);

  Sleep(2000);

  alSourceStop(source[2]);
  alSourceStop(source[1]);
  alSourceStop(source[0]);
}
