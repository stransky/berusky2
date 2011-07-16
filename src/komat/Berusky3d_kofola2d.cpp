/* --------------------------------------------------
   Rozhrani k 2D grafice
   --------------------------------------------------

  - vypnuti renderingu hry
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include "3d_all.h"

#include "Object.h"

#include "Berusky_universal.h"
#include "Berusky3d_castice.h"
#include "Berusky3d.h"
#include "Berusky3d_ini.h"
#include "Berusky3d_load.h"
#include "Berusky3d_render.h"
#include "Berusky3d_animace.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky3d_light.h"
#include "Berusky3d_kamery.h"

#include "Berusky3d_kofola2d.h"

#define  MAX_TEXT_BUFFER_X 2048
#define  MAX_TEXT_BUFFER_Y 2048

/*
  Alfa = 0 - viditelne
       = 1 - ne-viditelne
*/

#define  COLOR_MASKA        0x00ffffff
#define  PRUHL_BARVA        (0xff000000|slist.pruhledna_barva)

#define  bmp_nepruhl(barva) (barva&0x00ffffff)
#define  nepruhl(barva)     (barva&0x00ffffff)

#define  VELIKOST_PIXELU    (sizeof(dword))

static DDX2_SURFACE_DEVICE *p_dev;
static DDX2_SURFACE_DEVICE *p_dev_list;
static DDX2_SURFACE_LIST slist;
static int ddx2InitDone = FALSE;

#define  vysledna_barva(barva) ((nepruhl(barva) == slist.pruhledna_barva) ? PRUHL_BARVA : nepruhl(barva))

inline bitmapa *get_bmp(SurfaceHandle handle)
{
  if (handle == DDX2_BACK_BUFFER) {
    return (p_dev->p_back_buffer);
  }
  else {
    assert(handle < slist.surf_max);
    return (slist.p_slist[handle].p_bmp);
  }
}

// Kofolovo
extern RECT_LINE rline;

//------------------------------------------------------------------------------------------------
// Name: ddx2Init()
// Desc: Vyrobi rezervoar na surfacy
//------------------------------------------------------------------------------------------------
int ddx2Init(int max_surfacu, dword pruhledna_barva)
{
  if (!ddx2InitDone) {
    slist.p_slist =
      (DDX2_SURFACE *) mmalloc(sizeof(slist.p_slist[0]) * max_surfacu);
    slist.surf_max = max_surfacu;
    slist.pruhledna_barva = pruhledna_barva;
    slist.surf_num = 0;
    ddx2InitDone = TRUE;
  }
  return (TRUE);
}

//------------------------------------------------------------------------------------------------
// Name: ddx2Release()
// Desc: Zrusi rezervovar na surfacy
//------------------------------------------------------------------------------------------------
void ddx2Release(void)
{
  if (ddx2InitDone) {
    int i;

    for (i = 0; i < slist.surf_max; i++) {
      ddx2ReleaseBitmap(i);
    }
    null_free((void **) &slist.p_slist);
    ddx2InitDone = FALSE;
  }
}

//-----------------------------------------------------------------------------
// Name: ddx2DeviceCreate()
// Desc: Vyrobi renderovaci device a vlozi ho to seznamu devicu
//       filtr - TRUE - linear, FALSE - near
//       bpp - 16/32
//-----------------------------------------------------------------------------
DeviceHandle ddx2DeviceCreate(int linear_filtr, int bpp)
{
  DDX2_SURFACE_DEVICE *p_tmp =
    (DDX2_SURFACE_DEVICE *) mmalloc(sizeof(p_tmp[0]));
  p_tmp->p_next = p_dev_list;
  p_dev_list = p_tmp;
  p_tmp->hw.filtr = linear_filtr;
  p_tmp->hw.format = (bpp == 16) ? GL_RGB5_A1 : GL_RGBA8;
  return ((DeviceHandle) p_tmp);
}

//-----------------------------------------------------------------------------
// Name: ddx2DeviceSetActive()
// Desc: Nastavi renderovaci device jako aktivni
//-----------------------------------------------------------------------------
DeviceHandle ddx2DeviceSetActive(DeviceHandle handle)
{
  if (handle != K_CHYBA)
    p_dev = (DDX2_SURFACE_DEVICE *) handle;
  return (handle);
}

//-----------------------------------------------------------------------------
// Interni funkce
//-----------------------------------------------------------------------------
void ddx2DeviceRemoveRec(DDX2_SURFACE_DEVICE ** p_rm)
{
  DDX2_SURFACE_DEVICE *p_dev = *p_rm;

  if (p_dev) {
    DDX2_SURFACE_DEVICE_3D *p_hw = &p_dev->hw;

    if (p_hw->text)
      txt_zrus_2D_texturu(&p_hw->text);
    bmp_zrus(&p_dev->p_back_buffer);
    free(p_dev);
    p_rm = NULL;
  }
}

//-----------------------------------------------------------------------------
// Name: ddx2DeviceRemove()
// Desc: Zrusi zarizeni, pokud je aktivni nastavi aktivni prvni v seznamu
//-----------------------------------------------------------------------------
DeviceHandle ddx2DeviceRemove(DeviceHandle handle)
{
  DDX2_SURFACE_DEVICE *p_rm = (DDX2_SURFACE_DEVICE *) handle;
  DDX2_SURFACE_DEVICE *p_tmp = p_dev_list;
  int reset = (p_rm == p_dev);

  if (p_dev_list == p_rm) {
    p_dev_list = p_rm->p_next;
    ddx2DeviceRemoveRec(&p_rm);
    if (reset) {
      p_dev = p_dev_list;
    }
    return (TRUE);
  }
  else {
    while (p_tmp) {
      if (p_tmp->p_next == p_rm) {
        p_tmp->p_next = p_tmp->p_next->p_next;
        ddx2DeviceRemoveRec(&p_rm);
        if (reset) {
          p_dev = p_dev_list;
        }
        return (TRUE);
      }
    }
    return (FALSE);
  }
}

//-----------------------------------------------------------------------------
// Name: ddx2DeviceResBackBuffer(int back_dx, int back_dy)
// Desc: Nastavi rozmer back-bufferu
//-----------------------------------------------------------------------------
DeviceHandle ddx2DeviceSetBackBufferSize(int back_dx, int back_dy)
{
  if (p_dev) {
    DDX2_SURFACE_DEVICE_3D *p_hw = &p_dev->hw;

    p_hw->back_dx = back_dx;
    p_hw->back_dy = back_dy;

    if (p_dev->p_back_buffer)
      bmp_zrus(&p_dev->p_back_buffer);

    p_dev->p_back_buffer = bmp_vyrob(p_hw->back_dx, p_hw->back_dy);
//    MSS_SET_BLOCK_LABEL(p_dev->p_back_buffer, "back-buffer");

    if (p_dev->p_back_buffer)
      ddx2CleareSurface(DDX2_BACK_BUFFER);

    return ((DeviceHandle) p_dev);
  }
  else
    return (FALSE);
}

//-----------------------------------------------------------------------------
// Name: ddx2DeviceSetBackBufferRect(int text_x, int text_y, int text_dx, int text_dy)
// Desc: Nastavi pozici a rozmery textury v back-bufferu
//-----------------------------------------------------------------------------
DeviceHandle ddx2DeviceSetBackBufferRect(int text_x, int text_y, int text_dx,
  int text_dy)
{
  if (p_dev) {
    DDX2_SURFACE_DEVICE_3D *p_hw = &p_dev->hw;
    int ttdx, ttdy;

    if (text_dx > MAX_TEXT_BUFFER_X && text_dy > MAX_TEXT_BUFFER_Y)
      return (FALSE);

    p_hw->text_x = text_x;
    p_hw->text_y = text_y;

    p_hw->text_dx = text_dx;
    p_hw->text_dy = text_dy;

    if (p_hw->text)
      txt_zrus_2D_texturu(&p_hw->text);

    for (ttdx = 2; ttdx < MAX_TEXT_BUFFER_X && ttdx < p_hw->text_dx;
      ttdx <<= 1);
    for (ttdy = 2; ttdy < MAX_TEXT_BUFFER_X && ttdy < p_hw->text_dx;
      ttdy <<= 1);

    p_hw->text_real_dx = ttdx;
    p_hw->text_real_dy = ttdy;

    p_hw->text = txt_vyrob_2D_texturu(ttdx, ttdy, p_hw->filtr, p_hw->format);

    return ((DeviceHandle) p_dev);
  }
  else
    return (FALSE);
}



#define pomer(p,min,max) (((float)p-(float)min)/((float)max-(float)min))

//-----------------------------------------------------------------------------
// Name: ddx2DeviceSetTextRenderRec(int vx, int vy, int v_dx, int v_dy)
// Desc: Nastavi oblast textury, ktera se bude prenaset na obrazovku
//-----------------------------------------------------------------------------
DeviceHandle ddx2DeviceSetTextRenderRec(int vx, int vy, int v_dx, int v_dy)
{
  if (p_dev) {
    DDX2_SURFACE_DEVICE_3D *p_hw = &p_dev->hw;
    BODUV *p_v = p_hw->map;
    int tdx, tdy;

    tdx = p_hw->text_real_dx;
    tdy = p_hw->text_real_dy;

    if (vx < 0 || vy < 0 || v_dx > tdx || v_dy > tdy)
      return (FALSE);

    p_v->u = pomer(vx, 0, tdx);
    p_v->v = pomer(vy, 0, tdy);
    p_v++;

    p_v->u = pomer(vx + v_dx, 0, tdx);
    p_v->v = pomer(vy, 0, tdy);
    p_v++;

    p_v->u = pomer(vx + v_dx, 0, tdx);
    p_v->v = pomer(vy + v_dy, 0, tdy);
    p_v++;

    p_v->u = pomer(vx, 0, tdx);
    p_v->v = pomer(vy + v_dy, 0, tdy);
    p_v++;

    return ((DeviceHandle) p_dev);
  }
  else
    return (FALSE);
}


//-----------------------------------------------------------------------------
// Name: ddx2DeviceSetScreenRec(int scr_x, int scr_y, int scr_dx, int scr_dy)
// Desc: Nastavi pozici textury na obrazovce (umisteni + rozmer)
//-----------------------------------------------------------------------------
DeviceHandle ddx2DeviceSetScreenRec(int scr_x, int scr_y, int scr_dx,
  int scr_dy)
{
  if (p_dev) {
    DDX2_SURFACE_DEVICE_3D *p_hw = &p_dev->hw;


    p_hw->scr_x = scr_x;
    p_hw->scr_y = scr_y;

    p_hw->scr_kx = scr_x + scr_dx;
    p_hw->scr_ky = scr_y + scr_dy;

    return ((DeviceHandle) p_dev);
  }
  else
    return (FALSE);
}

//-----------------------------------------------------------------------------
// Name: ddx2DeviceSetRender(int draw)
// Desc: Povoli/zakaze kresleni device
//-----------------------------------------------------------------------------
DeviceHandle ddx2DeviceSetRender(int draw)
{
  if (p_dev) {
    p_dev->draw = draw;
    return ((DeviceHandle) p_dev);
  }
  else {
    return ((DeviceHandle) FALSE);
  }
}

//-----------------------------------------------------------------------------
// Name: ddx2DeviceDeaktivuj()
// Desc: Odmapuje aktivni zarizeni zobrazovky (smaze texturu a pod.)
//-----------------------------------------------------------------------------
DeviceHandle ddx2DeviceDeaktivuj(void)
{
  if (p_dev) {
    DDX2_SURFACE_DEVICE_3D *p_hw = &p_dev->hw;

    bmp_zrus(&p_dev->p_back_buffer);
    txt_zrus_2D_texturu(&p_hw->text);

    return ((DeviceHandle) p_dev);
  }
  else {
    return ((DeviceHandle) FALSE);
  }
}

//-----------------------------------------------------------------------------
// Name: ddx2DeviceGetInfo()
// Desc: Cte informace o device - vraci pocet surfacu v aktivnim device,
//       velikost obsazene pameti
//-----------------------------------------------------------------------------
DeviceHandle ddx2DeviceGetInfo(int *p_surfacu, int *p_mem)
{
  int i, s, num;

  if (p_dev) {
    for (i = 0, s = 0, num = 0; i < slist.surf_max; i++) {
      if (slist.p_slist[i].p_bmp) {
        s += bmp_velikost(slist.p_slist[i].p_bmp);
        num++;
      }
    }
    *p_surfacu = num;
    *p_mem = s;
    return ((DeviceHandle) p_dev);
  }
  else {
    return (FALSE);
  }
}

//-----------------------------------------------------------------------------
// Name: ddx2SetRect()
// Desc: Nastavi seznam recu na kresleni, NULL = nic nekreslit
//-----------------------------------------------------------------------------
void ddx2SetRect(RECT * p_rlist, int rnum)
{
  p_dev->p_rlist = p_rlist;
  p_dev->rnum = rnum;
}

// ----------------------------------------------------------------------------
// Interni funkce
// ----------------------------------------------------------------------------
void ddx2RenderujDevice(G_KONFIG * p_ber, DDX2_SURFACE_DEVICE * p_dev)
{
  DDX2_SURFACE_DEVICE_3D *p_hw = &p_dev->hw;
  RECT *p_rlist = p_dev->p_rlist;
  int i, rx, ry, rkx, rky;
  int tx, ty, tkx, tky, vdx, vdy;
  dword *p_text_start;
  BODUV *p_v = p_hw->map;

  glBindTexture(GL_TEXTURE_2D, p_hw->text);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, p_dev->p_back_buffer->x);

  if (p_rlist && p_dev->rnum) {
  
    tx = p_hw->text_x;
    ty = p_hw->text_y;
  
    tkx = tx + p_hw->text_dx;
    tky = ty + p_hw->text_dy;
  
    p_text_start = p_dev->p_back_buffer->data;
  
    for (i = 0; i < p_dev->rnum; i++, p_rlist++) {
  
      rx = p_rlist->left;
      ry = p_rlist->top;
      rkx = rx + p_rlist->right;
      rky = ry + p_rlist->bottom;
  
      if (rx < tx)
        rx = tx;
      if (ry < ty)
        ry = ty;
      if (rx > tkx)
        rx = tkx;
      if (ry > tky)
        ry = tky;
  
      if (rkx < tx)
        rkx = tx;
      if (rky < ty)
        rky = ty;
      if (rkx > tkx)
        rkx = tkx;
      if (rky > tky)
        rky = tky;
  
      vdx = rkx - rx;
      vdy = rky - ry;
  
      if (vdx && vdy) {
        glTexSubImage2D(GL_TEXTURE_2D,    // target
          0,                      // level (MIP map?)
          rx - tx, ry - ty,       // xoffset / yoffset
          vdx, vdy,               // width / height
          GL_RGBA,                // format
          GL_UNSIGNED_BYTE,       // data type
          p_text_start + rx + ry * p_hw->back_dx);        // pixel data
      }
    }
  }

  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

  glBegin(GL_QUADS);
  glTexCoord2fv((float *) (p_v + 3));
  glVertex2i(p_hw->scr_x, p_hw->scr_ky);

  glTexCoord2fv((float *) (p_v + 2));
  glVertex2i(p_hw->scr_kx, p_hw->scr_ky);

  glTexCoord2fv((float *) (p_v + 1));
  glVertex2i(p_hw->scr_kx, p_hw->scr_y);

  glTexCoord2fv((float *) (p_v));
  glVertex2i(p_hw->scr_x, p_hw->scr_y);
  glEnd();
}


//------------------------------------------------------------------------------------------------
// Nastavi pruhlednost
// 1 - nepruhledne
// 0 - plne pruhledne
//------------------------------------------------------------------------------------------------
void ddx2SetPruhlednost(float stupen)
{
  /*
     if(stupen >= 1.0f) {
     glColor4f(1,1,1,1);
     glDisable(GL_BLEND);
     glEnable(GL_ALPHA_TEST);
     glAlphaFunc(GL_LEQUAL,0);
     } else {
     glColor4f(1,1,1,stupen);
     glEnable(GL_ALPHA_TEST);
     glAlphaFunc(GL_LEQUAL,0);
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
     }
   */
}

/*

bila (1) nepruhledna
cerna (0) pruhledna

glColor4f(1,1,1,1);
glDisable(GL_DEPTH_TEST);
glEnable(GL_BLEND);

*/

//------------------------------------------------------------------------------------------------
// zacatek 3d kresleni
//------------------------------------------------------------------------------------------------
void ddx2StartRender2D(void)
{
  set_matrix_2d(OXRES, OYRES);
  glColor4f(1, 1, 1, 1);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_LEQUAL, 0);
  specular_off();
  specular_color(0, 0, 0);
}

//------------------------------------------------------------------------------------------------
// konec 3d kresleni
//------------------------------------------------------------------------------------------------
void ddx2StopRender2D(void)
{
  glDisable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.5f);
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  ret_matrix_2d();
}

// ----------------------------------------------------------------------------
// Name: ddx2RenderujMenu()
// Desc: Vykresli menu
//       Vola se pokud se menu ma kreslit jako soucast hry (volat po renderingu
//       menu s listim) - vykresli pouze menu a nic jineho
// ----------------------------------------------------------------------------
void ddx2RenderujMenu(G_KONFIG * p_ber)
{
  DDX2_SURFACE_DEVICE *p_tmp = p_dev_list;

  if (p_tmp) {
    ddx2StartRender2D();
    while (p_tmp) {
      if (p_tmp->draw)
        ddx2RenderujDevice(p_ber, p_tmp);
      p_tmp = p_tmp->p_next;
    }
    ddx2StopRender2D();
  }
}


// ----------------------------------------------------------------------------
// Name: ddx2RenderujVse()
// Desc: Vykresli menu
//       Kresli menu vcetne vymazani obrazovky
// ----------------------------------------------------------------------------
void ddx2RenderujVse(G_KONFIG * p_ber)
{
  glClear(GL_DEPTH_BUFFER_BIT);
  if (p_ber->conf_barva_pozadi_pouzit)
    glClear(GL_COLOR_BUFFER_BIT);

  /* Vypnuti scale-faktoru texturoveho filtru
   */
  text_sharp(0.0f);

  /* Update fps
   */
  ber_updatuj_fps(p_ber);

  /* Kofolovo nastaveni
   */
  text_set_num(0);
  text_on(GL_TEXTURE_2D);
  text_set_blok(0, BLOK_MODULACE);

  /* 2D rendering
   */
  ddx2RenderujMenu(p_ber);
}

//------------------------------------------------------------------------------------------------
// finds first free index
//------------------------------------------------------------------------------------------------
int ddx2FindFreeSurface(void)
{
  int i;

  if (!slist.p_slist[slist.surf_num].p_bmp) {
    return (slist.surf_num++);
  }
  for (i = 0; i < slist.surf_max; i++) {
    if (!slist.p_slist[i].p_bmp)
      return (i);
  }
  // We don't have a free surface
  assert(0);
  return (K_CHYBA);
}

//------------------------------------------------------------------------------------------------
// release bitmap
//------------------------------------------------------------------------------------------------
SurfaceHandle ddx2ReleaseBitmap(SurfaceHandle iSurface)
{
  assert(iSurface < slist.surf_max);
  if (slist.p_slist[iSurface].p_bmp) {
    bmp_zrus(&slist.p_slist[iSurface].p_bmp);
  }
  return (iSurface);
}

//------------------------------------------------------------------------------------------------
// load bitmap from APAK na pozici
//------------------------------------------------------------------------------------------------
SurfaceHandle ddx2LoadBitmapPos(SurfaceHandle handle, char *pFileName,
  APAK_HANDLE * pHandle)
{
  if (handle != K_CHYBA) {
    assert(handle < slist.surf_max);
    ddx2ReleaseBitmap(handle);
    if ((slist.p_slist[handle].p_bmp = bmp_nahraj(pHandle, pFileName))) {
      // for compatibility with old broken bmp loading
      bmp_prehod(slist.p_slist[handle].p_bmp);
      return (handle);
    }
  }

  kprintf(TRUE, "FILE %s LINE %d Chyba loadu bmp %s...", __FILE__, __LINE__,
    pFileName);
  assert(0);

  return (K_CHYBA);
}

//------------------------------------------------------------------------------------------------
// load bitmap from DISK na pozici
//------------------------------------------------------------------------------------------------
SurfaceHandle ddx2LoadBitmapPosDisk(SurfaceHandle handle, char *pFileName)
{
  if (handle != K_CHYBA) {
    assert(handle < slist.surf_max);
    ddx2ReleaseBitmap(handle);
    if ((slist.p_slist[handle].p_bmp = bmp_nahraj(pFileName))) {
      // for compatibility with old broken bmp loading
      bmp_prehod(slist.p_slist[handle].p_bmp);
      return (handle);
    }
  }

  kprintf(TRUE, "FILE %s LINE %d Chyba loadu bmp %s...", __FILE__, __LINE__,
    pFileName);
  assert(0);

  return (K_CHYBA);
}

//------------------------------------------------------------------------------------------------
// load bitmap from APAK
//------------------------------------------------------------------------------------------------
SurfaceHandle ddx2LoadBitmap(char *pFileName, APAK_HANDLE * pHandle)
{
  return (ddx2LoadBitmapPos(ddx2FindFreeSurface(), pFileName, pHandle));
}

//------------------------------------------------------------------------------------------------
// load bitmap from DISK
//------------------------------------------------------------------------------------------------
SurfaceHandle ddx2LoadBitmapDisk(char *pFileName)
{
  return (ddx2LoadBitmapPosDisk(ddx2FindFreeSurface(), pFileName));
}

//------------------------------------------------------------------------------------------------
// load list of bitmaps from APAK
//------------------------------------------------------------------------------------------------
BOOL ddx2LoadList(char *pFileName, APAK_HANDLE * pBmpArchive, char *p_bmp_dir)
{
  int c;
  char text[256];
  FILE *file = 0;
  DWORD Eplased = 0;
  DWORD Start, Stop;

  achdir(pBmpArchive, p_bmp_dir);

  kprintf(1, (char *) "Kofola: - Load bitmap pro herni menu");
  file = aopen(pBmpArchive, pFileName, "rb");
  if (!file) {
    kprintf(1, (char *) "File not found : %s", pFileName);
    konec(TRUE);
  }

  c = 0;
  Start = timeGetTime();
  while (!aeof(file)) {
    agets(text, 256, file);
    if (!aeof(file)) {
      text[strlen(text) - 1] = '\0';
      ddx2LoadBitmapPos(c, text, pBmpArchive);
    }
    c++;
  }

  aclose(file);

  Stop = timeGetTime();
  kprintf(1,
    (char *) "--------------Total load time %.1f s -----------------",
    Eplased / 1000.0f);

  return (TRUE);
}

//------------------------------------------------------------------------------------------------
// Create Surface
//------------------------------------------------------------------------------------------------
SurfaceHandle ddx2CreateSurface(int x, int y, int idx)
{
  assert(idx < slist.surf_max);
  if (idx > -1 && idx < slist.surf_max) {
    ddx2ReleaseBitmap(idx);

    slist.p_slist[idx].p_bmp = bmp_vyrob(x, y);

    if (!(slist.p_slist[idx].p_bmp)) {
      kprintf(TRUE, "FILE %s LINE %d vyrob bmp..." __FILE__, __LINE__);
      konec(TRUE);
    }

    if (slist.p_slist[idx].p_bmp)
      ddx2CleareSurface(idx);
    return (slist.p_slist[idx].p_bmp ? idx : K_CHYBA);
  }
  else {
    return (K_CHYBA);
  }
}

//------------------------------------------------------------------------------------------------
// nakresli obrazek
// com[1] je index v poli surafacu a 
// com[2],[3] je x,y kam se to ma kreslit
//------------------------------------------------------------------------------------------------
void ddx2DrawSurfaceColorKey(int iSurface, int *com, int layer,
  COLORREF color)
{
  bitmapa *p_src = get_bmp(com[1]);

  ddx2TransparentBlt(iSurface, com[2], com[3], p_src->x, p_src->y,
    com[1], 0, 0, p_src->x, p_src->y, color);
}

void ddx2DrawDisplayColorKey(int *com, int layer, COLORREF color)
{
  bitmapa *p_src = get_bmp(com[1]);

  ddx2TransparentBlt(DDX2_BACK_BUFFER, com[2], com[3], p_src->x, p_src->y,
    com[1], 0, 0, p_src->x, p_src->y, color);
}

// ----------------------------------------------------------
// Kresli do surface
// ----------------------------------------------------------
void ddx2DrawSurface(int iSurface, int *com, int layer)
{
  RECT r;

  bitmapa *p_src = get_bmp(com[1]);

  ddx2BitBlt(iSurface, com[2], com[3], p_src->x, p_src->y, com[1], 0, 0);

  r.left = com[2];
  r.top = com[3];
  r.right = p_src->x;
  r.bottom = p_src->y;

  ddx2AddRectItem(&rline, r, layer);
}

// ----------------------------------------------------------
// Kresli na displey
// ----------------------------------------------------------
void ddx2DrawDisplay(int *com, int layer)
{
  bitmapa *p_src = get_bmp(com[1]);

  ddx2BitBlt(DDX2_BACK_BUFFER, com[2], com[3], p_src->x, p_src->y, com[1], 0,
    0);
}

// color to make transparent
BOOL ddx2TransparentBlt(SurfaceHandle dst, int dx, int dy, int dsirka,
  int dvyska, SurfaceHandle src, int sx, int sy, int ssirka, int svyska,
  dword pruhledna)
{
  bitmapa *p_src = get_bmp(src);
  bitmapa *p_dst = get_bmp(dst);
  int sirka_src = p_src->x;
  int sirka_dst = p_dst->x;
  int rozdil_src = sirka_src - ssirka;
  int rozdil_dst = sirka_dst - ssirka;
  int src_x = sy * sirka_src + sx;
  int dst_x = dy * sirka_dst + dx;
  dword *p_src_data = p_src->data + src_x;
  dword *p_dst_data = p_dst->data + dst_x;
  dword barva;
  int x, y;

  if (dst == DDX2_BACK_BUFFER) {
    RECT r;

    r.left = dx;
    r.top = dy;
    r.right = dsirka;
    r.bottom = dvyska;

    ddx2AddRectItem(&rline, r, 0);
  }

  assert(dx < p_dst->x && dy < p_dst->y);
  assert(sx < p_src->x && sy < p_src->y);

  assert(dx + dsirka <= p_dst->x && dy + dvyska <= p_dst->y);
  assert(sx + ssirka <= p_src->x && sy + svyska <= p_src->y);

  pruhledna = bmp_nepruhl(pruhledna);

  for (y = 0; y < svyska; y++) {
    for (x = 0; x < ssirka; x++) {
      barva = bmp_nepruhl(*p_src_data);
      if (barva != pruhledna) {
        *p_dst_data = barva;
      }
      p_dst_data++;
      p_src_data++;
    }
    p_dst_data += rozdil_dst;
    p_src_data += rozdil_src;
  }

  return (TRUE);
}

BOOL ddx2TransparentBltFull(SurfaceHandle dst, int dx, int dy,
  SurfaceHandle src, dword barva)
{
  bitmapa *p_src = get_bmp(src);

  return (ddx2TransparentBlt(dst, dx, dy, p_src->x, p_src->y,
      src, 0, 0, p_src->x, p_src->y, barva));
}

BOOL ddx2TransparentBltDisplay(int dx, int dy, int dsirka, int dvyska,
  int dcSrcSurface, int sx, int sy, int ssirka, int svyska,
  UINT crTransparent)
{
  return (ddx2TransparentBlt(DDX2_BACK_BUFFER, dx, dy, dsirka, dvyska,
      dcSrcSurface, sx, sy, ssirka, svyska, crTransparent));
}

BOOL ddx2BitBlt(SurfaceHandle dst, int dx, int dy, int sirka, int vyska,
  SurfaceHandle src, int sx, int sy)
{
  bitmapa *p_src = get_bmp(src);
  bitmapa *p_dst = get_bmp(dst);
  int sirka_src = p_src->x;
  int sirka_dst = p_dst->x;
  int src_x = sy * sirka_src + sx;
  int dst_x = dy * sirka_dst + dx;
  dword *p_src_data = p_src->data + src_x;
  dword *p_dst_data = p_dst->data + dst_x;
  int y;

  if (dst == DDX2_BACK_BUFFER) {
    RECT r;

    r.left = dx;
    r.top = dy;
    r.right = sirka;
    r.bottom = vyska;

    ddx2AddRectItem(&rline, r, 0);
  }

  assert(dx < p_dst->x && dy < p_dst->y);
  assert(sx < p_src->x && sy < p_src->y);

  assert(dx + sirka <= p_dst->x && dy + vyska <= p_dst->y);
  assert(sx + sirka <= p_src->x && sy + vyska <= p_src->y);

  for (y = 0; y < vyska; y++) {
    memmove(p_dst_data, p_src_data, sizeof(p_dst_data[0]) * sirka);
    p_dst_data += sirka_dst;
    p_src_data += sirka_src;
  }

  return (TRUE);
}

BOOL ddx2BitBltFull(SurfaceHandle dst, int dx, int dy, SurfaceHandle src)
{
  bitmapa *p_src = get_bmp(src);

  return (ddx2BitBlt(dst, dx, dy, p_src->x, p_src->y, src, 0, 0));
}

BOOL ddx2BitBltDisplay(int dx, int dy, int sirka, int vyska,
  int dcSrcSurface, int sx, int sy)
{
  return (ddx2BitBlt(DDX2_BACK_BUFFER, dx, dy, sirka, vyska,
      dcSrcSurface, sx, sy));
}

int ddx2GetWidth(SurfaceHandle src)
{
  if (src == DDX2_BACK_BUFFER) {
    return (p_dev->hw.back_dx);
  }
  else {
    return (slist.p_slist[src].p_bmp->x);
  }
}

int ddx2GetHeight(SurfaceHandle src)
{
  if (src == DDX2_BACK_BUFFER) {
    return (p_dev->hw.back_dy);
  }
  else {
    return (slist.p_slist[src].p_bmp->y);
  }
}

void ddx2CleareSurfaceColor(SurfaceHandle iSurface, COLORREF color)
{
  bmp_smaz(get_bmp(iSurface), vysledna_barva(color));
}

void ddx2FillRect(SurfaceHandle iSurface, RECT * rect, COLORREF color)
{
  dword barva = vysledna_barva(color);
  bitmapa *p_src = get_bmp(iSurface);
  int sirka = rect->right - rect->left;
  int vyska = rect->bottom - rect->top;
  int sirka_src = p_src->x - sirka;
  dword *p_src_data = p_src->data + rect->top * (p_src->x) + rect->left;
  int x, y;

  assert(sirka <= p_src->x && vyska <= p_src->y);

  for (y = 0; y < vyska; y++) {
    for (x = 0; x < sirka; x++)
      *p_src_data++ = barva;
    p_src_data += sirka_src;
  }
}

void ddx2CleareSurface(SurfaceHandle iSurface)
{
  ddx2CleareSurfaceColor(iSurface, slist.pruhledna_barva);
}

void ddx2AddRectItem(RECT_LINE * p_rl, RECT rect, int iLayer)
{
  if (p_rl->rlast < DRAW_RECT_NUM) {
    p_rl->rect[p_rl->rlast].rect = rect;
    p_rl->rect[p_rl->rlast].iLayer = iLayer;
    p_rl->rect[p_rl->rlast].bUsed = 1;

    p_rl->rlast++;
  }
}

//------------------------------------------------------------------------------------------------
// Povoli/zakaze rendering hry
//------------------------------------------------------------------------------------------------
void ddx2GameRender(int render)
{
  p_ber->conf_menu_render = !render;
}

//------------------------------------------------------------------------------------------------
// Nastavi pruhlednost pro aktualni device
// 0 - nepruhledne
// 1 - plne pruhledne
//------------------------------------------------------------------------------------------------
void ddx2DeviceSetPruhlednost(float pruhlednost)
{


}
