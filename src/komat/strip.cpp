/* Strip interface :-(((
   Skurvena NVIDA a to jeji posrany c++
*/

//#include "mss_on.h"
#include <windows.h>
#include "strip.h"
#include "NvTriStrip.h"
#include "assert.h"
#include "mmalloc.h"

extern "C" int NvGenerateStrips(unsigned short *p_in_indices, unsigned int   in_numIndices,
                     STRIP_FACE     **p_facer, unsigned short *p_facenum)
{
  STRIP_FACE     *p_face;
  PrimitiveGroup *p_primGroups;
  unsigned short  numGroups;
  int             i,j,num;

  GenerateStrips(p_in_indices,in_numIndices,&p_primGroups,&numGroups);

  if(!numGroups)
    return(0);

  p_face = (STRIP_FACE *)mmalloc(sizeof(p_face[0])*numGroups);
  if(!p_face)
    return(NULL);

  for(i = 0; i < numGroups; i++) {
    num = p_primGroups[i].numIndices;
    p_face[i].facenum = num;
    p_face[i].typ = p_primGroups[i].type;
    p_face[i].p_face = (unsigned short *)mmalloc(sizeof(p_face[0].p_face[0])*num);
    if(p_face[i].p_face) {
      for(j = 0; j < num; j++) {
        p_face[i].p_face[j] = p_primGroups[i].indices[j];
      }
    }
  }

  *p_facer = p_face;
  *p_facenum = numGroups;

  delete[] p_primGroups;
  return(1);
}


