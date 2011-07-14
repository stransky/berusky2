/* Strip interface :-(((
   Skurvena NVIDA a to jeji posrany c++
   Je to banda vymastenejch vymrdancu!
*/

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __STRIP_H__
#define __STRIP_H__

#define ST_LIST   0
#define ST_STRIP  1
#define ST_FAN    2

  typedef struct _STRIP_FACE
  {

    int typ;
    unsigned int facenum;
    unsigned short *p_face;

  } STRIP_FACE;

  int NvGenerateStrips(unsigned short *p_in_indices,
    unsigned int in_numIndices, STRIP_FACE ** p_facer,
    unsigned short *p_facenum);

#endif

#ifdef __cplusplus
}
#endif
