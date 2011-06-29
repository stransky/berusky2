/*
  Modul s texturama - Load BMP
*/
#include "3d_all.h"
#include "Texture_import.h"
#include "Tga.h"

bitmapa * surface2bmp(SURFACE_SDL *p_surf);

/*
int uloz_aux(FILE *f, AUX_RGBImageRec *p_bmp)
{
  BITMAPFILEHEADER head;
  BITMAPINFOHEADER info;
  dword sirka,sirka_q,vyska,bpp = 3;
  int n;
    
  sirka = p_bmp->sizeX;
  vyska = p_bmp->sizeY;

  assert(sirka && vyska);

  sirka_q = sirka*bpp + ((sirka*bpp%4) ? sirka*bpp%4 : 0);
  n = sirka_q*vyska;

  // Ulozeni bitmapy
  memset(&head,0,sizeof(head));
  memset(&info,0,sizeof(info));

  head.bfType = 'MB';
  head.bfSize = sizeof(head)+sizeof(info)+sirka_q*vyska;
  head.bfOffBits = sizeof(info)+sizeof(head);

  info.biSize = sizeof(info);
  info.biWidth = sirka;
  info.biHeight = vyska;
  info.biPlanes = 1;
  info.biBitCount = (unsigned short)bpp*8;
  info.biCompression = BI_RGB;
  info.biSizeImage = n;
  
  fwrite(&head,sizeof(head),1,f);
  fwrite(&info,sizeof(info),1,f);
  fwrite(p_bmp->data,sizeof(byte),n,f);

  return(TRUE);
}

int uloz_aux_pack(FFILE f, AUX_RGBImageRec *p_bmp)
{

  BITMAPFILEHEADER head;
  BITMAPINFOHEADER info;
  dword sirka,sirka_q,vyska,bpp = 3;
  int n;
    
  sirka = p_bmp->sizeX;
  vyska = p_bmp->sizeY;

  assert(sirka && vyska);

  sirka_q = sirka*bpp + ((sirka*bpp%4) ? sirka*bpp%4 : 0);
  n = sirka_q*vyska;

  // Ulozeni bitmapy
  memset(&head,0,sizeof(head));
  memset(&info,0,sizeof(info));

  head.bfType = 'MB';
  head.bfSize = sizeof(head)+sizeof(info)+sirka_q*vyska;
  head.bfOffBits = sizeof(info)+sizeof(head);

  info.biSize = sizeof(info);
  info.biWidth = sirka;
  info.biHeight = vyska;
  info.biPlanes = 1;
  info.biBitCount = (unsigned short)bpp*8;
  info.biCompression = BI_RGB;
  info.biSizeImage = n;
  
  ffwrite(&head,sizeof(head),1,f);
  ffwrite(&info,sizeof(info),1,f);
  ffwrite(p_bmp->data,sizeof(byte),n,f);

  return(TRUE);
}
*/

bitmapa * txt_alfa2bmp(bitmapa *p_bmp, bitmapa *p_alfa)
{  
  int       x,y,dx,dy;  
  
  if(p_bmp->x != p_alfa->x || p_bmp->y != p_alfa->y) {
    kprintf(TRUE,"txt_alfa2bmp: wrong alpha texture size! (%dx%d)",p_alfa->x,p_alfa->y);
    abort();
  }
    
  dx = p_alfa->x;
  dy = p_alfa->y;

  for(y = 0; y < dy; y++) {
    for(x = 0; x < dx; x++) {
      dword src_a = bmp_getpixel(p_alfa,x,y);
      dword src_b = bmp_getpixel(p_bmp,x,y);
      bmp_putpixel(p_bmp, x, y, byte_a(src_b,(int)(rgb_intensity(src_a)*255)));
    }
  }
  return(p_bmp);
}

bitmapa * bmp_scale(bitmapa *p_bmp, int nx, int ny)
{  
  if(nx == p_bmp->x && ny == p_bmp->y) {    
    return(bmp_kopituj(p_bmp));
  } else {     
    if(nx < 1)
      nx = 1;
    if(ny < 1)
      ny = 1;
    
    {
      bitmapa *p_new = bmp_vyrob(nx,ny);
      dword *p_src,*p_dest;
      float  px,py,dx,dy;
      int    x,y,ys;
      
      dx = (float)p_bmp->x/(float)nx;
      dy = (float)p_bmp->y/(float)ny;
      p_src = p_bmp->data;
      py = 0;
      
      for(y = 0; y < ny; y++) {
        ys = ftoi(py)*p_bmp->x;
        px = 0;
        p_dest = p_new->data+y*nx;
        for(x = 0; x < nx; x++) {
          p_dest[x] = p_src[ys+ftoi(px)];
          px += dx;
        }
        py += dy;
      }
      return(p_new);
    }
  }
}

void txt_build_2D_mip_map(bitmapa *p_bmp, int format)
{
  bitmapa *p_scs;
  int x = p_bmp->x,
      y = p_bmp->y,    
      i = 0;
  
  while(x > 0 || y > 0) {
    p_scs = bmp_scale(p_bmp,x,y);
    glTexImage2D(GL_TEXTURE_2D, i, format, p_scs->x, p_scs->y, 0, GL_RGBA, GL_UNSIGNED_BYTE, p_scs->data);
    bmp_zrus(&p_scs);
    x /= 2;
    y /= 2;
    i++;
  }
}

/* Prevod vcetne alfa mapy
*/
bitmapa * txt_bmp2textura(bitmapa *p_bmp, EDIT_TEXT_OLD *p_text, EDIT_TEXT_KONFIG *p_konf, int _2d_text)
{ 
  int dx,dy,x,y,scale = FALSE;
  int wrap;

  if(p_konf) {
    p_text->typ = p_konf->typ;
  }
  
  glGenTextures(1, &p_text->text);  
  glBindTexture(p_text->typ, p_text->text);
  
  if(p_konf) {
    p_text->mip       = p_konf->mip;
    p_text->mip_filtr = p_konf->mip_filtr;
    p_text->format    = p_konf->format;
    p_text->std_filtr = p_konf->std_filtr;
    p_text->wrap_x    = p_konf->wrap_x;
    p_text->wrap_y    = p_konf->wrap_y;    
    p_text->alfa_stage= p_konf->alfa_stage;

    p_text->trida = txconf.text_trida;
    p_text->bump  = (txconf.text_bump[txconf.text_trida] && p_konf->bump);
    p_text->floyd = p_konf->floyd;
    p_text->floyd_transp = p_konf->floyd_transp;
    p_text->no_cull = p_konf->no_cull;
  }

  x = txconf.text_max_x;
  y = txconf.text_max_y;
  
  if(p_bmp->x <= x) 
    x = p_bmp->x;
  else
    scale = TRUE;
  
  if(p_bmp->y <= y)
    y = p_bmp->y;
  else
    scale = TRUE;
  
  for(dx = 2; dx < txconf.text_max_x && x > dx; dx <<= 1);
  for(dy = 2; dy < txconf.text_max_y && y > dy; dy <<= 1);
  
  if(x != dx || y != dy) {
    x = dx; y = dy;
    scale = TRUE;
  }
  
  if(txconf.text_detail[p_text->trida] > 0) {
    scale = TRUE;
    x >>= txconf.text_detail[p_text->trida];
    y >>= txconf.text_detail[p_text->trida];
  }
  
  if(!_2d_text && scale) {
    bitmapa *p_scs = bmp_scale(p_bmp,x,y);
    bmp_zrus(&p_bmp);
    p_bmp = p_scs;
  }  
    
  if(p_text->mip) {
    if(p_text->typ == GL_TEXTURE_2D) {
      txt_build_2D_mip_map(p_bmp, p_text->format);
    } else if(p_text->typ == GL_TEXTURE_1D) {
      gluBuild1DMipmaps(GL_TEXTURE_1D,p_text->format,p_bmp->x, GL_RGBA, GL_UNSIGNED_BYTE, p_bmp->data);
    }    
    glTexParameteri(p_text->typ,GL_TEXTURE_MIN_FILTER, (p_text->mip_filtr) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(p_text->typ,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  } else {
    if(p_text->typ == GL_TEXTURE_2D) {
      glTexImage2D(GL_TEXTURE_2D, 0, p_text->format, p_bmp->x, p_bmp->y, 0, GL_RGBA, GL_UNSIGNED_BYTE, p_bmp->data);
    } else if(p_text->typ == GL_TEXTURE_1D) {
      glTexImage1D(GL_TEXTURE_1D, 0, p_text->format, p_bmp->x, 0, GL_RGBA, GL_UNSIGNED_BYTE, p_bmp->data);
    }
    
    if(!p_text->std_filtr) {
      glTexParameteri(p_text->typ,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
      glTexParameteri(p_text->typ,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    } else {
      glTexParameteri(p_text->typ,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
      glTexParameteri(p_text->typ,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    }
  }

  if(txconf.text_anis) {
    glTexParameterfv(p_text->typ, GL_TEXTURE_MAX_ANISOTROPY_EXT, &txconf.text_anis_level);
  }

  switch(p_text->wrap_x) {
  case 0:
    if(gl_ext::extlist_text_edge_clamp) {
      wrap = GL_CLAMP_TO_EDGE;
    } else {
      wrap = GL_CLAMP;
    }
    break;
  case 1:
    wrap = GL_REPEAT;
    break;
  }
  glTexParameteri(p_text->typ,GL_TEXTURE_WRAP_S,wrap);
  
  switch(p_text->wrap_y) {
  case 0:
    if(gl_ext::extlist_text_edge_clamp) {
      wrap = GL_CLAMP_TO_EDGE;
    } else {
      wrap = GL_CLAMP;
    }
    break;
  case 1:
    wrap = GL_REPEAT;
    break;
  }
  glTexParameteri(p_text->typ,GL_TEXTURE_WRAP_T,wrap);

  return(p_bmp);
}

// filtr - Linear/Nearest
// bpp - 16/32
int txt_vyrob_2D_texturu(int x, int y, int filtr, int format)
{ 
  int      wrap,text;
  bitmapa *p_bmp;
  
  glGenTextures(1,(GLuint*)&text);
  glBindTexture(GL_TEXTURE_2D,text);
  
  p_bmp = bmp_vyrob(x,y);

  glTexImage2D(GL_TEXTURE_2D, 0, format, p_bmp->x, p_bmp->y, 0, GL_RGBA, GL_UNSIGNED_BYTE, p_bmp->data);
  bmp_zrus(&p_bmp);
  
  if(filtr){
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  } else {    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  }

  if(gl_ext::extlist_text_edge_clamp) {
    wrap = GL_CLAMP_TO_EDGE;
  } else {
    wrap = GL_CLAMP;
  }
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wrap);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wrap);

  return(text);
}

int txt_kopiruj(EDIT_TEXT_OLD *p_dest, EDIT_TEXT_OLD *p_src)
{
  *p_dest = *p_src;
  p_dest->load = FALSE;
  p_dest->p_bmp = bmp_kopituj(p_src->p_bmp);
  return(TRUE);
}

int txt_reload_lightmap(EDIT_TEXT_OLD *p_text, bitmapa *p_bmp)
{ 
  EDIT_TEXT_KONFIG cnf;  
  
  txt_lightmap_konfig(&cnf);
  glBindTexture(p_text->typ, p_text->text);
  glTexSubImage2D(p_text->typ, 0, 0, 0, p_bmp->x, p_bmp->y,
                  GL_RGBA, GL_UNSIGNED_BYTE, p_bmp->data);
  return(TRUE);
}

// load ok
int txt_nahraj_format(APAK_HANDLE *pAHandle, const char *p_file, EDIT_TEXT_KONFIG *p_text)
{
  char  jmeno[200];
  char *p_znak;
  int   format = -1;
  KFILE *f;

  strcpy(jmeno,p_file);
  zamen_koncovku(jmeno,KONCOVKA_TEXT_INFO);  
  
  p_text->alfa = kefile(pAHandle, p_text->alfamap);
  p_text->alfa_stage = K_CHYBA;
    
  if((f = kopen(pAHandle,jmeno,(char *)"r"))) {
    while(kgets(jmeno,200,f)) {
      fgets_korekce(jmeno);
      if(jmeno[0] == ';')
        continue;
      // Bump-mapping
      if(strstr(jmeno,"nobump")) {
        p_text->bump = FALSE;
        continue;
      }        
      // culling
      if(strstr(jmeno,"no_cull")) {
        p_text->no_cull = TRUE;
        continue;
      }        
      // Floyd-steinberg
      if(strstr(jmeno,"floyd")) {
        p_text->floyd = TRUE;
        continue;
      }
      // Floyd-steinberg s pruhlednyma
      if(strstr(jmeno,"floyd_transp")) {
        p_text->floyd_transp = TRUE;
        continue;
      }
      if((p_znak = strchr(jmeno,'='))) {
        *p_znak++ = 0;
        if(strstr(jmeno,"format")) {
          if(!preloz_texture_format(&format, p_znak)) {
            //kprintfl(TRUE,"Texture format %d",format);
          //} else {
            kprintfl(TRUE,"Bad texture format %s",p_znak);
          }
          continue;
        }        
        // Repeat/clamp mody
        if(strstr(jmeno,"wrap_x")) {
          p_text->wrap_x = atoi(p_znak);
          continue;
        }
        // Repeat/clamp mody
        if(strstr(jmeno,"wrap_y")) {
          p_text->wrap_y = atoi(p_znak);
          continue;
        }
        // Mip-mapping
        if(strstr(jmeno,"mip_map")) {
          p_text->mip = atoi(p_znak);
          continue;
        }
        // Typ 1D/2D
        if(strstr(jmeno,"typ")) {
          p_text->typ = (atoi(p_znak) == 1) ? GL_TEXTURE_1D : GL_TEXTURE_2D;
          continue;
        }
        // alfa-stage
        if(strstr(jmeno,"alfa_stage")) {
          p_text->alfa_stage = atoi(p_znak);
          continue;
        }
      }
    }
    kclose(f);
  } 

  if(p_text->typ == GL_TEXTURE_1D) {
    p_text->format = (format != K_CHYBA) ? txconf.format1d[format] : (p_text->alfa) ? txconf.format1d[FORMAT_RGBA1] : txconf.format1d[FORMAT_RGB];
  } else {
    p_text->format = (format != K_CHYBA) ? txconf.format2d[format] : (p_text->alfa) ? txconf.format2d[FORMAT_RGBA1] : txconf.format2d[FORMAT_RGB];
  }
  
  return(TRUE);
}

void txt_default_konfig(char *p_file, EDIT_TEXT_KONFIG *p_konf, char *p_koncovka)
{
  char pom[200] = "_a";

  strcpy(p_konf->bitmapa,p_file);
  strcpy(p_konf->alfamap,p_file);
  zamen_koncovku(p_konf->bitmapa,p_koncovka);
  strcat(pom,p_koncovka);
  zamen_koncovku(p_konf->alfamap,pom);

  p_konf->typ = GL_TEXTURE_2D;
  p_konf->format = txconf.format2d[FORMAT_RGB];
  
  p_konf->mip = txconf.text_mip_mapping;
  p_konf->mip_filtr = txconf.text_mip_filtr;

  p_konf->std_filtr = txconf.text_filtr;

  p_konf->wrap_x = txconf.text_wrap_x;
  p_konf->wrap_y = txconf.text_wrap_y;

  p_konf->bump = TRUE;

  p_konf->alfa_stage = K_CHYBA;
  p_konf->floyd = FALSE;
  
  p_konf->no_cull = FALSE;
}

void txt_lightmap_konfig(EDIT_TEXT_KONFIG *p_konf)
{
  p_konf->typ = GL_TEXTURE_2D;
  p_konf->format = txconf.format2d[FORMAT_LIGHTMAPY];
  p_konf->std_filtr = txconf.text_filtr;
  p_konf->mip = FALSE;
  p_konf->mip_filtr = FALSE;
  p_konf->wrap_x = 0;
  p_konf->wrap_y = 0;
  p_konf->bump = FALSE;
}

/* Textury
  +scale textur, je-li to potreba
*/
int txt_nahraj_texturu_z_func(APAK_HANDLE *pHandle, const char *p_file,
                              EDIT_TEXT_OLD *p_text, int save, int load,
                              EDIT_TEXT_KONFIG *p_konf,
                              bitmapa * (*p_load)(APAK_HANDLE *pAHandle, char *p_file))
{
  EDIT_TEXT_KONFIG konf;
  
  txt_default_konfig(p_file,&konf,cti_koncovku(p_file));
  txt_nahraj_format(pHandle,p_file,&konf);  

  if(!konf.alfa) {
    if(!(p_text->p_bmp = p_load(pHandle,konf.bitmapa)))
      return(FALSE);
    p_text->load = TRUE;
  } else {
    bitmapa *p_tmp = p_load(pHandle,konf.bitmapa);
    bitmapa *p_alf = p_load(pHandle,konf.alfamap);
    if(!p_tmp || !p_alf) {      
    /*
      if(!p_tmp)
        kprintf(TRUE,"txt_nahraj_texturu_z_func(): Error, unable to load %s",konf.bitmapa);
      if(!p_alf)
        kprintf(TRUE,"txt_nahraj_texturu_z_func(): Error, unable to load %s",konf.alfamap);
    */
      bmp_zrus(&p_tmp);
      bmp_zrus(&p_alf);
      p_text->load = FALSE;
      return(FALSE);
    }
    p_text->load = TRUE;
    p_text->p_bmp = p_tmp;
    p_text->p_bmp = txt_alfa2bmp(p_text->p_bmp, p_alf);
    bmp_zrus(&p_alf);
  }
  
  if(p_text->flag&TEXT_GEN_DOT3) {
    bitmapa *p_dot;
    p_dot = txt_bmp2dot3(p_text->p_bmp);
    bmp_zrus(&p_text->p_bmp);
    p_text->p_bmp = p_dot;
  }

  if(load) {
    p_text->p_bmp = txt_bmp2textura(p_text->p_bmp, p_text, &konf, FALSE);
  }

  if(p_konf)
    *p_konf = konf;

  if(!save) {
    bmp_zrus(&p_text->p_bmp);
  }

  return(p_text->load);
}

int txt_to_dot3(EDIT_TEXT_OLD *p_text, int save)
{
  bitmapa *p_dot;  
  p_dot = txt_bmp2dot3(p_text->p_bmp);
  bmp_zrus(&p_text->p_bmp);
  p_text->p_bmp = txt_bmp2textura(p_dot, p_text, NULL, FALSE);  
  p_text->load = TRUE;
  if(!save) {
    bmp_zrus(&p_text->p_bmp);
  }
  return(TRUE);
}

int txt_nahraj_texturu_do_vram(EDIT_TEXT_OLD *p_text, EDIT_TEXT_KONFIG *p_konf)
{
  return(p_text->p_bmp = txt_bmp2textura(p_text->p_bmp, p_text, p_konf, TRUE),TRUE);
}

int txt_zrus_texturu_ram(EDIT_TEXT_OLD *p_text)
{
  bmp_zrus(&p_text->p_bmp);
  return(TRUE);
}

int txt_nahraj_lightmapu_z_bmp(char *p_file, KFILE *f, EDIT_TEXT_OLD *p_text, int save)
{
  EDIT_TEXT_KONFIG konf;
      
  txt_lightmap_konfig(&konf);

  if(p_file) {
    if(!(p_text->p_bmp = bmp_nahraj(NULL, p_file)))
      return(FALSE);
  } else {
    if(!(p_text->p_bmp = bmp_nahraj(f)))
      return(FALSE);
  }
  p_text->load = TRUE;
  p_text->p_bmp = txt_bmp2textura(p_text->p_bmp, p_text, &konf, FALSE);
  
  if(!save) {
    bmp_zrus(&p_text->p_bmp);
  }
  return(p_text->load);
}

int txt_nahraj_texturu_z_dds(APAK_HANDLE *pHandle, char *p_file,  EDIT_TEXT_OLD *p_text, int save)
{
  /*
  EDIT_TEXT_KONFIG konf;  
  AUX_RGBImageRec *p_tmp = NULL;
  void            *p_vysl;
  int              ret;
    
  if((ret = txt_dds_to_bmp(pHandle,p_file,&p_vysl))) {
    konf.mip = txconf.text_mip_mapping;
    konf.mip_filtr = txconf.text_mip_filtr;
    konf.wrap_x = GL_REPEAT;
    konf.wrap_y = GL_REPEAT;
    if(ret == 3) {
      konf.format = txconf.format2d[FORMAT_RGB];
      p_tmp = (AUX_RGBImageRec *)p_vysl;
      p_text->p_bmp = txt_aux2bmp(p_tmp,FALSE);
      zrus_aux(&p_tmp);
    } else {
      konf.format = txconf.format2d[FORMAT_RGBA4];  
      p_text->p_bmp = (bitmapa *)p_vysl;
    }
    p_text->p_bmp = txt_bmp2textura(p_text->p_bmp, p_text, &konf, FALSE);
    p_text->load = TRUE;
    if(!save) {
      bmp_zrus(&p_text->p_bmp);
    }
  }
  */
  assert(0);
  return(p_text->load);
}

int txt_nahraj_texturu_z_tga(APAK_HANDLE *pHandle, char *p_file,  EDIT_TEXT_OLD *p_text)
{
  int              ret;
  
  if((ret = tgaLoad(pHandle, p_file, FALSE, GL_DUDV_ATI))) {
    p_text->load = TRUE;
    p_text->text = ret;
    p_text->p_bmp = NULL;
    strcpy(p_text->jmeno,p_file);

    glTexParameterf(p_text->typ, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(p_text->typ, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(p_text->typ, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(p_text->typ, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  return(p_text->load);
}

int txt_zrus_2D_texturu(int *p_text)
{
  if(glIsTexture(*p_text)) {
    glDeleteTextures(1,(GLuint *)p_text);
    *p_text = 0;
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int txt_zrus_texturu(EDIT_TEXT_OLD *p_text)
{
  int ret;
  
  ret = glIsTexture(p_text->text);
  if(ret) {
    glDeleteTextures(1,&p_text->text);
  } 
  
  memset(p_text,0,sizeof(p_text[0]));
  return(ret);
}

int txt_zrus_lightmapu(EDIT_TEXT_OLD *p_text)
{
  int ret;
  
  ret = glIsTexture(p_text->text);
  if(ret) {    
    glDeleteTextures(1,&p_text->text);
  } 

  if(p_text->p_bmp)
    bmp_zrus(&p_text->p_bmp);
  
  memset(p_text,0,sizeof(p_text[0]));
  return(ret);
}

int txt_zrus_texturu_z_vram(EDIT_TEXT_OLD *p_text)
{
  int ret;
  
  ret = glIsTexture(p_text->text);
  if(ret) {
    glDeleteTextures(1,&p_text->text);
    p_text->text = 0;
  } 

  return(ret);
}

/***************************************************************** 
  RGBA Bitmapy
 *****************************************************************
*/
bitmapa * bmp_vyrob(int x, int y)
{
  bitmapa *p_bmp;
  
  p_bmp = (bitmapa*)mmalloc(sizeof(*p_bmp));
  p_bmp->x = x;
  p_bmp->y = y;
  
  p_bmp->data = (dword*)mmalloc(sizeof(dword)*x*y);
  return(p_bmp);
}

void bmp_zrus(bitmapa **p_bmp)
{  
  if(*p_bmp) {
    free((*p_bmp)->data);
    free((*p_bmp));
    *p_bmp = NULL;
  } 
}

bitmapa * bmp_kopituj(bitmapa *p_bmp)
{
  if(p_bmp) {
    bitmapa *p_new = bmp_vyrob(p_bmp->x, p_bmp->y);
    memcpy(p_new->data,p_bmp->data,sizeof(dword)*p_bmp->x*p_bmp->y);
    return(p_new);
  } else {
    return(NULL);
  }
}

int bmp_uloz(char *p_file, bitmapa *p_bmp)
{
/*
 FILE *f = fopen(p_file,"wb");
 if(f) {
   int vel;
   AUX_RGBImageRec *p_aux = bmp2aux(p_bmp); 
   byte *p_buffer = ConvertRGBToBMPBuffer(p_aux->data,p_bmp->x,p_bmp->y,&vel);
   SaveBMP(f,p_buffer,p_bmp->x,p_bmp->y,vel);
   fclose(f);
   free(p_buffer);
   zrus_aux(&p_aux);
 }
*/
 assert(0);
 return(TRUE);
}

int bmp_uloz_pack(FFILE f, bitmapa *p_bmp)
{
/*
 AUX_RGBImageRec *p_aux = bmp2aux(p_bmp);
 uloz_aux_pack(f,p_aux);
 zrus_aux(&p_aux);   
*/
 assert(0);
 return(TRUE);
}

bitmapa * bmp_nahraj(APAK_HANDLE *pAHandle, char *p_file)
{
  byte   *p_mem;
  int     vel;

  p_mem = file_read(pAHandle,p_file,&vel);
  if(!p_mem)
    return(NULL);
      
  SURFACE_SDL srf(IMG_Load_RW(SDL_RWFromMem((void *)p_mem, vel), TRUE),SURFACE_TEXTURE,TRUE);
  bitmapa *p_bmp = surface2bmp(&srf);
  
  free(p_mem);
  return(p_bmp);
}

int rwseek(struct SDL_RWops *context, int offset, int whence)
{
  return(kseek((KFILE *)context->hidden.unknown.data1, offset, whence));
}

int rwread(struct SDL_RWops *context, void *ptr, int size, int maxnum)
{
  return(kread(ptr,size,maxnum,(KFILE *)context->hidden.unknown.data1));
}

int rwwrite(struct SDL_RWops *context, const void *ptr, int size, int num)
{
  return(kwrite((void *)ptr,size,num,(KFILE *)context->hidden.unknown.data1));
}

int rwclose(struct SDL_RWops *context)
{
  assert(0);
}

bitmapa * bmp_nahraj(KFILE *f)
{
  SDL_RWops *rw_ops = SDL_AllocRW();
  rw_ops->hidden.unknown.data1 = (void *)f;
  rw_ops->seek = rwseek;
  rw_ops->read = rwread;
  rw_ops->write = rwwrite;
  rw_ops->close = rwclose;

  SURFACE_SDL srf(IMG_Load_RW(rw_ops, FALSE),SURFACE_TEXTURE,TRUE);
  bitmapa *p_bmp = surface2bmp(&srf);
  SDL_FreeRW(rw_ops);

  return(p_bmp);
}

bitmapa * bmp_nahraj(char *p_file)
{     
  SURFACE_SDL srf(IMG_Load(p_file),SURFACE_TEXTURE,TRUE);
  bitmapa *p_bmp = surface2bmp(&srf);
  return(p_bmp);
}

bitmapa * bmp_prehod(bitmapa *p_bmp)
{
  bitmapa *p_src = bmp_kopituj(p_bmp);
  tpos y;
  tpos dx,
       dy;

  bmp_cti_rozmery(p_src,&dx,&dy);

  for(y = 0; y < dy; y++) {    
    memcpy(bmp_getpixel_ref(p_bmp, 0, y),
           bmp_getpixel_ref(p_src, 0, dy-y-1),
           sizeof(dword)*dx);
  }

  bmp_zrus(&p_src);
  return(p_bmp);
}

/* Prevede standartni lightmapu na dot3 lightmapu
*/
bitmapa * txt_bmp2dot3(bitmapa *p_bmp)
{
  bitmapa *p_dot = bmp_kopituj(p_bmp);
  bitmapa_pixel pix;
  float    dX, dY, nX, nY, nZ, oolen;
  int      x, y;

  for(y = 0; y < p_bmp->y; y++) {
    for(x = 0; x < p_bmp->x; x++) {
      // Do Y Sobel filter
      pix = bmp_getpixel_pix(p_bmp, (x-1+p_bmp->x) % p_bmp->x, (y+1) % p_bmp->y);
      dY  = ((float) pix.r) / 255.0f * -1.0f;
      
      pix = bmp_getpixel_pix(p_bmp,   x   % p_bmp->x, (y+1) % p_bmp->y);
      dY += ((float) pix.r) / 255.0f * -2.0f;
      
      pix = bmp_getpixel_pix(p_bmp, (x+1) % p_bmp->x, (y+1) % p_bmp->y);
      dY += ((float) pix.r) / 255.0f * -1.0f;
      
      pix = bmp_getpixel_pix(p_bmp, (x-1+p_bmp->x) % p_bmp->x, (y-1+p_bmp->y) % p_bmp->y);
      dY += ((float) pix.r) / 255.0f *  1.0f;
      
      pix = bmp_getpixel_pix(p_bmp,   x   % p_bmp->x, (y-1+p_bmp->y) % p_bmp->y);
      dY += ((float) pix.r) / 255.0f *  2.0f;
      
      pix = bmp_getpixel_pix(p_bmp, (x+1) % p_bmp->x, (y-1+p_bmp->y) % p_bmp->y);
      dY += ((float) pix.r) / 255.0f *  1.0f;
      
      // Do X Sobel filter
      pix = bmp_getpixel_pix(p_bmp, (x-1+p_bmp->x) % p_bmp->x, (y-1+p_bmp->y) % p_bmp->y);
      dX  = ((float) pix.r) / 255.0f * -1.0f;
      
      pix = bmp_getpixel_pix(p_bmp, (x-1+p_bmp->x) % p_bmp->x,   y   % p_bmp->y);
      dX += ((float) pix.r) / 255.0f * -2.0f;
      
      pix = bmp_getpixel_pix(p_bmp, (x-1+p_bmp->x) % p_bmp->x, (y+1) % p_bmp->y);
      dX += ((float) pix.r) / 255.0f * -1.0f;
      
      pix = bmp_getpixel_pix(p_bmp, (x+1) % p_bmp->x, (y-1+p_bmp->y) % p_bmp->y);
      dX += ((float) pix.r) / 255.0f *  1.0f;
      
      pix = bmp_getpixel_pix(p_bmp, (x+1) % p_bmp->x,   y   % p_bmp->y);
      dX += ((float) pix.r) / 255.0f *  2.0f;
      
      pix = bmp_getpixel_pix(p_bmp, (x+1) % p_bmp->x, (y+1) % p_bmp->y);
      dX += ((float) pix.r) / 255.0f *  1.0f;
      
      
      // Cross Product of components of gradient reduces to
      nX = -dX;
      nY = -dY;
      nZ = 1;
      
      // Normalize
      oolen = 1.0f/((float) sqrt(nX*nX + nY*nY + nZ*nZ));
      nX *= oolen;
      nY *= oolen;
      nZ *= oolen;
      
      pix.r = (byte) PackFloatInByte(nX);
      pix.g = (byte) PackFloatInByte(nY);
      pix.b = (byte) PackFloatInByte(nZ);
      
      add_color_rgb_byte((byte *)&pix,60,60,60);
      bmp_putpixel_pix(p_dot, x, y, pix);
    }
  }
  return(p_dot);
}

byte * file_read(APAK_HANDLE *pHandle, char *p_file, int *p_read)
{
  char   *p_buffer;
  char   *pBuffer;
  apuLong psize;
  FILE   *f;
  int     v;

  if(pHandle) {
    f = aopen(pHandle,p_file,"rb");
    if(!f)
      return(NULL);
    agetbuffer(f, &pBuffer, &psize);
    *p_read = psize;
    p_buffer = (char *)mmalloc(sizeof(byte)*psize);
    memmove(p_buffer,pBuffer,sizeof(byte)*psize);
    aclose(f);
    return((byte *)p_buffer);
  } else {
    f = fopen(p_file,"rb");
    if(!f)
      return(NULL);
    fseek(f,0,SEEK_END);
    v = ftell(f);
    fseek(f,0,SEEK_SET);
    p_buffer = (char *)mmalloc(sizeof(byte)*v);
    *p_read = fread(p_buffer,sizeof(byte),v,f);
    fclose(f);
    return((byte *)p_buffer);
  }
}

bitmapa * surface2bmp(SURFACE_SDL *p_surf)
{
  int   dx = p_surf->width_get(),
        dy = p_surf->height_get();
  int   x,y;

  bitmapa *p_vysl = bmp_vyrob(dx, dy);

  p_surf->lock();
  
  for(y = 0; y < dy; y++) {
    for(x = 0; x < dx; x++) {
      tcolor color = p_surf->pixel_get(x, dy-y-1)&0xffffff;
      bmp_putpixel(p_vysl, x, y, color);
    }
  }

  p_surf->unlock();

  return(p_vysl);
}

int txt_uloz_btx(char *p_file, int typ, int wrap_x, int wrap_y)
{
  FILE *f = fopen(p_file,"w");
  if(!f)
    return(FALSE);
  fprintf(f,"wrap_x=%d\n",wrap_x);
  fprintf(f,"wrap_y=%d\n",wrap_y);
  fprintf(f,"typ=%d\n",typ == GL_TEXTURE_1D ? 1 : 2);
  fclose(f);
  return(TRUE);
}

/* Nastavi tridu nahravanych textur
*/
void txt_trida(int trida)
{
  txconf.text_trida = trida;
}

/* Filtrace bmp pomoci floyd-stemberga -> prevod na 16K
   do 8,8,8 -> 5,5,5
*/
#define MASKA 0x7
#define PRAH  (MASKA>>1)

inline void bmp_pix_to_16b(byte *p_baze, int *p_er, int *p_eg, int *p_eb)
{ 
 *p_er = (p_baze[0]&MASKA)-PRAH;
 *p_eg = (p_baze[1]&MASKA)-PRAH;
 *p_eb = (p_baze[2]&MASKA)-PRAH;
  
  p_baze[0]&=~MASKA;
  p_baze[1]&=~MASKA;
  p_baze[2]&=~MASKA;
}

inline void add_posun_rgba(byte *p_baze, int r, int g, int b, int posun)
{
  r = (r < 0) ? p_baze[0]-((-r)>>posun) : p_baze[0]+(r>>posun);
  g = (g < 0) ? p_baze[1]-((-g)>>posun) : p_baze[1]+(g>>posun);
  b = (b < 0) ? p_baze[2]-((-b)>>posun) : p_baze[2]+(b>>posun);
  
  if(r > MAX_BYTE) r = MAX_BYTE;
  if(g > MAX_BYTE) g = MAX_BYTE;
  if(b > MAX_BYTE) b = MAX_BYTE;

  if(r < 0) r = 0;
  if(g < 0) g = 0;
  if(b < 0) b = 0;

  p_baze[0] = r;
  p_baze[1] = g;
  p_baze[2] = b;
}

/*
255, 0, 255
0, 0, 0
54, 47, 45
237, 77, 0
238, 77, 0
*/


#define BARVA1 (RGB(255,0,255))
#define BARVA2 (RGB(238,77,0))
#define BARVA3 (RGB(237,77,0))
#define BARVA4 (RGB(0,0,0))
#define BARVA5 (RGB(54,47,45))

#define pruhledna_barva(pruhl, barva) (barva = barva&0xffffff, (pruhl && (barva == BARVA1 || barva == BARVA2 || barva == BARVA3 || barva == BARVA4 ||barva == BARVA5)))

void bmp_to_16bit(bitmapa *p_bmp, byte maska, int pruhl)
{
  dword *p_pix;
  dword  barva;
  int    r,g,b;
  int    x,y,
         dx = p_bmp->x,
         dy = p_bmp->y;

  for(y = 0; y < dy; y++) {
    p_pix = p_bmp->data+(y*p_bmp->x);
    for(x = 0; x < dx; x++, p_pix++) {
      barva = *p_pix;
      
      if(pruhledna_barva(pruhl,barva))
        continue;

      bmp_pix_to_16b((byte *)p_pix, &r, &g, &b);
            
      if(x+1 < dx) {
        barva = p_pix[1];
        if(!pruhledna_barva(pruhl,barva))
          add_posun_rgba((byte *)(p_pix+1),r,g,b,1);
      }
      
      if(y+1 < dy) {
        p_pix += dx;
        barva = *p_pix;
        
        if(!pruhledna_barva(pruhl,barva))
          add_posun_rgba((byte *)(p_pix),r,g,b,2);
        
        if(x+1 < dx) {
          barva = p_pix[1];
          if(!pruhledna_barva(pruhl,barva))
            add_posun_rgba((byte *)(p_pix+1),r,g,b,4);
        }
        
        if(x > 0) {
          barva = p_pix[-1];
          if(!pruhledna_barva(pruhl,barva))
            add_posun_rgba((byte *)(p_pix-1),r,g,b,3);
        }
        
        p_pix -= dx;
      }
    }
  }
}
