/*
  3D interface
*/

#ifndef __3D_INTERFACE_H__
#define __3D_INTERFACE_H__

/*
  Stavovy stroj
*/
extern int glstav_blending;     // blending on-off
extern int glstav_cull;         // cull mod
extern int glstav_cull_mod;     // jaka strana se orezava
extern int glstav_lighting;     // gl svetla
extern int glstav_specular;     // spekularni svetla
extern int glstav_diffuse;      // diffusni svetla
extern int glstav_mlha;
extern int glstav_mlha_causal;
extern int glstav_array_diffuse;
extern int glstav_array_specular;
extern int glstav_array_normal;
extern int glstav_genenv_s[6];  // 1
extern int glstav_genenv_t[6];  // 2
extern int glstav_genenv_r[6];  // 3
extern int glstav_genenv_q[6];  // 4
extern int glstav_multitext_units;
extern int glstav_alfa_blok;
extern int glstav_alfa_test;
extern int glstav_text_blok[6];
extern int glstav_cliping;
extern int glstav_pn_triangles;
extern int glstav_deph_test;
extern int glstav_zmask;
extern int glstav_bump_mapping;
extern int glstav_zmena_kamery;

extern int glstav_text_akt;     // aktivni texury
extern int glstav_text_akt_client;      // aktivni klientska texura
extern int glstav_textury_1d[6];        // textury
extern int glstav_textury_2d[6];        // textury
extern GLuint glstav_last_text_1d[6];   // posledni textura
extern GLuint glstav_last_text_2d[6];   // posledni textura
extern int glstav_text_array[6];        // texturovaci pole
extern int glstav_text_array_akt;       // aktualni texturovaci pole
extern int glstav_text_map_indicie[6];  // mapovaci indicie pro textury (cislo mapy, cislo stavu)
extern int glstav_text_poly_indicie;    // mapovaci indicie pro poly (cislo mapy)

extern unsigned int glstav_posun_flagy2[6];
extern unsigned int glstav_texgen_on[6];
extern unsigned int glstav_texgen_eye_lin_flagy[6];
extern unsigned int glstav_texgen_sphere_flagy[6];
extern GLMATRIX glstav_texgen[6];

extern int glstav_view_x;
extern int glstav_view_y;
extern int glstav_view_dx;
extern int glstav_view_dy;

extern int glstav_matrix_2d_mod;

//#define STATE_BLOK_PRUHL      50 // zpruhlednovaci state blok
#define STATE_BLOK_PRUHL      50
#define STAGE_ALFA_FUNKCI     5
#define STAGE_TEXT_FUNKCI     9

extern STAGE_FUNC_ALFA p_alfa_stage_func[STAGE_ALFA_FUNKCI];
extern STAGE_FUNC p_text_stage_func[STAGE_TEXT_FUNKCI];
extern STAGE_FUNC_ELEM p_text_stage_func_elem[STAGE_TEXT_FUNKCI];

extern EDIT_STATE_ALFA_BLOK alfa_stage_edit_blok[STAGE_ALFA_FUNKCI + STATE_BLOK_PRUHL + 1];
extern EDIT_STATE_TEXT_BLOK text_stage_edit_blok[STAGE_TEXT_FUNKCI];

#define STAGE_ALFA_MASKA      4

#define BLOK_MODULACE         0
#define BLOK_ADD              1
#define BLOK_DECAL            2
#define BLOK_BLEND            3
#define BLOK_REPLACE          4
#define BLOK_DOT3_BUMP        5
#define BLOK_MODULACE2X       6
#define BLOK_BUMP_ENV         7

/* Define pro stage_funkce
*/
#define DOT3_BUMP             0
#define DOT3_TEXT             1
#define DOT3_ENV              2

#define DUDV_BUMP             0 // normalova mapa
#define DUDV_ENV              1 // Environment-mapa
#define DUDV_TEXT             2 // zakladni textura

/* Automaticky nahrazovani 
*/
#define AUTO_BUMP_DOT3        5 // dot3 color blok
#define AUTO_BUMP_DOT3ENV     6 // dot3 env blok


void nahod_state_bloky(void);

extern GLenum arb_prevodni_tabulka[6];

#define MAX_WORD_MATRIX_STACK 32

extern GLMATRIX __mat_kamera;
extern GLMATRIX __mat_world;
extern GLMATRIX __mat_vysl;
extern GLMATRIX __mat_camera_project;   // transformuje 3D do 2D
extern GLMATRIX __mat_init;
extern GLMATRIX __mat_top_matrix[MAX_WORD_MATRIX_STACK];        // druha word-matrix -> top-matrix
extern int __mat_top_matrix_akt;
extern GLMATRIX __mat_tmp;
extern int __mat_text[4];

#define TEXGEN_NIC          0
#define TEXGEN_SPHEREMAP    1
#define TEXGEN_EYE_LINEAR   2

void glstav_reset(void);

// matmult top_matrix*wordl*camera*project

/* Low-level matice
*/
inline GLMATRIX *set_matrix_texture(int num, GLMATRIX * p_text)
{
  glMatrixMode(GL_TEXTURE);
  glLoadMatrixf((float *) p_text);
  __mat_text[num] = TRUE;
  return (p_text);
}

inline void ret_matrix_texture(int num)
{
  if (__mat_text[num]) {
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    __mat_text[num] = FALSE;
  }
}

inline GLMATRIX *get_matrix_world_camera(void)
{
  return (&__mat_vysl);
}

inline GLMATRIX *set_matrix_world_top(GLMATRIX * p_top_world)
{
  if (__mat_top_matrix_akt + 1 < MAX_WORD_MATRIX_STACK) {

    if (!__mat_top_matrix_akt) {
      __mat_top_matrix[0] = *p_top_world;
    }
    else {
      mat_mult_dir(__mat_top_matrix + __mat_top_matrix_akt - 1, p_top_world,
        __mat_top_matrix + __mat_top_matrix_akt);
    }

    mat_mult_dir(&__mat_world, __mat_top_matrix + __mat_top_matrix_akt,
      &__mat_tmp);
    mat_mult_dir(&__mat_tmp, &__mat_kamera, &__mat_vysl);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float *) &__mat_vysl);

    __mat_top_matrix_akt++;

    return (p_top_world);
  }
  else {
    return (NULL);
  }
}

inline void ret_matrix_world_top(void)
{
  if (__mat_top_matrix_akt > 0) {
    __mat_top_matrix_akt--;
    if (__mat_top_matrix_akt) {
      mat_mult_dir(&__mat_world, __mat_top_matrix + __mat_top_matrix_akt - 1,
        &__mat_tmp);
      mat_mult_dir(&__mat_tmp, &__mat_kamera, &__mat_vysl);
    }
    else {
      mat_mult_dir(&__mat_world, &__mat_kamera, &__mat_vysl);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float *) &__mat_vysl);
  }
}

inline GLMATRIX *set_matrix_camera(GLMATRIX * p_cam)
{
  memcpy(&__mat_kamera, p_cam, sizeof(GLMATRIX));

  if (__mat_top_matrix_akt) {
    mat_mult_dir(&__mat_world, __mat_top_matrix + __mat_top_matrix_akt - 1,
      &__mat_tmp);
    mat_mult_dir(&__mat_tmp, &__mat_kamera, &__mat_vysl);
  }
  else {
    mat_mult_dir(&__mat_world, &__mat_kamera, &__mat_vysl);
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf((float *) &__mat_vysl);
  glstav_zmena_kamery = TRUE;

  return (p_cam);
}

inline GLMATRIX *get_matrix_camera(GLMATRIX * p_cam)
{
  memcpy(p_cam, &__mat_kamera, sizeof(GLMATRIX));
  return (p_cam);
}

inline GLMATRIX *get_matrix_camera_point(void)
{
  return (&__mat_kamera);
}

inline GLMATRIX *set_matrix_world(GLMATRIX * p_world)
{
  memcpy(&__mat_world, p_world, sizeof(GLMATRIX));

  if (__mat_top_matrix_akt) {
    mat_mult_dir(&__mat_world, __mat_top_matrix + __mat_top_matrix_akt - 1,
      &__mat_tmp);
    mat_mult_dir(&__mat_tmp, &__mat_kamera, &__mat_vysl);
  }
  else {
    mat_mult_dir(&__mat_world, &__mat_kamera, &__mat_vysl);
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf((float *) &__mat_vysl);

  return (p_world);
}

inline void set_matrix_world_init(void)
{
  memcpy(&__mat_world, &__mat_init, sizeof(GLMATRIX));

  if (__mat_top_matrix_akt) {
    mat_mult_dir(&__mat_world, __mat_top_matrix + __mat_top_matrix_akt - 1,
      &__mat_tmp);
    mat_mult_dir(&__mat_tmp, &__mat_kamera, &__mat_vysl);
  }
  else {
    mat_mult_dir(&__mat_world, &__mat_kamera, &__mat_vysl);
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf((float *) &__mat_vysl);
}

inline GLMATRIX *get_matrix_world(GLMATRIX * p_world)
{
  memcpy(p_world, &__mat_world, sizeof(GLMATRIX));
  return (p_world);
}

inline GLMATRIX *push_matrix_world(GLMATRIX * p_world)
{
  GLMATRIX tmp1;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  if (__mat_top_matrix_akt) {
    mat_mult_dir(p_world, __mat_top_matrix + __mat_top_matrix_akt - 1,
      &__mat_tmp);
    mat_mult_dir(&__mat_tmp, &__mat_kamera, &__mat_vysl);
  }
  else {
    mat_mult_dir(p_world, &__mat_kamera, &tmp1);
  }

  glLoadMatrixf((float *) &tmp1);
  return (p_world);
}

inline void pop_matrix_world(void)
{
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

inline void set_matrix_all(GLMATRIX * p_cam, GLMATRIX * p_world)
{
  memcpy(&__mat_kamera, p_cam, sizeof(GLMATRIX));
  memcpy(&__mat_world, p_world, sizeof(GLMATRIX));

  if (__mat_top_matrix_akt) {
    mat_mult_dir(&__mat_world, __mat_top_matrix + __mat_top_matrix_akt - 1,
      &__mat_tmp);
    mat_mult_dir(&__mat_tmp, &__mat_kamera, &__mat_vysl);
  }
  else {
    mat_mult_dir(&__mat_world, &__mat_kamera, &__mat_vysl);
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf((float *) &__mat_vysl);
  glstav_zmena_kamery = TRUE;
}

inline GLMATRIX *set_matrix_project(GLMATRIX * p_project)
{
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf((float *) p_project);
  return (p_project);
}

inline GLMATRIX *get_matrix_project(GLMATRIX * p_project)
{
  return (p_project);
}

inline GLMATRIX *set_matrix_project_push(GLMATRIX * p_project)
{
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadMatrixf((float *) p_project);
  return (p_project);
}

inline void set_matrix_project_pop(void)
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
}

inline void set_matrix_view(int x, int y, int max_x, int max_y)
{
  glstav_view_x = x;
  glstav_view_y = y;
  glstav_view_dx = max_x;
  glstav_view_dy = max_y;
  glViewport(x, y, max_x, max_y);
}

inline void get_matrix_view(int *p_x, int *p_y, int *p_max_x, int *p_max_y)
{
  *p_x = glstav_view_x;
  *p_y = glstav_view_y;
  *p_max_x = glstav_view_dx;
  *p_max_y = glstav_view_dy;
}

inline void set_matrix_2d(int xres, int yres)
{
  if (!glstav_matrix_2d_mod) {
    glstav_matrix_2d_mod = TRUE;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0f, xres, yres, 0.0f, 0.0f, 1.0f);
  }
}

inline void ret_matrix_2d(void)
{
  if (glstav_matrix_2d_mod) {
    glstav_matrix_2d_mod = FALSE;
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  }
}

inline void set_matrix_init(void)
{
  init_matrix(&__mat_init);
}

inline GLMATRIX *get_matrix_init(void)
{
  return (&__mat_init);
}

inline void set_matrix_camera_project(GLMATRIX * p_project)
{
  if (__mat_top_matrix_akt) {
    mat_mult_dir(__mat_top_matrix + __mat_top_matrix_akt - 1, &__mat_kamera,
      &__mat_tmp);
    mat_mult_dir(&__mat_tmp, p_project, &__mat_camera_project);
  }
  else {
    mat_mult_dir(&__mat_kamera, p_project, &__mat_camera_project);
  }
}

inline GLMATRIX *get_matrix_camera_project(void)
{
  return (&__mat_camera_project);
}


/*
  Nastaveni cullingu
*/
inline void cull_on(void)
{
  if (!glstav_cull) {
    glEnable(GL_CULL_FACE);
    glstav_cull = TRUE;
  }
}

inline void cull_off(void)
{
  if (glstav_cull) {
    glDisable(GL_CULL_FACE);
    glstav_cull = FALSE;
  }
}

inline void cull_back(void)
{
  if (glstav_cull_mod != 1) {
    glstav_cull_mod = 1;
    glFrontFace(GL_CCW);
  }
}

inline void cull_front(void)
{
  if (glstav_cull_mod != 2) {
    glstav_cull_mod = 2;
    glFrontFace(GL_CW);
  }
}

inline void cull_nastav(int cull)
{
  if (!cull) {
    cull_off();
  }
  else if (cull == 1) {
    cull_on();
    cull_back();
  }
  else if (cull == 2) {
    cull_on();
    cull_front();
  }
}

inline void cull_prehod(void)
{
  if (glstav_cull_mod) {
    if (glstav_cull_mod == 1) {
      cull_front();
    }
    else {
      cull_back();
    }
  }
}


/* Blend-funkce
*/
inline void blend_set_one_zero(void)
{
  if (glstav_blending) {
    glBlendFunc(GL_ONE, GL_ZERO);
  }
}

inline void blend_get_default(GLenum * p_src, GLenum * p_dest)
{
  *p_src = GL_ONE;
  *p_dest = GL_ZERO;
}

inline void blend_set(GLenum src, GLenum dest)
{
  if (glstav_blending) {
    glBlendFunc(src, dest);
  }
}

inline void blend_off(void)
{
  if (glstav_blending) {
    glstav_blending = FALSE;
    glDisable(GL_BLEND);
  }
}

inline void blend_on(void)
{
  if (!glstav_blending) {
    glstav_blending = TRUE;
    glEnable(GL_BLEND);
  }
}

inline void alfa_test_off(void)
{
  if (glstav_alfa_test) {
    glstav_alfa_test = FALSE;
    glDisable(GL_ALPHA_TEST);
  }
}

inline void alfa_test_on(void)
{
  if (!glstav_alfa_test) {
    glstav_alfa_test = TRUE;
    glEnable(GL_ALPHA_TEST);
  }
}

inline void blend_param(int stav)
{
  if (stav) {
    blend_on();
  }
  else {
    blend_off();
  }
}

/* Svetla
*/
inline void light_ambient_f(float *p_ambient)
{
  glLightfv(GL_LIGHT0, GL_AMBIENT, p_ambient);
}

inline void light_ambient_d(dword barva)
{
  float amb[4] = { 0, 0, 0, 1 };
  rgb_float(barva, amb);
  glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
}

inline void light_on_off(int stav)
{
  if (stav) {
    if (!glstav_lighting) {
      glstav_lighting = TRUE;
      glEnable(GL_LIGHTING);
    }
  }
  else {
    if (glstav_lighting) {
      glstav_lighting = FALSE;
      glDisable(GL_LIGHTING);
    }
  }
}

inline void light_shade_smooth(void)
{
  glShadeModel(GL_SMOOTH);
}

inline void light_shade_flat(void)
{
  glShadeModel(GL_FLAT);
}

/* Mlha
*/

void fog_param(int stav);
void fog_on(void);
void fog_off(void);
void aktivuj_mlhu(void);
inline void disable_fog(void);
inline void enable_fog(void);



/* Textury
*/

/* 
  Textury - Sigle-texturing
*/
inline void text_init(void)
{
  glstav_text_akt = 0;

  glstav_textury_1d[0] = TRUE;
  glstav_textury_2d[0] = TRUE;
  glstav_text_array[0] = FALSE;
  glstav_last_text_1d[0] = FALSE;
  glstav_last_text_2d[0] = FALSE;

  glstav_textury_1d[1] = FALSE;
  glstav_textury_2d[1] = FALSE;
  glstav_text_array[1] = FALSE;
  glstav_last_text_1d[1] = FALSE;
  glstav_last_text_2d[1] = FALSE;

  glstav_textury_1d[2] = FALSE;
  glstav_textury_2d[2] = FALSE;
  glstav_text_array[2] = FALSE;
  glstav_last_text_1d[2] = FALSE;
  glstav_last_text_2d[2] = FALSE;

  glstav_textury_1d[3] = FALSE;
  glstav_textury_2d[3] = FALSE;
  glstav_text_array[3] = FALSE;
  glstav_last_text_1d[3] = FALSE;
  glstav_last_text_2d[3] = FALSE;

  glActiveTextureARB(GL_TEXTURE1_ARB);
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);
  glActiveTextureARB(GL_TEXTURE0_ARB);
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);
}

inline void text_set(GLuint text, GLenum typ)
{
  if (typ == GL_TEXTURE_2D) {
    if (glstav_textury_2d[glstav_text_akt]
      && glstav_last_text_2d[glstav_text_akt] != text) {
      glstav_last_text_2d[glstav_text_akt] = text;
      glBindTexture(GL_TEXTURE_2D, text);
    }
  }
  else {
    if (glstav_textury_1d[glstav_text_akt]
      && glstav_last_text_1d[glstav_text_akt] != text) {
      glstav_last_text_1d[glstav_text_akt] = text;
      glBindTexture(GL_TEXTURE_1D, text);
    }
  }
}

inline void text_on(GLenum typ)
{
  if (typ == GL_TEXTURE_2D) {
    if (!glstav_textury_2d[glstav_text_akt]) {
      glstav_textury_2d[glstav_text_akt] = TRUE;
      glEnable(GL_TEXTURE_2D);
    }
    if (glstav_textury_1d[glstav_text_akt]) {
      glstav_textury_1d[glstav_text_akt] = FALSE;
      glDisable(GL_TEXTURE_1D);
    }
  }
  else {
    if (!glstav_textury_1d[glstav_text_akt]) {
      glstav_textury_1d[glstav_text_akt] = TRUE;
      glEnable(GL_TEXTURE_1D);
    }
    if (glstav_textury_2d[glstav_text_akt]) {
      glstav_textury_2d[glstav_text_akt] = FALSE;
      glDisable(GL_TEXTURE_2D);
    }
  }
}

inline void text_off(GLenum typ)
{
  if (typ == GL_TEXTURE_2D) {
    if (glstav_textury_2d[glstav_text_akt]) {
      glstav_textury_2d[glstav_text_akt] = FALSE;
      glDisable(GL_TEXTURE_2D);
    }
  }
  else {
    if (glstav_textury_1d[glstav_text_akt]) {
      glstav_textury_1d[glstav_text_akt] = FALSE;
      glDisable(GL_TEXTURE_1D);
    }
  }
}

inline int text_set_num(int num)
{
  if (num < glstav_multitext_units) {
    if (glstav_text_akt != num) {
      glstav_text_akt = num;
      glActiveTextureARB(arb_prevodni_tabulka[num]);
    }
    return (TRUE);
  }
  else {
    return (FALSE);
  }
}

inline int text_set_num_off(int num)
{
  if (num < glstav_multitext_units) {
    if (glstav_textury_1d[num] || glstav_textury_2d[num]) {
      if (glstav_text_akt != num) {
        glstav_text_akt = num;
        glActiveTextureARB(arb_prevodni_tabulka[num]);
      }
      if (glstav_textury_1d[num]) {
        glstav_textury_1d[num] = FALSE;
        glDisable(GL_TEXTURE_1D);
      }
      if (glstav_textury_2d[num]) {
        glstav_textury_2d[num] = FALSE;
        glDisable(GL_TEXTURE_2D);
      }
    }
    glstav_text_map_indicie[num] = K_CHYBA;
    return (TRUE);
  }
  else {
    return (FALSE);
  }
}


/* Odrazy
*/
inline void specular_on(void)
{
  if (!glstav_specular) {
    glstav_specular = TRUE;
    glEnable(GL_COLOR_SUM_EXT);
  }
}

inline void specular_off(void)
{
  if (glstav_specular) {
    glstav_specular = FALSE;
    glDisable(GL_COLOR_SUM_EXT);
  }
}

inline void specular_color(float r, float g, float b)
{
  glSecondaryColor3fEXT(r, g, b);
  glDisable(GL_COLOR_SUM_EXT);
}


/* Diffusni barvy
*/
inline void diffuse_off(float r, float g, float b, float a)
{
  glstav_diffuse = FALSE;
  glColor4f(r, g, b, a);
}

inline void diffuse_on(void)
{
  glstav_diffuse = TRUE;
}

/*
  Rendering z poli
*/
inline void array_specular_set(int stav)
{
  if (stav) {
    if (!glstav_array_specular) {
      glstav_array_specular = TRUE;
      glEnableClientState(GL_SECONDARY_COLOR_ARRAY_EXT);
    }
  }
  else {
    if (glstav_array_specular) {
      glstav_array_specular = FALSE;
      glDisableClientState(GL_SECONDARY_COLOR_ARRAY_EXT);
    }
  }
}

inline void array_specular_on(void)
{
  if (!glstav_array_specular) {
    glstav_array_specular = TRUE;
    glEnableClientState(GL_SECONDARY_COLOR_ARRAY_EXT);
  }
}

inline void array_specular_off(void)
{
  if (glstav_array_specular) {
    glstav_array_specular = FALSE;
    glDisableClientState(GL_SECONDARY_COLOR_ARRAY_EXT);
  }
}

inline void array_diffuse_on(void)
{
  if (!glstav_array_diffuse) {
    glstav_array_diffuse = TRUE;
    glEnableClientState(GL_COLOR_ARRAY);
  }
}

inline void array_diffuse_off(void)
{
  if (glstav_array_diffuse) {
    glstav_array_diffuse = FALSE;
    glDisableClientState(GL_COLOR_ARRAY);
  }
}


/* Normalove vektory
*/
inline void array_normal_set(int stav)
{
  if (stav) {
    if (!glstav_array_normal) {
      glstav_array_normal = TRUE;
      glEnableClientState(GL_NORMAL_ARRAY);
    }
  }
  else {
    if (glstav_array_normal) {
      glstav_array_normal = FALSE;
      glDisableClientState(GL_NORMAL_ARRAY);
    }
  }
}

inline void array_normal_on(void)
{
  if (!glstav_array_normal) {
    glstav_array_normal = TRUE;
    glEnableClientState(GL_NORMAL_ARRAY);
  }
}

inline void array_normal_off(void)
{
  if (glstav_array_normal) {
    glstav_array_normal = FALSE;
    glDisableClientState(GL_NORMAL_ARRAY);
  }
}

/* Vertex-listy
*/
inline int array_text_set_num(int num)
{
  if (num < glstav_multitext_units) {
    if (num != glstav_text_array_akt) {
      glstav_text_array_akt = num;
      glClientActiveTextureARB(arb_prevodni_tabulka[num]);
    }
    return (TRUE);
  }
  else
    return (FALSE);
}

inline void array_text_on(void)
{
  if (!glstav_text_array[glstav_text_array_akt]) {
    glstav_text_array[glstav_text_array_akt] = TRUE;
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }
}

inline void array_text_off(void)
{
  if (glstav_text_array[glstav_text_array_akt]) {
    glstav_text_array[glstav_text_array_akt] = FALSE;
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }
}


/* Mlha
*/
inline void nastav_fog(MLZNA_KOSTKA * p_mlha)
{
  if (p_mlha) {
    glFogi(GL_FOG_MODE, p_mlha->mod);
    glFogfv(GL_FOG_COLOR, &p_mlha->r);
    glFogf(GL_FOG_DENSITY, p_mlha->intenzita);
    glFogf(GL_FOG_START, p_mlha->start);
    glFogf(GL_FOG_END, p_mlha->stop);
  }
}

inline void disable_fog(void)
{
  if (glstav_mlha) {
    glDisable(GL_FOG);
    glstav_mlha = FALSE;
  }
}

inline void enable_fog(void)
{
  if (!glstav_mlha) {
    glEnable(GL_FOG);
    glstav_mlha = TRUE;
  }
}

inline void disable_fog_causal(void)
{
  if (glstav_mlha && glstav_mlha_causal) {
    glstav_mlha_causal = FALSE;
    glDisable(GL_FOG);
  }
}

inline void enable_fog_causal(void)
{
  if (glstav_mlha && !glstav_mlha_causal) {
    glstav_mlha_causal = TRUE;
    glEnable(GL_FOG);
  }
}

inline void text_sharp(float sharp)
{
  sharp = -sharp;
  int i;

  for (i = 0; i < glstav_multitext_units; i++) {
    text_set_num(i);
    glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, sharp);
  }
}

inline void reset_kamera_flag(void)
{
  glstav_zmena_kamery = FALSE;
}

/*
#define TEXGEN_NIC       0
#define TEXGEN_SPHEREMAP 1
#define TEXGEN_EYE_LINEAR    2
*/

inline void texgen_env_on(int num)
{
  if (glstav_genenv_s[num] != TEXGEN_SPHEREMAP) {
    glstav_genenv_s[num] = TEXGEN_SPHEREMAP;
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glEnable(GL_TEXTURE_GEN_S);
  }
  if (glstav_genenv_t[num] != TEXGEN_SPHEREMAP) {
    glstav_genenv_t[num] = TEXGEN_SPHEREMAP;
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glEnable(GL_TEXTURE_GEN_T);
  }
  if (glstav_genenv_r[num]) {
    glstav_genenv_r[num] = FALSE;
    glDisable(GL_TEXTURE_GEN_R);
  }
  if (glstav_genenv_q[num]) {
    glstav_genenv_q[num] = FALSE;
    glDisable(GL_TEXTURE_GEN_Q);
  }
}

// Musi se nastavit transformacni matice pro transformace tech souradnic
inline void texgen_linear_eye_2d_on(int num, float *p_spar, float *p_tpar)
{
  GLMATRIX *p_mat = glstav_texgen + num;

  if (!glstav_genenv_s[num])
    glEnable(GL_TEXTURE_GEN_S);
  if (!glstav_genenv_t[num])
    glEnable(GL_TEXTURE_GEN_T);

  if (glstav_genenv_s[num] != TEXGEN_EYE_LINEAR) {
    glstav_genenv_s[num] = TEXGEN_EYE_LINEAR;
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  }
  if (glstav_genenv_t[num] != TEXGEN_EYE_LINEAR) {
    glstav_genenv_t[num] = TEXGEN_EYE_LINEAR;
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  }

  if (glstav_zmena_kamery ||
    p_spar[0] != p_mat->_11 || p_spar[1] != p_mat->_21 ||
    p_spar[2] != p_mat->_31 || p_spar[3] != p_mat->_41 ||
    p_tpar[0] != p_mat->_12 || p_tpar[1] != p_mat->_22 ||
    p_tpar[2] != p_mat->_32 || p_tpar[3] != p_mat->_42) {

    p_mat->_11 = p_spar[0];
    p_mat->_21 = p_spar[1];
    p_mat->_31 = p_spar[2];
    p_mat->_41 = p_spar[3];

    p_mat->_12 = p_tpar[0];
    p_mat->_22 = p_tpar[1];
    p_mat->_32 = p_tpar[2];
    p_mat->_42 = p_tpar[3];

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float *) &__mat_kamera);

    glTexGenfv(GL_S, GL_EYE_PLANE, p_spar);
    glTexGenfv(GL_T, GL_EYE_PLANE, p_tpar);

    glLoadMatrixf((float *) &__mat_vysl);
  }

  if (glstav_genenv_r[num]) {
    glstav_genenv_r[num] = FALSE;
    glDisable(GL_TEXTURE_GEN_R);
  }
  if (glstav_genenv_q[num]) {
    glstav_genenv_q[num] = FALSE;
    glDisable(GL_TEXTURE_GEN_Q);
  }
}

inline void texgen_off(int num)
{
  if (glstav_genenv_s[num]) {
    glstav_genenv_s[num] = FALSE;
    glDisable(GL_TEXTURE_GEN_S);
  }
  if (glstav_genenv_t[num]) {
    glstav_genenv_t[num] = FALSE;
    glDisable(GL_TEXTURE_GEN_T);
  }
  if (glstav_genenv_r[num]) {
    glstav_genenv_r[num] = FALSE;
    glDisable(GL_TEXTURE_GEN_R);
  }
  if (glstav_genenv_q[num]) {
    glstav_genenv_q[num] = FALSE;
    glDisable(GL_TEXTURE_GEN_Q);
  }
}

inline void deph_test_set(int stav)
{
  if (stav) {
    if (!glstav_deph_test) {
      glstav_deph_test = TRUE;
      glEnable(GL_DEPTH_TEST);
    }
  }
  else {
    if (glstav_deph_test) {
      glstav_deph_test = FALSE;
      glDisable(GL_DEPTH_TEST);
    }
  }
}

inline void zmask_set(int stav)
{
  if (stav) {
    if (!glstav_zmask) {
      glstav_zmask = TRUE;
      glDepthMask(TRUE);
    }
  }
  else {
    if (glstav_zmask) {
      glstav_zmask = FALSE;
      glDepthMask(FALSE);
    }
  }
}

inline void zmask_set_fleky(int stav)
{
  glDepthMask((GLboolean) stav);
}

inline void set_alfa_blok(int blok)
{
  if (blok != glstav_alfa_blok) {
    glstav_alfa_blok = blok;
    p_alfa_stage_func[alfa_stage_edit_blok[blok].funkce] ();
  }
}

inline void text_set_blok(int text, int blok)
{
  if (blok != glstav_text_blok[text]) {
    glstav_text_blok[text] = blok;
    p_text_stage_func_elem[blok] ();
  }
}

inline void text_stage_func_nic(int last_text)
{
  int i = last_text;

  if (i != K_CHYBA) {
    for (; i < MAT_TEXTUR; i++) {
      if (!text_set_num_off(i))
        return;
    }
  }
}

inline void reset_stage_bloky(void)
{
  glstav_alfa_blok = K_CHYBA;
  glstav_text_blok[0] = K_CHYBA;
  glstav_text_blok[0] = K_CHYBA;
  glstav_text_blok[1] = K_CHYBA;
  glstav_text_blok[2] = K_CHYBA;
  glstav_text_blok[3] = K_CHYBA;
  glstav_text_blok[4] = K_CHYBA;
  glstav_text_blok[5] = K_CHYBA;
}


inline void text_set_num_poly(int num)
{
  text_set_num(num);
  text_on(GL_TEXTURE_2D);
  text_set_blok(num, BLOK_MODULACE);
  ret_matrix_texture(num);
  texgen_off(num);
  glstav_text_poly_indicie = num;
}

inline void text_stage_func_nic_poly(int last_text, int poly_text)
{
  int i = last_text;

  if (i != K_CHYBA) {
    for (; i < MAT_TEXTUR; i++) {
      if (!text_set_num_off(i)) {
        break;
      }
    }
  }
  text_set_num_poly(poly_text);
}

/* Vertexove pole
*/
void vertex_array_ini(void);

extern void (*mesh_vertex_array_init) (GAME_MESH_OLD * p_mesh);
extern void (*mesh_vertex_array_zrus) (GAME_MESH_OLD * p_mesh);
extern void (*mesh_vertex_array_upload) (GAME_MESH_OLD * p_mesh);
extern void (*mesh_vertex_array_set) (GAME_MESH_OLD * p_mesh);
extern void (*mesh_vertex_array_set_koord) (GAME_MESH_OLD * p_mesh);

extern void (*poly_vertex_array_init) (EDIT_MESH_POLY * p_poly);
extern void (*poly_vertex_array_zrus) (EDIT_MESH_POLY * p_poly);
extern void (*poly_vertex_array_upload) (EDIT_MESH_POLY * p_poly);
extern void (*poly_vertex_array_set) (EDIT_MESH_POLY * p_poly);

extern void (*vertex_array_start_render) (void);
extern void (*vertex_array_stop_render) (void);

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#endif
