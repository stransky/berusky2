/************************************************************************
    3DS file format reader v1.4
    Written by Lukas Pokorny, a.k.a.Shakul/MovSD.
    Based on 3DSRDR v1.2 by Javier Arevalo, a.k.a. Jare/Iguana.

/*************************************************************************

    - DISCLAIMER -

    I hope I have not broken any patents or trade secrets by releasing
        this info. This is purely a mind exercise to break into a file
        format that is quite useful to know. As far as I have been told
        a file format is not subject to anything such as copyright or
        patent, so I have done this because I believe I'm allowed to.

    I PLACE THIS FILE IN THE PUBLIC DOMAIN, SO EVERYTHING CONTAINED HERE
        IS TOTALLY FREE FOR YOU TO EXPLORE AND USE. I DISCLAIM ANY AND ALL
        EVENTS COMING OUT OF ANY POSSIBLE USE (OR LACK OF USE) OR EXISTANCE
        OF THIS FILE. I WON'T BE LIABLE FOR ANYTHING RELATED TO THIS FILE,
        OR ANY PRIOR OR FUTURE VERSION OF IT.

    All trademarks mentioned are property of their respective holders.


/*************************************************************************

This 3DS reader is based on 3DSRDR v1.2 by Javier Arevalo, AKA Jare/Iguana.

I needed good 3DS reader and 3DSRDR v1.2 was best one around.
So I rewrote it and added lot of new chunks readers (nearly 40 of them.)
Actually I changed the structure of the original source i lot, so that's
why I don't call this 3DSRDR.

Sources of chunk info were:
    The Unofficial 3DS File Format v1.0
    by Jeff Lewis (werewolf@worldgate.com)

and

    3D Studio File Format
    Autodesk Ltd.
    Document Revision 0.91 - June 1996
     Rewritten by Martin van Velsen (email: vvelsen@ronix.ptf.hro.nl )
     and Robin Fercoq ( 3ds-bin + mli )(email: robin@msrwww.fc-net.fr)
     Based on Original document by Jim Pitts ( email: jp5@ukc.ac.uk )

     (It's quite strange that this Autodesk text is still missing
      many importand chunks)

Some undocumented chunks (e.g. BoxMap) digged by me (Lukas Pokorny.)


Original info from 3DSRDR v1.2 follows:

    Heavily based on info on the file 3DS_08.TXT by Jim Pitts
      (jp5@ukc.ac.uk)

    Basic material-related stuff digged up by Jare. Track info stuff too.

    Thanks to the Egerter brothers of WGT fame and to Walken/Impact studios
    for extra info, Rex Deathstar for support. And definitely to
    Xanthome/Darkzone for you know why. And of course, respect to
    Avatar/Legend Design for being here before all of us.

    For a cool example of actual reading of 3DS files, look no
    further than 3DSCO20.ZIP by Mats Byggmastar aka. MRI. I
    personally prefer using a table-driven modification of this
    code, but both approaches are quite ok and his is much faster
    to write and follow.

    Now only lack is someone to explain how to make use of all this
    stuff i.e. how exactly is data stored, how spline interpolations
    are performed, what are those things called quaternions, etc. And
    also, maybe, dig the rest of the chunks until we are actually able
    to write 3DS files instead of just being bored reading. There's
    lots to do.

    If you decide to work on this further, please make your findings
    public like we have already done, ok? Upload it to
    x2ftp.oulu.fi, THE place for programming info, and/or to
    ftp.cdrom.com. But please PUBLISH it!

/*************************************************************************

Change log:

    3DSREAD v1.4:     (Lukas Pokorny)
        - Added file version and mesh version readers.
        - Added more material attributes readers (shiness, transparency,
          map flags, shading type, and more!
        - Added box mapping reader.
        - Added dummy name reader.
        - Info about track flags on keyframes.
        - Added color, morph, hide, fov, roll tracks.
        - Error checking on file reading improved.
        - Chunk reading routines now take only one file handle,
          other parametrs are global variables. I like it more this
          way.
        - Code structure changed, I like it more this way :-)

    3DSRDR v1.2:      (Javier Arevalo)
        - Added change log to have some idea what's going on.
        - Added pivot point reading inside tracks stuff.
        - Info about spline flags on keyframes.
        - Added face edge visibility info.
        - Finally!! Those flags that mark when the texture is wrapping
          around inside a face. This happens when you apply spherical
          or cylindrical coordinates, the faces along the 0§ axis don't
          get proper mapping coords. Someone describe how to fix this?
        - Added -quiet parm, only displays minimal chunk info.
        - Object parent number is stored in CHUNK_TRACKOBJNAME.
          This makes reference to the node number in CHUNK_OBJNUMBER.
        - Object number changed to unsigned. Parent 65535 means none.
        - Added CHUNK_PRJ and CHUNK_MLI to allow inspecting .PRJ and
          .MLI files (they're basically the same chunks as .3DS).
        - Added banner to identify myself, and disclaimer for "just in
          case" possibilities.
        - Corrected possible bug when chunklen == 0 (it was not a chunk).
        - Added several name descriptions of chunks.
/***********************************************************************/
/*
  import/export 3ds-meshu
*/
#include "mss_on.h"
#include <windows.h>
#include "objekt_editor_all.h"

#pragma pack(2)

typedef struct
{
  word id;
  dword len;
} TChunkHeader, *PChunkHeader;

#pragma pack()

enum
{
  CHUNK_VER = 0x0002,
  CHUNK_RGBF = 0x0010,
  CHUNK_RGBB = 0x0011,
  CHUNK_RGBB2 = 0x0012,
  CHUNK_AMOUNTOF = 0x0030,      // added
  CHUNK_AMOUNTOFF = 0x0031,     // tbc (float)
  CHUNK_MASTERSCALE = 0x0100,   // added
  CHUNK_PRJ = 0xC23D,
  CHUNK_MLI = 0x3DAA,
  CHUNK_MESHVER = 0x3D3E,

  CHUNK_MAIN = 0x4D4D,
  CHUNK_OBJMESH = 0x3D3D,
  CHUNK_BKGCOLOR = 0x1200,
  CHUNK_AMBCOLOR = 0x2100,
  CHUNK_OBJBLOCK = 0x4000,
  CHUNK_TRIMESH = 0x4100,
  CHUNK_VERTLIST = 0x4110,
  CHUNK_VERTFLAGS = 0x4111,     // added
  CHUNK_FACELIST = 0x4120,
  CHUNK_FACEMAT = 0x4130,
  CHUNK_MAPLIST = 0x4140,
  CHUNK_SMOOLIST = 0x4150,
  CHUNK_TRMATRIX = 0x4160,
  CHUNK_MESHCOLOR = 0x4165,     // added
  CHUNK_TXTINFO = 0x4170,       // added
  CHUNK_BOXMAP = 0x4190,        // added
  CHUNK_LIGHT = 0x4600,
  CHUNK_SPOTLIGHT = 0x4610,
  CHUNK_CAMERA = 0x4700,
  CHUNK_HIERARCHY = 0x4F00,

  CHUNK_VIEWPORT = 0x7001,

  CHUNK_MATERIAL = 0xAFFF,
  CHUNK_MATNAME = 0xA000,
  CHUNK_AMBIENT = 0xA010,
  CHUNK_DIFFUSE = 0xA020,
  CHUNK_SPECULAR = 0xA030,
  CHUNK_SHININESS = 0xA040,     // added
  CHUNK_SHINSTRENGTH = 0xA041,  // added
  CHUNK_TRANSPARENCY = 0xA050,  // added
  CHUNK_TRANSFALLOFF = 0xA052,  // added
  CHUNK_REFBLUR = 0xA053,       // added
  CHUNK_SELFILLUM = 0xA084,     // added
  CHUNK_TWOSIDED = 0xA081,      // added
  CHUNK_TRANSADD = 0xA083,      // added
  CHUNK_WIREON = 0xA085,        // added
  CHUNK_WIRESIZE = 0xA087,      // added
  CHUNK_SOFTEN = 0xA08C,        // added
  CHUNK_SHADING = 0xA100,       // added
  CHUNK_TEXTURE = 0xA200,
  CHUNK_REFLECT = 0xA220,       // added
  CHUNK_BUMPMAP = 0xA230,
  CHUNK_BUMPAMOUNT = 0xA252,    // added
  CHUNK_MAPFILE = 0xA300,
  CHUNK_MAPFLAGS = 0xA351,      // added
  CHUNK_MAPBLUR = 0xA353,       // added
  CHUNK_MAPUSCALE = 0xA354,     // added
  CHUNK_MAPVSCALE = 0xA356,     // added
  CHUNK_MAPUOFFSET = 0xA358,    // added
  CHUNK_MAPVOFFSET = 0xA35A,    // added
  CHUNK_MAPROTANGLE = 0xA35C,   // added
  CHUNK_MATXPFALLIN = 0xA08A,

  CHUNK_KEYFRAMER = 0xB000,
  CHUNK_AMBIENTKEY = 0xB001,
  CHUNK_TRACKINFO = 0xB002,
  CHUNK_TRACKCAMERA = 0xB003,
  CHUNK_TRACKCAMTGT = 0xB004,
  CHUNK_TRACKLIGHT = 0xB005,
  CHUNK_TRACKLIGTGT = 0xB006,
  CHUNK_TRACKSPOTL = 0xB007,
  CHUNK_FRAMES = 0xB008,
  CHUNK_KFCURTIME = 0xB009,     // added
  CHUNK_KFHDR = 0xB00A,         // added

  CHUNK_TRACKOBJNAME = 0xB010,
  CHUNK_DUMMYNAME = 0xB011,     // added
  CHUNK_PRESCALE = 0xB012,      // added
  CHUNK_TRACKPIVOT = 0xB013,
  CHUNK_BOUNDBOX = 0xB014,      // added
  CHUNK_MORPH_SMOOTH = 0xB015,  // added
  CHUNK_TRACKPOS = 0xB020,
  CHUNK_TRACKROTATE = 0xB021,
  CHUNK_TRACKSCALE = 0xB022,    // added?
  CHUNK_TRACKFOV = 0xB023,
  CHUNK_TRACKROLL = 0xB024,
  CHUNK_TRACKCOLOR = 0xB025,    // added
  CHUNK_TRACKMORPH = 0xB026,    // added
  CHUNK_TRACKHOTSPOT = 0xB027,  // added
  CHUNK_TRACKFALLOFF = 0xB028,  // added
  CHUNK_TRACKHIDE = 0xB029,     // added
  CHUNK_OBJNUMBER = 0xB030,
};

#define Read_NULL NULL
#define OK 1
#define ERROR_READER (-1)

#define  COLOR_AMBIENT 1
#define  COLOR_DIFFUSE 2

static EDIT_KONTEJNER *p_kont;  //akt kont
static EDIT_OBJEKT *p_obj;      //akt obj
static int akt_objekt;
static int akt_matros;
static int akt_text;
static int akt_kamera;
static int track_mod;           // 0 = objekt,

                                      // 1 = camera track, 
                                      // 2 = target track
static int akt_light;
static int akt_cislo_objektu;
static int hd_okno;
static int druh_barvy;
static int texture_map;
static KEYFRAMER_DATA keyframe;
static TRACK_INFO *p_track;     // pointer na aktualni track
static int dummy_objektu;
static byte jmeno_objektu[200];
static KAMERA kamery[MAX_EDIT_KAMER];

#define  X_LOAD  100
#define  Y_LOAD  450
#define  X_RADKU 50
#define  Y_RADKU 6


static int Read_Skip(FILE * f)
{
  if (f) {
  };
  return OK;
}

//static int Read_NULL (FILE *f) {if (f) {}; return OK;}

static word Face_Num;
static int ind;

#define spaces(n) printf("%*s", n, "")

void key_3ds_to_ber(KEY_POINT_3DS * p_3ds, KEY_POINT_BRS * p_brs)
{
  p_brs->bias = p_3ds->bias;
  p_brs->continuity = p_3ds->continuity;
  p_brs->easefrom = p_3ds->easefrom;
  p_brs->easeto = p_3ds->easeto;
  p_brs->tension = p_3ds->tension;
  p_brs->time = p_3ds->frame * SIM_KONSTI;
}

static int check_track(KEY_POINT_BRS * p_key, int num, int *p_klic)
{
  int i;

  for (i = 1; i < num; i++) {
    if (p_key[i - 1].time == p_key[i].time) {
      *p_klic = i - 1;
      return (FALSE);
    }
  }

  return (TRUE);
}

// 3DS version reader
static int Read_Ver(FILE * f)
{
  short v;

  if (fread(&v, sizeof(v), 1, f) != 1)
    return ERROR_READER;
  v++;
  kprintf(TRUE, "Version of 3ds file: %d", v);
  return OK;
}

// Mesh version reader
static int Read_MeshVer(FILE * f)
{
  short v;

  if (fread(&v, sizeof(v), 1, f) != 1)
    return ERROR_READER;
  v++;
  kprintf(TRUE, "Mesh version: %d", v);
  return OK;
}

// RGB (float) reader
static int Read_RGBF(FILE * f)
{
  float c[3];

  if (fread(c, sizeof(c), 1, f) != 1)
    return ERROR_READER;
  oe_wrlog(p_cnf, "Red: %f, Green: %f, Blue: %f", c[0], c[1], c[2]);
  return OK;
}

// RGB (byte) reader
static int Read_RGBB(FILE * f)
{
  byte c[3];

  if (fread(c, sizeof(c), 1, f) != 1)
    return ERROR_READER;

  oe_wrlog(p_cnf, "Red: %d, Green: %d, Blue: %d", c[0], c[1], c[2]);

  if (druh_barvy == COLOR_AMBIENT) {
    p_cnf->p_mat[akt_matros]->dxmat.ambient_r = (float) c[0] / MAX_BYTE_F;
    p_cnf->p_mat[akt_matros]->dxmat.ambient_g = (float) c[1] / MAX_BYTE_F;
    p_cnf->p_mat[akt_matros]->dxmat.ambient_b = (float) c[2] / MAX_BYTE_F;
  }
  else if (druh_barvy == COLOR_DIFFUSE) {
    p_cnf->p_mat[akt_matros]->dxmat.diffuse_r = (float) c[0] / MAX_BYTE_F;
    p_cnf->p_mat[akt_matros]->dxmat.diffuse_g = (float) c[1] / MAX_BYTE_F;
    p_cnf->p_mat[akt_matros]->dxmat.diffuse_b = (float) c[2] / MAX_BYTE_F;
    p_cnf->p_mat[akt_matros]->dxmat.diffuse_a = 1.0f;
  }
  druh_barvy = 0;
  return OK;
}

// Amount reader
static int Read_AmountOf(FILE * f)
{
  word a;

  if (fread(&a, sizeof(a), 1, f) != 1)
    return ERROR_READER;
  oe_wrlog(p_cnf, "Amount: %d", a);
  return OK;
}

// Master scale reader
static int Read_MasterScale(FILE * f)
{
  float s;

  if (fread(&s, sizeof(s), 1, f) != 1)
    return ERROR_READER;
  oe_wrlog(p_cnf, "Master scale: %f\n", s);
  return OK;
}

// ASCIIZ name reader (!!!)
static int ASCIIZReader(FILE * f)
{
  int c;

  while ((c = fgetc(f)) != EOF && c != '\0');
  if (c == EOF)
    return ERROR_READER;
  return OK;
}

static int ASCIIZReader_muj(FILE * f, byte * p_buffer, int max)
{
  int c, i = 0;

  while ((c = fgetc(f)) != EOF && c != '\0') {
    if (i < max)
      p_buffer[i++] = c;
  }
  p_buffer[i] = 0;
  if (c == EOF)
    return ERROR_READER;
  return OK;
}

// Object block reader
/*
  Nacteni jmena objektu + zacatek objektu
*/
static int Read_ObjBlock(FILE * f)
{
  if (ASCIIZReader_muj(f, jmeno_objektu, MAX_JMENO) != OK)
    return ERROR_READER;
  akt_cislo_objektu++;
  kprintf(TRUE, "Object name: %s cislo %d", jmeno_objektu, akt_cislo_objektu);
  return OK;
}

static int Read_TriMesh(FILE * f)
{
  kprintf(TRUE, "Mesh %s na pozici %d", jmeno_objektu, akt_objekt);
  p_obj = p_kont->p_obj[akt_objekt] = vyrob_objekt();
  p_obj->Objekt_ID = akt_objekt;
  strcpy(p_obj->jmeno, jmeno_objektu);
  akt_objekt++;
  return OK;
}

// Vertex list reader
static int Read_VertList(FILE * f)
{
  word nv, i;
  float c[3];

  if (!p_obj) {
    ddw("Chyba nacitani objektu ! -> nulovy obj");
    return ERROR_READER;
  }
  if (p_obj->p_vertex) {
    ddw("Chyba nacitani objektu ! -> Obsazene vertexy !");
    return ERROR_READER;
  }

  if (fread(&nv, sizeof(nv), 1, f) != 1)
    return (ERROR_READER);

  kprintf(TRUE, "Vertexu %d", nv);

  p_obj->vertexnum = nv;
  if ((p_obj->p_vertex = (MUJ_BOD *) malloc(sizeof(MUJ_BOD) * nv)) == NULL) {
    chyba("pamet");
  }

  for (i = 0; i < nv; i++) {
    if (fread(c, sizeof(c), 1, f) != 1)
      return ERROR_READER;
    p_obj->p_vertex[i].x = c[0];
    p_obj->p_vertex[i].y = c[2];
    p_obj->p_vertex[i].z = c[1];
  }
  return OK;
}

// Vertex flags reader
static int Read_VertFlags(FILE * f)
{
  word nv;
  word fl;

  if (fread(&nv, sizeof(nv), 1, f) != 1)
    return ERROR_READER;
//  spaces(ind);
//  printf("Vertices flags: %d\n", nv);

  while (nv-- > 0) {
    if (fread(&fl, sizeof(fl), 1, f) != 1)
      return ERROR_READER;
//    spaces(ind+4);
//    printf("Flags: %d\n", fl);
  }
  return OK;
}

// Face list reader
static int Read_FaceList(FILE * f)
{
  word nv, i;
  word c[3];
  word flags;

  if (f) {
  };

  if (!p_obj) {
    ddw("Chyba - prazdny objekt !");
    return (ERROR_READER);
  }
  if (p_obj->p_face) {
    ddw("Chyba - obsazene facy !");
    return (ERROR_READER);
  }

  if (fread(&nv, sizeof(nv), 1, f) != 1)
    return (ERROR_READER);

  // vyrobeni plosek
  if ((p_obj->p_face = (FACE *) malloc(sizeof(FACE) * nv * 3)) == NULL) {
    chyba("pamet");
  }
  p_obj->facenum = nv * 3;
  kprintf(TRUE, "Facu %d", nv);

  Face_Num = nv;
  for (i = 0, nv *= 3; i < nv; i += 3) {
    if (fread(c, sizeof(c), 1, f) != 1)
      return (ERROR_READER);
    if (fread(&flags, sizeof(flags), 1, f) != 1)
      return (ERROR_READER);
    p_obj->p_face[i] = c[0];
    p_obj->p_face[i + 1] = c[1];
    p_obj->p_face[i + 2] = c[2];

/*  spaces(ind+2);
    printf("A %d, B %d, C %d, 0x%X:", c[0], c[1], c[2], flags);
    printf(" AB %d BC %d CA %d UWrap %d VWrap %d\n",
           (flags & 0x04) != 0, (flags & 0x02) != 0, (flags & 0x01) != 0,
           (flags & 0x08) != 0, (flags & 0x10) != 0);
*/
  }
  return OK;
}

// Face material reader
static int Read_FaceMat(FILE * f)
{
  word n, nf, i;
  byte mtname[50];

//  spaces(ind);
//  printf("Material name for faces: ");
  if (ASCIIZReader_muj(f, mtname, 49) != OK)
    return (ERROR_READER);

// najdu si material pro akt objekt

  _strlwr(mtname);
  i = lo_najdi_material(p_cnf->p_mat, MAX_EDIT_MATERIALU, mtname);
  if (i != K_CHYBA) {
    p_obj->material = i;
    p_obj->m1flag = p_cnf->p_mat[i]->flag;
    p_obj->m2flag = p_cnf->p_mat[i]->flag2;
  }

  if (fread(&n, sizeof(n), 1, f) != 1)
    return ERROR_READER;
//  spaces(ind);
//  printf("Faces with this material: %d\n", n);

  while (n-- > 0) {
    if (fread(&nf, sizeof(nf), 1, f) != 1)
      return ERROR_READER;
//    spaces(ind+4);
//    printf("Face %d\n", nf);
  }
  return OK;
}

// Maping list reader
static int Read_MapList(FILE * f)
{
  word nv, i;
  float c[2];

  if (!p_obj) {
    ddw("p_obj = 0");
    return (ERROR_READER);
  }

  if (fread(&nv, sizeof(nv), 1, f) != 1)
    return (ERROR_READER);

  if (p_obj->vertexnum != nv) {
    ddw("Rozdilne pocty map-list a vertexlist !");
    if (p_obj->vertexnum < nv)
      return (ERROR_READER);
  }

  for (i = 0; i < nv; i++) {
    if (fread(c, sizeof(c), 1, f) != 1)
      return (ERROR_READER);
    if ((c[0] < 1024 * 1024) && (c[1] < 1024 * 1024) &&
      (c[0] > -1024 * 1024) && (c[1] > -1024 * 1024))
      p_obj->p_vertex[i].tu1 = c[0];
    p_obj->p_vertex[i].tv1 = c[1];
  }
  return OK;
}

// Smoothing list reader
static int Read_SmooList(FILE * f)
{
  dword s;
  word nv;

  nv = Face_Num;

  while (nv-- > 0) {
    if (fread(&s, sizeof(s), 1, f) != 1)
      return ERROR_READER;
/*
    spaces(ind+4);
    for (i = 0; i < 32; i++) if (s & (1 << i)) printf("%d, ", i + 1);
    printf("\n");
*/
  }
  return OK;
}

// Transformation matrix reader
static int Read_TrMatrix(FILE * f)
{
  GLMATRIX mat;
  static int cislo = 0;
  float m[4][3];
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 3; j++) {
      fread(&m[i][j], sizeof(float), 1, f);
    }
  }

  init_matrix(&mat);

  mat._11 = m[0][0];
  mat._21 = m[2][0];
  mat._31 = m[1][0];

  mat._12 = m[0][2];
  mat._22 = m[2][2];
  mat._32 = m[1][2];

  mat._13 = m[0][1];
  mat._23 = m[2][1];
  mat._33 = m[1][1];

  mat._41 = m[3][0];
  mat._42 = m[3][2];
  mat._43 = m[3][1];

  p_obj->m = mat;

  //invert_matrix(&mat,&p_obj->m);
  oe_wrlog_matrix(p_cnf, &mat, "Matice objektu %d", cislo++);
  return OK;
}

// Box mapping reader
static int Read_BoxMap(FILE * f)
{

  oe_wrlog(p_cnf, "Box-Mapa");
  if (ASCIIZReader(f) != OK)
    return ERROR_READER;
  if (ASCIIZReader(f) != OK)
    return ERROR_READER;
  if (ASCIIZReader(f) != OK)
    return ERROR_READER;
  if (ASCIIZReader(f) != OK)
    return ERROR_READER;
  if (ASCIIZReader(f) != OK)
    return ERROR_READER;
  if (ASCIIZReader(f) != OK)
    return ERROR_READER;

  return OK;
}

// Light position reader
static int Read_Light(FILE * f)
{
  float c[3];

  if (fread(c, sizeof(c), 1, f) != 1)
    return ERROR_READER;
  oe_wrlog(p_cnf, "Pozice svetla - X: %f, Y: %f, Z: %f", c[0], c[1], c[2]);
  return OK;
}

// Spotlight target reader
static int Read_SpotLight(FILE * f)
{
  float c[5];

  if (fread(c, sizeof(c), 1, f) != 1)
    return ERROR_READER;
  oe_wrlog(p_cnf,
    "Spotlight - Target X: %f, Y: %f, Z: %f; Hotspot %f, Falloff %f", c[0],
    c[1], c[2], c[3], c[4]);
  return OK;
}

// Camera reader
static int Read_Camera(FILE * f)
{
  float c[8];
  BOD p = { 0, 0, 0 };

  if (fread(c, sizeof(c), 1, f) != 1)
    return ERROR_READER;

  kprintf(TRUE, "Camera %s %d", jmeno_objektu, akt_kamera);
  kprintf(TRUE, "Position: X: %f, Y: %f, Z: %f", c[0], c[1], c[2]);
  kprintf(TRUE, "Target: X: %f, Y: %f, Z: %f", c[3], c[4], c[5]);
  kprintf(TRUE, "Bank: %f, Lens: %f", c[6], c[7]);

  kamery[akt_kamera].cislo = akt_cislo_objektu;
  strcpy(kamery[akt_kamera].jmeno, jmeno_objektu);

  // pozice
  kamery[akt_kamera].p.x = c[0];
  kamery[akt_kamera].p.z = c[1];
  kamery[akt_kamera].p.y = c[2];

  // target
  kamery[akt_kamera].t.x = c[3];
  kamery[akt_kamera].t.y = c[5];
  kamery[akt_kamera].t.z = c[4];

  // roll
  kamery[akt_kamera].roll = DEG2RAD(c[6]);      // ratace kamery podle osy pohledu

  // fov
  kamery[akt_kamera].fov =
    DEG2RAD((fabs(c[7]) < EPSILON) ? 45.0f : 2400.0f / c[7]);

  // pyramida
  kamery[akt_kamera].p_objekt = kameru_dratovou_vyrob(&kamery[akt_kamera].p,
    &kamery[akt_kamera].t, kamery[akt_kamera].fov, c[6]);

  // position-body  
  kamery[akt_kamera].p_pos =
    vyrob_krychli_dratovou(&p, 0.2f, 0.2f, 0.2f, 0xffffffff);

  // target-body
  p.z = vzdal_bodu_bod(&kamery[akt_kamera].p, &kamery[akt_kamera].t);
  kamery[akt_kamera].p_target =
    vyrob_krychli_dratovou(&p, 0.2f, 0.2f, 0.2f, 0xffffffff);

  akt_kamera++;
  return OK;
}

// Material name reader
// novy material - asi tezko ma jeden material 2 jmena
static int Read_MatName(FILE * f)
{

  if ((akt_matros =
      lo_najdi_prazdny_material(p_cnf->p_mat,
        MAX_EDIT_MATERIALU)) == K_CHYBA) {
    chyba("Neni volny material !");
  }
  if (!(p_cnf->p_mat[akt_matros] = vyrob_material()))
    chyba("Tvorby matrose !");

  if (ASCIIZReader_muj(f, p_cnf->p_mat[akt_matros]->jmeno, 30) != OK)
    return (ERROR_READER);

  _strlwr(p_cnf->p_mat[akt_matros]->jmeno);

  kprintf(TRUE, "Material %d '%s'", akt_matros,
    p_cnf->p_mat[akt_matros]->jmeno);
  return OK;
}

static int Read_Transparent(FILE * f)
{
  if (f) {
  };
  kprintf(TRUE, "Transparency");
  return OK;
}

// Two sided material reader
static int Read_TwoSided(FILE * f)
{
  if (f) {
  };

  kprintf(TRUE, "Two-sided");
  return OK;
}

// Trans add material
static int Read_TransAdd(FILE * f)
{
  if (f) {
  };
  kprintf(TRUE, "Trans-add");
  return OK;
}

// Wireframe material
static int Read_WireOn(FILE * f)
{
  if (f) {
  };
  kprintf(TRUE, "Wireframe\n");
  return OK;
}

// Wire size reader
static int Read_WireSize(FILE * f)
{
  float w;

  if (fread(&w, sizeof(w), 1, f) != 1)
    return ERROR_READER;
  kprintf(TRUE, "Wire size: %f\n", w);
  return OK;
}

// Soften material
static int Read_Soften(FILE * f)
{
  if (f) {
  };
  kprintf(TRUE, "Soften\n");
  return OK;
}

// Shading type reader
static int Read_Shading(FILE * f)
{
  word w;

  if (fread(&w, sizeof(w), 1, f) != 1)
    return ERROR_READER;
  kprintf(TRUE, "Shading: %d\n", w);
  return OK;
}

// Map file name reader
static int Read_MapFile(FILE * f)
{
  byte file[200];
  int t;

  if (!texture_map) {
    if (ASCIIZReader(f) != OK)
      return (ERROR_READER);
  }
  else {
    // nacti texturu
    if ((t =
        lo_najdi_volnou_texturu(p_cnf->p_text, MAX_EDIT_TEXTUR)) == K_CHYBA)
      chyba("Neni volna textura !");

    if (ASCIIZReader_muj(f, file, 200) != OK)
      return (ERROR_READER);

    kprintf(TRUE, "Textura '%s'", file);

    _strlwr(file);
    strcpy(p_cnf->p_text[t].jmeno, file);

    strcpy(p_cnf->p_mat[akt_matros]->textfile[0], p_cnf->p_text[t].jmeno);
    p_cnf->p_mat[akt_matros]->p_text[0] = p_cnf->p_text + t;
  }
  texture_map = 0;
  return OK;
}


// Map flags reader
static int Read_MapFlags(FILE * f)
{
  word flags;

  if (fread(&flags, sizeof(flags), 1, f) != 1)
    return ERROR_READER;
  kprintf(TRUE, "Map flags: %i", flags);
  return OK;
}

// Map blurring reader
static int Read_MapBlur(FILE * f)
{
  float blur;

  if (fread(&blur, sizeof(blur), 1, f) != 1)
    return ERROR_READER;
  kprintf(TRUE, "Map blurring: %f", blur);
  return OK;
}

// Map U scale reader
static int Read_MapUScale(FILE * f)
{
  float u;

  if (fread(&u, sizeof(u), 1, f) != 1)
    return ERROR_READER;
  kprintf(TRUE, "Map U scale: %f", u);
  p_cnf->p_mat[akt_matros]->tds.u_scale = u;
  p_cnf->p_mat[akt_matros]->flag |= MAT_3DS_FLAGS;
  return OK;
}

// Map V scale reader
static int Read_MapVScale(FILE * f)
{
  float v;

  if (fread(&v, sizeof(v), 1, f) != 1)
    return ERROR_READER;
  kprintf(TRUE, "Map V scale: %f", v);
  p_cnf->p_mat[akt_matros]->tds.v_scale = v;
  p_cnf->p_mat[akt_matros]->flag |= MAT_3DS_FLAGS;
  return OK;
}

// Map U offset reader
static int Read_MapUOffset(FILE * f)
{
  float u;

  if (fread(&u, sizeof(u), 1, f) != 1)
    return ERROR_READER;
  kprintf(TRUE, "Map U offset: %f", u);
  p_cnf->p_mat[akt_matros]->tds.u_offset = u;
  p_cnf->p_mat[akt_matros]->flag |= MAT_3DS_FLAGS;
  return OK;
}

// Map V scale reader
static int Read_MapVOffset(FILE * f)
{
  float v;

  if (fread(&v, sizeof(v), 1, f) != 1)
    return ERROR_READER;
  kprintf(TRUE, "Map V offset: %f", v);
  p_cnf->p_mat[akt_matros]->tds.v_offset = v;
  p_cnf->p_mat[akt_matros]->flag |= MAT_3DS_FLAGS;
  return OK;
}

// Map rotation angle reader
static int Read_MapRotAngle(FILE * f)
{
  float a;

  if (fread(&a, sizeof(a), 1, f) != 1)
    return ERROR_READER;
  kprintf(TRUE, "Map rotation angle: %f", a);
  p_cnf->p_mat[akt_matros]->tds.rot_angle = DEG2RAD(-a);
  p_cnf->p_mat[akt_matros]->flag |= MAT_3DS_FLAGS;
  return (OK);
}


/********** Keyframer reading routines: **********/

// novy track-info
static int Read_TrackInfo(FILE * f)
{
  if (!keyframe.p_track) {      // insert-first
    if (!(keyframe.p_track = malloc(sizeof(TRACK_INFO))))
      chyba("Pamet!");
    memset(keyframe.p_track, 0, sizeof(TRACK_INFO));
    p_track = keyframe.p_track;
  }
  else {                        // insert
    TRACK_INFO *p_tmp;

    p_tmp = keyframe.p_track;

    while (p_tmp->p_next)       // posun v lin. seznamu
      p_tmp = p_tmp->p_next;

    if (!(p_tmp->p_next = malloc(sizeof(TRACK_INFO))))
      chyba("Pamet!");
    memset(p_tmp->p_next, 0, sizeof(TRACK_INFO));

    p_track = p_tmp->p_next;
  }

  track_mod = 0;

  return OK;
}

// Frame interval reader
static int Read_Frames(FILE * f)
{
  dword c[2];

  if (fread(c, sizeof(c), 1, f) != 1)
    return ERROR_READER;

  kprintf(TRUE, "Keyframe informace...");
  kprintf(TRUE, "Start: %ld, End: %ld", c[0], c[1]);

  keyframe.frame_start = c[0];
  keyframe.frame_stop = c[1];

  // Prekopiruj informace o kamerach
  p_cnf->kamakt = 0;
  p_cnf->kamnum = akt_kamera;

  akt_kamera = -1;              //pri cteni tracku se udela +1

  return OK;
}

// Track object name reader
static int Read_TrackObjName(FILE * f)
{
  byte pom[50];
  word w[2];
  word parent;

  if (ASCIIZReader_muj(f, pom, 50) != OK)
    return ERROR_READER;
  if (fread(w, sizeof(w), 1, f) != 1)
    return ERROR_READER;
  if (fread(&parent, sizeof(parent), 1, f) != 1)
    return ERROR_READER;

  kprintf(TRUE, "Track object name %s", pom);
  kprintf(TRUE, "Object name data: Flags 0x%X, 0x%X, Parent %d", w[0], w[1],
    parent);

  switch (track_mod) {
    case 0:
      assert(p_track);
      strcpy(p_track->jmeno, pom);
      p_track->parent = parent;
      p_track->flags[0] = w[0];
      p_track->flags[1] = w[1];
      break;
    case 1:
      strcpy(kamery[akt_kamera].tracky[0].jmeno, pom);
      kamery[akt_kamera].tracky[0].parent = parent;
//      kamery[akt_kamera].tracky[0].flags[0] = w[0];
//      kamery[akt_kamera].tracky[0].flags[1] = w[1];
      break;
    default:
      break;
  }
  return OK;
}

// Dummy object name reader
static int Read_DummyName(FILE * f)
{
  byte jmeno[500];

  if (ASCIIZReader_muj(f, jmeno, 500) != OK)
    return ERROR_READER;
  kprintf(TRUE, "Dummy objekt %s", jmeno);
  assert(p_track);
  dummy_objektu++;
  return OK;
}

// Pivot point position reader
static int Read_PivotPoint(FILE * f)
{
  float pos[3];

  if (fread(pos, sizeof(pos), 1, f) != 1)
    return ERROR_READER;
  oe_wrlog(p_cnf, "Pivot X: %f, Y: %f, Z: %f", pos[0], pos[2], pos[1]);

  assert(p_track);
  p_track->pivot.x = pos[0];
  p_track->pivot.y = pos[2];
  p_track->pivot.z = pos[1];

  return OK;
}

/*
 * Key info flags for position, rotation and scaling:
 * Until I know the meaning of each bit in flags I assume all mean
 * a following float data.
 */

// Spline flags reader
// NOTE THIS IS NOT A CHUNK, but A PART OF SEVERAL CHUNKS
static int SplineFlagsReader(FILE * f, int ind, word * nf,
  KEY_POINT_3DS * p_key)
{
  unsigned int i;
  float dat;
  word unknown;
  word flags;

  if (fread(nf, sizeof(word), 1, f) != 1)
    return ERROR_READER;
  if (fread(&unknown, sizeof(unknown), 1, f) != 1)
    return ERROR_READER;
  if (fread(&flags, sizeof(flags), 1, f) != 1)
    return ERROR_READER;
//  spaces(ind+2);
//  printf("Frame %3d: Flags 0x%X\n", *nf, flags);
  oe_wrlog(p_cnf, "Frame %3d: Flags 0x%X", *nf, flags);
  memset(p_key, 0, sizeof(*p_key));

  p_key->frame = *nf;
  p_key->flags = flags;

  for (i = 0; i < 16; i++) {
    static const char *flagnames[] =
      { "Tension", "Continuity", "Bias", "Ease To", "Ease From", };

    if (flags & (1 << i)) {
      if (fread(&dat, sizeof(dat), 1, f) != 1)
        return ERROR_READER;
      if (i < (sizeof(flagnames) / sizeof(*flagnames))) {
        /*
           i = 0 -> Tension
           i = 1 -> Continuity
           i = 2 -> Bias
           i = 3 -> Erase To
           i = 4 -> Erase From
         */
        switch (i) {
          case 0:
            p_key->tension = dat;
            break;
          case 1:
            p_key->continuity = dat;
            break;
          case 2:
            p_key->bias = dat;
            break;
          case 3:
            p_key->easeto = dat;
            break;
          case 4:
            p_key->easefrom = dat;
            break;
        }
      }
      else {
//        spaces(ind+13);
//        printf("%-15s = %f\n", "Unknown", dat);
      }
    }
  }
  return OK;
}

// Track flags reader
// NOTE THIS IS NOT A CHUNK, but A PART OF SEVERAL CHUNKS
static int TrackFlagsReader(FILE * f, word * n, word * p_flags)
{
  if (fread(p_flags, sizeof(word) * 7, 1, f) != 1)
    return ERROR_READER;
  if ((p_flags[0] & 0x02) == 0x02)
    oe_wrlog(p_cnf, "repeat");
  if ((p_flags[0] & 0x03) == 0x03)
    oe_wrlog(p_cnf, "loop");
  *n = p_flags[5];
  return OK;
}

// Position track reader
static int Read_TrackPos(FILE * f)
{
  KEY_POINT_3DS key;
  KEY_POINT_BRS *p_key;
  BOD *p_pos;
  word i, n, nf;
  word flags[7];
  float v[3];
  int klic;

  if (TrackFlagsReader(f, &n, flags) != OK)
    return (ERROR_READER);

  if (n) {
    if (!(p_key = malloc(sizeof(p_key[0]) * n)))
      chyba("Pamet");
    if (!(p_pos = (BOD *) malloc(sizeof(BOD) * n)))
      chyba("Pamet");

    // rozdeleni podle druhu nacitanych tracku
    switch (track_mod) {
      case 0:                  // Objekt-track
        assert(p_track);
        p_track->p_pkeys = p_key;
        p_track->p_pos = p_pos;
        p_track->pos_keys = n;
        break;

      case 1:                  // Camera track
        kamery[akt_kamera].tracky[0].p_pkeys = p_key;
        kamery[akt_kamera].tracky[0].p_pos = p_pos;
        kamery[akt_kamera].tracky[0].pos_keys = n;
        break;

      case 2:                  // Camera - Target Track      
        kamery[akt_kamera].tracky[0].p_tkeys = p_key;
        kamery[akt_kamera].tracky[0].p_trg = p_pos;
        kamery[akt_kamera].tracky[0].trg_keys = n;
        break;
    }

    // pozicnich klicu
    kprintf(TRUE, "Pozicnich klicu %d", n);
    for (i = 0; i < n; i++) {
      if (SplineFlagsReader(f, ind, &nf, &key) != OK)
        return (ERROR_READER);
      key_3ds_to_ber(&key, p_key + i);
      if (fread(v, sizeof(v), 1, f) != 1)
        return (ERROR_READER);
      p_pos[i].x = v[0];
      p_pos[i].y = v[2];
      p_pos[i].z = v[1];
      oe_wrlog(p_cnf, "Key %d X = %f Y = %f Z = %f", i, v[0], v[2], v[1]);
    }

    if (!check_track(p_key, n, &klic)) {
      kprintf(TRUE, "**** Vicenasobny frame na klici cislo %d frame %d", klic,
        p_key[klic]);
      //assert(0);
    }
  }
  return OK;
}

// Color track reader
static int Read_TrackColor(FILE * f)
{
  word n, nf, flags[7];
  float pos[3];
  KEY_POINT_3DS key;

  if (TrackFlagsReader(f, &n, flags) != OK)
    return ERROR_READER;
  oe_wrlog(p_cnf, "Color keys: %d", n);

  while (n-- > 0) {
    if (SplineFlagsReader(f, ind, &nf, &key) != OK)
      return ERROR_READER;
    if (fread(pos, sizeof(pos), 1, f) != 1)
      return ERROR_READER;
    oe_wrlog(p_cnf, "R: %f, G: %f, B: %f", pos[0], pos[1], pos[2]);
  }
  return OK;
}

// Rotation track reader
static int Read_TrackRot(FILE * f)
{
  KEY_POINT_3DS key;
  word i, n, nf;
  float v[4];
  int klic;
  QUAT q;
  BOD osa;
  float uhel;

  assert(p_track);

  if (TrackFlagsReader(f, &n, p_track->rot_flag) != OK)
    return ERROR_READER;
  kprintf(TRUE, "Rotacnich klicu: %d", n);

  p_track->rot_keys = n;

  if (n) {
    if (!(p_track->p_rkeys = malloc(sizeof(p_track->p_rkeys[0]) * n)))
      chyba("Pamet");
    if (!(p_track->p_rot = (ROTKEY *) malloc(sizeof(ROTKEY) * n)))
      chyba("Pamet");

    for (i = 0; i < n; i++) {
      if (SplineFlagsReader(f, ind, &nf, &key) != OK)
        return ERROR_READER;
      key_3ds_to_ber(&key, p_track->p_rkeys + i);
      if (fread(v, sizeof(v), 1, f) != 1)
        return ERROR_READER;
      p_track->p_rot[i].angle = v[0];
      p_track->p_rot[i].x = v[1];
      p_track->p_rot[i].y = v[3];
      p_track->p_rot[i].z = v[2];
      oe_wrlog(p_cnf, "Angle: %f§, X: %f, Y: %f, Z: %f", v[0] * 180.0 / PI,
        v[1], v[3], v[2]);
      key_rotkey_to_quat(p_track->p_rot + i, &q);
      oe_wrlog(p_cnf, "Prevod - Quat: X: %f, Y: %f, Z: %f W: %f", q.x, q.y,
        q.z, q.w);
      quat_to_angle(&q, &osa, &uhel);
      oe_wrlog(p_cnf, "Prevod - Angle: %f§ (%f), X: %f, Y: %f, Z: %f",
        uhel * 180.0 / PI, uhel, osa.x, osa.y, osa.z);
    }

    if (!check_track(p_track->p_rkeys, n, &klic)) {
      kprintf(TRUE, "**** Vicenasobny frame na klici cislo %d frame %d", klic,
        p_track->p_rkeys[klic]);
    }
  }
  return OK;
}

static int Read_CameraTrack(FILE * f)
{
  kprintf(TRUE, "Camera Track");

  track_mod = 1;
  akt_kamera++;

  //kamery[akt_kamera].tracky[0].framestart = keyframe.frame_start;
  kamery[akt_kamera].tracky[0].endtime =
    calc_endtime(keyframe.frame_stop - keyframe.frame_start);
  return OK;
}

static int Read_CameraTargetTrack(FILE * f)
{
  kprintf(TRUE, "Camera Target Track");
  track_mod = 2;
  // cte uplne stejny track jako camera track ale je to ten target
  return OK;
}

// FOV track reader
static int Read_TrackFOV(FILE * f)
{
  KEY_POINT_3DS key;
  float fov;
  word i, n, nf, flags[7];

  if (TrackFlagsReader(f, &n, flags) != OK)
    return ERROR_READER;

  kprintf(TRUE, "FOV keys: %d", n);
  kamery[akt_kamera].tracky[0].fov_keys = n;
  // alokace

  if (n) {
    if (!(kamery[akt_kamera].tracky[0].p_fkeys =
        malloc(sizeof(kamery[akt_kamera].tracky[0].p_fkeys[0]) * n)))
      chyba("Pamet");
    if (!(kamery[akt_kamera].tracky[0].p_fov =
        malloc(sizeof(kamery[akt_kamera].tracky[0].p_fov[0]) * n)))
      chyba("Pamet");

    for (i = 0; i < n; i++) {
      if (SplineFlagsReader(f, ind, &nf, &key) != OK)
        return ERROR_READER;
      key_3ds_to_ber(&key, kamery[akt_kamera].tracky[0].p_fkeys + i);
      if (fread(&fov, sizeof(kamery[akt_kamera].tracky[0].p_fov[0]), 1,
          f) != 1)
        return ERROR_READER;
      kamery[akt_kamera].tracky[0].p_fov[i] = DEG2RAD(fov);
    }
  }
  return OK;
}

// Roll track reader -> vkladam to do rotacniho klice
static int Read_TrackRoll(FILE * f)
{
  KEY_POINT_3DS key;
  float roll;
  word i, n, nf, flags[7];

  if (TrackFlagsReader(f, &n, flags) != OK)
    return ERROR_READER;

  kprintf(TRUE, "Roll track - Rotation keys: %d", n);
  kamery[akt_kamera].tracky[0].roll_keys = n;

  if (n) {
    if (!(kamery[akt_kamera].tracky[0].p_rlkeys =
        malloc(sizeof(kamery[akt_kamera].tracky[0].p_rlkeys[0]) * n)))
      chyba("Pamet");
    if (!(kamery[akt_kamera].tracky[0].p_roll =
        malloc(sizeof(kamery[akt_kamera].tracky[0].p_roll[0]) * n)))
      chyba("Pamet");

    for (i = 0; i < n; i++) {
      if (SplineFlagsReader(f, ind, &nf, &key) != OK)
        return ERROR_READER;
      key_3ds_to_ber(&key, kamery[akt_kamera].tracky[0].p_rlkeys + i);
      if (fread(&roll, sizeof(kamery[akt_kamera].tracky[0].p_roll[0]), 1,
          f) != 1)
        return ERROR_READER;
      kamery[akt_kamera].tracky[0].p_roll[i] = DEG2RAD(roll);
    }
  }
  return OK;
}

// Scale track reader
static int Read_TrackScale(FILE * f)
{
  KEY_POINT_3DS key;
  word i, n, nf;
  int klic;
  float v[3];

  assert(p_track);

  if (TrackFlagsReader(f, &n, p_track->scs_flag) != OK)
    return ERROR_READER;
  kprintf(TRUE, "Scale keys: %d", n);

  p_track->scs_keys = n;

  if (n) {
    if (!(p_track->p_skeys = malloc(sizeof(p_track->p_skeys[0]) * n)))
      chyba("Pamet");
    if (!(p_track->p_scale = (BOD *) malloc(sizeof(BOD) * n)))
      chyba("Pamet");

    for (i = 0; i < n; i++) {
      if (SplineFlagsReader(f, ind, &nf, &key) != OK)
        return ERROR_READER;
      key_3ds_to_ber(&key, p_track->p_skeys + i);
      if (fread(v, sizeof(v), 1, f) != 1)
        return ERROR_READER;

      p_track->p_scale[i].x = v[0];
      p_track->p_scale[i].y = v[2];
      p_track->p_scale[i].z = v[1];
      //p_track->p_scale[i].y = v[1];
      //p_track->p_scale[i].z = v[2];
      oe_wrlog(p_cnf, "X: %f, Y: %f, Z: %f", v[0], v[2], v[1]);
    }

    if (!check_track(p_track->p_skeys, n, &klic)) {
      kprintf(TRUE, "**** Vicenasobny frame na klici cislo %d frame %d", klic,
        p_track->p_skeys[klic]);
      //assert(0);
    }
  }
  return OK;
}

//Morph track reader
static int Read_TrackMorph(FILE * f)
{
  KEY_POINT_3DS key;
  word flag[7];
  word n, nf;
  byte jmeno[500];

  if (TrackFlagsReader(f, &n, flag) != OK)
    return ERROR_READER;
  kprintf(TRUE, "Morph keys: %d", n);

  while (n-- > 0) {
    if (SplineFlagsReader(f, ind, &nf, &key) != OK)
      return ERROR_READER;
    if (ASCIIZReader_muj(f, jmeno, 500) != OK)
      return ERROR_READER;
    oe_wrlog(p_cnf, "%s", jmeno);
  }
  return OK;
}

// Hide track reader
static int Read_TrackHide(FILE * f)
{
  word unknown[2];
  word n, nf;
  int hide = 0;
  int i;
  word flag[7];

  if (TrackFlagsReader(f, &n, flag) != OK)
    return ERROR_READER;
  kprintf(TRUE, "Hide keys: %d", n);

  while (n-- > 0) {
    if (fread(&nf, sizeof(nf), 1, f) != 1)
      return ERROR_READER;
    if (fread(unknown, sizeof(unknown), 1, f) != 1)
      return ERROR_READER;
    i = (hide ^= 1);
    oe_wrlog(p_cnf, "Hide: %i", i);
  }
  return OK;
}

// Object number reader
static int Read_ObjNumber(FILE * f)
{
  word n;

  if (fread(&n, sizeof(n), 1, f) != 1)
    return ERROR_READER;
  kprintf(TRUE, "\nObject number: %d", n);

  switch (track_mod) {
    case 0:
      assert(p_track);
      p_track->objekt = n;
      break;
    case 1:
      kamery[akt_kamera].tracky[0].objekt = n;
      break;
    default:
      break;
  }

  return OK;
}

/********** End of keyframer reading **********/
static int Read_Ambient(FILE * f)
{
  druh_barvy = COLOR_AMBIENT;
  return (OK);
}

static int Read_Diffuse(FILE * f)
{
  druh_barvy = COLOR_DIFFUSE;
  return (OK);
}

static int Read_TextureMap(FILE * f)
{
  texture_map = 1;
  return (OK);
}

struct
{
  word id;
  const char *name;
  int sub;
  int (*func) (FILE * f);
} ChunkNames[] = {
/* Chunk ID,       Chunk Description,   1=Sub-chunks,  Chunk Reader */
  {
  CHUNK_VER, "Version", 0, Read_Ver},   // added
  {
  CHUNK_RGBF, "RGB float", 0, Read_RGBF}, {
  CHUNK_RGBB, "RGB byte", 0, Read_RGBB}, {
  CHUNK_RGBB2, "RGB byte 2", 0, Read_RGBB}, {
  CHUNK_AMOUNTOF, "Amount", 0, Read_AmountOf},  // added
  {
  CHUNK_MASTERSCALE, "Master scale", 0, Read_MasterScale},      // added
  {
  CHUNK_PRJ, "Project", 1, Read_NULL}, {
  CHUNK_MLI, "Material Library", 1, Read_NULL}, {
  CHUNK_MESHVER, "Mesh version", 0, Read_MeshVer},      // added
  {
  CHUNK_MAIN, "Main", 1, Read_NULL}, {
  CHUNK_OBJMESH, "Object Mesh", 1, Read_NULL}, {
  CHUNK_BKGCOLOR, "Background color", 1, Read_NULL}, {
  CHUNK_AMBCOLOR, "Ambient color", 1, Read_NULL}, {
  CHUNK_OBJBLOCK, "Object Block", 1, Read_ObjBlock}, {
  CHUNK_TRIMESH, "Tri-Mesh", 1, Read_TriMesh}, {
  CHUNK_VERTLIST, "Vertex list", 0, Read_VertList}, {
  CHUNK_VERTFLAGS, "Vertex flags", 0, Read_VertFlags}, {
  CHUNK_FACELIST, "Face list", 1, Read_FaceList}, {
  CHUNK_FACEMAT, "Face material", 0, Read_FaceMat}, {
  CHUNK_MAPLIST, "Mappings list", 0, Read_MapList}, {
  CHUNK_SMOOLIST, "Smoothings", 0, Read_SmooList}, {
  CHUNK_TRMATRIX, "Matrix", 0, Read_TrMatrix}, {
  CHUNK_MESHCOLOR, "Mesh Color", 0, Read_Skip}, // added
  {
  CHUNK_TXTINFO, "TxtInfo", 0, Read_Skip},      // added
  {
  CHUNK_BOXMAP, "BoxMap", 0, Read_BoxMap},      // added
  {
  CHUNK_LIGHT, "Light", 1, Read_Light}, // !!
  {
  CHUNK_SPOTLIGHT, "Spotlight", 0, Read_SpotLight},     // !!
  {
  CHUNK_CAMERA, "Camera", 0, Read_Camera},      // !!
  {
  CHUNK_HIERARCHY, "Hierarchy", 1, Read_NULL}, {
  CHUNK_VIEWPORT, "Viewport info", 0, Read_Skip}, {
  CHUNK_MATERIAL, "Material", 1, Read_NULL}, {
  CHUNK_MATNAME, "Material name", 0, Read_MatName}, {
  CHUNK_AMBIENT, "Ambient color", 1, Read_Ambient}, {
  CHUNK_DIFFUSE, "Diffuse color", 1, Read_Diffuse}, {
  CHUNK_SPECULAR, "Specular color", 1, Read_NULL}, {
  CHUNK_SHININESS, "Shiness", 1, Read_NULL},    // added
  {
  CHUNK_SHINSTRENGTH, "Shiness strength", 1, Read_NULL},        // added
  {
  CHUNK_TRANSPARENCY, "Transparency", 1, Read_Transparent},     // added
  {
  CHUNK_TRANSFALLOFF, "Falloff", 1, Read_NULL}, // added
  {
  CHUNK_REFBLUR, "Reflection blur", 1, Read_NULL},      // added
  {
  CHUNK_SELFILLUM, "Self illumination", 1, Read_NULL},  // added
  {
  CHUNK_TWOSIDED, "Twosided", 0, Read_TwoSided},        // added
  {
  CHUNK_TRANSADD, "TransAdd", 0, Read_TransAdd},        // added
  {
  CHUNK_WIREON, "Wire On", 0, Read_WireOn},     // added
  {
  CHUNK_WIRESIZE, "Wire Size", 0, Read_WireSize},       // added
  {
  CHUNK_SOFTEN, "Soften", 0, Read_Soften},      // added
  {
  CHUNK_SHADING, "Shading", 0, Read_Shading},   // added
  {
  CHUNK_MATXPFALLIN, "XPFALLIN", 0, Read_NULL}, {
  CHUNK_TEXTURE, "Texture map", 1, Read_TextureMap}, {
  CHUNK_REFLECT, "Reflection map", 1, Read_NULL},       // added
  {
  CHUNK_BUMPMAP, "Bump map", 1, Read_NULL}, {
  CHUNK_BUMPAMOUNT, "Bump amount", 0, Read_AmountOf},   // added
  {
  CHUNK_MAPFILE, "Map filename", 0, Read_MapFile}, {
  CHUNK_MAPFLAGS, "Map flags", 0, Read_MapFlags},       // added
  {
  CHUNK_MAPBLUR, "Map blurring", 0, Read_MapBlur},      // added
  {
  CHUNK_MAPUSCALE, "Map U scale", 0, Read_MapUScale},   // added
  {
  CHUNK_MAPVSCALE, "Map V scale", 0, Read_MapVScale},   // added
  {
  CHUNK_MAPUOFFSET, "Map U offset", 0, Read_MapUOffset},        // added
  {
  CHUNK_MAPVOFFSET, "Map V offset", 0, Read_MapVOffset},        // added
  {
  CHUNK_MAPROTANGLE, "Map rot angle", 0, Read_MapRotAngle},     // added
  {
  CHUNK_KEYFRAMER, "Keyframer data", 1, Read_NULL}, {
  CHUNK_AMBIENTKEY, "Ambient key", 1, Read_NULL}, {
  CHUNK_TRACKINFO, "Track info", 1, Read_TrackInfo}, {
  CHUNK_FRAMES, "Frames", 0, Read_Frames}, {
  CHUNK_TRACKOBJNAME, "Track obj. name", 0, Read_TrackObjName}, {
  CHUNK_DUMMYNAME, "Dummy name", 0, Read_DummyName},    // added
  {
  CHUNK_TRACKPIVOT, "Pivot point", 0, Read_PivotPoint}, {
  CHUNK_TRACKPOS, "Position keys", 0, Read_TrackPos}, {
  CHUNK_TRACKCOLOR, "Color track", 0, Read_TrackColor}, // added
  {
  CHUNK_TRACKROTATE, "Rotation keys", 0, Read_TrackRot}, {
  CHUNK_TRACKSCALE, "Scale keys", 0, Read_TrackScale}, {
  CHUNK_TRACKMORPH, "Morph track", 0, Read_TrackMorph}, // added
  {
  CHUNK_TRACKHIDE, "Hide track", 0, Read_TrackHide},    // added
  {
  CHUNK_OBJNUMBER, "Object number", 0, Read_ObjNumber}, {
  CHUNK_TRACKCAMERA, "Camera track", 1, Read_CameraTrack}, {
  CHUNK_TRACKCAMTGT, "Camera target track", 1, Read_CameraTargetTrack}, {
  CHUNK_TRACKLIGHT, "Pointlight track", 1, Read_NULL}, {
  CHUNK_TRACKLIGTGT, "Pointlight target track", 1, Read_NULL}, {
  CHUNK_TRACKSPOTL, "Spotlight track", 1, Read_NULL}, {
  CHUNK_TRACKFOV, "FOV track", 0, Read_TrackFOV},       // added
  {
  CHUNK_TRACKROLL, "Roll track", 0, Read_TrackRoll},    // added
};

int FindChunk(word id)
{
  unsigned int i;

  for (i = 0; i < (sizeof(ChunkNames) / sizeof(ChunkNames[0])); i++) {
    if (id == ChunkNames[i].id)
      return i;
  }
  return -1;
}

// ------------------------------------

int Verbose = 0;
int Quiet = 0;

int ChunkReader(FILE * f, long p)
{
  TChunkHeader h;
  int n;
  long pc;
  int res;

  while (ftell(f) < p) {
    pc = ftell(f);
    if (fread(&h, sizeof(h), 1, f) != 1)
      return ERROR_READER;
    if (h.len == 0)
      return ERROR_READER;
    n = FindChunk(h.id);
    if (n < 0) {
      fseek(f, pc + h.len, SEEK_SET);
    }
    else {
      pc = pc + h.len;

      if (ChunkNames[n].func != NULL) {
        ind += 2;
        res = ChunkNames[n].func(f);
        ind -= 2;
        if (res != OK)
          return (res);
      }

      if (ChunkNames[n].sub) {
        ind += 2;
        res = ChunkReader(f, pc);
        ind -= 2;
        if (res != OK)
          return (res);
      }

      fseek(f, pc, SEEK_SET);
    }
    if (ferror(f))
      return ERROR_READER;
  }
  return OK;
}

// ------------------------------------
int ds_read(FILE * f)
{
  long p;

//  int res;

  // Find file size.
  fseek(f, 0, SEEK_END);
  p = ftell(f);
  fseek(f, 0, SEEK_SET);

// Go!
  ind = 0;
  return (ChunkReader(f, p));
}

EDIT_KONTEJNER *ds_import_kontejner(byte * p_file, int mat, byte key,
  byte text, byte staticky, byte kamery_import)
{
  HWND hwnd;
  FILE *f;
  int i;

  if ((f = fopen(p_file, "rb")) == NULL)
    return (NULL);

  p_kont = vyrob_kontejner();
  akt_cislo_objektu = -1;
  hd_okno = 0;
  akt_objekt = 0;
  akt_matros = 0;
  akt_kamera = 0;
  akt_light = 0;
  druh_barvy = 0;
  texture_map = 0;
  dummy_objektu = 0;
  p_obj = NULL;
  memset(&keyframe, 0, sizeof(keyframe));

  hwnd =
    CreateDialog(hinst, MAKEINTRESOURCE(IDD_LOADING), hwnd_aplikace,
    DefWindowProc);

  SetDlgItemText(hwnd, IDC_STATIC1, "3DS file format reader v1.4 - LP & JA");

  /* 
     Import 3ds souboru do kontejneru
   */
  if (ds_read(f) == ERROR_READER) {
    ddw("Chyba cteni 3ds souboru (offset 0x%x - %d)!", i, i = ftell(f));
  }

  /* 
     Aplikuje mapovaci flagy
   */
  ds_aplikuj_3ds_text_mapping(p_cnf->p_mat, p_kont);

  /*
     Pokud jsou povoleny animace -> prilep keyframy
   */
  if (key) {
    HIERARCHY_ROOT root;

    memset(&root, 0, sizeof(root));

    p_kont->akt_animace = 0;

    // prikopiruju tracky k jednotlivym objektum
    ds_vloz_keyframy(p_kont, &keyframe);

    // Prevede do animacniho stromu
    if (key_root_vyrob_indir(p_kont, &root)) {  // pokud jsou key-framy
      key_root_to_zero(&root);
      key_kontejner_reanimuj(p_kont, &root);
      strcpy(root.jmeno, p_file);
    }

    // prevod do sim-animaci
    if (!staticky) {
      key_root_to_sim_indir(p_kont, p_kont->sim, &root, FALSE);
    }

    // Prevod do sim-animaci + smazani key-frame
    if (root.framenum)
      key_root_zrus(&root);

  }                             // jinak to je staticky kontejner
  else {
    p_kont->kflag |= KONT_STATIC;
  }

  // pokud je to pozadovano smaz dummy objekty
  if (p_cnf->cnf.mazat_dummy) {
    for (i = 0; i < MAX_KONT_OBJEKTU; i++) {
      if (p_kont->p_obj[i] && !strcmp(p_kont->p_obj[i]->jmeno, DUMMY_OBJEKT)) {
        kprintf(1, "Mazu dummy objekt %d...", i);
        zrus_objekt(p_kont->p_obj + i);
      }
    }
  }

  /*
     Pokud je pozadavek na staticky kontejner -> vymaz keyframy
   */
  if (staticky) {
    ds_kontejner_preved_na_staticky(p_kont);
  }

  /* Kopie kamer, pokud jsou ve scene
   */
  if (kamery_import && p_cnf->kamnum) {
    memcpy(p_cnf->kamery, kamery, sizeof(kamery[0]) * p_cnf->kamnum);
  }

  /*
     Reload textur 3ds souboru
   */
  strcpy(p_cnf->tdir.scene_texture_dir, work_dir);
  if (text) {
    oe_reload_textur(p_cnf);
  }

  /*
     Konecna faze
   */
  updatuj_kontejner_statistika(p_kont, TRUE);
  nastav_barvu_bodu_kont_dword(p_kont, 0xffffffff, COLOR_SET);
  fclose(f);

  EndDialog(hwnd, TRUE);
  return (p_kont);
}

// upravi texture koordinaty podle flagu u mappingu
void ds_aplikuj_3ds_text_mapping(EDIT_MATERIAL ** p_mat,
  EDIT_KONTEJNER * p_kont)
{
  TDS_MAT_FLAGS tds;
  int o, v;
  float tu, tv, uhel;

  for (o = 0; o < p_kont->max_objektu; o++) {
    if (p_kont->p_obj[o] && p_mat[p_kont->p_obj[o]->material]
      && p_mat[p_kont->p_obj[o]->material]->flag & MAT_3DS_FLAGS) {
      tds = p_mat[p_kont->p_obj[o]->material]->tds;
      for (v = 0; v < p_kont->p_obj[o]->vertexnum; v++) {
        if (tds.rot_angle != 0.0f) {
          uhel = tds.rot_angle; // uhel -> radiany
          tu = p_kont->p_obj[o]->p_vertex[v].tu1;
          tv = p_kont->p_obj[o]->p_vertex[v].tv1;
          p_kont->p_obj[o]->p_vertex[v].tu1 =
            (float) (tu * cos(uhel) - tv * sin(uhel));
          p_kont->p_obj[o]->p_vertex[v].tv1 =
            (float) (tu * sin(uhel) + tv * cos(uhel));
        }
        if (tds.u_scale != 0.0f) {
          tu = p_kont->p_obj[o]->p_vertex[v].tu1 - 0.5f;
          tu *= tds.u_scale;
          p_kont->p_obj[o]->p_vertex[v].tu1 = 0.5f + tu;
        }
        if (tds.v_scale != 0.0f) {
          tv = p_kont->p_obj[o]->p_vertex[v].tv1 - 0.5f;
          tv *= tds.v_scale;
          p_kont->p_obj[o]->p_vertex[v].tv1 = 0.5f + tv;
        }
        p_kont->p_obj[o]->p_vertex[v].tu1 += tds.u_offset;
        p_kont->p_obj[o]->p_vertex[v].tv1 += tds.v_offset;
      }
    }
  }
}

/*
  Presklada kontejner tak, aby cisla objektu odpovidali keyframu
*/
void ds_vloz_keyframy(EDIT_KONTEJNER * p_kont, KEYFRAMER_DATA * p_key)
{
  TRACK_INFO *p_track = p_key->p_track;
  QUAT q = { 0, 0, 0, 1 };
  EDIT_KONTEJNER k;
  int o, num = (p_key->frame_stop + 1) - p_key->frame_start,
    start = p_key->frame_start;

  if (p_track) {
    p_kont->kflag |= KONT_KEYFRAME;
    k = *p_kont;
    memset(p_kont->p_obj, 0, sizeof(EDIT_OBJEKT *) * p_kont->max_objektu);

    while (p_track) {
      p_track->framenum = num;
      p_track->framestart = start;
      key_tri_calc_absolutne(p_track, &q);

      if (!strcmp(p_track->jmeno, DUMMY_OBJEKT)) {

        ddw
          ("Importovany soubor obsahuje DUMMY objekt (cislo %d) !!\n Objekty se muzou spatne importovat a animovat !!",
          o);
        kprintf(1, "Dummy objekt !!! Objekt cislo %d", o);

        o = p_track->objekt;
        p_kont->p_obj[o] = vyrob_objekt();
        strcpy(p_kont->p_obj[o]->jmeno, DUMMY_OBJEKT);
        p_kont->p_obj[o]->kreslit = FALSE;
        p_kont->p_obj[o]->p_track = p_track;
        key_tri_calc_absolutne(p_track, &q);
      }
      else {
        // najiti pripadneho objektu
        if ((o = lo_najdi_objekt_kont(&k, p_track->jmeno)) != K_CHYBA) {
          if (p_track->objekt == o) {
            kprintf(TRUE, "Umistuji objekt '%s' na %d ", p_track->jmeno, o);
            p_kont->p_obj[o] = k.p_obj[o];
            p_kont->p_obj[o]->p_track = p_track;
          }
          else {
            // je to kopie
            kprintf(TRUE, "Kopiruji objekt '%s' z %d na %d", p_track->jmeno,
              o, p_track->objekt);
            p_kont->p_obj[p_track->objekt] = kopiruj_objekt(k.p_obj[o]);
            p_kont->p_obj[p_track->objekt]->p_track = p_track;
          }
        }
        else {
          kprintf(TRUE, "Neznamy track objektu %s...", p_track->jmeno);
        }
      }
      p_track = p_track->p_next;
    }
  }
}

void ds_kontejner_preved_na_staticky(EDIT_KONTEJNER * p_kont)
{
  int i;

  for (i = 0; i < p_kont->max_objektu; i++) {
    if (p_kont->p_obj[i]) {
      transformuj_objekt_matici(p_kont->p_obj[i], &p_kont->p_obj[i]->m);
      init_matrix(&p_kont->p_obj[i]->m);
    }
  }

  p_kont->kflag |= KONT_STATIC;
  p_kont->kflag &= ~KONT_KEYFRAME;
}

__inline static EDIT_KONTEJNER *ds_objekt_to_kontejner(EDIT_KONTEJNER *
  p_kont, int o)
{
  EDIT_KONTEJNER *p_new = vyrob_kontejner();

  p_new->kflag = p_kont->kflag; // skopiruj flagy bez material flagu
  p_new->k2flag = p_kont->k2flag;       // skopiruj flagy bez material flagu
  p_new->m1flag = p_kont->p_obj[o]->m1flag;
  p_new->m2flag = p_kont->p_obj[o]->m2flag;
  if (strlen(p_kont->jmeno) + strlen(p_kont->p_obj[o]->jmeno) + 1 < MAX_JMENO) {
    sprintf(p_new->jmeno, "%s_%s", p_kont->jmeno, p_kont->p_obj[o]->jmeno);
  }
  else {
    strcpy(p_new->jmeno, p_kont->p_obj[o]->jmeno);
  }
  assert(strlen(p_new->jmeno) < MAX_JMENO);
  p_new->p_obj[0] = p_kont->p_obj[o];
  p_new->world = p_kont->world;
  if (p_kont->p_mlha) {
    p_new->p_mlha = kopiruj_mlhokostku(p_kont->p_mlha, p_new->jmeno);
    p_new->p_mlha->p_kont = p_new;
  }
  p_kont->p_obj[o] = NULL;
  updatuj_kontejner_statistika(p_new, TRUE);
  return (p_new);
}

// rozlozi kontejner -> kazdy objekt bude samostatny kontejner
int ds_kontejner_rozloz(K_EDITOR * p_cnf, EDIT_KONTEJNER * p_kont, int m1flag)
{
  int i;
  int o;

  if (p_kont->kflag & KONT_STATIC && p_kont->objektu > 1) {
    for (i = 0; i < p_kont->max_objektu; i++) {
      if (p_kont->p_obj[i] && (m1flag == ROZLOZ_MAX
          || p_kont->p_obj[i]->m1flag & m1flag)) {
        o = lo_najdi_volny_kontejner(p_cnf->p_kont, p_cnf->max_kont);
        if (o == K_CHYBA) {
          ddw("Neni volny kontejner !");
          assert(0);
        }
        else {
          p_cnf->p_kont[o] = ds_objekt_to_kontejner(p_kont, i);
          updatuj_kontejner_flag(p_cnf->p_kont[o], p_cnf->p_mat);
        }
      }
    }
    return (TRUE);
  }
  else
    return (FALSE);
}


/*******************************************************************
  Export 3DS souboru
 *******************************************************************
*/
/*
  Pridej Mesh
*/
#define GRANULARITA 1000

typedef struct _PRIDAVAC
{

  byte **p_pol;                 // polozky pridavace
  int *p_vel;                   // pole polozek
  int akt;                      // poktualni posledni
  int max;                      // max pocet

} PRIDAVAC;

void prid_init(PRIDAVAC * p_prd)
{
  memset(p_prd, 0, sizeof(*p_prd));
  p_prd->max = GRANULARITA;
  p_prd->akt = 0;

  p_prd->p_pol = malloc(sizeof(p_prd->p_pol[0]) * p_prd->max);
  mtest(p_prd->p_pol);
  memset(p_prd->p_pol, 0, sizeof(p_prd->p_pol[0]) * p_prd->max);

  p_prd->p_vel = malloc(sizeof(p_prd->p_vel[0]) * p_prd->max);
  mtest(p_prd->p_vel);
  memset(p_prd->p_vel, 0, sizeof(p_prd->p_vel[0]) * p_prd->max);
}

void prid_zrus(PRIDAVAC * p_prd)
{
  int i;

  for (i = 0; i < p_prd->akt; i++) {
    if (p_prd->p_pol[i])
      free(p_prd->p_pol[i]);
  }
  free(p_prd->p_pol);
  free(p_prd->p_vel);
}

void prid_add_granularita(PRIDAVAC * p_prd)
{
  int max = p_prd->max + GRANULARITA;
  byte **p_pol = malloc(sizeof(p_pol[0]) * max);
  int *p_vel = malloc(sizeof(p_vel[0]) * max);

  mtest(p_pol);
  mtest(p_vel);

  memset(p_pol, 0, sizeof(p_pol[0]) * max);
  memset(p_vel, 0, sizeof(p_vel[0]) * max);

  memcpy(p_pol, p_prd->p_pol, sizeof(p_pol[0]) * p_prd->max);
  memcpy(p_vel, p_prd->p_vel, sizeof(p_vel[0]) * p_prd->max);

  free(p_prd->p_pol);
  free(p_prd->p_vel);

  p_prd->p_pol = p_pol;
  p_prd->p_vel = p_vel;
  p_prd->max = max;
}

void prid_add(PRIDAVAC * p_prd, int vel, byte * p_pol)
{
  int akt = p_prd->akt;

  if (akt >= p_prd->max) {
    prid_add_granularita(p_prd);
  }

  assert(akt < p_prd->max);

  p_prd->p_pol[akt] = p_pol;
  p_prd->p_vel[akt] = vel;
  p_prd->akt++;
}

void *prid_add_create(PRIDAVAC * p_prd, int vel)
{
  int akt = p_prd->akt;

  if (akt >= p_prd->max) {
    prid_add_granularita(p_prd);
  }

  assert(akt < p_prd->max);

  p_prd->p_pol[akt] = malloc(vel);
  mtest(p_prd->p_pol[akt]);
  p_prd->p_vel[akt] = vel;
  p_prd->akt++;

  return (p_prd->p_pol[akt]);
}

void prid_add_copy(PRIDAVAC * p_prd, int vel, void *p_pol)
{
  int akt = p_prd->akt;

  if (akt >= p_prd->max) {
    prid_add_granularita(p_prd);
  }

  assert(akt < p_prd->max);

  p_prd->p_pol[akt] = malloc(vel);
  mtest(p_prd->p_pol[akt]);
  memcpy(p_prd->p_pol[akt], p_pol, vel);

  p_prd->p_vel[akt] = vel;
  p_prd->akt++;
}

byte *prid_get_rec(PRIDAVAC * p_prd, int rec)
{
  return (rec < p_prd->akt ? p_prd->p_pol[rec] : NULL);
}

int prid_get_size(PRIDAVAC * p_prd)
{
  int vel;
  int i;

  for (i = 0, vel = 0; i < p_prd->akt; i++) {
    if (p_prd->p_pol[i]) {
      vel += p_prd->p_vel[i];
    }
  }
  return (vel);
}

// Nacte velikos a zapise ji do hlavicky v prvnim zaznamu
int prid_get_size_head(PRIDAVAC * p_prd)
{
  TChunkHeader *p_head = (TChunkHeader *) p_prd->p_pol[0];

  p_head->len = prid_get_size(p_prd);
  return (p_head->len);
}

byte *prid_slep(PRIDAVAC * p_prd, int *p_vel)
{
  byte *p_vys;
  byte *p_tmp;
  int vel = prid_get_size(p_prd);
  int i;

  p_tmp = p_vys = malloc(sizeof(byte) * vel);
  mtest(p_vys);

  for (i = 0; i < p_prd->akt; i++) {
    if (p_prd->p_pol[i]) {
      memcpy(p_tmp, p_prd->p_pol[i], sizeof(byte) * p_prd->p_vel[i]);
      p_tmp += p_prd->p_vel[i];
    }
  }

  *p_vel = vel;
  return (p_vys);
}

byte *prid_slep_a_zrus(PRIDAVAC * p_prd, int *p_vel)
{
  byte *p_vys = prid_slep(p_prd, p_vel);

  prid_zrus(p_prd);
  return (p_vys);
}

/* Exporty meshe
*/
void *ds_vyrob_chunk_trimesh(EDIT_OBJEKT * p_obj, int *p_velikost)
{
  TChunkHeader head;
  BOD *p_bod;
  BODUV *p_map;
  FACE *p_face;
  word *p_word;
  int *p_smooth;
  int i, vel;
  word facu;
  PRIDAVAC prd;
  PRIDAVAC prd_flist;

  prid_init(&prd);
  head.id = CHUNK_TRIMESH;
  prid_add_copy(&prd, sizeof(head), &head);

  head.id = CHUNK_VERTLIST;
  head.len =
    p_obj->vertexnum * 3 * sizeof(float) + sizeof(word) +
    sizeof(TChunkHeader);
  prid_add_copy(&prd, sizeof(head), &head);

  p_word =
    prid_add_create(&prd,
    p_obj->vertexnum * 3 * sizeof(float) + sizeof(word));
  *p_word = p_obj->vertexnum;
  p_bod = (BOD *) (p_word + 1);
  for (i = 0; i < p_obj->vertexnum; i++) {
    p_bod->x = p_obj->p_vertex[i].x;
    p_bod->y = p_obj->p_vertex[i].z;
    p_bod->z = p_obj->p_vertex[i].y;
    p_bod++;
  }

  head.id = CHUNK_MAPLIST;
  head.len =
    p_obj->vertexnum * 2 * sizeof(float) + sizeof(word) +
    sizeof(TChunkHeader);
  prid_add_copy(&prd, sizeof(head), &head);

  p_word =
    prid_add_create(&prd,
    p_obj->vertexnum * 2 * sizeof(float) + sizeof(word));
  *p_word = p_obj->vertexnum;
  p_map = (BODUV *) (p_word + 1);
  for (i = 0; i < p_obj->vertexnum; i++) {
    p_map->u = p_obj->p_vertex[i].tu1;
    p_map->v = p_obj->p_vertex[i].tv1;
    p_map++;
  }

  facu = p_obj->facenum / 3;
  prid_init(&prd_flist);
  head.id = CHUNK_FACELIST;
  prid_add_copy(&prd_flist, sizeof(head), &head);
  prid_add_copy(&prd_flist, sizeof(facu), &facu);

  p_face = prid_add_create(&prd_flist, facu * 4 * sizeof(word));
  for (i = 0; i < p_obj->facenum; i += 3) {
    p_face[0] = p_obj->p_face[i];
    p_face[1] = p_obj->p_face[i + 1];
    p_face[2] = p_obj->p_face[i + 2];
    p_face[3] = 7;
    p_face += 4;
  }

  i = strlen(p_cnf->p_mat[p_obj->material]->jmeno) + 1;
  head.id = CHUNK_FACEMAT;
  head.len = i + facu * sizeof(word) + sizeof(word) + sizeof(TChunkHeader);
  prid_add_copy(&prd_flist, sizeof(head), &head);
  prid_add_copy(&prd_flist, i, p_cnf->p_mat[p_obj->material]->jmeno);

  p_word = prid_add_create(&prd_flist, facu * sizeof(word) + sizeof(word));
  *p_word = facu;
  p_face = (FACE *) (p_word + 1);
  for (i = 0; i < facu; i++) {
    *p_face++ = i;
  }

  head.id = CHUNK_SMOOLIST;
  head.len = facu * sizeof(dword) + sizeof(TChunkHeader);
  prid_add_copy(&prd_flist, sizeof(head), &head);

  p_smooth = prid_add_create(&prd_flist, facu * sizeof(dword));
  for (i = 0; i < facu; i++) {
    *p_smooth = 1;
    p_smooth++;
  }

  prid_get_size_head(&prd_flist);
  prid_add(&prd, vel, prid_slep_a_zrus(&prd_flist, &vel));

  prid_get_size_head(&prd);
  return (prid_slep_a_zrus(&prd, p_velikost));
}

void *ds_vyrob_chunk_objblock(EDIT_OBJEKT * p_obj, int *p_velikost)
{
  TChunkHeader head;
  int vel;
  PRIDAVAC prd;

  prid_init(&prd);
  head.id = CHUNK_OBJBLOCK;
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, strlen(p_obj->jmeno) + 1, p_obj->jmeno);
  prid_add(&prd, vel, ds_vyrob_chunk_trimesh(p_obj, &vel));
  prid_get_size_head(&prd);
  return (prid_slep_a_zrus(&prd, p_velikost));
}

void *ds_vyrob_chunk_material_name(EDIT_MATERIAL * p_mat, int *p_velikost)
{
  byte *p_tmp;
  TChunkHeader head;
  PRIDAVAC prd;

  head.id = CHUNK_MATNAME;
  prid_init(&prd);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, strlen(p_mat->jmeno) + 1, p_mat->jmeno);
  prid_get_size_head(&prd);
  p_tmp = prid_slep(&prd, p_velikost);
  prid_zrus(&prd);
  return (p_tmp);
}

void *ds_vyrob_chunk_material_ambient(EDIT_MATERIAL * p_mat, int *p_velikost)
{
  byte *p_tmp, c[3] = { 150, 150, 150 };
  TChunkHeader head;
  PRIDAVAC prd;

  head.id = CHUNK_AMBIENT;
  prid_init(&prd);
  prid_add_copy(&prd, sizeof(head), &head);
  head.id = CHUNK_RGBB;
  head.len = sizeof(head) + sizeof(byte) * 3;
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, sizeof(c), c);
  prid_get_size_head(&prd);
  p_tmp = prid_slep(&prd, p_velikost);
  prid_zrus(&prd);
  return (p_tmp);
}

void *ds_vyrob_chunk_material_diffuse(EDIT_MATERIAL * p_mat, int *p_velikost)
{
  byte *p_tmp, c[3] = { 150, 150, 150 };
  TChunkHeader head;
  PRIDAVAC prd;

  head.id = CHUNK_DIFFUSE;
  prid_init(&prd);
  prid_add_copy(&prd, sizeof(head), &head);
  head.id = CHUNK_RGBB;
  head.len = sizeof(head) + sizeof(byte) * 3;
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, sizeof(c), c);
  prid_get_size_head(&prd);
  p_tmp = prid_slep(&prd, p_velikost);
  prid_zrus(&prd);
  return (p_tmp);
}

void *ds_vyrob_chunk_material_specular(EDIT_MATERIAL * p_mat, int *p_velikost)
{
  byte *p_tmp, c[3] = { 229, 229, 229 };
  TChunkHeader head;
  PRIDAVAC prd;

  head.id = CHUNK_SPECULAR;
  prid_init(&prd);
  prid_add_copy(&prd, sizeof(head), &head);
  head.id = CHUNK_RGBB;
  head.len = sizeof(head) + sizeof(byte) * 3;
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, sizeof(c), c);
  prid_get_size_head(&prd);
  p_tmp = prid_slep(&prd, p_velikost);
  prid_zrus(&prd);
  return (p_tmp);
}

void *ds_vyrob_chunk_material_shiness(EDIT_MATERIAL * p_mat, int *p_velikost)
{
  byte *p_tmp;
  word c = 10;
  TChunkHeader head;
  PRIDAVAC prd;

  head.id = CHUNK_SHININESS;
  prid_init(&prd);
  prid_add_copy(&prd, sizeof(head), &head);
  head.id = CHUNK_AMOUNTOF;
  head.len = sizeof(head) + sizeof(c);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, sizeof(c), &c);
  prid_get_size_head(&prd);
  p_tmp = prid_slep(&prd, p_velikost);
  prid_zrus(&prd);
  return (p_tmp);
}

void *ds_vyrob_chunk_material_shiness_strenght(EDIT_MATERIAL * p_mat,
  int *p_velikost)
{
  byte *p_tmp;
  word c = 0;
  TChunkHeader head;
  PRIDAVAC prd;

  head.id = CHUNK_SHINSTRENGTH;
  prid_init(&prd);
  prid_add_copy(&prd, sizeof(head), &head);
  head.id = CHUNK_AMOUNTOF;
  head.len = sizeof(head) + sizeof(c);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, sizeof(c), &c);
  prid_get_size_head(&prd);
  p_tmp = prid_slep(&prd, p_velikost);
  prid_zrus(&prd);
  return (p_tmp);
}

void *ds_vyrob_chunk_material_transparency(EDIT_MATERIAL * p_mat,
  int *p_velikost)
{
  byte *p_tmp;
  word c = 0;
  TChunkHeader head;
  PRIDAVAC prd;

  head.id = CHUNK_TRANSPARENCY;
  prid_init(&prd);
  prid_add_copy(&prd, sizeof(head), &head);
  head.id = CHUNK_AMOUNTOF;
  head.len = sizeof(head) + sizeof(c);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, sizeof(c), &c);
  prid_get_size_head(&prd);
  p_tmp = prid_slep(&prd, p_velikost);
  prid_zrus(&prd);
  return (p_tmp);
}

void *ds_vyrob_chunk_material_fall_off(EDIT_MATERIAL * p_mat, int *p_velikost)
{
  byte *p_tmp;
  word c = 0;
  TChunkHeader head;
  PRIDAVAC prd;

  head.id = CHUNK_TRANSFALLOFF;
  prid_init(&prd);
  prid_add_copy(&prd, sizeof(head), &head);
  head.id = CHUNK_AMOUNTOF;
  head.len = sizeof(head) + sizeof(c);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, sizeof(c), &c);
  prid_get_size_head(&prd);
  p_tmp = prid_slep(&prd, p_velikost);
  prid_zrus(&prd);
  return (p_tmp);
}

void *ds_vyrob_chunk_material_reflection_blur(EDIT_MATERIAL * p_mat,
  int *p_velikost)
{
  byte *p_tmp;
  word c = 0;
  TChunkHeader head;
  PRIDAVAC prd;

  head.id = CHUNK_REFBLUR;
  prid_init(&prd);
  prid_add_copy(&prd, sizeof(head), &head);
  head.id = CHUNK_AMOUNTOF;
  head.len = sizeof(head) + sizeof(c);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, sizeof(c), &c);
  prid_get_size_head(&prd);
  p_tmp = prid_slep(&prd, p_velikost);
  prid_zrus(&prd);
  return (p_tmp);
}

void *ds_vyrob_chunk_material_shading(EDIT_MATERIAL * p_mat, int *p_velikost)
{
  byte *p_tmp;
  word c = 3;
  TChunkHeader head;
  PRIDAVAC prd;

  head.id = CHUNK_SHADING;
  prid_init(&prd);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, sizeof(c), &c);
  prid_get_size_head(&prd);
  p_tmp = prid_slep(&prd, p_velikost);
  prid_zrus(&prd);
  return (p_tmp);
}


void *ds_vyrob_chunk_material_self_illum(EDIT_MATERIAL * p_mat,
  int *p_velikost)
{
  byte *p_tmp;
  word c = 0;
  TChunkHeader head;
  PRIDAVAC prd;

  head.id = CHUNK_SELFILLUM;
  prid_init(&prd);
  prid_add_copy(&prd, sizeof(head), &head);
  head.id = CHUNK_AMOUNTOF;
  head.len = sizeof(head) + sizeof(c);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, sizeof(c), &c);
  prid_get_size_head(&prd);
  p_tmp = prid_slep(&prd, p_velikost);
  prid_zrus(&prd);
  return (p_tmp);
}

void *ds_vyrob_chunk_material_wire_size(EDIT_MATERIAL * p_mat,
  int *p_velikost)
{
  byte *p_tmp;
  float c = 1.0f;
  TChunkHeader head;
  PRIDAVAC prd;

  head.id = CHUNK_WIRESIZE;
  head.len = sizeof(head) + sizeof(float);
  prid_init(&prd);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, sizeof(c), &c);
  p_tmp = prid_slep(&prd, p_velikost);
  prid_zrus(&prd);
  return (p_tmp);
}

void *ds_vyrob_chunk_material_texture(EDIT_MATERIAL * p_mat, int *p_velikost)
{
  byte *p_tmp;
  word c = 100;
  float f;
  TChunkHeader head;
  PRIDAVAC prd;

  head.id = CHUNK_TEXTURE;
  prid_init(&prd);
  prid_add_copy(&prd, sizeof(head), &head);

  head.id = CHUNK_AMOUNTOF;
  head.len = sizeof(head) + sizeof(c);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, sizeof(c), &c);

  head.id = CHUNK_MAPFILE;
  head.len = sizeof(head) + strlen(p_mat->textfile[0]) + 1;
  _strupr(p_mat->textfile[0]);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, strlen(p_mat->textfile[0]) + 1, p_mat->textfile[0]);

  c = 0;
  head.id = CHUNK_MAPFLAGS;
  head.len = sizeof(head) + sizeof(word);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, sizeof(c), &c);

  f = 0.0f;
  head.id = CHUNK_MAPBLUR;
  head.len = sizeof(head) + sizeof(f);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add_copy(&prd, sizeof(f), &f);

  prid_get_size_head(&prd);
  p_tmp = prid_slep(&prd, p_velikost);
  prid_zrus(&prd);
  return (p_tmp);
}


void *ds_vyrob_chunk_material(EDIT_MATERIAL * p_mat, int *p_velikost)
{
  byte *p_tmp;
  TChunkHeader head;
  PRIDAVAC prd;
  int vel;

  prid_init(&prd);
  head.id = CHUNK_MATERIAL;

  prid_add_copy(&prd, sizeof(head), &head);
  prid_add(&prd, vel, ds_vyrob_chunk_material_name(p_mat, &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_material_ambient(p_mat, &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_material_diffuse(p_mat, &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_material_specular(p_mat, &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_material_shiness(p_mat, &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_material_shiness_strenght(p_mat, &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_material_transparency(p_mat, &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_material_fall_off(p_mat, &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_material_reflection_blur(p_mat, &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_material_shading(p_mat, &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_material_self_illum(p_mat, &vel));
  head.id = CHUNK_MATXPFALLIN;
  head.len = sizeof(head);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add(&prd, vel, ds_vyrob_chunk_material_wire_size(p_mat, &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_material_texture(p_mat, &vel));

  prid_get_size_head(&prd);
  p_tmp = prid_slep(&prd, p_velikost);
  prid_zrus(&prd);
  return (p_tmp);
}

void *ds_vyrob_chunk_mesh_version(int *p_vel)
{
  TChunkHeader *p_head;
  byte *p_mem;
  word *p_ver;

  p_mem = malloc(sizeof(p_head[0]) + sizeof(dword));
  mtest(p_mem);
  p_head = (TChunkHeader *) p_mem;
  p_head->id = CHUNK_MESHVER;
  *p_vel = p_head->len = sizeof(p_head[0]) + sizeof(p_ver);
  p_ver = (word *) (p_head + 1);
  *p_ver = 3;
  return (p_mem);
}

void *ds_vyrob_chunk_mesh_master_scale(int *p_vel)
{
  TChunkHeader *p_head;
  byte *p_mem;
  float *p_scale;

  p_mem = malloc(sizeof(p_head[0]) + sizeof(dword));
  mtest(p_mem);
  p_head = (TChunkHeader *) p_mem;
  p_head->id = CHUNK_MASTERSCALE;
  *p_vel = p_head->len = sizeof(p_head[0]) + sizeof(p_scale);
  p_scale = (float *) (p_head + 1);
  *p_scale = 1.0f;
  return (p_mem);
}


/* Vyrobi list objektu
*/
void *ds_vyrob_chunk_obj_mesh(EDIT_KONTEJNER * p_kont, int *p_velikost)
{
  TChunkHeader head;
  int vel;
  int i, m;
  PRIDAVAC prd;

  prid_init(&prd);

  head.id = CHUNK_OBJMESH;
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add(&prd, vel, ds_vyrob_chunk_mesh_version(&vel));

  for (m = 0; m < MAX_CELKEM_MATERIALU; m++) {
    if (p_cnf->p_mat[m] && p_cnf->p_mat[m]->flag & MAT_POUZITY) {
      prid_add(&prd, vel, ds_vyrob_chunk_material(p_cnf->p_mat[m], &vel));
    }
  }

  prid_add(&prd, vel, ds_vyrob_chunk_mesh_master_scale(&vel));

  for (i = 0; i < MAX_KONT_OBJEKTU; i++) {
    if (p_kont->p_obj[i]) {
      prid_add(&prd, vel, ds_vyrob_chunk_objblock(p_kont->p_obj[i], &vel));
    }
  }

  prid_get_size_head(&prd);
  return (prid_slep_a_zrus(&prd, p_velikost));
}

void *ds_vyrob_chunk_verze(int *p_vel)
{
  TChunkHeader *p_head;
  byte *p_mem;
  word *p_ver;

  p_mem = malloc(sizeof(p_head[0]) + sizeof(word));
  mtest(p_mem);
  p_head = (TChunkHeader *) p_mem;
  p_head->id = CHUNK_VER;
  *p_vel = p_head->len = sizeof(p_head[0]) + sizeof(float);
  p_ver = (word *) (p_head + 1);
  *p_ver = 3;
  return (p_mem);
}

void *ds_vyrob_chunk_keyframer_frames(int *p_vel)
{
  TChunkHeader *p_head;
  byte *p_mem;
  dword *p_frames;
  int delka;

  delka = sizeof(p_head[0]) + 2 * sizeof(dword);
  p_mem = malloc(delka);
  mtest(p_mem);

  p_head = (TChunkHeader *) p_mem;
  p_head->id = CHUNK_FRAMES;
  p_head->len = delka;
  p_head++;

  p_frames = (dword *) (p_head);
  *p_frames++ = 0;
  *p_frames++ = 100;

  *p_vel = delka;
  return (p_mem);
}

void *ds_vyrob_chunk_keyframer_trackinfo_objnum(EDIT_OBJEKT * p_obj, word n,
  int *p_vel)
{
  TChunkHeader *p_head;
  byte *p_mem;
  word *p_num;
  int delka;

  delka = sizeof(p_head[0]) + sizeof(p_num[0]);
  p_mem = malloc(delka);
  mtest(p_mem);

  p_head = (TChunkHeader *) p_mem;
  p_head->id = CHUNK_OBJNUMBER;
  p_head->len = delka;
  p_head++;

  p_num = (word *) (p_head);
  *p_num = n;

  *p_vel = delka;
  return (p_mem);
}

void *ds_vyrob_chunk_keyframer_trackinfo_objname(EDIT_OBJEKT * p_obj,
  int *p_vel)
{
  TChunkHeader *p_head;
  byte *p_mem;
  word *p_num;
  int delka;

  delka = sizeof(p_head[0]) + 3 * sizeof(word) + strlen(p_obj->jmeno) + 1;
  p_mem = malloc(delka);
  mtest(p_mem);

  p_head = (TChunkHeader *) p_mem;
  p_head->id = CHUNK_TRACKOBJNAME;
  p_head->len = delka;
  p_head++;

  strcpy((char *) p_head, p_obj->jmeno);
  p_num = (word *) (p_mem + sizeof(TChunkHeader) + strlen(p_obj->jmeno) + 1);
  *p_num++ = 0x4000;
  *p_num++ = 0x0;
  *p_num++ = 0xffff;

  *p_vel = delka;
  return (p_mem);
}

// Pivot point position reader
void *ds_vyrob_chunk_keyframer_trackinfo_pivot(EDIT_OBJEKT * p_obj,
  int *p_vel)
{
  TChunkHeader *p_head;
  byte *p_mem;
  int delka;
  BOD *p_pivot;

  delka = sizeof(p_head[0]) + sizeof(BOD);
  p_mem = malloc(delka);
  mtest(p_mem);

  p_head = (TChunkHeader *) p_mem;
  p_head->id = CHUNK_TRACKPIVOT;
  p_head->len = delka;
  p_head++;

  p_pivot = (BOD *) p_head;
  p_pivot->x = p_pivot->y = p_pivot->z = 0;
  *p_vel = delka;

  return (p_mem);
}

int ds_zapis_chunk_keyframer_trackinfo_flags(byte * p_flags, int num)
{
  word *p_flag = (word *) p_flags;

  memset(p_flag, 0, sizeof(word) * 7);
  p_flag[0] = 0;
  p_flag[5] = num;
  return (sizeof(word) * 7);
}

int ds_zapis_chunk_keyframer_trackinfo_spline_flags(byte * p_flags, int frame)
{
  word *p_word = (word *) p_flags;

  p_word[0] = frame;            // frame
  p_word[1] = 0;                // nezname
  p_word[2] = 0;                // flag

  // na parametry animace prdim

  return (sizeof(word) * 3);
}

void *ds_vyrob_chunk_keyframer_trackinfo_position_keys(EDIT_OBJEKT * p_obj,
  int *p_vel)
{
  TChunkHeader *p_head;
  byte *p_mem;
  byte *p_tmp;
  int delka;
  BOD *p_p;

  delka = sizeof(p_head[0])     // hlavicka
    + sizeof(word) * 7          // flagy
    + sizeof(word) * 3          // 1 spline flag
    + sizeof(BOD);              // 1 pozice

  p_mem = malloc(delka);
  mtest(p_mem);

  p_head = (TChunkHeader *) p_mem;
  p_head->id = CHUNK_TRACKPOS;
  p_head->len = delka;
  p_tmp = (byte *) (p_head + 1);

  p_tmp += ds_zapis_chunk_keyframer_trackinfo_flags(p_tmp, 1);
  p_tmp += ds_zapis_chunk_keyframer_trackinfo_spline_flags(p_tmp, 0);
  p_p = (BOD *) p_tmp;

  p_p->x = p_p->y = p_p->z = 0.0f;

  *p_vel = delka;
  return (p_mem);
}

void *ds_vyrob_chunk_keyframer_trackinfo_scale_keys(EDIT_OBJEKT * p_obj,
  int *p_vel)
{
  TChunkHeader *p_head;
  byte *p_mem;
  byte *p_tmp;
  int delka;
  BOD *p_p;

  delka = sizeof(p_head[0])     // hlavicka
    + sizeof(word) * 7          // flagy
    + sizeof(word) * 3          // 1 spline flag
    + sizeof(BOD);              // 1 pozice

  p_mem = malloc(delka);
  mtest(p_mem);

  p_head = (TChunkHeader *) p_mem;
  p_head->id = CHUNK_TRACKSCALE;
  p_head->len = delka;
  p_tmp = (byte *) (p_head + 1);

  p_tmp += ds_zapis_chunk_keyframer_trackinfo_flags(p_tmp, 1);
  p_tmp += ds_zapis_chunk_keyframer_trackinfo_spline_flags(p_tmp, 0);
  p_p = (BOD *) p_tmp;

  p_p->x = p_p->y = p_p->z = 1.0f;

  *p_vel = delka;
  return (p_mem);
}

void *ds_vyrob_chunk_keyframer_trackinfo_rotation_keys(EDIT_OBJEKT * p_obj,
  int *p_vel)
{
  TChunkHeader *p_head;
  byte *p_mem;
  byte *p_tmp;
  int delka;
  float *p_v;

  delka = sizeof(p_head[0])     // hlavicka
    + sizeof(word) * 7          // flagy
    + sizeof(word) * 3          // 1 spline flag
    + sizeof(float) * 4;        // 1 rotace

  p_mem = malloc(delka);
  mtest(p_mem);

  p_head = (TChunkHeader *) p_mem;
  p_head->id = CHUNK_TRACKROTATE;
  p_head->len = delka;
  p_tmp = (byte *) (p_head + 1);

  p_tmp += ds_zapis_chunk_keyframer_trackinfo_flags(p_tmp, 1);
  p_tmp += ds_zapis_chunk_keyframer_trackinfo_spline_flags(p_tmp, 0);
  p_v = (float *) p_tmp;

  p_v[0] = p_v[1] = p_v[2] = p_v[3] = 0.0f;

  *p_vel = delka;
  return (p_mem);
}

// zapis celeho trackinfa
void *ds_vyrob_chunk_keyframer_trackinfo(EDIT_OBJEKT * p_obj, int n,
  int *p_vel)
{
  TChunkHeader head;
  int vel;
  PRIDAVAC prd;

  head.id = CHUNK_TRACKINFO;
  prid_init(&prd);
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add(&prd, vel, ds_vyrob_chunk_keyframer_trackinfo_objnum(p_obj,
      (word) n, &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_keyframer_trackinfo_objname(p_obj,
      &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_keyframer_trackinfo_pivot(p_obj, &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_keyframer_trackinfo_position_keys(p_obj,
      &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_keyframer_trackinfo_rotation_keys(p_obj,
      &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_keyframer_trackinfo_scale_keys(p_obj,
      &vel));
  prid_get_size_head(&prd);
  return (prid_slep_a_zrus(&prd, p_vel));
}

// key-frame
void *ds_vyrob_chunk_keyframer(EDIT_KONTEJNER * p_kont, int *p_vel)
{
  TChunkHeader head;
  int i, j;
  int vel;
  PRIDAVAC prd;

  prid_init(&prd);
  head.id = CHUNK_KEYFRAMER;
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add(&prd, vel, ds_vyrob_chunk_keyframer_frames(&vel));

  for (i = 0, j = 0; i < MAX_KONT_OBJEKTU; i++) {
    if (p_kont->p_obj[i]) {
      prid_add(&prd, vel, ds_vyrob_chunk_keyframer_trackinfo(p_kont->p_obj[i],
          j, &vel));
      j++;
    }
  }
  prid_get_size_head(&prd);
  return (prid_slep_a_zrus(&prd, p_vel));
}

void *ds_vyrob_chunk_main(EDIT_KONTEJNER * p_kont, int *p_velikost)
{
  TChunkHeader head;
  int vel;
  PRIDAVAC prd;

  prid_init(&prd);
  head.id = CHUNK_MAIN;
  prid_add_copy(&prd, sizeof(head), &head);
  prid_add(&prd, vel, ds_vyrob_chunk_verze(&vel));
  prid_add(&prd, vel, ds_vyrob_chunk_obj_mesh(p_kont, &vel));
  prid_add(&prd, vel, ds_vyrob_chunk_keyframer(p_kont, &vel));
  prid_get_size_head(&prd);
  return (prid_slep_a_zrus(&prd, p_velikost));
}

/* Exportuju 3DS soubor
*/
int ds_export_kontejner(byte * p_file, EDIT_KONTEJNER * p_kont)
{
  void *p_data;
  int velikost;
  FILE *f;

  oe_oznac_pouzite_materialy_kont(p_cnf, p_kont);
  if (oe_oznac_dlouhe_materialy_kont(p_cnf, p_kont)) {
    ddw("Nemuzu exportovat dlouhe jmena textur");
    kprintf(TRUE, "Nemuzu exportovat dlouhe jmena textur");
    return (FALSE);
  }

  lo_kontejner_jedinecne_jmena(p_kont, TRUE);

  f = fopen(p_file, "wb");
  if (!f) {
    return (FALSE);
  }

  p_data = ds_vyrob_chunk_main(p_kont, &velikost);
  fwrite(p_data, sizeof(byte), velikost, f);
  free(p_data);

  fclose(f);
  return (TRUE);
}

int ds_export_sceny(byte * p_file)
{
  EDIT_KONTEJNER *p_kont = oe_slep_kontejnery_vsechny(p_cnf);

  if (p_kont) {
    int ret = ds_export_kontejner(p_file, p_kont);

    zrus_kontejner_rec(&p_kont, NULL);
  }
  return (TRUE);
}
