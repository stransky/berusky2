/*
  animace
*/
#include "mss_on.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <direct.h>
#include <float.h>
#include <assert.h>
#include "objekt_editor_all.h"
#include "oe_import_ase.h"

#include "dbgwnd.h"

static EDIT_KONTEJNER *p_kont;
static EDIT_MATERIAL *p_mat;
static EDIT_MATERIAL **p_mat_list;
static SIMPLE_TRACK_INFO *p_track;
static int sub_mat;             // flag pro sub-materialy
static int mat_list_num;
static byte node_name[300];
static byte obj_jmeno[MAX_JMENO];
static byte obj_jmeno_parent[MAX_JMENO];
static BOD obj_pos;
static BOD obj_scale;
static QUAT obj_rot;
static int vertexnum;
static BOD *p_vertex;
static int tvertexnum;
static BODUV *p_tvertex;
static int facenum;
static int *p_face;
static int *p_face_mat;
static int tfacenum;
static int *p_tface;
static int akt_map_face;
static int akt_map_face_sub;
static int ftvertexnum;
static int ftfacenum;
static int frame_first;
static int frame_last;
static int frame_speed;
static int frame_tick_per_frame;

static int sim_framenum;
static int load_animaci;


int frame_to_sim_frame(float frame)
{
  frame /= (float) frame_speed;
  return (ftoi(frame * FRAMU_PER_SEC));
}

// prevede sample -> time (v milisekundach)
int sample_to_sim_frame(int sample)
{
  float frame = ((float) sample) / ((float) frame_tick_per_frame);

  return (frame_to_sim_frame(frame));
}

typedef struct _PARSE_TABLE
{

  byte id[100];
  int (*func) (FILE * f, byte * p_line);
  int delka;

} PARSE_TABLE;

byte *ase_najdi_filename(byte * p_line)
{
  byte *p_pom = p_line;

  while (*p_line) {
    if (*p_line == '\\')
      p_pom = p_line + 1;
    p_line++;
  }
  return (p_pom);
}

byte *ase_zrus_tabulatory(byte * p_line)
{
  int i = 0;

  while (p_line[i] && (p_line[i] == 0x9 || p_line[i] == '*'))
    i++;
  return (p_line + i);
}

int ase_najdi_mezeru(byte * p_line)
{
  int i = 0;

  while (p_line[i] && (p_line[i] != ' ' && p_line[i] != 0x9))
    i++;
  return (p_line[i] ? i : strlen(p_line));
}

int ase_najdi_znak_point(byte * p_line, byte ** p_first, byte ** p_next)
{
  int i = 0;

  while (p_line[i] && (p_line[i] == ' ' || p_line[i] == 0x9))
    i++;
  if (!p_line[i])
    return (FALSE);
  *p_first = p_line + i;

  while (p_line[i] && (p_line[i] != ' ' && p_line[i] != 0x9))
    i++;
  if (p_line[i]) {
    p_line[i] = '\0';
    i++;
  }
  *p_next = p_line + i;
  return (TRUE);
}

int ase_odstran_dvojtecku(byte * p_line)
{
  int i = 0;

  while (p_line[i] && p_line[i] != ':')
    i++;
  if (p_line[i] == ':')
    p_line[i] = '\0';
  return (i);
}

byte *ase_string_reader(byte * p_line, byte * p_jmeno)
{
  byte *p_pom;
  int i = 0;

  while (p_line[i] && (p_line[i] == ' ' || p_line[i] == 0x9
      || p_line[i] == '"'))
    i++;
  p_pom = p_jmeno;
  while (p_line[i] && (p_line[i] != ' ' && p_line[i] != 0x9
      && p_line[i] != '"'))
    *p_pom++ = p_line[i++];
  return (p_line + i);
}

byte *ase_name_reader(byte * p_line, byte * p_jmeno)
{
  byte *p_pom;

  p_pom = strchr(p_line, '"') + 1;
  while (*p_pom && *p_pom != '"')
    *p_jmeno++ = *p_pom++;
  *p_jmeno = 0;
  return (p_pom - 1);
}

int ase_read_frame_first(FILE * f, byte * p_line)
{
  frame_first = atoi(p_line);
  return (TRUE);
}

int ase_read_frame_last(FILE * f, byte * p_line)
{
  frame_last = atoi(p_line);
  return (TRUE);
}

int ase_read_frame_speed(FILE * f, byte * p_line)
{
  //frame_speed = atoi(p_line);
  frame_speed = 20;
  return (TRUE);
}

int ase_read_frame_tpf(FILE * f, byte * p_line)
{
  frame_tick_per_frame = atoi(p_line);
  sim_framenum = frame_to_sim_frame((float) frame_last);
  return (TRUE);
}

int ase_read_matnum(FILE * f, byte * p_line)
{
  int num = atoi(p_line);

  mat_list_num = num;
  p_mat_list = malloc(sizeof(p_mat_list[0]) * num);
  mtest(p_mat_list);
  memset(p_mat_list, 0, sizeof(p_mat_list[0]) * num);
  return (TRUE);
}

int ase_read_material_num(FILE * f, byte * p_line)
{
  int num;

  sscanf(p_line, " %d {", &num);
  p_mat = p_mat_list[num] = vyrob_material();
  sub_mat = FALSE;
  return (TRUE);
}

int ase_read_material_sub(FILE * f, byte * p_line)
{
  int num;

  sscanf(p_line, " %d {", &num);
  if (!sub_mat) {               // prvni sub-material - load do stavajiciho matrose
    sub_mat = TRUE;
  }
  else {
    p_mat = (p_mat->p_sub = vyrob_material());
  }
  return (TRUE);
}

int ase_read_material_jmeno(FILE * f, byte * p_line)
{
  if (p_mat) {
    ase_name_reader(p_line, p_mat->jmeno);
    return (TRUE);
  }
  else
    return (FALSE);
}

int ase_read_material_ambient(FILE * f, byte * p_line)
{
  sscanf(p_line, " %f %f %f", &p_mat->dxmat.ambient_r,
    &p_mat->dxmat.ambient_g, &p_mat->dxmat.ambient_b);
  //kprintf(TRUE,"Ambient barva %f %f %f",p_mat->dxmat.ambient_r,p_mat->dxmat.ambient_g,p_mat->dxmat.ambient_b);
  return (TRUE);
}

int ase_read_material_diffuse(FILE * f, byte * p_line)
{
  sscanf(p_line, " %f %f %f", &p_mat->dxmat.diffuse_r,
    &p_mat->dxmat.diffuse_g, &p_mat->dxmat.diffuse_b);
  //kprintf(TRUE,"Diffuse barva %f %f %f",p_mat->dxmat.diffuse_r,p_mat->dxmat.diffuse_g,p_mat->dxmat.diffuse_b);
  return (TRUE);
}

int ase_read_material_specular(FILE * f, byte * p_line)
{
  sscanf(p_line, " %f %f %f", &p_mat->dxmat.specular_r,
    &p_mat->dxmat.specular_g, &p_mat->dxmat.specular_b);
  //kprintf(TRUE,"Specular barva %f %f %f",p_mat->dxmat.specular_r,p_mat->dxmat.specular_g,p_mat->dxmat.specular_b);
  return (TRUE);
}

int ase_read_material_bitmap(FILE * f, byte * p_line)
{
  byte jmeno[MAX_JMENO];
  int t;

  if (p_mat) {
    ase_name_reader(p_line, jmeno);
    strcpy(jmeno, ase_najdi_filename(jmeno));

    if ((t =
        lo_najdi_texturu(p_cnf->p_text, MAX_EDIT_TEXTUR, jmeno,
          0)) == K_CHYBA) {
      if ((t =
          lo_najdi_volnou_texturu(p_cnf->p_text, MAX_EDIT_TEXTUR)) == K_CHYBA)
        chyba("Neni volna textura !");
      strcpy(p_cnf->p_text[t].jmeno, jmeno);
      _strlwr(p_cnf->p_text[t].jmeno);
    }

    //kprintf(TRUE,"Textura '%s'",jmeno);
    strcpy(p_mat->textfile[0], p_cnf->p_text[t].jmeno);
    p_mat->p_text[0] = p_cnf->p_text + t;

    return (TRUE);
  }
  else
    return (FALSE);
}

int ase_read_material_bitmap_offset_u(FILE * f, byte * p_line)
{
  if (p_mat) {
    p_mat->flag |= MAT_3DS_FLAGS;
    p_mat->tds.u_offset = (float) atof(p_line);
    return (TRUE);
  }
  else
    return (FALSE);
}

int ase_read_material_bitmap_offset_v(FILE * f, byte * p_line)
{
  if (p_mat) {
    p_mat->flag |= MAT_3DS_FLAGS;
    p_mat->tds.v_offset = (float) atof(p_line);
    return (TRUE);
  }
  else
    return (FALSE);
}

int ase_read_material_bitmap_scale_u(FILE * f, byte * p_line)
{
  if (p_mat) {
    p_mat->flag |= MAT_3DS_FLAGS;
    p_mat->tds.u_scale = (float) atof(p_line);
    return (TRUE);
  }
  else
    return (FALSE);
}

int ase_read_material_bitmap_scale_v(FILE * f, byte * p_line)
{
  if (p_mat) {
    p_mat->flag |= MAT_3DS_FLAGS;
    p_mat->tds.v_scale = (float) atof(p_line);
    return (TRUE);
  }
  else
    return (FALSE);
}

int ase_read_material_bitmap_uhel(FILE * f, byte * p_line)
{
  if (p_mat) {
    p_mat->flag |= MAT_3DS_FLAGS;
    p_mat->tds.rot_angle = (float) atof(p_line);
    return (TRUE);
  }
  else
    return (FALSE);
}

int ase_read_node_name(FILE * f, byte * p_line)
{
  ase_name_reader(p_line, obj_jmeno);
  return (TRUE);
}

int ase_read_node_name_parent(FILE * f, byte * p_line)
{
  ase_name_reader(p_line, obj_jmeno_parent);
  //obj_jmeno_parent[0] = 0;
  return (TRUE);
}

int ase_read_node_pos(FILE * f, byte * p_line)
{
  byte *p_first, *p_next;

  if (!ase_najdi_znak_point(p_line, &p_first, &p_next))
    return (FALSE);
  obj_pos.x = (float) atof(p_first);

  if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
    return (FALSE);
  obj_pos.z = (float) atof(p_first);

  if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
    return (FALSE);
  obj_pos.y = (float) atof(p_first);

  return (TRUE);
}

int ase_read_node_scale(FILE * f, byte * p_line)
{
  byte *p_first, *p_next;

  if (!ase_najdi_znak_point(p_line, &p_first, &p_next))
    return (FALSE);
  obj_scale.x = (float) atof(p_first);

  if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
    return (FALSE);
  obj_scale.z = (float) atof(p_first);

  if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
    return (FALSE);
  obj_scale.y = (float) atof(p_first);

  return (TRUE);
}

int ase_read_node_rot_osa(FILE * f, byte * p_line)
{
  byte *p_first, *p_next;

  if (!ase_najdi_znak_point(p_line, &p_first, &p_next))
    return (FALSE);
  obj_rot.x = (float) atof(p_first);

  if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
    return (FALSE);
  obj_rot.z = (float) atof(p_first);

  if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
    return (FALSE);
  obj_rot.y = (float) atof(p_first);

  return (TRUE);
}

int ase_read_node_rot_uhel(FILE * f, byte * p_line)
{
  QUAT q;
  byte *p_first, *p_next;

  if (!ase_najdi_znak_point(p_line, &p_first, &p_next))
    return (FALSE);
  obj_rot.w = (float) atof(p_first);

  angle_to_quat(&q, (BOD *) (&obj_rot.x), obj_rot.w);
  obj_rot = q;

  return (TRUE);
}

int ase_read_mesh_start(FILE * f, byte * p_line)
{
  if (!p_kont)
    p_kont = vyrob_kontejner();

  obj_jmeno[0] = 0;
  obj_jmeno_parent[0] = 0;

  return (TRUE);
}

int ase_read_mesh_numvertex(FILE * f, byte * p_line)
{
  vertexnum = atoi(p_line);
  if (vertexnum) {
    p_vertex = mmalloc(sizeof(p_vertex[0]) * vertexnum);
  }
  return (TRUE);
}

int ase_read_mesh_numface(FILE * f, byte * p_line)
{
  facenum = atoi(p_line) * 3;
  if (facenum) {
    p_face = mmalloc(sizeof(p_face[0]) * facenum);
    p_face_mat = mmalloc(sizeof(p_face_mat[0]) * facenum);
  }
  return (TRUE);
}

int ase_read_mesh_numtface(FILE * f, byte * p_line)
{
  tfacenum = atoi(p_line) * 3;
  if (tfacenum) {
    p_tface = mmalloc(sizeof(p_tface[0]) * tfacenum);
  }
  return (TRUE);
}

int ase_read_mesh_vertex(FILE * f, byte * p_line)
{
  byte *p_first, *p_next;
  int d;

  if (p_vertex) {
    if (!ase_najdi_znak_point(p_line, &p_first, &p_next))
      return (FALSE);
    d = atoi(p_first);
    assert(d < vertexnum);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    p_vertex[d].x = (float) atof(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    p_vertex[d].z = (float) atof(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    p_vertex[d].y = (float) atof(p_first);

    return (TRUE);
  }
  else
    return (FALSE);
}

int ase_read_mesh_tvertex(FILE * f, byte * p_line)
{
  byte *p_first, *p_next;
  int d;

  if (p_tvertex) {
    if (!ase_najdi_znak_point(p_line, &p_first, &p_next))
      return (FALSE);
    d = atoi(p_first);
    assert(d < tvertexnum);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    p_tvertex[d].u = (float) atof(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    p_tvertex[d].v = (float) atof(p_first);

    return (TRUE);
  }
  else
    return (FALSE);
}

int ase_read_mesh_face(FILE * f, byte * p_line)
{
  byte *p_first, *p_next;
  int d, num;

  if (p_face) {
    // cislo face
    if (!ase_najdi_znak_point(p_line, &p_first, &p_next))
      return (FALSE);

    ase_odstran_dvojtecku(p_first);
    d = atoi(p_first) * 3;
    assert(d < facenum);

    //a cislo
    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    p_face[d] = atoi(p_first);

    // b cislo
    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    p_face[d + 1] = atoi(p_first);

    // c cislo
    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    p_face[d + 2] = atoi(p_first);

    //
    p_next = strstr(p_next, "*MESH_MTLID");
    if (p_next) {
      sscanf(p_next + 11, " %d", &num);
    }
    else {
      num = 0;
    }
    p_face_mat[d] = p_face_mat[d + 1] = p_face_mat[d + 2] = num;

    return (TRUE);
  }
  else
    return (FALSE);
}

int ase_read_mesh_numtvertex(FILE * f, byte * p_line)
{
  tvertexnum = atoi(p_line);
  if (tvertexnum) {
    p_tvertex = mmalloc(sizeof(p_tvertex[0]) * tvertexnum);
  }
  return (TRUE);
}

int ase_read_mesh_tface(FILE * f, byte * p_line)
{
  byte *p_first, *p_next;
  int d;

  if (p_tface) {
    if (!ase_najdi_znak_point(p_line, &p_first, &p_next))
      return (FALSE);

    d = atoi(p_first) * 3;
    assert(d + 2 < tfacenum);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    p_tface[d] = atoi(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    p_tface[d + 1] = atoi(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    p_tface[d + 2] = atoi(p_first);

    return (TRUE);
  }
  else
    return (FALSE);
}

// vyrobi pole facu + texure vertexu
int ase_read_mesh_face_map_list(FILE * f, byte * p_line)
{
  // Vyrob pole facu
  tfacenum = facenum;
  if (tfacenum) {
    p_tface = mmalloc(sizeof(p_tface[0]) * tfacenum);
  }
  // Vyrob pole vertexu
  tvertexnum = facenum;
  if (tvertexnum) {
    p_tvertex = mmalloc(sizeof(p_tvertex[0]) * tvertexnum);
  }
  ftvertexnum = ftfacenum = 0;
  return (TRUE);
}

// Nactu cislo aktualni face
int ase_read_mesh_face_map(FILE * f, byte * p_line)
{
  akt_map_face = atoi(p_line);
  akt_map_face_sub = 0;
  return (TRUE);
}

// Nacti cislo aktualni face
int ase_read_mesh_face_map_vertex(FILE * f, byte * p_line)
{
  byte *p_first, *p_next;
  int i;
  float u, v;

  if (p_tface) {
    if (!ase_najdi_znak_point(p_line, &p_first, &p_next))
      return (FALSE);
    u = (float) atof(p_first);
    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    v = (float) atof(p_first);

    assert(ftvertexnum <= ftvertexnum);
    assert(akt_map_face <= tfacenum);
    assert(akt_map_face_sub < 3);

    for (i = 0; i < ftvertexnum; i++) {
      if (p_tvertex[i].u == u && p_tvertex[i].v == v) {
        break;
      }
    }

    if (i == ftvertexnum) {
      p_tvertex[i].u = u;
      p_tvertex[i].v = v;
      ftvertexnum++;
    }

    p_tface[akt_map_face * 3 + akt_map_face_sub] = i;
    akt_map_face_sub++;

    return (TRUE);
  }
  else
    return (FALSE);
}

int ase_spocitej_materialy(EDIT_MATERIAL * p_mat)
{
  if (!p_mat)
    return (0);
  if (!p_mat->p_sub)
    return (1);
  return (1 + ase_spocitej_materialy(p_mat->p_sub));
}

// Seskladani celeho objektu dohromady
int ase_read_mesh_material(FILE * f, byte * p_line)
{
  EDIT_OBJEKT **p_list, *p_tmp;
  byte pom[50];
  int fc;
  int map, pos, i, mat = atoi(p_line);
  int objnum = ase_spocitej_materialy(p_mat_list[mat]);

  if (objnum) {
    p_list = mmalloc(sizeof(p_list[0]) * objnum);
    for (i = 0; i < objnum; i++) {
      p_list[i] = vyrob_objekt();
      strcpy(p_list[i]->jmeno, obj_jmeno);
      strcpy(p_list[i]->jmeno_parent, obj_jmeno_parent);
      if (objnum > 1) {
        strcat(p_list[i]->jmeno, "_sub_");
        strcat(p_list[i]->jmeno, itoa(i, pom, 10));
      }
      p_list[i]->p_vertex = mmalloc(sizeof(p_list[i]->p_vertex[0]) * facenum);
      p_list[i]->vertexnum = 0;
      p_list[i]->p_face = mmalloc(sizeof(p_list[i]->p_face[0]) * facenum);
      p_list[i]->facenum = 0;
    }

    /* Vertex + mapovani
     */
    if (p_vertex && p_tvertex && p_face && p_tface) {
      assert(tfacenum == facenum);

      for (fc = 0; fc < facenum; fc++) {
        // material 0...n
        p_tmp = p_list[p_face_mat[fc] < objnum ? p_face_mat[fc] : 0];
        pos = p_face[fc];       // cislo pos-vertexu
        map = p_tface[fc];      // cislo map-vertexu
        for (i = 0; i < p_tmp->vertexnum; i++) {
          if (p_tmp->p_vertex[i].x == p_vertex[pos].x &&
            p_tmp->p_vertex[i].y == p_vertex[pos].y &&
            p_tmp->p_vertex[i].z == p_vertex[pos].z &&
            p_tmp->p_vertex[i].tu1 == p_tvertex[map].u &&
            p_tmp->p_vertex[i].tv1 == p_tvertex[map].v) {
            p_tmp->p_face[p_tmp->facenum++] = i;
            goto konec_hledani;
          }
        }
        p_tmp->p_face[p_tmp->facenum++] = i;
        p_tmp->vertexnum++;

        p_tmp->p_vertex[i].x = p_vertex[pos].x;
        p_tmp->p_vertex[i].y = p_vertex[pos].y;
        p_tmp->p_vertex[i].z = p_vertex[pos].z;
        p_tmp->p_vertex[i].tu1 = p_tvertex[map].u;
        p_tmp->p_vertex[i].tv1 = p_tvertex[map].v;

      konec_hledani:;
      }

      null_free(&p_vertex);
      null_free(&p_tvertex);
      null_free(&p_face);
      null_free(&p_tface);
      null_free(&p_face_mat);
    }
    else {
      p_tmp = p_list[0];
      if (p_vertex && p_face) {
        p_tmp->vertexnum = vertexnum;
        p_tmp->facenum = facenum;
        for (fc = 0; fc < facenum; fc++) {
          p_tmp->p_face[fc] = pos = p_face[fc];
          p_tmp->p_vertex[pos].x = p_vertex[pos].x;
          p_tmp->p_vertex[pos].y = p_vertex[pos].y;
          p_tmp->p_vertex[pos].z = p_vertex[pos].z;
        }
        null_free(&p_vertex);
        null_free(&p_face);

      }
      else {
        kprintf(TRUE, "%s - CHYBA - if(!(p_vertex && p_face))", p_tmp->jmeno);
        return (TRUE);
      }
    }

    for (i = 0; i < objnum; i++) {
      int o = lo_najdi_volny_objekt_kont(p_kont);

      if (o == K_CHYBA)
        return (FALSE);

      p_tmp = p_kont->p_obj[o] = p_list[i];
      p_tmp->material = mat;
      p_tmp->material_sub = i;
      matrix_all(&p_tmp->m, &obj_pos, &obj_rot, &obj_scale);

      if (p_track) {
        p_tmp->p_tmp_sim = p_track;
        p_track = NULL;
      }
    }

    return (TRUE);
  }
  return (FALSE);
}

void ase_vyrob_sim_track_pos(BOD * p_pos, int prepis)
{
  int i;

  if (!p_track) {
    p_track = mmalloc(sizeof(p_track[0]));
    p_track->keynum = sim_framenum;
  }

  if (!p_track->p_pos || prepis) {
    if (!p_track->p_pos)
      p_track->p_pos = mmalloc(sizeof(p_track->p_pos[0]) * sim_framenum);
    for (i = 0; i < sim_framenum; i++) {
      p_track->p_pos[i] = *p_pos;
    }
  }
}

void ase_vyrob_sim_track_rot(QUAT * p_rot, int prepis)
{
  int i;

  if (!p_track) {
    p_track = mmalloc(sizeof(p_track[0]));
    p_track->keynum = sim_framenum;
  }

  if (!p_track->p_at || prepis) {
    if (!p_track->p_at)
      p_track->p_at = mmalloc(sizeof(p_track->p_at[0]) * sim_framenum);
    for (i = 0; i < sim_framenum; i++) {
      p_track->p_at[i] = *p_rot;
    }
  }
}

void ase_vyrob_sim_track_scale(BOD * p_scs, int prepis)
{
  int i;

  if (!p_track) {
    p_track = mmalloc(sizeof(p_track[0]));
    p_track->keynum = sim_framenum;
  }

  if (!p_track->p_scale || prepis) {
    if (!p_track->p_scale)
      p_track->p_scale = mmalloc(sizeof(p_track->p_scale[0]) * sim_framenum);
    for (i = 0; i < sim_framenum; i++) {
      p_track->p_scale[i] = *p_scs;
    }
  }
}

void ase_vloz_sample(int vzorek, BOD * p_pos, QUAT * p_at, BOD * p_scale)
{
  int frame = sample_to_sim_frame(vzorek);

  if (frame >= p_track->keynum)
    frame = p_track->keynum - 1;
  if (frame < 0)
    frame = 0;

  if (p_pos) {
    p_track->p_pos[frame] = *p_pos;
  }
  if (p_at) {
    p_track->p_at[frame] = *p_at;
  }
  if (p_scale) {
    p_track->p_scale[frame] = *p_scale;
  }
}

//static SIMPLE_TRACK_INFO *p_track;
int ase_read_mesh_tman_node_name(FILE * f, byte * p_line)
{
  if (load_animaci) {
    if (obj_scale.x != 1.0f || obj_scale.y != 1.0f || obj_scale.z != 1.0f) {
      ase_vyrob_sim_track_scale(&obj_scale, TRUE);
    }
  }
  return (TRUE);
}

int ase_read_mesh_tman_pos_track(FILE * f, byte * p_line)
{
  BOD pos = { FLT_MAX, FLT_MAX, FLT_MAX };
  if (load_animaci) {
    ase_vyrob_sim_track_pos(&pos, TRUE);
  }
  return (TRUE);
}

int ase_read_mesh_tman_pos_sample(FILE * f, byte * p_line)
{
  byte *p_first, *p_next;
  int sample;
  BOD p;

  if (load_animaci) {
    if (!ase_najdi_znak_point(p_line, &p_first, &p_next))
      return (FALSE);
    sample = atoi(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    p.x = atoff(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    p.z = atoff(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    p.y = atoff(p_first);

    ase_vloz_sample(sample, &p, NULL, NULL);
  }
  return (TRUE);
}

int ase_read_mesh_tman_rot_track(FILE * f, byte * p_line)
{
  QUAT q = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };

  if (load_animaci) {
    ase_vyrob_sim_track_rot(&q, TRUE);
  }
  return (TRUE);
}

int ase_read_mesh_tman_rot_sample(FILE * f, byte * p_line)
{
  byte *p_first, *p_next;
  int sample;
  QUAT q;

  if (load_animaci) {
    if (!ase_najdi_znak_point(p_line, &p_first, &p_next))
      return (FALSE);
    sample = atoi(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    q.x = atoff(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    q.z = atoff(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    q.y = atoff(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    q.w = atoff(p_first);

    ase_vloz_sample(sample, NULL, &q, NULL);
  }
  return (TRUE);
}

int ase_read_mesh_tman_scs_track(FILE * f, byte * p_line)
{
  BOD scs = { FLT_MAX, FLT_MAX, FLT_MAX };
  if (load_animaci) {
    ase_vyrob_sim_track_scale(&scs, TRUE);
  }
  return (TRUE);
}

int ase_read_mesh_tman_scs_sample(FILE * f, byte * p_line)
{
  byte *p_first, *p_next;
  int sample;
  BOD s;

  if (load_animaci) {
    if (!ase_najdi_znak_point(p_line, &p_first, &p_next))
      return (FALSE);
    sample = atoi(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    s.x = atoff(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    s.z = atoff(p_first);

    if (!ase_najdi_znak_point(p_next, &p_first, &p_next))
      return (FALSE);
    s.y = atoff(p_first);

    ase_vloz_sample(sample, NULL, NULL, &s);
  }
  return (TRUE);
}

PARSE_TABLE ase_table[] = {
  {"}", NULL, 0}
  ,

  {"3DSMAX_ASCIIEXPORT", NULL, 0}
  ,
  {"COMMENT", NULL, 0}
  ,

  {"SCENE", NULL, 0}
  ,
  {"SCENE_FILENAME", NULL, 0}
  ,
  {"SCENE_FIRSTFRAME", ase_read_frame_first, 0}
  ,
  {"SCENE_LASTFRAME", ase_read_frame_last, 0}
  ,
  {"SCENE_FRAMESPEED", ase_read_frame_speed, 0}
  ,
  {"SCENE_TICKSPERFRAME", ase_read_frame_tpf, 0}
  ,
  {"SCENE_BACKGROUND_STATIC", NULL, 0}
  ,
  {"SCENE_AMBIENT_STATIC", NULL, 0}
  ,

  {"MATERIAL_LIST", NULL, 0}
  ,
  {"MATERIAL_COUNT", ase_read_matnum, 0}
  ,
  {"MATERIAL", ase_read_material_num, 0}
  ,                             // ctu cislo materialu
  {"MATERIAL_NAME", ase_read_material_jmeno, 0}
  ,
  {"SUBMATERIAL", ase_read_material_sub, 0}
  ,                             // pripojim sub-material
  {"MATERIAL_CLASS", NULL, 0}
  ,
  {"MATERIAL_AMBIENT", ase_read_material_ambient, 0}
  ,
  {"MATERIAL_DIFFUSE", ase_read_material_diffuse, 0}
  ,
  {"MATERIAL_SPECULAR", ase_read_material_specular, 0}
  ,
  {"MATERIAL_SHINE", NULL, 0}
  ,
  {"MATERIAL_SHINESTRENGTH", NULL, 0}
  ,
  {"MATERIAL_TRANSPARENCY", NULL, 0}
  ,
  {"MATERIAL_WIRESIZE", NULL, 0}
  ,
  {"MATERIAL_SHADING", NULL, 0}
  ,
  {"MATERIAL_XP_FALLOFF", NULL, 0}
  ,
  {"MATERIAL_SELFILLUM", NULL, 0}
  ,
  {"MATERIAL_FALLOFF", NULL, 0}
  ,
  {"MATERIAL_XP_TYPE", NULL, 0}
  ,
  {"MAP_DIFFUSE", NULL, 0}
  ,
  {"MAP_NAME", NULL, 0}
  ,
  {"MAP_CLASS", NULL, 0}
  ,
  {"MAP_SUBNO", NULL, 0}
  ,
  {"MAP_AMOUNT", NULL, 0}
  ,
  {"BITMAP", ase_read_material_bitmap, 0}
  ,
  {"MAP_TYPE", NULL, 0}
  ,
  {"UVW_U_OFFSET", ase_read_material_bitmap_offset_u, 0}
  ,
  {"UVW_V_OFFSET", ase_read_material_bitmap_offset_v, 0}
  ,
  {"UVW_U_TILING", ase_read_material_bitmap_scale_u, 0}
  ,
  {"UVW_V_TILING", ase_read_material_bitmap_scale_v, 0}
  ,
  {"UVW_ANGLE", ase_read_material_bitmap_uhel, 0}
  ,
  {"UVW_BLUR", NULL, 0}
  ,
  {"UVW_BLUR_OFFSET", NULL, 0}
  ,
  {"UVW_NOUSE_AMT", NULL, 0}
  ,
  {"UVW_NOISE_SIZE", NULL, 0}
  ,
  {"UVW_NOISE_LEVEL", NULL, 0}
  ,
  {"UVW_NOISE_PHASE", NULL, 0}
  ,
  {"BITMAP_FILTER", NULL, 0}
  ,

  {"GEOMOBJECT", ase_read_mesh_start, 0}
  ,
  {"NODE_NAME", ase_read_node_name, 0}
  ,
  {"NODE_PARENT", ase_read_node_name_parent, 0}
  ,
  {"NODE_TM", NULL, 0}
  ,
  //{"NODE_NAME",              NULL,0},
  {"INHERIT_POS", NULL, 0}
  ,
  {"INHERIT_ROT", NULL, 0}
  ,
  {"INHERIT_SCL", NULL, 0}
  ,
  {"TM_ROW0", NULL, 0}
  ,
  {"TM_ROW1", NULL, 0}
  ,
  {"TM_ROW2", NULL, 0}
  ,
  {"TM_ROW3", NULL, 0}
  ,
  {"TM_POS", ase_read_node_pos, 0}
  ,
  {"TM_ROTAXIS", ase_read_node_rot_osa, 0}
  ,
  {"TM_ROTANGLE", ase_read_node_rot_uhel, 0}
  ,
  {"TM_SCALE", ase_read_node_scale, 0}
  ,
  {"TM_SCALEAXIS", NULL, 0}
  ,
  {"TM_SCALEAXISANG", NULL, 0}
  ,

  {"MESH", NULL, 0}
  ,
  {"TIMEVALUE", NULL, 0}
  ,
  {"MESH_NUMVERTEX", ase_read_mesh_numvertex, 0}
  ,
  {"MESH_NUMFACES", ase_read_mesh_numface, 0}
  ,

  {"MESH_VERTEX_LIST", NULL, 0}
  ,
  {"MESH_VERTEX", ase_read_mesh_vertex, 0}
  ,

  {"MESH_FACE_LIST", NULL, 0}
  ,
  {"MESH_FACE", ase_read_mesh_face, 0}
  ,
  //{"MESH_SMOOTHING",     NULL,0},
  //{"MESH_MTLID",       NULL,0},

  {"MESH_FACEMAPLIST", ase_read_mesh_face_map_list, 0}
  ,
  {"MESH_FACEMAP", ase_read_mesh_face_map, 0}
  ,
  {"MESH_FACEMAPVERT", ase_read_mesh_face_map_vertex, 0}
  ,


  {"MESH_NUMTVERTEX", ase_read_mesh_numtvertex, 0}
  ,
  {"MESH_TVERTLIST", NULL, 0}
  ,
  {"MESH_TVERT", ase_read_mesh_tvertex, 0}
  ,

  {"MESH_NUMTVFACES", ase_read_mesh_numtface, 0}
  ,
  {"MESH_TFACELIST", NULL, 0}
  ,
  {"MESH_TFACE", ase_read_mesh_tface, 0}
  ,

  {"MESH_NUMCVERTEX", NULL, 0}
  ,
  {"MESH_NORMALS", NULL, 0}
  ,
  {"MESH_FACENORMAL", NULL, 0}
  ,
  {"MESH_VERTEXNORMAL", NULL, 0}
  ,

  {"TM_ANIMATION", NULL, 0}
  ,
  {"NODE_NAME", ase_read_mesh_tman_node_name, 0}
  ,
  {"CONTROL_POS_TRACK", ase_read_mesh_tman_pos_track, 0}
  ,
  {"CONTROL_POS_SAMPLE", ase_read_mesh_tman_pos_sample, 0}
  ,
  {"CONTROL_ROT_TRACK", ase_read_mesh_tman_rot_track, 0}
  ,
  {"CONTROL_ROT_SAMPLE", ase_read_mesh_tman_rot_sample, 0}
  ,
  {"CONTROL_SCALE_TRACK", ase_read_mesh_tman_scs_track, 0}
  ,
  {"CONTROL_SCALE_SAMPLE", ase_read_mesh_tman_scs_sample, 0}
  ,

  {"PROP_MOTIONBLUR", NULL, 0}
  ,
  {"PROP_CASTSHADOW", NULL, 0}
  ,
  {"PROP_RECVSHADOW", NULL, 0}
  ,
  {"MATERIAL_REF", ase_read_mesh_material, 0}
  ,
};

int ase_chunk_skip(FILE * f, byte * p_chunk)
{
  byte line[1000];
  byte *p_first;

  if (p_chunk[0] == '}') {
    return (TRUE);
  }
  if (strchr(p_chunk, '{')) {
    while (fgets(line, 1000, f)) {
      fgets_korekce(line);
      p_first = ase_zrus_tabulatory(line);
      if (ase_chunk_skip(f, p_first))
        break;
    }
  }
  return (FALSE);
}

int ase_chunk_reader(FILE * f)
{
  byte line[1000];
  byte *p_first;
  int i, d;

  while (fgets(line, 1000, f)) {
    fgets_korekce(line);
    //kprintf(TRUE,line);
    p_first = ase_zrus_tabulatory(line);
    if (p_first) {
      d = ase_najdi_mezeru(p_first);

      // Skusi najit chnuk
      for (i = 0; i < sizeof(ase_table) / sizeof(ase_table[0]); i++) {
        if (d == ase_table[i].delka && !strncmp(p_first, ase_table[i].id, d)) {
          if (ase_table[i].func) {
            //kprintf(TRUE,"Ctu chunk %d",d);
            if (!ase_table[i].func(f, p_first + d)) {
              return (FALSE);
            }
          }
          break;
        }
      }

      // Nenasel odpovidajici zaznam-skip chunk
      if (i == sizeof(ase_table) / sizeof(ase_table[0])) {
        //kprintf(TRUE,"Skip chunk %d",d);
        ase_chunk_skip(f, p_first);
      }
    }
  }
  return (TRUE);
}

void ase_prejmenuj_materialy(EDIT_MATERIAL * p_mat, byte * p_prefix)
{
  byte jmeno[MAX_JMENO];

  while (p_mat) {
    strcpy(jmeno, p_prefix);
    strcat(jmeno, p_mat->jmeno);
    strcpy(p_mat->jmeno, jmeno);
    p_mat = p_mat->p_sub;
  }
}

EDIT_MATERIAL *ase_vrat_material_sub(EDIT_MATERIAL * p_mat, int material)
{
  EDIT_MATERIAL *p_tmp = p_mat;
  int i = 0;

  while (p_tmp) {
    if (material == i)
      return (p_tmp);
    i++;
    p_tmp = p_tmp->p_sub;
  }
  return (NULL);
}

/*
typedef struct _SIMPLE_TRACK_INFO {
  
  int       Objekt_ID;      // id objektu ke kteremu animace patri
  dword     flag;
  
  int       keynum;         // pocet klicu
  BOD      *p_pos;          // Mesh position keys
  BOD      *p_scale;        // Mesh scaling keys
  QUAT     *p_at;           // Absolutni traky
  float    *p_norm;         // Normal-scale animace
  BOD       pivot;          // pivot point animace -> kopiruje se z objektu
  BOD       pivot3ds;       // pivot 3ds animace

  GLMATRIX *p_m;          // kam to hazet
  float    *p_object_norm;// semka strkat normal-scale

  OBB      *p_obb_local;
  OBB      *p_obb_world;
  
  int       childnum;      // pocet svistich traku
  struct   _SIMPLE_TRACK_INFO *p_child; // svisti traky
  struct   _SIMPLE_TRACK_INFO *p_otec;  // otcovy traky
  
} SIMPLE_TRACK_INFO;

typedef struct _HIERARCHY_SIM {
  
  float     delka;         // delka casu animace (fps)
  byte      stav;          // 0 = stop, 1 = run
  
  int       flag;          // flagy animace (GL_LOOP,GL_REMOVE)

  dword     time;          // aktualni cas animace
  dword     time_start;    // aktualni start
  dword     time_stop;     // aktualni stop
  dword     time_delka;    // aktualni delka animace
  
  dword     start;
  dword     stop;  
  
  int       keynum;         // pocet klicu
  byte      jmeno[MAX_JMENO]; // jmeno animace
  int       childnum;         // pocet detskych traku
  SIMPLE_TRACK_INFO *p_child;

} HIERARCHY_SIM;

HIERARCHY_SIM sim[KONT_MAX_ANIM];
int           akt_animace;
*/

void ase_sim_dopln(SIMPLE_TRACK_INFO * p_sim)
{
  BOD lp = { FLT_MAX, FLT_MAX, FLT_MAX };
  BOD ls = { FLT_MAX, FLT_MAX, FLT_MAX };
  QUAT lq = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
  int i;

  // cesta tam
  if (p_sim->p_pos) {
    for (i = 0; i < p_sim->keynum; i++) {
      if (p_sim->p_pos[i].x == FLT_MAX) {
        p_sim->p_pos[i] = lp;
      }
      else {
        lp = p_sim->p_pos[i];
      }
    }
    // a zase zpatky
    for (i = p_sim->keynum - 1; i > -1; i--) {
      if (p_sim->p_pos[i].x == FLT_MAX) {
        p_sim->p_pos[i] = lp;
      }
      else {
        lp = p_sim->p_pos[i];
      }
    }
  }

  // cesta tam
  if (p_sim->p_scale) {
    for (i = 0; i < p_sim->keynum; i++) {
      if (p_sim->p_scale[i].x == FLT_MAX) {
        p_sim->p_scale[i] = ls;
      }
      else {
        ls = p_sim->p_scale[i];
      }
    }
    // a zase zpatky
    for (i = p_sim->keynum - 1; i > -1; i--) {
      if (p_sim->p_scale[i].x == FLT_MAX) {
        p_sim->p_scale[i] = ls;
      }
      else {
        ls = p_sim->p_scale[i];
      }
    }
  }

  // cesta tam
  if (p_sim->p_at) {
    key_sim_calc_absolutne(p_sim);

    for (i = 0; i < p_sim->keynum; i++) {
      if (p_sim->p_at[i].x == FLT_MAX) {
        p_sim->p_at[i] = lq;
      }
      else {
        lq = p_sim->p_at[i];
      }
    }

    for (i = p_sim->keynum - 1; i > -1; i--) {
      if (p_sim->p_at[i].x == FLT_MAX) {
        p_sim->p_at[i] = lq;
      }
      else {
        lq = p_sim->p_at[i];
      }
    }
  }
}

SIMPLE_TRACK_INFO *ase_vrat_objekty(EDIT_KONTEJNER * p_kont, byte * p_otec,
  int *p_num)
{
  SIMPLE_TRACK_INFO *p_child;
  EDIT_OBJEKT *p_obj;
  int o, i, num;

  num = 0;
  oe_olist_reset(&o);
  while ((p_obj = oe_olist_next(p_kont, &o))) {
    if (p_obj->p_tmp_sim) {
      if ((!p_otec[0] && !(p_obj->jmeno_parent[0]))
        || !strcmp(p_otec, p_obj->jmeno_parent)) {
        num++;
      }
    }
  }

  if (!num)
    return (NULL);

  p_child = mmalloc(sizeof(SIMPLE_TRACK_INFO) * num);
  num = 0;
  oe_olist_reset(&o);
  while ((p_obj = oe_olist_next(p_kont, &o))) {
    if (p_obj->p_tmp_sim) {
      if ((!p_otec[0] && !(p_obj->jmeno_parent[0]))
        || !strcmp(p_otec, p_obj->jmeno_parent)) {
        memcpy(p_child + num, p_obj->p_tmp_sim, sizeof(p_obj->p_tmp_sim[0]));
        free(p_obj->p_tmp_sim);
        p_obj->p_tmp_sim = p_child + num;
        num++;
      }
    }
  }

  for (i = 0; i < num; i++) {
    p_obj = lo_najdi_objekt_kont_poiter_ID(p_kont, p_child[i].Objekt_ID);
    assert(p_obj);
    p_child[i].p_child =
      ase_vrat_objekty(p_kont, p_obj->jmeno, &p_child[i].childnum);
  }

  *p_num = num;
  return (p_child);
}

/* Prevede animace z objektu do kontejneru
*/
void ase_uprav_animace(EDIT_KONTEJNER * p_kont)
{
  HIERARCHY_SIM *p_hi = p_kont->sim;

  p_hi->keynum = sim_framenum;
  strcpy(p_hi->jmeno, "ase_animace");

  p_hi->p_child = ase_vrat_objekty(p_kont, "", &p_hi->childnum);
}

int ase_premapuj_kontejner(K_EDITOR * p_cnf, EDIT_KONTEJNER * p_kont,
  EDIT_MATERIAL ** p_list, int matlistnum)
{
  EDIT_MATERIAL *p_tmp;
  int i, l;
  int mat, smat;

  for (i = 0; i < matlistnum; i++) {
    if (p_list[i]->p_sub)
      ase_prejmenuj_materialy(p_list[i]->p_sub, p_list[i]->jmeno);
  }

  if (p_kont) {
    for (i = 0; i < MAX_KONT_OBJEKTU; i++) {
      if (p_kont->p_obj[i]) {
        mat = p_kont->p_obj[i]->material;
        smat = p_kont->p_obj[i]->material_sub;
        p_tmp = ase_vrat_material_sub(p_list[mat], smat);
        l = lo_najdi_material(p_cnf->p_mat, MAX_EDIT_MATERIALU, p_tmp->jmeno);
        if (l == K_CHYBA) {     // novy material
          l = lo_najdi_prazdny_material(p_cnf->p_mat, MAX_EDIT_MATERIALU);
          p_cnf->p_mat[l] = kopiruj_material(p_tmp);
        }
        p_kont->p_obj[i]->material = l;
        p_kont->p_obj[i]->material_sub = 0;
        if (p_kont->p_obj[i]->p_tmp_sim) {
          p_kont->p_obj[i]->Objekt_ID = i;
          p_kont->p_obj[i]->p_tmp_sim->Objekt_ID = i;
          p_kont->p_obj[i]->p_tmp_sim->p_m = &p_kont->p_obj[i]->m;
          p_kont->kflag |= KONT_KEYFRAME;
          ase_sim_dopln(p_kont->p_obj[i]->p_tmp_sim);
        }
      }
    }
  }

  ase_uprav_animace(p_kont);

  // Smazani seznamu materialu  
  for (i = 0; i < matlistnum; i++) {
    if (p_list[i]) {
      zrus_material(p_list + i);
    }
  }

  return (TRUE);
}

void ase_kont_to_world(EDIT_KONTEJNER * p_kont)
{
  EDIT_OBJEKT *p_obj;
  GLMATRIX m;
  int o;

  if (p_kont->kflag & KONT_KEYFRAME) {

    oe_olist_reset(&o);
    while ((p_obj = oe_olist_next(p_kont, &o))) {
      if (p_obj->p_tmp_sim) {
        p_obj->p_tmp_sim->pivot = p_obj->pivot;
      }
    }

    p_kont->akt_animace = 0;
    key_kontejner_sim_reanimuj_pivoty(p_kont);

    oe_olist_reset(&o);
    while ((p_obj = oe_olist_next(p_kont, &o))) {
      if (p_obj->p_tmp_sim) {
        p_obj->pivot = p_obj->p_tmp_sim->pivot;
      }
    }

    key_kontejner_sim_reanimuj(p_kont);
    oe_olist_reset(&o);
    while ((p_obj = oe_olist_next(p_kont, &o))) {
      if (p_obj->p_tmp_sim) {
        invert_matrix(&p_obj->m, &m);
        transformuj_objekt_matici(p_obj, &m);
      }
    }
  }
}


EDIT_KONTEJNER *ase_import(byte * p_file, byte * p_dir)
{
  FILE *f;
  int i;

  _chdir(p_dir);
  f = fopen(p_file, "r");
  if (!f)
    return (NULL);

  p_kont = NULL;
  p_mat = NULL;
  p_mat_list = NULL;
  p_vertex = NULL;
  p_tvertex = NULL;
  p_face = NULL;
  p_tface = NULL;
  p_track = NULL;
  sub_mat = 0;
  load_animaci = TRUE;

  for (i = 0; i < sizeof(ase_table) / sizeof(ase_table[0]); i++) {
    ase_table[i].delka = ase_najdi_mezeru(ase_table[i].id);
  }

  if (!ase_chunk_reader(f)) {
    kprintf(TRUE, "Chyba nacitani ASE souboru!");
    if (p_kont)
      zrus_kontejner(&p_kont, &p_cnf->p_mlha);
    return (NULL);
  }

  oe_kont_dopln_prazdne_child(p_kont);
  ds_aplikuj_3ds_text_mapping(p_mat_list, p_kont);

  if (!ase_premapuj_kontejner(p_cnf, p_kont, p_mat_list, mat_list_num)) {
    if (p_kont)
      zrus_kontejner(&p_kont, &p_cnf->p_mlha);
    return (NULL);
  }

  ase_transformuj_objekty(p_kont);
  ase_dopln_animace(p_kont);

  null_free((void **) &p_mat_list);

  updatuj_kontejner_statistika(p_kont, TRUE);
  nastav_barvu_bodu_kont_dword(p_kont, 0xffffffff, COLOR_SET);

  strcpy(p_cnf->tdir.scene_texture_dir, work_dir);
  oe_reload_textur(p_cnf);

  return (p_kont);
}


// doplni animaci tohoto objektu - je to objekt bez animace
void oe_kont_dopln_prazdne_child_rec(EDIT_KONTEJNER * p_kont,
  EDIT_OBJEKT * p_obj)
{
  SIMPLE_TRACK_INFO *p_track;
  EDIT_OBJEKT *p_parent;

  p_track = mmalloc(sizeof(p_track[0]));
  p_track->keynum = sim_framenum;
  p_obj->p_tmp_sim = p_track;

  if (p_obj->jmeno_parent[0]
    && (p_parent = lo_najdi_objekt_kont_poiter(p_kont, p_obj->jmeno_parent))
    && !p_parent) {
    oe_kont_dopln_prazdne_child_rec(p_kont, p_parent);
  }
}


void oe_kont_dopln_prazdne_child(EDIT_KONTEJNER * p_kont)
{
  EDIT_OBJEKT *p_obj;
  int o;

  oe_olist_reset(&o);
  while (p_obj = oe_olist_next(p_kont, &o)) {
    if (!p_obj->p_tmp_sim) {    // najdu paret a doplnim ho
      oe_kont_dopln_prazdne_child_rec(p_kont, p_obj);
    }
  }
}

void ase_transformuj_anim_objekt(EDIT_OBJEKT * p_obj)
{
  GLMATRIX m;

  invert_matrix(&p_obj->m, &m);
  transformuj_objekt_matici(p_obj, &m);
}

void ase_dopln_sim_track_pos(SIMPLE_TRACK_INFO * p_track, BOD * p_pos)
{
  int i;

  if (!p_track->p_pos) {
    p_track->p_pos = mmalloc(sizeof(p_track->p_pos[0]) * p_track->keynum);
    for (i = 0; i < p_track->keynum; i++) {
      p_track->p_pos[i] = *p_pos;
    }
  }
}

void ase_dopln_sim_track_rot(SIMPLE_TRACK_INFO * p_track, QUAT * p_quat)
{
  int i;

  if (!p_track->p_at) {
    p_track->p_at = mmalloc(sizeof(p_track->p_at[0]) * p_track->keynum);
    for (i = 0; i < p_track->keynum; i++) {
      p_track->p_at[i] = *p_quat;
    }
  }
}

void ase_dopln_sim_track_scs(SIMPLE_TRACK_INFO * p_track, BOD * p_scs)
{
  int i;

  if (!p_track->p_scale) {
    p_track->p_scale = mmalloc(sizeof(p_track->p_scale[0]) * p_track->keynum);
    for (i = 0; i < p_track->keynum; i++) {
      p_track->p_scale[i] = *p_scs;
    }
  }
}

/* Rekurzivne doplni pos
*/
void ase_dopln_animace_rec(EDIT_KONTEJNER * p_kont, EDIT_OBJEKT * p_obj)
{
  SIMPLE_TRACK_INFO *p_sim;
  EDIT_OBJEKT *p_top;
  GLMATRIX *p_diff, tm;
  BOD p, s;
  QUAT q;

  p_top = lo_najdi_objekt_kont_poiter(p_kont, p_obj->jmeno_parent);
  if (p_top) {
    invert_matrix(&p_top->m, &tm);
    p_diff = mat_mult(&p_obj->m, &tm, &tm);
  }
  else {
    p_diff = &p_obj->m;
  }

  p_sim = p_obj->p_tmp_sim;
  if (p_sim->p_pos || p_sim->p_at || p_sim->p_scale) {
    if (!p_sim->p_pos) {
      matrix_to_pos(p_diff, &p);
      ase_dopln_sim_track_pos(p_sim, &p);
    }
    if (!p_sim->p_at) {
      matrix_to_quat(p_diff, &q);
      ase_dopln_sim_track_rot(p_sim, &q);
    }
    if (!p_sim->p_scale) {
      matrix_to_scale(p_diff, &s);
      ase_dopln_sim_track_scs(p_sim, &s);
    }
  }
  else {
    p_sim->p_all_matrix = mmalloc(sizeof(p_sim->p_all_matrix[0]));
    *(p_sim->p_all_matrix) = *p_diff;
  }

  if (p_top && p_top->p_tmp_sim) {
    p_sim = p_top->p_tmp_sim;
    if (p_sim && p_sim->p_at && !p_sim->p_pos)
      ase_dopln_animace_rec(p_kont, p_top);
  }
}

/* Pojedu po animacnim stromu a pokud zjistim ze neco ma rotaci a nema
   posun, doplnim to rekurzivne
*/
void ase_dopln_animace(EDIT_KONTEJNER * p_kont)
{
  EDIT_OBJEKT *p_obj;
  int o;

  // Doplnim animace
  oe_olist_reset(&o);
  while (p_obj = oe_olist_next(p_kont, &o)) {
    if (p_obj->p_tmp_sim) {
      ase_dopln_animace_rec(p_kont, p_obj);
    }
  }

  // Vymaznu reference na animace
  oe_olist_reset(&o);
  while (p_obj = oe_olist_next(p_kont, &o)) {
    p_obj->p_tmp_sim = NULL;
  }
}

void ase_transformuj_objekty(EDIT_KONTEJNER * p_kont)
{
  EDIT_OBJEKT *p_obj;
  GLMATRIX m;
  int o;

  oe_olist_reset(&o);
  while (p_obj = oe_olist_next(p_kont, &o)) {
    invert_matrix(&p_obj->m, &m);
    transformuj_objekt_matici(p_obj, &m);
  }
}
