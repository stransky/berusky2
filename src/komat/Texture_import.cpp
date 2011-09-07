/* Import textur
*/
#include "3d_all.h"
static KFILE *file_load;


/*
  Importni interface soubory pro textury DDS
  vraci pocet plan -> 3/4
*/
int txt_dds_to_bmp(APAK_HANDLE * pHandle, char *p_file, void **p_return)
{

/*
  unsigned char   *p_data;
  unsigned char   *p_rd;
  AUX_RGBImageRec *p_rec;
  RBITMAP         *p_bmp;
  byte            *p_src,r;
  int              width;
  int              height;
  int              planes;
  int              lTotalWidth; 
  int              rowBytes;
  int              x,y,dl;

  if((file_load = kopen(pHandle,p_file,"rb")) == NULL) {
    return(FALSE);
  }

  p_data = nvDXTdecompress(width, height, planes, lTotalWidth, rowBytes);

  assert(width == lTotalWidth);
  
  // prehodi r<->b
  for(y = 0; y < height; y ++ ) {    
    for(x = 0; x < width; x++) {
      p_src = p_data+(y*width*planes)+x*planes;
      r = p_src[2];
      p_src[2] = p_src[0];
      p_src[0] = r;                  
    }
  }
  // prehodi radky
  dl = sizeof(byte)*width*planes;
  p_rd = (byte *)malloc(dl*height);
  for(y = 0; y < height; y++ ) {
    memcpy(p_rd+(height-y-1)*dl,p_data+y*dl,dl);
  }
  memcpy(p_data,p_rd,dl*height);

  if(planes == 3) {
    p_rec = (AUX_RGBImageRec *)malloc(sizeof(*p_rec));
    assert(p_rec);      
    p_rec->sizeX = width;
    p_rec->sizeY = height;
    p_rec->data = p_data;
    *p_return = p_rec;
  } else {
    p_bmp = (RBITMAP *)malloc(sizeof(*p_bmp));
    assert(p_bmp);      
    p_bmp->x = width;
    p_bmp->y = height;
    p_bmp->data = (DWORD *)p_data;
    *p_return = p_bmp;
  }

  free(p_rd);
  kclose(file_load);
  return(planes);
  */
  *p_return = NULL;
  return (0);
}

void WriteDTXnFile(DWORD count, void *buffer)
{
} void ReadDTXnFile(DWORD count, void *buffer)
{
  kread(buffer, sizeof(byte), count, file_load);
}
