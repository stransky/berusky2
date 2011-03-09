/*
  Okeni interface k objekt editoru
*/
#include "mss_on.h"
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <direct.h>
#include <float.h>
#include <assert.h>
#include "objekt_editor_all.h"
#include "..\\kofola\\editor levelu.h"

#define  UDM_GETPOS32      0x0472
#define  UDM_SETPOS32      0x0471
#ifndef  UDM_SETRANGE32
#define  UDM_SETRANGE32    0x046F
#endif

#include "dbgwnd.h"

static K_EDITOR     *p_cnf = &cnf;
static POM_PROMENE   tmp;
static MAPING_PROM   mmp;


/* Alfa-blending
*/
#define  BLEND_FAKTORU_SRC  9
byte blend_src_text[][40] = {"GL_ZERO", "GL_ONE", "GL_DST_COLOR", 
                             "GL_ONE_MINUS_DST_COLOR", "GL_SRC_ALPHA", 
                             "GL_ONE_MINUS_SRC_ALPHA", "GL_DST_ALPHA", 
                             "GL_ONE_MINUS_DST_ALPHA", "GL_SRC_ALPHA_SATURATE"};

GLenum blend_src_val[] = {GL_ZERO, GL_ONE, GL_DST_COLOR, 
                          GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, 
                          GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, 
                          GL_ONE_MINUS_DST_ALPHA, GL_SRC_ALPHA_SATURATE};


#define  BLEND_FAKTORU_DEST  8
byte blend_dest_text[][40] = {"GL_ZERO", "GL_ONE", "GL_SRC_COLOR", 
                              "GL_ONE_MINUS_SRC_COLOR", "GL_SRC_ALPHA", 
                              "GL_ONE_MINUS_SRC_ALPHA", "GL_DST_ALPHA", 
                              "GL_ONE_MINUS_DST_ALPHA"};

GLenum blend_dest_val[] = {GL_ZERO, GL_ONE, GL_SRC_COLOR, 
                           GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, 
                           GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, 
                           GL_ONE_MINUS_DST_ALPHA};

#define RGBF "%.1f"
/* Michani textur
*/
#define TARGET_PARAM      1
byte   text_target_text[][30] = {"GL_TEXTURE_ENV"};
GLenum text_target_val[] =      { GL_TEXTURE_ENV };

#define TEXT_PARAM      1
byte   text_pname_text[][30] = {"GL_TEXTURE_ENV_MODE"};
GLenum text_pname_val[] =      { GL_TEXTURE_ENV_MODE };

#define TEXT_ARGUMETU   5
byte  text_argument_text[][30] = {"GL_MODULATE","GL_ADD","GL_DECAL","GL_BLEND","GL_REPLACE"};
GLint text_argument_val[] =      { GL_MODULATE,  GL_ADD,  GL_DECAL,  GL_BLEND,  GL_REPLACE };


int zobraz_akt_material(HWND hDlg, EDIT_MATERIAL *p_tmm);
static void doe_vloz_stat_svetla(HWND hDlg, int lightakt, int combo);
static void doe_vloz_stat_svetla_combo(HWND hDlg, int lightakt);

__inline void EnableDlgItem(HWND hDlg, int res, int stav)
{
  EnableWindow(GetDlgItem(hDlg,res), stav ? TRUE : FALSE);
}

__inline void doe_button_stlac(HWND hDlg, int res, int stav)
{  
  SendMessage(GetDlgItem(hDlg,res),BM_SETSTATE,stav,0);
} 

__inline void doe_check_stlac(HWND hDlg, int res, int stav)
{   
  CheckDlgButton(hDlg, res, stav ? BST_CHECKED : BST_UNCHECKED);
} 

__inline int doe_get_check_flag(HWND hDlg, int res, int flag, int test_flag)
{
  return(IsDlgButtonChecked(hDlg,res) ? flag|test_flag : flag&~test_flag);
}

void doe_sec_to_time(float sec, int *p_des, int *p_sec, int *p_min)
{
  div_t rsl;
  double s;
  *p_des = ftoi((float)modf(sec,&s)*100.0f);
  *p_sec = ftoi((float)s);
  rsl = div((int)s,60);
  *p_min = rsl.quot;
  *p_sec = rsl.rem;
}

void doe_nastav_k_editor(K_EDITOR *p_config)
{
  p_cnf = p_config;
  memset(&tmp,0,sizeof(tmp));
}

dword doe_vyber_barvu_do_kolonky(HWND hDlg, int control)
{
  dword alfa;
  dword barva;
  byte  pom[50];
  GetDlgItemText(hDlg,control,pom,49);
  sscanf(pom,"%x",&barva);
  alfa = barva&0xff000000;
  if(doe_prikaz_vyber_barvu(p_cnf,hDlg,&barva)) {
    SetDlgItemText( hDlg, control, itoa(barva|alfa,pom,16));
  }
  return(barva);
}


__inline int doe_vyber_kolonku_int(HWND hDlg, int control)
{  
  byte  pom[50];
  GetDlgItemText(hDlg,control,pom,49);  
  return(atoi(pom));
}

__inline int doe_vyber_kolonku_int_hexa(HWND hDlg, int control)
{
  dword barva;
  byte  pom[50];
  GetDlgItemText(hDlg,control,pom,49);  
  sscanf(pom,"%x",&barva);
  return(barva);
}

__inline float doe_vyber_kolonku_float(HWND hDlg, int control)
{  
  byte  pom[50];
  GetDlgItemText(hDlg,control,pom,49);
  return(atoff(pom));
}

dword doe_vyber_barvu_do_kolonek(HWND hDlg, int cr, int cg, int cb, float *b)
{
  dword barva;

  b[0] = doe_vyber_kolonku_float(hDlg,cr);
  b[1] = doe_vyber_kolonku_float(hDlg,cg);
  b[2] = doe_vyber_kolonku_float(hDlg,cb);
  barva = float_rgb(b);
  if(doe_prikaz_vyber_barvu(p_cnf,hDlg,&barva)) {
    rgb_float(barva,b);
    doe_printf(hDlg,cr,RGBF,b[0]);
    doe_printf(hDlg,cg,RGBF,b[1]);
    doe_printf(hDlg,cb,RGBF,b[2]);
  }
  return(barva);
}

void doe_prikaz_kresli_vsechno(K_EDITOR *p_cnf)
{
  int k,o;
  for(k = 0; k < p_cnf->max_kont; k++) {
    if(p_cnf->p_kont[k]) {
      for(o = 0; o < p_cnf->p_kont[k]->max_objektu; o++) {
        if(p_cnf->p_kont[k]->p_obj[o])
          p_cnf->p_kont[k]->p_obj[o]->kreslit = TRUE;
      }
      p_cnf->p_kont[k]->kreslit = TRUE;
    }
  }
}

// prevede kontejner na stage bloky
void _preved_kontejner(K_EDITOR *p_cnf, byte *p_file)
{
 EDIT_KONTEJNER *p_kont; 
 int  i,m;

 chdir(p_cnf->dir.out_dir);
 p_kont = lo_nahraj_kontejner_out(p_cnf->p_mat, MAX_EDIT_MATERIALU,p_cnf->p_text, MAX_EDIT_TEXTUR, p_file, TRUE);
 nastav_barvu_bodu_kont_dword(p_kont,0xffffffff,COLOR_SET);
 for(i = 0; i < p_kont->max_objektu; i++) {
   if(p_kont->p_obj[i]) {
     m = p_kont->p_obj[i]->material;
     mat_default_stage_bloky(p_cnf->p_mat[m]);
   }
 }
 chdir(p_cnf->dir.out_dir);
 lo_uloz_kontejner(p_cnf->p_mat,MAX_EDIT_MATERIALU,p_kont,p_file,FALSE,NULL);
}

void doe_prevod_kontejneru(K_EDITOR *p_cnf)
{
  struct _finddata_t file;
  byte pom[100];
  HWND hwnd;
  int done = 0, handle = 0;

  if(MessageBox(hwnd_aplikace,"Tato operace infikuje kontejnery stage state \
blokem cislo 0, aby se daly pouzit ve hre. Chces pokracovat ?",
"Infikace Stage State Blokem...",MB_YESNO|MB_ICONINFORMATION|MB_DEFBUTTON2) == IDYES) {

    hwnd = CreateDialog(hinst,MAKEINTRESOURCE(IDD_LOADING),hwnd_aplikace,DefWindowProc);
    chdir(p_cnf->dir.out_dir);
    done = handle = _findfirst( KONCOVKA_MESH_OLD, &file);
    while(done != -1) {
      sprintf(pom,"Konvertuji soubor '%s' ...",file.name);
      SetDlgItemText(hwnd, IDC_STATIC1, pom);
      _preved_kontejner(p_cnf,file.name);      
      done = _findnext(handle,&file);
    }
    EndDialog(hwnd, TRUE);
  }
}

// zvedne kontejner o X_LEVEL/2
void _preved_kontejner_pozvedni(K_EDITOR *p_cnf, byte *p_file)
{
 EDIT_KONTEJNER *p_kont = NULL; 

 chdir(p_cnf->dir.out_dir);
 p_kont = lo_nahraj_kontejner_out(p_cnf->p_mat, MAX_EDIT_MATERIALU,p_cnf->p_text, MAX_EDIT_TEXTUR, p_file, TRUE);
 if(p_kont) {
   init_matrix(&p_kont->world);
   translate_matrix(&p_kont->world,0,Y_PRVEK/2,0);
   transformuj_kontejner(p_kont);
   chdir(p_cnf->dir.out_dir);
   updatuj_kontejner_statistika(p_kont,TRUE);
   lo_uloz_kontejner(p_cnf->p_mat,MAX_EDIT_MATERIALU,p_kont,p_file,FALSE,NULL);
 } else
   ddw("Chyba loadu !");
}

void doe_prevod_kontejneru_nahoru(K_EDITOR *p_cnf)
{
  struct _finddata_t file;
  byte pom[100];
  int done = 0, handle = 0;

  if(MessageBox(hwnd_aplikace,"Vyzvednuti kontejneru. Pokracovat ?",
"Pozvednuti kontejneru...",MB_YESNO|MB_ICONINFORMATION|MB_DEFBUTTON2) == IDYES) {
    
    chdir(p_cnf->dir.out_dir);
    done = handle = _findfirst( KONCOVKA_MESH_OLD, &file);
    while(done != -1) {
      sprintf(pom,"Prevest kontejner '%s' ?",file.name);
      if(MessageBox(hwnd_aplikace,pom,"Pozvednuti kontejneru...",MB_YESNO|MB_ICONINFORMATION|MB_DEFBUTTON2) == IDYES)
        _preved_kontejner_pozvedni(p_cnf,file.name);
      done = _findnext(handle,&file);
    }
  }
}

BOOL CALLBACK doe_callback_cti_jmeno( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {          
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT1),tmp.p_pom,20);
          EndDialog( hDlg, TRUE );
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT1),tmp.p_pom);
      if(tmp.p_pom1)
        SetWindowText(hDlg,tmp.p_pom1);
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_cti_jmeno(HWND hwnd, byte *p_jmeno)
{
  tmp.p_pom = p_jmeno;
  tmp.p_pom1 = NULL;
  return(DialogBox( hinst, MAKEINTRESOURCE(IDD_JMENO), hwnd, doe_callback_cti_jmeno));
}

int doe_cti_jmeno_ext(HWND hwnd, byte *p_jmeno, byte *p_nadpis)
{
  tmp.p_pom = p_jmeno;
  tmp.p_pom1 = p_nadpis;
  return(DialogBox( hinst, MAKEINTRESOURCE(IDD_JMENO), hwnd, doe_callback_cti_jmeno));
}

BOOL CALLBACK doe_callback_export_sceny( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {          
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT1),tmp.p_pom,20);
          *(int *)tmp.p_pom1 = IsDlgButtonChecked(hDlg,IDC_CHECK1);
          EndDialog( hDlg, TRUE );
          break;
        }
        default:
          return FALSE;
      }
      break;
    case WM_INITDIALOG: {
      int ch = *(int *)tmp.p_pom1;
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT1),tmp.p_pom);
      CheckDlgButton(hDlg, IDC_CHECK1, ch ? BST_CHECKED : BST_UNCHECKED);
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

//
int doe_export_sceny(HWND hwnd, byte *p_prefix, int *p_dos)
{
  tmp.p_pom = p_prefix;
  tmp.p_pom1 = p_dos;
  return(DialogBox( hinst, MAKEINTRESOURCE(IDD_EXPORT_SCENY), hwnd, doe_callback_export_sceny));
}

BOOL CALLBACK doe_callback_kopie_kontejneru( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {
          tmp.i[0] = doe_get_check_flag(hDlg, IDC_CHECK1, tmp.i[0], COPY_GEOMETRY);
          tmp.i[0] = doe_get_check_flag(hDlg, IDC_CHECK2, tmp.i[0], COPY_DIFFUSE);
          tmp.i[0] = doe_get_check_flag(hDlg, IDC_CHECK3, tmp.i[0], COPY_SPECULAR);
          tmp.i[0] = doe_get_check_flag(hDlg, IDC_CHECK4, tmp.i[0], COPY_MAP1);
          tmp.i[0] = doe_get_check_flag(hDlg, IDC_CHECK5, tmp.i[0], COPY_MAP2);
          tmp.i[0] = doe_get_check_flag(hDlg,IDC_CHECK25, tmp.i[0], COPY_MATERIAL);
          EndDialog( hDlg, TRUE );
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      doe_check_stlac(hDlg, IDC_CHECK1, tmp.i[0]&COPY_GEOMETRY);
      doe_check_stlac(hDlg, IDC_CHECK2, tmp.i[0]&COPY_DIFFUSE);
      doe_check_stlac(hDlg, IDC_CHECK3, tmp.i[0]&COPY_SPECULAR);
      doe_check_stlac(hDlg, IDC_CHECK4, tmp.i[0]&COPY_MAP1);
      doe_check_stlac(hDlg, IDC_CHECK5, tmp.i[0]&COPY_MAP2);
      doe_check_stlac(hDlg,IDC_CHECK25, tmp.i[0]&COPY_MATERIAL);
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_kopie_kontejneru(HWND hwnd, int *p_flag)
{
  tmp.i[0] = *p_flag;
  if(DialogBox( hinst, MAKEINTRESOURCE(IDD_KOPIE_KONTEJNERU), hwnd, doe_callback_kopie_kontejneru)) {
    *p_flag = tmp.i[0];
    return(TRUE);
  } else {
    return(FALSE);
  }
}

BOOL CALLBACK doe_callback_kamera_animace_uprav( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: 
          {
            KAMERA_TRACK_INFO *p_track = tmp.p_pom;
            byte pom[200];
            
            GetDlgItemText(hDlg,IDC_EDIT1,p_track->jmeno,50);
            GetDlgItemText(hDlg,IDC_EDIT2,pom,99);
            p_track->endtime = calc_endtime(atoi(pom));
            
            EndDialog( hDlg, TRUE );
            break;
          }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: 
      {
        KAMERA_TRACK_INFO *p_track = tmp.p_pom;
        byte pom[200];
        int  framenum = calc_keynum(p_track->endtime);
        SetDlgItemText(hDlg,IDC_EDIT1,p_track->jmeno);
        SetDlgItemText(hDlg,IDC_EDIT2,itoa(framenum,pom,10));
        break;
      }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_kamera_animace_uprav(HWND hwnd, KAMERA_TRACK_INFO *p_track)
{
  int ret;
  tmp.p_pom = p_track;
  ret = DialogBox( hinst, MAKEINTRESOURCE(IDD_KAMERA_ANIMACE_UPRAVIT),
                   hwnd, doe_callback_kamera_animace_uprav);
  return(ret);
}

BOOL CALLBACK doe_callback_animace_uprav( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: 
          {            
            byte pom[200];
            GetDlgItemText(hDlg,IDC_EDIT2,pom,99);
            tmp.i[0] = atoi(pom);
            EndDialog( hDlg, TRUE );
            break;
          }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: 
      {        
        doe_printf(hDlg,IDC_EDIT1,"Defaultni a jednina animace");
        doe_printf(hDlg,IDC_EDIT2,"%d",tmp.i[0]);
        break;
      }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_animace_uprav(HWND hwnd, int *p_framenum)
{
  tmp.i[0] = *p_framenum;
  if(DialogBox( hinst, MAKEINTRESOURCE(IDD_KAMERA_ANIMACE_UPRAVIT),hwnd, doe_callback_animace_uprav)) {
    *p_framenum = tmp.i[0];
  }
  return(TRUE);
}

BOOL CALLBACK doe_callback_vlastnosti_prvku( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {          
          PRVEK_LEVELU *p_bunka = tmp.p_pom;
          byte pom[100];
          
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT1),pom,100);
          p_bunka->guid = atoi(pom);
          
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT8),pom,100);
          p_bunka->Vazba1 = atoi(pom);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT9),pom,100);
          p_bunka->Vazba2 = atoi(pom);
          
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT10),pom,100);
          p_bunka->Rotace = atoi(pom);
          
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT11),pom,100);
          p_bunka->mesh = atoi(pom);                    

          EndDialog( hDlg, TRUE );
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {      
      PRVEK_LEVELU *p_bunka = tmp.p_pom;
      byte pom[100];
      int  x,y,z,p,guid;
      
      guid = p_bunka->guid ? p_bunka->guid : p_bunka->p_prv_databaze->guid;          
      itoa(guid,pom,10);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT1),pom);

      itoa(p_bunka->Vazba1,pom,10);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT8),pom);
      itoa(p_bunka->Vazba2,pom,10);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT9),pom);
      
      itoa(p_bunka->Rotace,pom,10);      
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT10),pom);

      itoa(p_bunka->mesh,pom,10);      
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT11),pom);

      // kont
      itoa(p_bunka->p_prv_databaze->kont,pom,10);      
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT12),pom);
      itoa(p_bunka->p_prv_databaze->kof_index,pom,10);      
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT13),pom);
      itoa((dword)(p_bunka->p_prv_databaze->p_kont),pom,16);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT20),pom);

      be_pozice_na_disku(p_bunka->lev_index,x,y,z,p,p_cnf->bnf.x_num,p_cnf->bnf.z_num);
      doe_printf(hDlg,IDC_EDIT21,"%d",x);
      doe_printf(hDlg,IDC_EDIT22,"%d",y);
      doe_printf(hDlg,IDC_EDIT23,"%d",z);
      doe_printf(hDlg,IDC_EDIT41,"%d",p);
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_vlastnosti_prvku(HWND hwnd, PRVEK_LEVELU *p_bunka)
{
  tmp.p_pom = p_bunka;
  return(DialogBox( hinst, MAKEINTRESOURCE(IDD_BER_VLASTNOSTI_PRVKU), hwnd, doe_callback_vlastnosti_prvku));
}

BOOL CALLBACK doe_callback_vertex_info( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {          
          EndDialog( hDlg, FALSE );
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {      
      MUJ_BOD *p_bod = tmp.p_pom;
      byte pom[100];
      sprintf(pom,"%d - %s",tmp.i[0],p_cnf->p_kont[tmp.i[0]]->jmeno);
      SetWindowText(GetDlgItem(hDlg,IDC_STATIC1),pom);
      sprintf(pom,"%d - %s",tmp.i[1],p_cnf->p_kont[tmp.i[0]]->p_obj[tmp.i[1]]->jmeno);
      SetWindowText(GetDlgItem(hDlg,IDC_STATIC2),pom);
      sprintf(pom,"%d",tmp.i[2]);
      SetWindowText(GetDlgItem(hDlg,IDC_STATIC3),pom);
      sprintf(pom,"%f",p_bod->x);
      SetWindowText(GetDlgItem(hDlg,IDC_STATIC4),pom);
      sprintf(pom,"%f",p_bod->y);
      SetWindowText(GetDlgItem(hDlg,IDC_STATIC5),pom);
      sprintf(pom,"%f",p_bod->z);
      SetWindowText(GetDlgItem(hDlg,IDC_STATIC6),pom);
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_vertex_info(HWND hwnd, MUJ_BOD *p_bod, int k, int o, int v)
{
  tmp.p_pom = p_bod;
  tmp.i[0] = k;
  tmp.i[1] = o;
  tmp.i[2] = v;
  DialogBox( hinst, MAKEINTRESOURCE(IDD_VERTEXT_INFO), hwnd, doe_callback_vertex_info);
}

int doe_dialog_load(HWND hwnd, byte *p_filename, byte max_delka, byte *p_nadpis_okna, byte *p_maska, byte *p_popis_masky)
{
 OPENFILENAME soubor;
 byte pom[500],pom2[500];
 int r,i = 0;
 
 p_filename[0] = pom[0] = 0;
 sprintf(pom2,"%s\1%s\1All Files\1*.*\1\1",p_popis_masky,p_maska);
 
 assert(strlen(pom2) < 499);
 
 while(pom2[i]) {
   if(pom2[i] == 1)
     pom2[i] = 0;
   i++;
 }
 
 memset(&soubor,0,sizeof(soubor));
 soubor.lStructSize = sizeof(OPENFILENAME);
 soubor.hwndOwner = hwnd;
 soubor.lpstrFilter = pom2;
 soubor.lpstrFile = pom;
 soubor.nMaxFile = 99;
 soubor.lpstrInitialDir = ".";
 soubor.lpstrTitle = p_nadpis_okna;
 //soubor.Flags = OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
 soubor.Flags = OFN_HIDEREADONLY;
 soubor.lpstrDefExt = p_maska;
 soubor.lpstrFileTitle = p_filename;
 soubor.nMaxFileTitle = max_delka;

 r = GetOpenFileName(&soubor);
 if(r)
   get_work_dir(work_dir, 200);
 return(r);
}

int doe_dialog_save(byte *p_filename, byte max_delka, byte *p_nadpis, byte *p_maska, byte *p_popis_masky)
{
 OPENFILENAME soubor;
 byte pom[100],pom2[100];
 int r,i = 0;
 
 p_filename[0] = pom[0] = 0;
 sprintf(pom2,"%s\1%s\1All Files\1*.*\1\1",p_popis_masky,p_maska);
 
 while(pom2[i]) {
   if(pom2[i] == 1)
     pom2[i] = 0;
   i++;
 }

 memset(&soubor,0,sizeof(soubor));
 soubor.lStructSize = sizeof(OPENFILENAME);
 soubor.hwndOwner = hwnd_aplikace;
 soubor.lpstrFilter = pom2;
 soubor.lpstrFile = pom;
 soubor.nMaxFile = 99;
 soubor.lpstrInitialDir = ".";
 soubor.lpstrTitle = p_nadpis;
 soubor.Flags = OFN_HIDEREADONLY;
 soubor.lpstrDefExt = KONCOVKA_MESH;
 soubor.lpstrFileTitle = p_filename;
 soubor.nMaxFileTitle = max_delka;

 r = GetSaveFileName(&soubor);
 if(r)
   get_work_dir(work_dir, 200);
 return(r);
}


BOOL CALLBACK doe_calback_dialog_textury( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {          
          int  k = p_cnf->vybr_kont;
          EndDialog( hDlg, FALSE );
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {      
      byte pom[50];
      int  k = p_cnf->vybr_kont;
//      int  t = p_cnf->vybr_text;
//      sprintf(pom,"Textura cislo %d ",p_cnf->vybr_text);
      SetDlgItemText(hDlg, IDC_STATIC_51, pom);
//      SetWindowText(GetDlgItem(hDlg,IDC_EDIT1),p_cnf->p_kont[k]->textury[t]);
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}


void doe_edit_jmeno_textury(HWND hwnd)
{
 DialogBox( hinst, MAKEINTRESOURCE(IDD_EDIT_JM_TEXT), hwnd,  doe_calback_dialog_textury);
}


BOOL CALLBACK doe_calback_dialog_kontejner( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {                    
          int  k = p_cnf->vybr_kont;
 
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT1),p_cnf->p_kont[k]->jmeno,MAX_JMENO);

          // staticky prvek
          if(IsDlgButtonChecked( hDlg, IDC_RADIO1))
            p_cnf->p_kont[k]->kflag |= KONT_STATIC;
          else
            p_cnf->p_kont[k]->kflag &= ~KONT_STATIC;          

          // herni prvek
          if(IsDlgButtonChecked( hDlg, IDC_RADIO3))
            p_cnf->p_kont[k]->kflag |= KONT_PRVEK;
          else
            p_cnf->p_kont[k]->kflag &= ~KONT_PRVEK;
          
          // bez stinu          
          if(IsDlgButtonChecked( hDlg, IDC_CHECK2))
            p_cnf->p_kont[k]->kflag |= KONT_NO_DRAW_SHADOW;
          else
            p_cnf->p_kont[k]->kflag &= ~KONT_NO_DRAW_SHADOW;


          if(IsDlgButtonChecked( hDlg, IDC_CHECK25))
            p_cnf->p_kont[k]->kflag |= KONT_NO_SELF_SHADOW;
          else
            p_cnf->p_kont[k]->kflag &= ~KONT_NO_SELF_SHADOW;

          // vertex-light
          if(IsDlgButtonChecked( hDlg, IDC_CHECK3))
            p_cnf->p_kont[k]->kflag |= KONT_VERTEX_LIGHT;
          else
            p_cnf->p_kont[k]->kflag &= ~KONT_VERTEX_LIGHT;
          
          // zrcadlo
          if(IsDlgButtonChecked( hDlg, IDC_CHECK1))
            p_cnf->p_kont[k]->kflag |= KONT_ZRCADLO;
          else
            p_cnf->p_kont[k]->kflag &= ~KONT_ZRCADLO;

          // mlha na kontejner
          if(IsDlgButtonChecked( hDlg, IDC_CHECK6))
            p_cnf->p_kont[k]->kflag |= KONT_NO_FOG;
          else
            p_cnf->p_kont[k]->kflag &= ~KONT_NO_FOG;

          // brutal-normal
          if(IsDlgButtonChecked( hDlg, IDC_CHECK7))
            p_cnf->p_kont[k]->kflag |= KONT_BRUTALNORMAL;
          else
            p_cnf->p_kont[k]->kflag &= ~KONT_BRUTALNORMAL;          
                      
          if(IsDlgButtonChecked( hDlg, IDC_CHECK5))
            p_cnf->p_kont[k]->kflag |= KONT_KEYFRAME;
          else
            p_cnf->p_kont[k]->kflag &= ~KONT_KEYFRAME;          
          
          if(IsDlgButtonChecked( hDlg, IDC_CHECK4))
            p_cnf->p_kont[k]->m1flag |= MAT_PRUHLEDNY;
          else
            p_cnf->p_kont[k]->m1flag &= ~MAT_PRUHLEDNY;

          if(IsDlgButtonChecked( hDlg, IDC_CHECK8))
            p_cnf->p_kont[k]->kflag |= KONT_MPMATRIX;
          else
            p_cnf->p_kont[k]->kflag &= ~KONT_MPMATRIX;

          if(IsDlgButtonChecked( hDlg, IDC_CHECK9))
            p_cnf->p_kont[k]->kflag |= KONT_PLG_OFFSET;
          else
            p_cnf->p_kont[k]->kflag &= ~KONT_PLG_OFFSET;

          if(IsDlgButtonChecked( hDlg, IDC_CHECK10))
            p_cnf->p_kont[k]->kflag |= KONT_NORMAL_ANIM;
          else
            p_cnf->p_kont[k]->kflag &= ~KONT_NORMAL_ANIM;

          if(IsDlgButtonChecked( hDlg, IDC_CHECK11))
            p_cnf->p_kont[k]->k2flag |= KONT2_NO_PHONG;
          else
            p_cnf->p_kont[k]->k2flag &= ~KONT2_NO_PHONG;          

          if(IsDlgButtonChecked( hDlg, IDC_CHECK26))
            p_cnf->p_kont[k]->kflag |= KONT_NORMAL_ANIM_POS;
          else
            p_cnf->p_kont[k]->kflag &= ~KONT_NORMAL_ANIM_POS;

          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {      
      byte pom[200];
      int  k = p_cnf->vybr_kont;
      int  ch;
      sprintf(pom,"Prejmenovat kontejner %s",p_cnf->p_kont[k]->jmeno);
      SetDlgItemText(hDlg, IDC_STATIC_51, pom);
            
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT1),p_cnf->p_kont[k]->jmeno);
      
      // Staticky prvek
      ch = (p_cnf->p_kont[k]->kflag&KONT_STATIC) ? IDC_RADIO1 : IDC_RADIO2;
      CheckDlgButton(hDlg, ch, BST_CHECKED);

      // herni prvek
      ch = (p_cnf->p_kont[k]->kflag&KONT_PRVEK) ? IDC_RADIO3 : IDC_RADIO4;
      CheckDlgButton(hDlg, ch, BST_CHECKED);

      // bez stinu
      CheckDlgButton(hDlg, IDC_CHECK2,  p_cnf->p_kont[k]->kflag&KONT_NO_DRAW_SHADOW ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg, IDC_CHECK25, p_cnf->p_kont[k]->kflag&KONT_NO_SELF_SHADOW ? BST_CHECKED : BST_UNCHECKED);

      // vertex-light
      CheckDlgButton(hDlg, IDC_CHECK3, p_cnf->p_kont[k]->kflag&KONT_VERTEX_LIGHT ? BST_CHECKED : BST_UNCHECKED);
      
      // mlha kontejneru
      CheckDlgButton(hDlg, IDC_CHECK6, p_cnf->p_kont[k]->kflag&KONT_NO_FOG ? BST_CHECKED : BST_UNCHECKED);

      // brutal-normal
      CheckDlgButton(hDlg, IDC_CHECK7, p_cnf->p_kont[k]->kflag&KONT_BRUTALNORMAL ? BST_CHECKED : BST_UNCHECKED);
          
      // dynamicky prvek
      CheckDlgButton(hDlg, IDC_CHECK5, p_cnf->p_kont[k]->kflag&KONT_KEYFRAME ? BST_CHECKED : BST_UNCHECKED);

      // zrcadlo
      CheckDlgButton(hDlg, IDC_CHECK1, p_cnf->p_kont[k]->kflag&KONT_ZRCADLO ? BST_CHECKED : BST_UNCHECKED);

      // pruhledny
      CheckDlgButton(hDlg, IDC_CHECK4, p_cnf->p_kont[k]->m1flag&MAT_PRUHLEDNY ? BST_CHECKED : BST_UNCHECKED);

      // MpMatrix
      CheckDlgButton(hDlg, IDC_CHECK8, p_cnf->p_kont[k]->kflag&KONT_MPMATRIX ? BST_CHECKED : BST_UNCHECKED);

      // Polygon-offset
      CheckDlgButton(hDlg, IDC_CHECK9, p_cnf->p_kont[k]->kflag&KONT_PLG_OFFSET ? BST_CHECKED : BST_UNCHECKED);

      // normal anim
      CheckDlgButton(hDlg, IDC_CHECK10, p_cnf->p_kont[k]->kflag&KONT_NORMAL_ANIM ? BST_CHECKED : BST_UNCHECKED);

      // Dist-animace
      CheckDlgButton(hDlg, IDC_CHECK11, p_cnf->p_kont[k]->k2flag&KONT2_NO_PHONG ? BST_CHECKED : BST_UNCHECKED);

      // Pos-scale animace
      CheckDlgButton(hDlg, IDC_CHECK26, p_cnf->p_kont[k]->kflag&KONT_NORMAL_ANIM_POS ? BST_CHECKED : BST_UNCHECKED);

      // Flag kontejneru
      sprintf(pom,"0x%x",p_cnf->p_kont[k]->kflag);
      SetDlgItemText(hDlg, IDC_EDIT9, pom);
      
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_edit_jmeno_kontejneru(HWND hwnd)
{
  int k = p_cnf->vybr_kont;
  n_kont(k);
  DialogBox( hinst, MAKEINTRESOURCE(IDD_EDIT_KONT), hwnd, doe_calback_dialog_kontejner);
}


BOOL CALLBACK doe_calback_kontejner_flagovac( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: 
          {
            int kflag1_set = 0;
            int kflag1_rem = 0;
            
            int kflag2_set = 0;
            int kflag2_rem = 0;
            
            int mflag1_set = 0;
            int mflag1_rem = 0;
            
            /* Nastaveni
            */
            if(IsDlgButtonChecked( hDlg, IDC_CHECK26))
              kflag1_set |= KONT_STATIC;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK5))
              kflag1_set |= KONT_KEYFRAME;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK10))
              kflag1_set |= KONT_NORMAL_ANIM;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK12))
              kflag1_set |= KONT_PRVEK;

            if(IsDlgButtonChecked( hDlg, IDC_CHECK3))
              kflag1_set |= KONT_VERTEX_LIGHT;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK7))
              kflag1_set |= KONT_BRUTALNORMAL;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK9))
              kflag1_set |= KONT_PLG_OFFSET;
            
            if(IsDlgButtonChecked( hDlg, IDC_CHECK2))
              kflag1_set |= KONT_NO_DRAW_SHADOW;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK25))
              kflag1_set |= KONT_NO_SELF_SHADOW;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK6))
              kflag1_set |= KONT_NO_FOG;

            if(IsDlgButtonChecked( hDlg, IDC_CHECK1))
              kflag1_set |= KONT_ZRCADLO;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK4))
              mflag1_set |= MAT_PRUHLEDNY;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK11))
              kflag2_set |= KONT2_NO_PHONG;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK8))
              kflag1_set |= KONT_MPMATRIX;

            /* Smazani
            */
            if(IsDlgButtonChecked( hDlg, IDC_CHECK34))
              kflag1_rem |= KONT_STATIC;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK30))
              kflag1_rem |= KONT_KEYFRAME;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK18))
              kflag1_rem |= KONT_NORMAL_ANIM;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK22))
              kflag1_rem |= KONT_PRVEK;

            if(IsDlgButtonChecked( hDlg, IDC_CHECK14))
              kflag1_rem |= KONT_VERTEX_LIGHT;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK27))
              kflag1_rem |= KONT_BRUTALNORMAL;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK33))
              kflag1_rem |= KONT_PLG_OFFSET;
            
            if(IsDlgButtonChecked( hDlg, IDC_CHECK13))
              kflag1_rem |= KONT_NO_DRAW_SHADOW;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK32))
              kflag1_rem |= KONT_NO_SELF_SHADOW;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK15))
              kflag1_rem |= KONT_NO_FOG;

            if(IsDlgButtonChecked( hDlg, IDC_CHECK31))
              kflag1_rem |= KONT_ZRCADLO;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK16))
              mflag1_rem |= MAT_PRUHLEDNY;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK19))
              kflag2_rem |= KONT2_NO_PHONG;
            if(IsDlgButtonChecked( hDlg, IDC_CHECK17))
              kflag1_rem |= KONT_MPMATRIX;
            
            tmp.i[0] = kflag1_set;
            tmp.i[1] = kflag1_rem;
            
            tmp.i[2] = kflag2_set;
            tmp.i[3] = kflag2_rem;
            
            tmp.i[4] = mflag1_set;
            tmp.i[5] = mflag1_rem;
            
            EndDialog( hDlg, 1);
            break;
          }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_kontejner_flagovac(int *p_flags)
{ 
  int i;  
  if(DialogBox(hinst, MAKEINTRESOURCE(IDD_EDIT_KONT_FLAGS), hwnd_aplikace, doe_calback_kontejner_flagovac)) {
    for(i = 0; i < 6; i++)
      p_flags[i] = tmp.i[i];
    return(TRUE);
  } else {
    return(FALSE);
  }
}

//IDC_EDIT17 - material
//IDC_EDIT18 - jmeno
BOOL CALLBACK doe_calback_jmeno_objektu( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{

  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {
          byte pom[50];
          int  k = p_cnf->vybr_kont;
          int  o = p_cnf->vybr_objekt;
          EDIT_OBJEKT *p_obj = p_cnf->p_kont[k]->p_obj[o];
          
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT1),p_obj->jmeno,MAX_JMENO);
          
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT17),pom,MAX_JMENO);
          p_obj->material = atoi(pom);
          p_obj->m1flag = p_cnf->p_mat[p_obj->material]->flag;
          p_obj->m2flag = p_cnf->p_mat[p_obj->material]->flag2;
      
          p_obj->pivot.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT2);
          p_obj->pivot.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT3);
          p_obj->pivot.z = doe_vyber_kolonku_float(hDlg,IDC_EDIT7);
          
          key_sim_root_vloz_pivoty_do_animace_kont(p_cnf->p_kont[k]);

          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {      
      byte pom[50];
      int  k = p_cnf->vybr_kont;
      int  o = p_cnf->vybr_objekt;
      int  m;
      EDIT_OBJEKT *p_obj = p_cnf->p_kont[k]->p_obj[o];
      
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT1),p_obj->jmeno);
      
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT17),itoa(m = p_obj->material,pom,10));
      if(m && p_cnf->p_mat[m])
        SetWindowText(GetDlgItem(hDlg,IDC_EDIT18),p_cnf->p_mat[m]->jmeno);

      doe_printf(hDlg,IDC_EDIT2,"%f",p_obj->pivot.x);
      doe_printf(hDlg,IDC_EDIT3,"%f",p_obj->pivot.y);
      doe_printf(hDlg,IDC_EDIT7,"%f",p_obj->pivot.z);
     
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_edit_jmeno_objektu(HWND hwnd)
{
  if(p_cnf->p_kont[p_cnf->vybr_kont] && p_cnf->p_kont[p_cnf->vybr_kont]->p_obj[p_cnf->vybr_objekt])
    DialogBox( hinst, MAKEINTRESOURCE(IDD_EDIT_OBJEKT), hwnd,  doe_calback_jmeno_objektu);
  else
    ddw("Objekt %d je prazdny",p_cnf->vybr_objekt);
}

int doe_vyber_texturu_dialog(HWND hwnd, K_EDITOR *p_cnf, byte *p_text)
{
  chdir(p_cnf->tdir.texture_dir[0]);

  if(doe_dialog_load(hwnd, p_text, 50, "Vyber texturu", "DDs\1*.dds\1Tga\1*.tga", "BMP\1*.bmp"))
    return(TRUE);
  else
    return(FALSE);
}


HRESULT doe_init_dialog_objekty(HWND hdlg)
{ 
  EDIT_KONTEJNER *p_kont;
  int  i,k,o,p,j,v;
  char pom[100];
  HWND hCombo2;

  k = p_cnf->vybr_kont;
  o = p_cnf->vybr_objekt;

  if(!(p_kont = p_cnf->p_kont[k]))
    return(S_OK);

  hCombo2 = GetDlgItem( hdlg, IDC_LIST4 );
  SendMessage(hCombo2, LB_RESETCONTENT, 0, 0);
  
  p = p_kont->max_objektu;
  
  for(i = 0, j = 0; i < p; i++) {
    if(o == i) v = j;
    if(p_kont->p_obj[i]) {
      if(p_kont->p_obj[i]->material)
        sprintf(pom,"%.3d %s %s %d %d",i,p_kont->p_obj[i]->jmeno, p_cnf->p_mat[p_kont->p_obj[i]->material]->jmeno ,p_kont->p_obj[i]->facenum,p_kont->p_obj[i]->vertexnum);
      else
        sprintf(pom,"%.3d %s %s %d %d",i,p_kont->p_obj[i]->jmeno, "NIC" ,p_kont->p_obj[i]->facenum,p_kont->p_obj[i]->vertexnum);
      SendMessage(hCombo2, LB_SETITEMDATA, SendMessage( hCombo2, LB_ADDSTRING, 0,(LPARAM)pom),i);
      j++;
    }
  } 
  SendMessage(hCombo2,LB_SETCURSEL,(WPARAM)v,0);
  return S_OK;
}

HRESULT doe_init_dialog_kontejnery(HWND hdlg)
{ 
  int  i,k,j,v;
  char pom[100];
  HWND hCombo1;

  k = p_cnf->vybr_kont;
     
  hCombo1 = GetDlgItem( hdlg, IDC_LIST3 );
  SendMessage(hCombo1, LB_RESETCONTENT, 0, 0);

  for(i = 0,j = 0; i < p_cnf->max_kont; i++) {
    if(i == k) v = j;
    if(p_cnf->p_kont[i]) {
      sprintf(pom,"%.3d %s %d %d",i,p_cnf->p_kont[i]->jmeno,p_cnf->p_kont[i]->objektu,p_cnf->p_kont[i]->facu);   
      SendMessage(hCombo1, LB_SETITEMDATA, SendMessage( hCombo1, LB_ADDSTRING, 0,(LPARAM)pom),i);
      j++;
    }
  } 
  SendMessage(hCombo1,LB_SETCURSEL,(WPARAM)v,0);

  return S_OK;
}

HRESULT doe_init_dialog_text(HWND hdlg)
{
  EDIT_KONTEJNER *p_kont;
  int  k,o;
  char pom[100];
  
  k = p_cnf->vybr_kont;
  o = p_cnf->vybr_objekt;
  p_kont = p_cnf->p_kont[k];

  SetDlgItemText(hdlg, IDC_STATIC_1, "");
  SetDlgItemText(hdlg, IDC_STATIC_2, p_cnf->p_kont[k]->jmeno);
  if(p_kont == NULL || !p_kont->objektu || !p_kont->p_obj[o]) {
    SetDlgItemText(hdlg, IDC_STATIC_3, ""); 
    SetDlgItemText(hdlg, IDC_STATIC_4, "");
    SetDlgItemText(hdlg, IDC_STATIC_5, "");
    SetDlgItemText(hdlg, IDC_STATIC_9, "");
    SetDlgItemText(hdlg, IDC_STATIC_6, ""); 
    SetDlgItemText(hdlg, IDC_STATIC_7, ""); 
    SetDlgItemText(hdlg, IDC_STATIC_8, ""); 
    SetDlgItemText(hdlg, IDC_STATIC_10, ""); 
  } else { 
    SetDlgItemText(hdlg, IDC_STATIC_3, itoa(p_kont->objektu,pom,10)); 
    SetDlgItemText(hdlg, IDC_STATIC_6, itoa(p_kont->p_obj[o]->facenum,pom,10)); 
    SetDlgItemText(hdlg, IDC_STATIC_7, itoa(p_kont->p_obj[o]->vertexnum,pom,10)); 
    SetDlgItemText(hdlg, IDC_STATIC_8, itoa(p_kont->facu/3,pom,10)); 
    SetDlgItemText(hdlg, IDC_STATIC_10, itoa(p_kont->kreslit,pom,10)); 
  }
  return(S_OK);
}

HRESULT doe_init_dialog_textury(HWND hdlg)
{
  int  i;
  HWND hCombo;    
  byte pom[100];

  hCombo = GetDlgItem( hdlg, IDC_LIST5 ); 
  SendMessage(hCombo, LB_RESETCONTENT, 0, 0);
  
  for(i = 0; i < MAX_EDIT_TEXTUR; i++) {
    if(p_cnf->p_text[i].jmeno[0]) {
       sprintf(pom,"%0.3d %s",i,p_cnf->p_text[i].jmeno);
       SendMessage( hCombo, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) pom);
    }
  }   
  SendMessage(hCombo,LB_SETCURSEL,(WPARAM)0,0);

  return S_OK;
}


HRESULT doe_init_dialog_materialy(HWND hdlg, int akt_pol)
{
  int  i;
  HWND hCombo;    
  byte pom[100];

  hCombo = GetDlgItem( hdlg, IDC_LIST7 );
  SendMessage(hCombo, LB_RESETCONTENT, 0, 0);
  
  for(i = 0; i < MAX_EDIT_MATERIALU; i++) {
    if(p_cnf->p_mat[i]) {
       sprintf(pom,"%0.3d %s",i,p_cnf->p_mat[i]->jmeno);
       SendMessage(hCombo, LB_SETITEMDATA, SendMessage( hCombo, LB_ADDSTRING, 0,(LPARAM)pom),i);
    }
  }   
  SendMessage(hCombo,LB_SETCURSEL,(WPARAM)akt_pol,0);

  return S_OK;
}

#define pridej_string_matrix(aa,bb)\
{\
  sprintf(pom,"%.3f",aa);\
  SetDlgItemText(hDlg, bb, pom); \
}\

HRESULT doe_init_dialog_world_matrix(HWND hDlg)
{
  int  k = p_cnf->vybr_kont;
  GLMATRIX *p_mat;
  byte pom[100];
  
  if(!p_cnf->p_kont[k])
    return S_OK;
  
  p_mat = &p_cnf->p_kont[k]->world;
  pridej_string_matrix(p_mat->_11,IDC_STATIC_11);
  pridej_string_matrix(p_mat->_12,IDC_STATIC_12);
  pridej_string_matrix(p_mat->_13,IDC_STATIC_13);
  pridej_string_matrix(p_mat->_14,IDC_STATIC_14);

  pridej_string_matrix(p_mat->_21,IDC_STATIC_21);
  pridej_string_matrix(p_mat->_22,IDC_STATIC_22);
  pridej_string_matrix(p_mat->_23,IDC_STATIC_23);
  pridej_string_matrix(p_mat->_24,IDC_STATIC_24);

  pridej_string_matrix(p_mat->_31,IDC_STATIC_31);
  pridej_string_matrix(p_mat->_32,IDC_STATIC_32);
  pridej_string_matrix(p_mat->_33,IDC_STATIC_33);
  pridej_string_matrix(p_mat->_34,IDC_STATIC_34);

  pridej_string_matrix(p_mat->_41,IDC_STATIC_41);
  pridej_string_matrix(p_mat->_42,IDC_STATIC_42);
  pridej_string_matrix(p_mat->_43,IDC_STATIC_43);
  pridej_string_matrix(p_mat->_44,IDC_STATIC_44);
   
  return S_OK;
}


BOOL CALLBACK doe_calback_dialog_objekty( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
 
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_LIST3:
          if(HIWORD(wParam) == LBN_SELCHANGE || HIWORD(wParam) == LBN_DBLCLK) {
            p_cnf->vybr_kont = SendMessage((HWND)lParam,LB_GETITEMDATA,SendMessage((HWND)lParam,LB_GETCURSEL,0,0),0);
            updatuj_obalku_kontejner(p_cnf->p_kont[p_cnf->vybr_kont]);
            if(p_cnf->p_kont[p_cnf->vybr_kont] && HIWORD(wParam) == LBN_DBLCLK) {
              doe_edit_jmeno_kontejneru(hDlg);
              doe_init_dialog_kontejnery(hDlg);              
            }
            else {
              doe_init_dialog_world_matrix(hDlg);
              doe_init_dialog_materialy(hDlg,0);
              doe_init_dialog_objekty(hDlg);
              doe_init_dialog_textury(hDlg);
              doe_init_dialog_text(hDlg);
            }
            break;
          }
          break;

        case IDC_LIST4:
          if(HIWORD(wParam) == LBN_DBLCLK|| HIWORD(wParam) == LBN_SELCHANGE) {
            p_cnf->vybr_objekt = SendMessage((HWND)lParam,LB_GETITEMDATA,SendMessage((HWND)lParam,LB_GETCURSEL,0,0),0);
            if(HIWORD(wParam) == LBN_DBLCLK) {
              doe_edit_jmeno_objektu(hDlg);
              doe_init_dialog_objekty(hDlg);
            }
            doe_init_dialog_text(hDlg);
            break;            
          }
          break;

        case IDC_LIST5:
          if(HIWORD(wParam) == LBN_DBLCLK || HIWORD(wParam) == LBN_SELCHANGE) {
            p_cnf->vybr_text = SendMessage((HWND)lParam,LB_GETCURSEL,0,0);
            if(HIWORD(wParam) == LBN_DBLCLK) {
              doe_edit_jmeno_textury(hDlg);
              doe_init_dialog_textury(hDlg);
            }
          }
          break;

        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;

        default:
          return FALSE;
      }
      break;
      
    case WM_INITDIALOG: {
      byte pom[200];
      doe_init_dialog_objekty(hDlg);
      doe_init_dialog_kontejnery(hDlg);
      doe_init_dialog_text(hDlg);
      doe_init_dialog_textury(hDlg);
      doe_init_dialog_world_matrix(hDlg);
      doe_init_dialog_materialy(hDlg,0);
      sprintf(pom,"Game editor - %s - Seznam kontejneru a objektu",p_cnf->jmeno_sceny);
      SetWindowText(hDlg,pom);
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_dialog_objekty(void)
{
 DialogBox( hinst, MAKEINTRESOURCE(IDD_SEZNAM), hwnd_aplikace,  doe_calback_dialog_objekty);
}

void doe_prekresli_hlavni_okno(void)
{
  if(p_cnf) {
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }
}

BOOL CALLBACK doe_calback_scale_kontejneru( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {          
          byte pom[21];
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT4),pom,20);
          tmp.f[1] = (float)atof(pom);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT5),pom,20);  				
          tmp.f[2] = (float)atof(pom);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT6),pom,20);  				
          tmp.f[3] = (float)atof(pom);
          EndDialog(hDlg,1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {      
      byte pom[30];
      itoa(tmp.i[0],pom,10);
      SetDlgItemText(hDlg, IDC_EDIT4, pom);
      SetDlgItemText(hDlg, IDC_EDIT5, pom);
      SetDlgItemText(hDlg, IDC_EDIT6, pom);     

      SetWindowText(hDlg, tmp.pom);
      SetDlgItemText(hDlg, IDC_STATIC1, tmp.pom1);
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, 0);
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_dialog_scale(byte *p_nadpis, byte *p_dalsi, float *p_s)
{  
 tmp.i[0] = 1;
 strcpy(tmp.pom,p_nadpis);
 strcpy(tmp.pom1,p_dalsi);
 if(DialogBox( hinst, MAKEINTRESOURCE(IDD_SCALE), hwnd_aplikace,  doe_calback_scale_kontejneru)) {
   *p_s = tmp.f[1];
   return(TRUE);
 }
 return(FALSE);
}


void doe_prikaz_scale_kontejneru_cisla_dialog(void)
{
 int k = p_cnf->vybr_kont;

 n_kont(k);
  
 tmp.i[0] = 1;
 strcpy(tmp.pom,"Scale kontejneru");
 strcpy(tmp.pom1,"Zmena meritka ve 3 osach");
 if(DialogBox( hinst, MAKEINTRESOURCE(IDD_SCALE), hwnd_aplikace,  doe_calback_scale_kontejneru)) {   
   int handle = oe_vybr_kont_reset();
   if(handle) {
     EDIT_KONTEJNER *p_kont = oe_vybr_kont_get(&handle);
     oe_scale_kontejner(p_kont,tmp.f[1],tmp.f[2],tmp.f[3]);
     key_kontejner_sim_reanimuj(p_kont);
   }   
 }
}

void doe_prikaz_scale_objektu_cisla(void)
{
 int k = p_cnf->vybr_kont;

 n_kont(k);

 tmp.i[0] = 1;
 strcpy(tmp.pom,"Scale objektu");
 strcpy(tmp.pom1,"Zmena meritka ve 3 osach");
 if(DialogBox( hinst, MAKEINTRESOURCE(IDD_SCALE), hwnd_aplikace,  doe_calback_scale_kontejneru)) {
   int k = p_cnf->vybr_kont;
   int o = p_cnf->vybr_objekt;
   n_objekt(k,o);
   oe_scale_objekt(p_cnf->p_kont[k]->p_obj[o],tmp.f[1],tmp.f[2],tmp.f[3]);
 }
}

void doe_prikaz_rotace_kontejneru_cislama(K_EDITOR *p_cnf)
{
  int k = p_cnf->vybr_kont;

  n_kont(k);

  tmp.i[0] = 0;
  strcpy(tmp.pom,"Rotace kontejneru");
  strcpy(tmp.pom1,"Uhel v jednotlivych osach");
  if(DialogBox( hinst, MAKEINTRESOURCE(IDD_SCALE), hwnd_aplikace,  doe_calback_scale_kontejneru)) {
    oe_prikaz_rotace_kontejneru_cislama(p_cnf,tmp.f[1],tmp.f[2],tmp.f[3]);
  }
}

void doe_prikaz_rotace_objektu_cislama(K_EDITOR *p_cnf)
{
  int k = p_cnf->vybr_kont;
  
  n_kont(k);

  tmp.i[0] = 0;
  strcpy(tmp.pom,"Rotace objektu");
  strcpy(tmp.pom1,"Uhel v jednotlivych osach");
  if(DialogBox( hinst, MAKEINTRESOURCE(IDD_SCALE), hwnd_aplikace,  doe_calback_scale_kontejneru)) {
    oe_prikaz_rotace_objektu_cislama(p_cnf, tmp.f[1],tmp.f[2],tmp.f[3]);
  }
}

HRESULT doe_init_hlavni_menu(HWND hDlg)
{
 HICON hIcon = LoadIcon( hinst, MAKEINTRESOURCE(IDI_ICON3));
 PostMessage( hDlg, WM_SETICON, ICON_SMALL, (LPARAM) hIcon);
 return(S_OK);
}

BOOL CALLBACK doe_calback_hlavni_menu( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      doe_proved_prikaz_menu(LOWORD(wParam),FALSE,TRUE);
      switch(LOWORD(wParam)) {        
        case IDOK:
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
      }
      break;    
    case WM_INITDIALOG:
      doe_init_hlavni_menu(hDlg);
      break;

    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, 0);
      break; 
    default:
      doe_prekresli_hlavni_okno();
      return FALSE;
  }
  return TRUE;
}

void doe_hlavni_menu(void)
{
 DialogBox( hinst, MAKEINTRESOURCE(IDD_MENU), hwnd_aplikace,  doe_calback_hlavni_menu);
}

BOOL CALLBACK doe_calback_dialog_edit_vertex( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        
        case IDC_BUTTON1:
        case IDOK: {
          byte pom[30];
          memset(tmp.pom,1,sizeof(tmp.pom[0])*8);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT1),pom,20);
          tmp.f[0] = strlen(pom) ? (float)atof(pom) : (tmp.pom[0] = 0);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT8),pom,20);
          tmp.f[1] = strlen(pom) ? (float)atof(pom) : (tmp.pom[1] = 0);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT9),pom,20);
          tmp.f[2] = strlen(pom) ? (float)atof(pom) : (tmp.pom[2] = 0);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT10),pom,20);
          tmp.f[3] = strlen(pom) ? (float)atof(pom) : (tmp.pom[3] = 0);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT11),pom,20);
          tmp.f[4] = strlen(pom) ? (float)atof(pom) : (tmp.pom[4] = 0);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT12),pom,20);
          tmp.f[5] = strlen(pom) ? (float)atof(pom) : (tmp.pom[5] = 0);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT13),pom,20);
          tmp.f[6] = strlen(pom) ? (float)atof(pom) : (tmp.pom[6] = 0);
             
          if(IsDlgButtonChecked(hDlg,IDC_RADIO1)) {
            if(IsDlgButtonChecked(hDlg,IDC_RADIO4)) {
              oe_prikaz_editace_group(p_cnf, tmp.f, tmp.pom);
            } else {
              oe_prikaz_editace_vertex(p_cnf, tmp.f, tmp.pom);
            }            
          } else if(IsDlgButtonChecked(hDlg,IDC_RADIO2)) {
            if(IsDlgButtonChecked(hDlg,IDC_RADIO4))
              oe_prikaz_nasobeni_group(p_cnf, tmp.f, tmp.pom);
            else
              oe_prikaz_nasobeni_vertex(p_cnf, tmp.f, tmp.pom);
          } else {
            if(IsDlgButtonChecked(hDlg,IDC_RADIO4))
              oe_prikaz_add_group(p_cnf, tmp.f, tmp.pom);
            else
              oe_prikaz_add_vertex(p_cnf, tmp.f, tmp.pom);
          }
          
          pom[0] = 0;
          SetWindowText(GetDlgItem(hDlg,IDC_EDIT1),pom);
          SetWindowText(GetDlgItem(hDlg,IDC_EDIT8),pom);
          SetWindowText(GetDlgItem(hDlg,IDC_EDIT9),pom);
          SetWindowText(GetDlgItem(hDlg,IDC_EDIT10),pom);
          SetWindowText(GetDlgItem(hDlg,IDC_EDIT11),pom);
          SetWindowText(GetDlgItem(hDlg,IDC_EDIT12),pom);
          SetWindowText(GetDlgItem(hDlg,IDC_EDIT13),pom);

          if(LOWORD(wParam) == IDOK)
            EndDialog( hDlg, 1);

          break;
        }         
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      CheckDlgButton(hDlg, IDC_RADIO1, TRUE);
      CheckDlgButton(hDlg, IDC_RADIO4, TRUE);
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, 0);
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_dialog_edit_vertex(void)
{ 
  DialogBox( hinst, MAKEINTRESOURCE(IDD_EDIT_XYZ), hwnd_aplikace,  doe_calback_dialog_edit_vertex);
}


BOOL CALLBACK doe_calback_vyroba_zeme_textura( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON1: // prochazet texturu
          break;
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {          
          byte pom[100];
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT2),pom,19);
          tmp.f[0] = (float)atof(pom);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT3),pom,19);
          tmp.f[1] = (float)atof(pom);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT1),pom,19);
          tmp.i[0] = atoi(pom);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT4),pom,190);
          tmp.i[1] = atoi(pom);
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_vyroba_zeme_textura(int cislo)
{
  if(DialogBox( hinst, MAKEINTRESOURCE(IDD_EDIT_TEXT), hwnd_aplikace,  doe_calback_vyroba_zeme_textura)) {
    int k = p_cnf->vybr_kont;
    int o = p_cnf->vybr_objekt;
    texturuj_zemi(p_cnf->p_kont[k]->p_obj[o], tmp.i[5], tmp.i[6], cislo, tmp.i[0], tmp.i[1], tmp.f[0], tmp.f[1]);
    ddw("Potahuji zemi texturou c. %d",tmp.i[1]);
  }
}

BOOL CALLBACK doe_calback_vyroba_zeme( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON1: // textura 1
          doe_vyroba_zeme_textura(0);
          break;
        case IDC_BUTTON2: // textura 2
          doe_vyroba_zeme_textura(1);
          break;
        case IDC_BUTTON3: // textura 3
          doe_vyroba_zeme_textura(2);
          break;
        case IDC_BUTTON4: { // vyroba zeme
          byte pom1[11],pom2[11],pom3[11];
          int  o,k = p_cnf->vybr_kont;
          
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT1),pom1,10);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT2),pom2,10);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT3),pom3,10);
          

          //if(!p_cnf->p_kont[k]) {          
            k = lo_najdi_volny_kontejner(p_cnf->p_kont,p_cnf->max_kont);
            p_cnf->p_kont[k] = vyrob_kontejner();
            strcpy(p_cnf->p_kont[k]->jmeno,"voda.b2m");
          //}

          if(oe_vyber_volny_objekt(p_cnf) == K_CHYBA)
            break;
          o = p_cnf->vybr_objekt;

          p_cnf->p_kont[k]->p_obj[o] = vyrob_zemi((tmp.i[5] = atoi(pom1)), (tmp.i[6] = atoi(pom2)), (float)atof(pom3));
          strcpy(p_cnf->p_kont[k]->p_obj[o]->jmeno,"Voda");

          EnableWindow(GetDlgItem(hDlg,IDC_BUTTON1),TRUE);
          EnableWindow(GetDlgItem(hDlg,IDC_BUTTON2),TRUE);
          EnableWindow(GetDlgItem(hDlg,IDC_BUTTON3),TRUE);
          break;
        }
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {          
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG:
      if(tmp.i[2]) {
        SetWindowText(hDlg,"Vyrobeni zeme");        
        EnableWindow(GetDlgItem(hDlg,IDC_BUTTON4),TRUE);
      }
      else {
        SetWindowText(hDlg,"Uprava zeme");        
        EnableWindow(GetDlgItem(hDlg,IDC_BUTTON1),TRUE);
        EnableWindow(GetDlgItem(hDlg,IDC_BUTTON2),TRUE);
        EnableWindow(GetDlgItem(hDlg,IDC_BUTTON3),TRUE);
        EnableWindow(GetDlgItem(hDlg,IDC_EDIT1),FALSE);
        EnableWindow(GetDlgItem(hDlg,IDC_EDIT2),FALSE);
        EnableWindow(GetDlgItem(hDlg,IDC_EDIT3),FALSE);
      }
      break;
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_vyroba_zeme(void)
{
 tmp.i[2] = 1;
 DialogBox( hinst, MAKEINTRESOURCE(IDD_VYROBA_ZEME), hwnd_aplikace,  doe_calback_vyroba_zeme);
}

void doe_uprava_zeme(void)
{
 int o = p_cnf->vybr_objekt;
 int k = p_cnf->vybr_kont;

 n_objekt(k,o);
 
 tmp.i[5] = p_cnf->p_kont[k]->p_obj[o]->x_rect;
 tmp.i[6] = p_cnf->p_kont[k]->p_obj[o]->y_rect;
 tmp.i[2] = 0;
 DialogBox( hinst, MAKEINTRESOURCE(IDD_VYROBA_ZEME), hwnd_aplikace,  doe_calback_vyroba_zeme);
}


BOOL CALLBACK doe_calback_novy_kontejner( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {          
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT1),tmp.pom,MAX_JMENO);
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_novy_kontejner(void)
{
  int k;
  if(DialogBox( hinst, MAKEINTRESOURCE(IDD_NOVY_KONT), hwnd_aplikace, doe_calback_novy_kontejner)) {
    if(oe_vyber_volny_kontejner(p_cnf) == K_CHYBA)
      return;
    k = p_cnf->vybr_kont;
    if((p_cnf->p_kont[k] = vyrob_kontejner()) == NULL)
      ddw("Chyba vyroby kontejneru!");
    strcpy(p_cnf->p_kont[k]->jmeno,tmp.pom);
  }
}

  
  //message of the day:
  

  //komat je strasna LAMA!!!!
  
  
  //end of massage of the day...
  
  
  // svetla



BOOL CALLBACK doe_calback_prikaz_rozrez_zemi( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {          
          byte pom[100];
          GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
          tmp.i[0] = atoi(pom);
          GetDlgItemText(hDlg,IDC_EDIT8,pom,99);
          tmp.i[1] = atoi(pom);
          tmp.i[2] = IsDlgButtonChecked( hDlg, IDC_CHECK1); // pretexturovat         
          tmp.i[3] = IsDlgButtonChecked( hDlg, IDC_CHECK2); // pretexturovat
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      SetDlgItemText(hDlg, IDC_EDIT1, "1");
      SetDlgItemText(hDlg, IDC_EDIT8, "1");
      CheckDlgButton( hDlg, IDC_CHECK1, 1);     
      CheckDlgButton( hDlg, IDC_CHECK2, 1);     
      break;                    
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_prikaz_rozrez_zemi(void)
{
  int ok = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;
  EDIT_OBJEKT *p_obj;

  n_objekt(ok,o);

  p_obj = p_cnf->p_kont[ok]->p_obj[o];
  if(!p_obj->x_rect || !p_obj->y_rect) {
    ddw("Dopln pocet ctvercu zeme (klavesa W a 2x kliknout na objekt zeme)");
    return;
  }

  if(DialogBox( hinst, MAKEINTRESOURCE(IDD_ROZREZ_ZEMI), hwnd_aplikace, doe_calback_prikaz_rozrez_zemi)) {

    oe_prikaz_all_ungroup_vertex(p_cnf);
/*
    if(tmp.i[3]) {// zrusit puvodni zemi
      rozrez_zemi(p_obj,p_cnf->p_kont[ok],o,tmp.i[0], tmp.i[1], tmp.i[2]);
    }
    else {
      k = oe_najdi_volny_kontejner(p_cnf->p_kont,p_cnf->max_kont);
      rozrez_zemi(p_obj,p_cnf->p_kont[k],FALSE,tmp.i[0], tmp.i[1], tmp.i[2]);
      strcpy(p_cnf->p_kont[k]->jmeno,"no_name.out");
    }
*/
  }
}


int doe_prikaz_vyber_barvu(K_EDITOR *p_cnf, HWND hwnd, dword *p_rgb)
{
  CHOOSECOLOR cc;
  
  memset(&cc,0x0,sizeof(cc));

  cc.lStructSize = sizeof(cc);
  cc.hwndOwner = hwnd;
  cc.Flags = CC_FULLOPEN|CC_RGBINIT;
  cc.lpCustColors = p_cnf->user_color;
  
  cc.rgbResult = *p_rgb;

  if(ChooseColor(&cc)) { // prehod barvy
    *p_rgb = cc.rgbResult;
    return(TRUE);
  }
  else {
    return(FALSE);
  }
}

int doe_prikaz_vyber_barvu_float(K_EDITOR *p_cnf, HWND hwnd, float *p_rgb)
{
  CHOOSECOLOR cc;
  
  memset(&cc,0x0,sizeof(cc));

  cc.lStructSize = sizeof(cc);
  cc.hwndOwner = hwnd;
  cc.Flags = CC_FULLOPEN|CC_RGBINIT;
  cc.lpCustColors = p_cnf->user_color;
  
  cc.rgbResult = float_rgb(p_rgb);  

  if(ChooseColor(&cc)) { // prehod barvy
    rgb_float(cc.rgbResult,p_rgb);
    return(TRUE);
  }
  else {
    return(FALSE);
  }
}

#define cti_polozku_float(a,b)\
{\
  GetWindowText(GetDlgItem(hDlg,a),pom,99);\
  b = (float)atof(pom);\
}\

#define cti_polozku_int(a,b)\
{\
  GetWindowText(GetDlgItem(hDlg,a),pom,99);\
  b = atoi(pom);\
}\

#define vyber(prvek,min,max)\
{                           \
  if(prvek < min)           \
    min = prvek;            \
  else if(prvek > max)      \
    max = prvek;            \
}                           \



BOOL CALLBACK doe_callback_prikaz_zrcadleni_objektu( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {                 
          tmp.i[0] = IsDlgButtonChecked(hDlg,IDC_CHECK1);
          tmp.i[1] = IsDlgButtonChecked(hDlg,IDC_CHECK2);
          tmp.i[2] = IsDlgButtonChecked(hDlg,IDC_CHECK3);
          tmp.i[3] = IsDlgButtonChecked(hDlg,IDC_CHECK4);
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      CheckDlgButton(hDlg,IDC_CHECK4,1);
      SetWindowText(hDlg,tmp.pom);
      SetDlgItemText(hDlg, IDC_CHECK4, tmp.pom1);
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_prikaz_zrcadleni_objektu(void)
{
  EDIT_OBJEKT *p_obj;
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;

  n_objekt(k,o);


  strcpy(tmp.pom,"Zrcadleni objektu");
  strcpy(tmp.pom1,"Prepisovat objekty");

  if(DialogBox(hinst, MAKEINTRESOURCE(IDD_ZRCADLENI), hwnd_aplikace, doe_callback_prikaz_zrcadleni_objektu)) {
    p_obj = mirror_objekt(p_cnf->p_kont[k]->p_obj[o], TRUE, tmp.i[0], tmp.i[1], tmp.i[2]);
    if(tmp.i[3]) { // prepsat
      zrus_objekt(&p_cnf->p_kont[k]->p_obj[o]);
      p_cnf->p_kont[k]->p_obj[o] = p_obj;
    } else { // pridat kontejner
      if((o = oe_najdi_volny_objekt(p_cnf)) == K_CHYBA)
        ddw("Neni volny objekt !");
      else {
        p_cnf->p_kont[k]->p_obj[o] = p_obj;
        strcpy(p_cnf->p_kont[k]->p_obj[o]->jmeno,"mirror");
      }
    }
  }
}

void doe_prikaz_zrcadleni_kontejneru(void)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT    *p_obj;
  int k = p_cnf->vybr_kont,o;

  n_kont(k);

  p_kont = p_cnf->p_kont[k];


  strcpy(tmp.pom,"Zrcadleni kontejneru");
  strcpy(tmp.pom1,"Prepisat kontejner");
  
  if(DialogBox(hinst, MAKEINTRESOURCE(IDD_ZRCADLENI), hwnd_aplikace, doe_callback_prikaz_zrcadleni_objektu)) {
    if(tmp.i[3]) {
      oe_prikaz_kopiruj_kontejner(p_cnf);
      p_kont = p_cnf->p_kont[p_cnf->vybr_kont];
    }
    for(o = 0; o < p_kont->max_objektu; o++) {
      if(!p_kont->p_obj[o])
        continue;
      p_obj = mirror_objekt(p_kont->p_obj[o], FALSE, tmp.i[0], tmp.i[1], tmp.i[2]);
      zrus_objekt(&p_kont->p_obj[o]);
      p_kont->p_obj[o] = p_obj;
    }
  }
}


BOOL CALLBACK doe_callback_rotace( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {                 
          tmp.i[0] = IsDlgButtonChecked(hDlg,IDC_CHECK1);
          tmp.i[1] = IsDlgButtonChecked(hDlg,IDC_CHECK2);
          tmp.i[2] = IsDlgButtonChecked(hDlg,IDC_CHECK3);
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      SetWindowText(hDlg,tmp.pom);
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_prikaz_rotace_osove_objektu(void)
{
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;

  n_objekt(k,o);
  oe_prikaz_rotace_objektu_osove(p_cnf);
}

void doe_prikaz_rotace_osove_kontejneru(void)
{
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;

  n_objekt(k,o);

  strcpy(tmp.pom,"Rotace kontejneru");
 
  if(DialogBox(hinst, MAKEINTRESOURCE(IDD_ROTACE), hwnd_aplikace, doe_callback_rotace)) {
    system_kurzor = 0;
    oe_prikaz_rotace_kontejneru_osove(p_cnf, tmp.i[0], tmp.i[1], tmp.i[2]);
  }
}


BOOL CALLBACK doe_callback_singlepass( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {                 
          HWND hCombo1 = GetDlgItem( hDlg, IDC_COMBO1);
          HWND hCombo2 = GetDlgItem( hDlg, IDC_COMBO2);
          HWND hCombo3 = GetDlgItem( hDlg, IDC_COMBO4);
          MULTITEXT_FLAG *p_tblok = tmp.p_pom;
          
          p_tblok->mod = SendMessage(hCombo1,CB_GETITEMDATA,SendMessage(hCombo1,CB_GETCURSEL,0,0),0);
          p_tblok->param = SendMessage(hCombo2,CB_GETITEMDATA,SendMessage(hCombo2,CB_GETCURSEL,0,0),0);
          p_tblok->cil = SendMessage(hCombo3,CB_GETITEMDATA,SendMessage(hCombo3,CB_GETCURSEL,0,0),0);

          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    

    case WM_INITDIALOG: {
      HWND hCombo1 = GetDlgItem( hDlg, IDC_COMBO1);
      HWND hCombo2 = GetDlgItem( hDlg, IDC_COMBO2);
      HWND hCombo3 = GetDlgItem( hDlg, IDC_COMBO4);
      MULTITEXT_FLAG *p_tblok = tmp.p_pom;
      int  i, par = 0, arg = 0, tar = 0;

      for(i = 0; i < TARGET_PARAM; i++) {
        SendMessage( hCombo3, CB_SETITEMDATA, SendMessage( hCombo3, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) text_target_text[i]), text_target_val[i]);
        if(p_tblok->cil == text_target_val[i])
          tar = i;
      }
      for(i = 0; i < TEXT_PARAM; i++) {
        SendMessage( hCombo1, CB_SETITEMDATA, SendMessage( hCombo1, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) text_pname_text[i]), text_pname_val[i]);
        if(p_tblok->mod == text_pname_val[i])
          par = i;
      }
      for(i = 0; i < TEXT_ARGUMETU; i++) {
        SendMessage( hCombo2, CB_SETITEMDATA, SendMessage( hCombo2, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) text_argument_text[i]), text_argument_val[i]);
        if(p_tblok->param == text_argument_val[i])
          arg = i;
      }

	    SendMessage( hCombo1, CB_SETCURSEL, par, 0 );
	    SendMessage( hCombo2, CB_SETCURSEL, arg, 0 );
      SendMessage( hCombo3, CB_SETCURSEL, tar, 0 );
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_dialog_singlepass(HWND hwnd, MULTITEXT_FLAG *p_tblok)
{
  POM_PROMENE zal = tmp;
  int ret;
  tmp.p_pom = p_tblok;
  
  ret = DialogBox(hinst, MAKEINTRESOURCE(IDD_SINGLE_PASS), hwnd, doe_callback_singlepass);
  
  tmp = zal;
  
  return(ret);
}

BOOL CALLBACK doe_callback_multipass( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {
          GLenum *p_src = (GLenum *)tmp.p_pom;
          GLenum *p_dest = (GLenum *)tmp.p_pom1;
          HWND hCombo1 = GetDlgItem( hDlg, IDC_COMBO6);
          HWND hCombo2 = GetDlgItem( hDlg, IDC_COMBO7);
          *p_src = SendMessage( hCombo1, CB_GETITEMDATA, SendMessage( hCombo1, CB_GETCURSEL, 0, 0 ),0);
          *p_dest = SendMessage( hCombo2, CB_GETITEMDATA, SendMessage( hCombo2, CB_GETCURSEL, 0, 0 ),0);
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    

    case WM_INITDIALOG: {
      HWND hCombo1 = GetDlgItem( hDlg, IDC_COMBO6);
      HWND hCombo2 = GetDlgItem( hDlg, IDC_COMBO7);
      GLenum *p_src = (GLenum *)tmp.p_pom;
      GLenum *p_dest = (GLenum *)tmp.p_pom1;
      int  i, sr = 1, ds = 0;

      for(i = 0; i < BLEND_FAKTORU_SRC; i++) {        
        SendMessage( hCombo1, CB_SETITEMDATA, SendMessage( hCombo1, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) blend_src_text[i]),blend_src_val[i]);
        if(*p_src == blend_src_val[i])
          sr = i;
      }       
      for(i = 0; i < BLEND_FAKTORU_DEST; i++) {
        SendMessage( hCombo2, CB_SETITEMDATA, SendMessage( hCombo2, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) blend_dest_text[i]),blend_dest_val[i]);
        if(*p_dest == blend_dest_val[i])
          ds = i;
      }       
	    
      SendMessage( hCombo1, CB_SETCURSEL, sr, 0 );
	    SendMessage( hCombo2, CB_SETCURSEL, ds, 0 );
      
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_dialog_multipass(HWND hwnd, GLenum *p_src, GLenum *p_dest)
{
  POM_PROMENE zal = tmp;
  int stav;
  
  tmp.p_pom = p_src;
  tmp.p_pom1 = p_dest;
  
  stav = DialogBox(hinst, MAKEINTRESOURCE(IDD_MULTI_PASS), hwnd, doe_callback_multipass);
  
  tmp = zal;
  
  return(stav);
}

BOOL CALLBACK doe_callback_zrcadleni_textury( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {                 
          int k = p_cnf->vybr_kont;
          int o = p_cnf->vybr_objekt;
          int p1,p2;
          
          if(!p_cnf->p_kont[k] || !p_cnf->p_kont[k]->p_obj[o])
            return(FALSE);

          p1 = IsDlgButtonChecked(hDlg,IDC_CHECK2);
          p2 = IsDlgButtonChecked(hDlg,IDC_CHECK3);

          mirror_text(p_cnf->p_kont[k]->p_obj[o], 0, p1, p2);

          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    

    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}
int doe_prikaz_zrcadleni_textury(HWND hwnd)
{
  return(DialogBox(hinst, MAKEINTRESOURCE(IDD_ZDCADLENI_TEXTURY), hwnd, doe_callback_zrcadleni_textury));
}

BOOL CALLBACK doe_callback_rotace_textury( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {                 
          int k = p_cnf->vybr_kont;
          int o = p_cnf->vybr_objekt;
          int uhel;
          
          if(!p_cnf->p_kont[k] || !p_cnf->p_kont[k]->p_obj[o])
            return(FALSE);
          
          if(IsDlgButtonChecked(hDlg,IDC_CHECK1))
            uhel = 1;
          else if(IsDlgButtonChecked(hDlg,IDC_CHECK2))
            uhel = 2;
          else if(IsDlgButtonChecked(hDlg,IDC_CHECK3))
            uhel = 3;
          else
            uhel = 0;
                    
          rotuj_text(p_cnf->p_kont[k]->p_obj[o],0,uhel);
          
          tmp.i[9] = uhel;

          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    

    case WM_INITDIALOG:
      if(tmp.i[9] == 1)
        CheckDlgButton(hDlg,IDC_CHECK1,TRUE);
      else if(tmp.i[9] == 2)
        CheckDlgButton(hDlg,IDC_CHECK2,TRUE);
      else if(tmp.i[9] == 3)
        CheckDlgButton(hDlg,IDC_CHECK3,TRUE);
      break;
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_prikaz_rotace_textury(HWND hwnd)
{
  return(DialogBox(hinst, MAKEINTRESOURCE(IDD_ROTACE_TEXTURY), hwnd, doe_callback_rotace_textury));
}

BOOL CALLBACK doe_callback_vloz_krychli( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {                 
          int k = lo_najdi_volny_kontejner(p_cnf->p_kont,p_cnf->max_kont);
          BOD p = {0,0,0};
          float x,y,z;
          byte pom[100];
          
          if(k == K_CHYBA) {
            ddw("Spatny kontejner");
            EndDialog( hDlg, 1);
            return(FALSE);
          }
          
          cti_polozku_float(IDC_EDIT4,x);
          cti_polozku_float(IDC_EDIT5,y);
          cti_polozku_float(IDC_EDIT6,z);
                    
          p_cnf->p_kont[k] = vyrob_kontejner();
          p_cnf->p_kont[k]->p_obj[0] = vyrob_krychli(&p,x,y,z,0xffffffff);
          strcpy(p_cnf->p_kont[k]->jmeno,"krychle");
          updatuj_kontejner_statistika(p_cnf->p_kont[k],TRUE);

          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    

    case WM_INITDIALOG: {
      byte pom[30];
      itoa(tmp.i[0],pom,10);
      SetDlgItemText(hDlg, IDC_EDIT4, pom);
      SetDlgItemText(hDlg, IDC_EDIT5, pom);
      SetDlgItemText(hDlg, IDC_EDIT6, pom);
      SetWindowText(hDlg, tmp.pom);
      SetDlgItemText(hDlg, IDC_STATIC1, tmp.pom1);
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_prikaz_vyrob_krychli(K_EDITOR *p_cnf)
{

  tmp.i[0] = 1;
  strcpy(tmp.pom,"Vlozeni krychle");
  strcpy(tmp.pom1,"Velikost objektu:");
  return(DialogBox(hinst, MAKEINTRESOURCE(IDD_SCALE), hwnd_aplikace, doe_callback_vloz_krychli));
}

BOOL CALLBACK doe_callback_vloz_krychli_rozloz( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {                 
          int k = lo_najdi_volny_kontejner(p_cnf->p_kont,p_cnf->max_kont);
          BOD p = {0,0,0};
          float x,y,z;
          byte pom[100];
          
          if(k == K_CHYBA) {
            ddw("Spatny kontejner");
            EndDialog( hDlg, 1);
            return(FALSE);
          }
          
          cti_polozku_float(IDC_EDIT4,x);
          cti_polozku_float(IDC_EDIT5,y);
          cti_polozku_float(IDC_EDIT6,z);

          p.x = 0;
          p.z = 0;
          p.y = y/2;

          p_cnf->p_kont[k] = vyrob_krychli_rozlozenou(&p,x,y,z);
          strcpy(p_cnf->p_kont[k]->jmeno,"krychle");
          
          p_cnf->vybr_kont_last = p_cnf->vybr_kont;
          p_cnf->vybr_kont = k;
          oe_prikaz_prirad_material_kontejner(p_cnf);

          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    

    case WM_INITDIALOG: {
      byte pom[30];
      itoa(tmp.i[0],pom,10);
      SetDlgItemText(hDlg, IDC_EDIT4, pom);
      SetDlgItemText(hDlg, IDC_EDIT5, pom);
      SetDlgItemText(hDlg, IDC_EDIT6, pom);
      SetWindowText(hDlg, tmp.pom);
      SetDlgItemText(hDlg, IDC_STATIC1, tmp.pom1);
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_prikaz_vyrob_krychli_rozlozenou(K_EDITOR *p_cnf)
{

  tmp.i[0] = 1;
  strcpy(tmp.pom,"Vlozeni krychle");
  strcpy(tmp.pom1,"Velikost objektu:");
  return(DialogBox(hinst, MAKEINTRESOURCE(IDD_SCALE), hwnd_aplikace, doe_callback_vloz_krychli_rozloz));
}

void nahraj_list_textur_to_material(K_EDITOR *p_cnf, byte *p_file)
{
  FILE *f;
  byte file[100],*p_pom;
  int  i,t;

  if((f = fopen(p_file,"r")) == NULL)
    tiskni_chybu(__LINE__,__FILE__,hwnd_aplikace,"Soubor '%s'",p_file);
  
  while((i = lo_najdi_prazdny_material(p_cnf->p_mat,MAX_EDIT_MATERIALU)) != K_CHYBA && fgets(file,MAX_JMENO,f)) {
    if(p_pom = strchr(file,'\n'))
      *p_pom = 0;
    // file je jmeno nahravane textury
    if((t = lo_najdi_texturu(p_cnf->p_text,MAX_EDIT_TEXTUR,file,FALSE)) == K_CHYBA) {
      if((t = lo_najdi_volnou_texturu(p_cnf->p_text,MAX_EDIT_TEXTUR)) == K_CHYBA)
        chyba("Neni volna textura");
      strcpy(p_cnf->p_text[t].jmeno,file);
    }

    p_cnf->p_mat[i] = vyrob_material();
    
    strcpy(p_cnf->p_mat[i]->textfile[0],file);
    p_cnf->p_mat[i]->p_text[0] = p_cnf->p_text+t;
    if(p_pom = strchr(file,'.'))
      *p_pom = 0;
    strcpy(p_cnf->p_mat[i]->jmeno,file);
  }
  fclose(f);  

  oe_reload_textur(p_cnf);
}

void doe_nahraj_list_textur(K_EDITOR *p_cnf)
{
  byte file[100];
  
  chdir(p_cnf->tdir.texture_dir[0]);

  if(!doe_dialog_load(hwnd_aplikace,file, 99, "Vyber seznam textur", "*.txt","List textur"))
    return;
  
  nahraj_list_textur_to_material(p_cnf, file);
}


HRESULT doe_init_dialog_databaze_objekty(HWND hdlg, byte *p_dir, byte *p_maska)
{ 
  struct  _finddata_t data;
  long    handle;
  HWND    hCombo2;
  int     i = 0;
  byte    *p_pom;

  hCombo2 = GetDlgItem( hdlg, IDC_LIST1 );
  SendMessage(hCombo2, LB_RESETCONTENT, 0, 0);
    
  chdir(p_dir);
  
  if((handle = _findfirst(p_maska,&data)) == -1)
    return(S_OK);
  do {   
    if((p_pom = strchr(data.name,'.')))
      *p_pom = 0;
    SendMessage(hCombo2, LB_SETITEMDATA, SendMessage( hCombo2, LB_ADDSTRING, 0,(LPARAM)data.name),i++);
  } while(!_findnext(handle,&data));

  _findclose(handle);
  SendMessage(hCombo2,LB_SETCURSEL,(WPARAM)0,0);

  return S_OK;
}

BOOL CALLBACK doe_callback_vloz_objekt( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_LIST1:
          if(HIWORD(wParam) == LBN_DBLCLK) {
            HWND hCombo2 = GetDlgItem( hDlg, IDC_LIST1);
            SendMessage( hCombo2, LB_GETTEXT, SendMessage(hCombo2,LB_GETCURSEL,(WPARAM)0,0),(LPARAM)tmp.pom);
            EndDialog( hDlg, 1);
            break;
          }
          break;
      case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {
          HWND hCombo2 = GetDlgItem( hDlg, IDC_LIST1);
  				SendMessage( hCombo2, LB_GETTEXT, SendMessage(hCombo2,LB_GETCURSEL,(WPARAM)0,0),(LPARAM)tmp.pom);
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG:
      SetWindowText(hDlg,tmp.pom2);
      doe_init_dialog_databaze_objekty(hDlg,tmp.pom,tmp.pom1);
      break;
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

/*
void doe_prikaz_vyrob_krychli(K_EDITOR *p_cnf)
{
 int k = p_cnf->vybr_kont;
 int o = p_cnf->vybr_objekt;
 BOD p = {0,0,0};

 if(!p_cnf->p_kont[k]) {
   p_cnf->p_kont[k] = vyrob_kontejner();
 }
 o = oe_najdi_volny_objekt(p_cnf);
 if(!o) {
   ddw("Neni volny objekt!");
   return;
 }

 tmp.i[0] = 1;
 
 strcpy(tmp.pom,"Vyroba krychle");
 strcpy(tmp.pom1,"Zadej rozmer ve 3 osach:");
 
 if(DialogBox( hinst, MAKEINTRESOURCE(IDD_SCALE), hwnd_aplikace,  doe_calback_scale_kontejneru)) {
   p_cnf->p_kont[k]->p_obj[o] = vyrob_krychli(&p,tmp.f[1],tmp.f[2],tmp.f[3]);
   strcpy(p_cnf->p_kont[k]->p_obj[o]->jmeno,"Krychle");
   if(!p_cnf->p_kont[k]->p_obj[o])
     ddw("Chyba tvorby !");
   updatuj_kontejner_statistika(p_cnf->p_kont[k]);
 }
}
*/
void doe_prikaz_nahraj_svetelne_textury(K_EDITOR *p_cnf)
{
 /*
  int k = oe_najdi_volny_kontejner(p_cnf);
 
 p_cnf->p_kont[k] = vyrob_kontejner();
 chdir(p_cnf->texture_dir);
 nahraj_list_textur(p_cnf->p_kont[k], "svetla.txt");
 nahraj_textury(p_cnf->p_kont[k], ".", TRUE);
 */
}

BOOL CALLBACK doe_callback_abount( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      byte pom[100];
      sprintf(pom,"%s - %s",__DATE__,__TIME__);
      SetDlgItemText(hDlg,IDC_EDIT1,pom);
      SetDlgItemText(hDlg,IDC_EDIT8,"komat@email.cz");
      SetDlgItemText(hDlg,IDC_STATIC1,VERZE_KOMAT);
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_prikaz_abount(void)
{
  DialogBox(hinst, MAKEINTRESOURCE(IDD_O_PROGRAMU), hwnd_aplikace, doe_callback_abount);
}

void doe_prikaz_help_ovladani(void)
{
  //byte cmd[500];
  //sprintf(cmd,"notepad.exe %s\\release_test.txt",work_dir);
  //system(cmd);
}

void doe_prikaz_help_news(void)
{
  //byte cmd[500];
  //sprintf(cmd,"notepad.exe %s\\release_3d_editor.txt",work_dir);
  //system(cmd);
}

void doe_proved_vybr_prikaz(void)
{
 doe_proved_prikaz_menu(p_cnf->vybr_prikaz,TRUE,TRUE);
}

dword doe_get_vybr_prikaz(void)
{
  return(p_cnf->vybr_prikaz);
}

void doe_vyber_prikaz(void)
{
 p_cnf->fprikaz = 1;
}

#define cekej_stisk_mysi()\
{\
  if(!rychle) {\
    while(stisk_mys())\
      doe_prekresli_hlavni_okno();\
  }\
}\


#define cekej_stisk_mysi_br()        while(stisk_mys()) doe_prekresli_hlavni_okno();


//ID_K_DEBUG_WORDMATRIXAKTKONTEJNERU
static void doe_word_matrix_kont(K_EDITOR *p_cnf)
{
 int k = p_cnf->vybr_kont;
 n_kont(k);
 kprintf(TRUE,"Kontejner %d",k);
 kprint_matrix(&p_cnf->p_kont[k]->world);
}

static void doe_schovej_word_matrix_objektu(K_EDITOR *p_cnf)
{
 int k = p_cnf->vybr_kont;
 int o = p_cnf->vybr_objekt;
 n_objekt(k,o); 
 p_cnf->tmp_matrix = p_cnf->p_kont[k]->p_obj[o]->m;
}

static void doe_vloz_word_matrix_objektu(K_EDITOR *p_cnf)
{
 int k = p_cnf->vybr_kont;
 int o = p_cnf->vybr_objekt;
 n_objekt(k,o); 
 p_cnf->p_kont[k]->p_obj[o]->m = p_cnf->tmp_matrix;
}

static void doe_word_matrix_objektu(K_EDITOR *p_cnf)
{
 int k = p_cnf->vybr_kont;
 int o = p_cnf->vybr_objekt;
 n_objekt(k,o);
 kprintf(TRUE,"Kontejner %d objekt %d",k,o);
 kprint_matrix(&p_cnf->p_kont[k]->p_obj[o]->m);
}

static void doe_mp_matrix_kont(K_EDITOR *p_cnf)
{
 int k = p_cnf->vybr_kont;
 n_kont(k);
 kprintf(TRUE,"Kontejner %d",k);
 kprint_matrix(&p_cnf->p_kont[k]->mpmatrix);
}

int oe_uloz_undo(K_EDITOR *p_cnf)
{  
  byte pom[200];
  if(p_cnf->cnf.undo) {
    sprintf(pom,"und%d.bpr",p_cnf->cnf.undo_num);
    oe_oznac_pouzite_materialy(p_cnf);
    oe_uloz_projekt(p_cnf, pom, p_cnf->dir.data_dir,(int)p_cnf->bnf.p_lev,FALSE);
    sprintf(pom,"und%d.lv6",p_cnf->cnf.undo_num++);
    if(!be_save_level(&p_cnf->bnf, pom, p_cnf->dir.data_dir)) {
      _chdir(p_cnf->dir.data_dir);
      remove(pom);      
    }
    return(TRUE);
  }
  return(FALSE);
}

int oe_proved_undo(K_EDITOR *p_cnf)
{  
  B_KONFIG *p_bnf = &p_cnf->bnf;  // berusci konfig  
  byte      pom[200];
  int       i;

  if(p_cnf->cnf.undo && p_cnf->cnf.undo_num) {    
    oe_smaz_scenu(p_cnf);

    sprintf(pom,"und%d.bpr",--p_cnf->cnf.undo_num);
    oe_nahraj_projekt(p_cnf, pom, p_cnf->dir.data_dir,FALSE,FALSE);
    
    if(p_bnf->p_lev) {
      be_zrus_level(p_bnf);
    }
    sprintf(pom,"und%d.lv6",p_cnf->cnf.undo_num);
    be_load_level(p_bnf, pom, p_cnf->dir.data_dir,FALSE);
        
    for(i = 0; i < p_bnf->y_num; i++)
      be_vyrob_grid_level(p_bnf, i);
    
    p_bnf->cnf.kostka_remake = TRUE;
    return(TRUE);
  }
  return(FALSE);
}

int doe_proved_prikaz_menu(UINT prikaz, int rychle, int ulozit)
{
  if(p_cnf->fprikaz) {
    p_cnf->vybr_prikaz = prikaz;
    p_cnf->fprikaz = 0;
    ddw("Vybiram prikaz cislo %d",p_cnf->vybr_prikaz);
    return(TRUE);
  }

  if(ulozit) {
    if(prikaz != ID_SCENA_ZOPAKUJPOSLEDNIPRIKAZ)
      p_cnf->cnf.posledni_prikaz = prikaz;
    else {
      doe_proved_prikaz_menu(p_cnf->cnf.posledni_prikaz,FALSE,TRUE);
      return(TRUE);
    }
  }

  switch(prikaz) {
    case ID_K_KONFIGURACE_UNDO:
      oe_proved_undo(p_cnf);
      break;
    case ID_PRIKAZ_PRIRAD_MATERIAL_OBJEKT:
      oe_uloz_undo(p_cnf);
      oe_prikaz_prirad_material_objekt(p_cnf);
      break;
    case ID_PRIKAZ_PRIRAD_MATERIAL_KONTEJNER:
      oe_uloz_undo(p_cnf);
      oe_prikaz_prirad_material_kontejner(p_cnf);
      break;
    case ID_PRIKAZ_SET_BARVA_KONTEJNERU:
      oe_uloz_undo(p_cnf);
      oe_prikaz_set_barva_kontejner(p_cnf);      
      break;
    case ID_PRIKAZ_RESET_BARVA_KONTEJNER:
      oe_uloz_undo(p_cnf);
      oe_prikaz_reset_barva_kontejner(p_cnf);
      break;
    case ID_PRIKAZ_GOURAND_MAPING_OBJ:
      oe_uloz_undo(p_cnf);
      oe_prikaz_gourand_maping_obj(p_cnf);
      break;
    case ID_PRIKAZ_SET_BARVA_OBJEKT:
      oe_uloz_undo(p_cnf);
      oe_prikaz_set_barva_objekt(p_cnf);
      break;
    case ID_PRIKAZ_RESET_BARVA_OBJEKT:
      oe_uloz_undo(p_cnf);
      oe_prikaz_reset_barva_objekt(p_cnf);
      break;
    case ID_VERTEX_INFO:
      if(p_cnf->groupnum) {
        int k = p_cnf->p_group[p_cnf->vybr_group].k;
        int o = p_cnf->p_group[p_cnf->vybr_group].o;
        int v = p_cnf->p_group[p_cnf->vybr_group].v;
        doe_vertex_info(hwnd_aplikace, p_cnf->p_kont[k]->p_obj[o]->p_vertex+v, k, o, v);
      }
      break;
    case ID_PRIKAZ_GOURAND_MAPING_BODY:
      oe_uloz_undo(p_cnf);
      oe_prikaz_gourand_maping_body(p_cnf);
      break;
    case ID_PRIKAZ_SET_BARVA_BODY:
      oe_uloz_undo(p_cnf);
      oe_prikaz_set_barva_body(p_cnf);
      break;
    case ID_PRIKAZ_SET_BARVA:
      oe_uloz_undo(p_cnf);
      oe_prikaz_set_barva(p_cnf);
      break;
    case ID_PRIKAZ_RYCHLY_MIRROR_U:
      oe_uloz_undo(p_cnf);
      oe_prikaz_rychly_mirror(p_cnf, 0, 1, 0);
      break;
    case ID_PRIKAZ_RYCHLY_MIRROR_V:
      oe_uloz_undo(p_cnf);
      oe_prikaz_rychly_mirror(p_cnf, 0, 0, 1);
      break;
    case ID_PRIKAZ_RYCHLA_ROTACE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_rychla_rotace(p_cnf,0);
      break;


/*
  Kontejnery - load/save
*/
    case ID_K_KONTEJNER_NAHRAT3DSSOUBOR:
      oe_uloz_undo(p_cnf);
      prikaz(oe_prikaz_nahraj_kontejner(p_cnf,LOAD_KONT_3DS,TRUE));
      break;
    case ID_K_NAHRATKONTEJNER:
      oe_uloz_undo(p_cnf);
      prikaz(oe_prikaz_nahraj_kontejner(p_cnf,LOAD_KONT_BTX,TRUE));
      break;
    case ID_K_ULOZITKONTEJNERJAKO:
      prikaz(oe_prikaz_uloz_kontejner_jako(p_cnf));
      break;
    case ID_K_ULOZITKONTEJNER:
      prikaz(oe_prikaz_uloz_kontejner(p_cnf,TRUE));
      break;
    case ID_K_SMAZATKONTEJNER:
      oe_uloz_undo(p_cnf);      
      oe_prikaz_mazani_kontejneru(p_cnf);
      break;
    case ID_K_KONEC:
      oe_konec(p_cnf);
      break;

    case ID_NASTROJE_MATERIALEDITOR:
      oe_uloz_undo(p_cnf);
      oe_vyber_material(p_cnf);
      prikaz(doe_editor_materialu(p_cnf));
      break;
/*
  Konfigurace
*/
    case ID_KONFIGURACE_KONFIGURACE:
      prikaz(doe_prikaz_konfigurace_table(p_cnf,hwnd_aplikace,0));
      break;
    case ID_K_KONFIGURACE_VYBERPRIKAZ:
      doe_vyber_prikaz();
      break;
    case ID_K_KONFIGURACE_PROVEDVYBRANYPRIKAZ:
      doe_proved_vybr_prikaz();
      break;
//-----
    case ID_K_KONFIGURACE_DRATYTEXTURY:
      oe_prikaz_zmena_textury_draty(p_cnf);
      break;
    case ID_K_KONFIGURACE_VYBEROBJEKTUKEKRSLENI:
      oe_prikaz_selekce_objektu(p_cnf);
      break;
    case ID_K_KONFIGURACE_ZAPNUTIVYPNUTIKONZOLE:
      oe_prikaz_konzole_on_off(p_cnf);
      break;
    case ID_K_KONFIGURACE_SEZNAMYOBJEKTUONOFF:
      oe_prikaz_zmena_konzole_objekty(p_cnf);
      break;
    case ID_K_KONFIGURACE_SMAZANIKONZOLE:
      oe_prikaz_smazani_konzole(p_cnf);
      break;
    case ID_K_KONFIGURACE_POPISYOBJEKTU:
      oe_prikaz_zmena_origin(p_cnf);
      break;
    case ID_K_KONFIGURACE_KRESLITNEKRESLITAKTKONTEJNER:
      oe_prikaz_zmena_kresleni_kontejneru(p_cnf);
      break;
    case ID_K_KONFIGURACE_KRESLITNEKRESLITPOZICEKAMER:
      oe_prikaz_zmena_kresleni_kamer(p_cnf);
      break;
    case ID_K_KONFIGURACE_BACKFACECULMOD:
      oe_prikaz_zmena_cul(p_cnf);
      break;
    case ID_K_KONFIGURACE_NITKOVYKRIZKURZOR:
      oe_prikaz_nitkovy_kriz(p_cnf);
      break;

/*
  Operace se scenou
*/
    case ID_K_CELASCENA_ULOZIT:
      prikaz(oe_prikaz_uloz_projekt(p_cnf));
      break;
    case ID_K_CELASCENA_NAHRAT: // f8
    case ID_SOUBOR_IMPORTOVATOKOLI:
      oe_uloz_undo(p_cnf);
      prikaz(oe_prikaz_nahraj_projekt(p_cnf));
      break;
    case ID_K_CELASCENA_SMAZATVSECHNO:
      oe_uloz_undo(p_cnf);
      oe_prikaz_mazani_sceny(p_cnf);
      break;
    case ID_POHLED_XZ:
      oe_nahod_mod(p_cnf, 0);
      break;
    case ID_POHLED_XY:
      oe_nahod_mod(p_cnf, 1);
      break;
    case ID_POHLED_ZY:
      oe_nahod_mod(p_cnf, 2);
      break;
    case ID_POHLED_GAMEPOHLED:
      oe_nahod_mod(p_cnf, 3);
      break;
    case ID_POHLED_RESETXZ:
      oe_ini_mod(p_cnf, 0, 1);
      //oe_nahod_mod(p_cnf, 0);
      break;
    case ID_POHLED_RESETXY:
      oe_ini_mod(p_cnf, 1, 1);
      //oe_nahod_mod(p_cnf, 1);
      break;
    case ID_POHLED_RESETZY:
      oe_ini_mod(p_cnf, 2 , 1);
      //oe_nahod_mod(p_cnf, 2);
      break;
    case ID_POHLED_RESETGAME:
      oe_ini_mod(p_cnf, 3, 0);
      //oe_nahod_mod(p_cnf, 3);
      break;
    case ID_POHLED_POSUNSCENY:
      cekej_stisk_mysi();
      oe_prikaz_posun_sceny(p_cnf);
      break;
    case ID_POHLED_POSUNVHERNIROVINE:
      cekej_stisk_mysi();
      oe_prikaz_posun_sceny_xz(p_cnf);
      break;
    case ID_POHLED_ROTACESCENY:
      cekej_stisk_mysi();
      oe_prikaz_rotace_sceny(p_cnf,FALSE);
      break;
    case ID_POHLED_ROTACESCENY2:
      cekej_stisk_mysi();
      oe_prikaz_rotace_sceny(p_cnf,TRUE);
      break;
/*
  Operace s kontejnerem
*/
    case ID_K_KONTEJNER_KOPIE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kopiruj_kontejner(p_cnf);
      break;
    case ID_K_KONTEJNER_ZRCADLENI:
      oe_uloz_undo(p_cnf);
      doe_prikaz_zrcadleni_kontejneru();
      break;
    case ID_K_KONTEJNER_ROTACECISLAMA:
      oe_uloz_undo(p_cnf);
      doe_prikaz_rotace_kontejneru_cislama(p_cnf);
      break;
    case ID_K_KONTEJNER_ROTACEOSOVE:
      oe_uloz_undo(p_cnf);
      prikaz(doe_prikaz_rotace_osove_kontejneru());
      break;
    case ID_K_VYROBAZEME:
    case ID_K_VODA_VYROBITVODU:
      oe_uloz_undo(p_cnf);
      prikaz(doe_vyroba_zeme());
      break;
    case ID_K_KONTEJNER_UPRAVITZEMI:    
      oe_uloz_undo(p_cnf);
      prikaz(doe_uprava_zeme());
      break;
    case ID_KONTEJNER_NOVY:
      oe_uloz_undo(p_cnf);
      prikaz(doe_novy_kontejner());
      break;
    case ID_KONTEJNER_CETRUJ:
      oe_uloz_undo(p_cnf);
      oe_prikaz_centruj_kontejner(p_cnf);
      break;
    case ID_K_CELASCENA_SOUHRN:
      oe_uloz_undo(p_cnf);
      prikaz(doe_dialog_objekty());
      break;
    case ID_KONTEJNER_TRANSFORMUJ:
      oe_uloz_undo(p_cnf);
      oe_prikaz_transformace_kontejneru(p_cnf);
      break;
    case ID_KONTEJNER_SLEPMAXIMALNE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_slep_kontejner(p_cnf);
      break;
    case ID_KONTEJNER_SLEPENIDVOUOBJEKTU:
      oe_uloz_undo(p_cnf);
      oe_prikaz_slepeni_objektu(p_cnf);
      break;
    case ID_KONTEJNER_SCALEMYSI:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_scale_kontejneru(p_cnf);
      break;
    case ID_KONTEJNER_SCALECISLAMA:
      oe_uloz_undo(p_cnf);
      prikaz(oe_prikaz_scale_kontejneru_cisla(p_cnf));
      break;
    case ID_KONTEJNER_ROTACE:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_rotace_kontejneru(p_cnf);
      break;
    case ID_K_KONTEJNER_POSUNKOPIE:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      key[K_CTRL] = TRUE;
      oe_prikaz_posun_kontejner(p_cnf);
      break;
    case ID_KONTEJNER_POSUN:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_posun_kontejner(p_cnf);
      break;
    case ID_KONTEJNER_OPTIMALIZACE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_smaz_divoke_vertexy_kontejneru(p_cnf);
      break;
    case ID_K_KONTEJNER_VYMAZATOBJEKTY:
      oe_uloz_undo(p_cnf);
      oe_prikaz_vymazat_kontejner(p_cnf);
      break;
    case ID_K_KONTEJNER_NORMALOVEVEKTORY:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kont_norm_vect(p_cnf);
      break;
    case ID_K_KONTEJNER_ZMENAKRESLENI:
      oe_uloz_undo(p_cnf);
      oe_prikaz_zmena_kresleni_kontejneru(p_cnf);
      break;
/*
  operace s objektem
*/
    case ID_K_OBJEKT_ROTACE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_rotace_objektu(p_cnf);
      break;
    case ID_K_OBJEKT_ROTACECISLAMA:
      oe_uloz_undo(p_cnf);
      prikaz(doe_prikaz_rotace_objektu_cislama(p_cnf));
      break;
    case ID_K_OBJEKT_ROTACEOSOVE:
      oe_uloz_undo(p_cnf);
      prikaz(doe_prikaz_rotace_osove_objektu());
      break;
    case ID_OBJEKT_SMAZ:
      oe_uloz_undo(p_cnf);
      oe_prikaz_mazani_objektu(p_cnf);
      break;
    case ID_OBJEKT_POSUN:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_posun_objekt(p_cnf);
      break;
    case ID_K_OBJEKT_POSUNKOPII:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      key[K_CTRL] = TRUE;
      oe_prikaz_posun_objekt(p_cnf);
      break;
    case ID_OBJEKT_OPTIMALIZUJ:
      oe_uloz_undo(p_cnf);
      oe_prikaz_smaz_divoke_vertexy_objektu(p_cnf);
      break;
    case ID_K_OBJEKT_ZRCADLENI:
      oe_uloz_undo(p_cnf);
      prikaz(doe_prikaz_zrcadleni_objektu());
      break;
    case ID_K_OBJEKT_KOPIRUJ:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kopiruj_objekt(p_cnf);
      break;
    case ID_K_OBJEKT_SCALE:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_scale_objektu(p_cnf);
      break;
    case ID_K_OBJEKT_SCALECISLAMA:
      oe_uloz_undo(p_cnf);
      doe_prikaz_scale_objektu_cisla();
      break;
    case ID_K_OBJEKT_ZMENAKRESLENI:
      oe_uloz_undo(p_cnf);
      oe_prikaz_zmena_kresleni_objekt(p_cnf);
      break;
    case ID_K_OBJEKT_ROZDROB:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_rozdrob_objekt(p_cnf,FALSE);
      break;
/*
  Operace s facama
*/
    case ID_K_FACE_SMAZAT:
      oe_uloz_undo(p_cnf);
      oe_prikaz_smaz_face(p_cnf,TRUE);
      break;
/*
   Vyber vertexu
*/
    case ID_K_GROUPBODU_VYBRAT:
      cekej_stisk_mysi();
      oe_prikaz_group_vertex(p_cnf);
      break;
    case ID_K_GROUPBODU_PRIDATDOVYBERU:
      cekej_stisk_mysi();
      oe_prikaz_group_vertex_pridej(p_cnf);
      break;
    case ID_K_VYBERBODU_VYBRATJEDEN:
      oe_prikaz_vyber_vertex(p_cnf);
      break;
    case ID_K_VYBERBODU_PRIDATJEDEN:
      oe_prikaz_pridej_vertex(p_cnf);
      break;
    case ID_K_GROUPBODU_VYBRATPLOSKOU:
      oe_prikaz_vyber_vertex_face(p_cnf);
      break;
    case ID_K_GROUPBODU_PRIDATFACE:
      oe_prikaz_pridej_vertex_face(p_cnf);
      break;
    case ID_K_VYBERBODU_UNGROUPVSECH:
      oe_prikaz_all_ungroup_vertex(p_cnf);
      break;
    case ID_K_VYBERBODU_UNGROUPAKTUALNIHO:
      oe_prikaz_ungroup_aktualni_vertex(p_cnf);
      break;
    case ID_K_VYBERBODU_UNGROUPCTVERCEM:
      cekej_stisk_mysi();
      oe_prikaz_ungroup_group_vertex(p_cnf);
      break;
    case ID_K_VYBERBODU_UNGROUPJEDNOHO:
      oe_prikaz_ungroup_vertex(p_cnf);
      break;
/*
 Operace s group vertexama
*/
    case ID_K_GROUPBODU_EDITACE:
      oe_uloz_undo(p_cnf);
      prikaz(doe_dialog_edit_vertex());
      break;
    case ID_K_GROUPBODU_SMAZANI:
      oe_uloz_undo(p_cnf);
      oe_prikaz_smaz_group_vertex(p_cnf);
      break;
/*
 Operace s jednim vertexem
*/
    case ID_K_BODU_PRIMAEDITACE:
      oe_uloz_undo(p_cnf);
      prikaz(doe_dialog_edit_vertex());
      break;
    case ID_K_BODU_SMAZANI:
      oe_uloz_undo(p_cnf);
      oe_prikaz_smaz_vertex(p_cnf);
      break;
    case ID_K_BODY_ULOZITPRIMKU:
      oe_vyber_primku(p_cnf);
      break;
    case ID_K_BODY_PRIPOJITBODY:
      oe_uloz_undo(p_cnf);
      oe_prikaz_prusecik_cnf(p_cnf);
      break;

/*
  Editace zeme
*/

    case ID_K_VYROBA_ROZREZATZEMI:
      oe_uloz_undo(p_cnf);
      prikaz(doe_prikaz_rozrez_zemi());
      break;

    case ID_K_EDITACEZEME_POSUNVSECHBODU:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_posun_group_vertex(p_cnf);
      break;
    case ID_K_EDITACEZEME_VYSKAVSECHBODU:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_zvedni_group_vertex(p_cnf);
      break;
    case ID_K_EDITACEZEME_POSUNVYBRANEHOBODU:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_posun_vertex(p_cnf);
      break;
    case ID_K_EDITACEZEME_VYSKAVYBRANEHOBODU:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_zvedni_vertex(p_cnf);
      break;
    case ID_K_ZEMEOBJEKTY_VYROBKRYCHLIROZREZANOU:
      oe_uloz_undo(p_cnf);
      prikaz(doe_prikaz_vyrob_krychli_rozlozenou(p_cnf));
      break;
    case ID_K_EDITACEZEME_VYBERBODU:
      cekej_stisk_mysi();
      oe_vyber_nejblizsi_group_vertex(p_cnf);
      break;
/*
  Editace texelu
*/
    case ID_SCENA_NAROVNEJKAMERU:
      oe_prikaz_narovnej_kameru(p_cnf);
      break;
/*
  Textury
*/
    case ID_K_TEXTURY_ROTACE:
      oe_uloz_undo(p_cnf);
      prikaz(doe_prikaz_rotace_textury(hwnd_aplikace));
      break;
    case ID_K_TEXTURY_ZRCADLENI:
      oe_uloz_undo(p_cnf);
      prikaz(doe_prikaz_zrcadleni_textury(hwnd_aplikace));
      break;
    case ID_K_TEXTURY_NAHRATLISTTEXTUR:
      oe_uloz_undo(p_cnf);
      prikaz(doe_nahraj_list_textur(p_cnf));
      break;
/*
  Sesiti
*/
    case ID_K_VSECHNYBODY_SJEDNOT:
      oe_uloz_undo(p_cnf);
      oe_prikaz_sjednot_group_vertex(p_cnf);
      break;
    case ID_K_EDITACEZEME_SJEDNOTDVOJICE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_sesij_group_vertex(p_cnf);
      break;

/*
  Svetla
*/
    case ID_K_SVETLA_ENVIRONMENTMAPPING:
      oe_uloz_undo(p_cnf);
      oe_prikaz_env_mapping(p_cnf);
      break;
    case ID_K_SVETLA_ENVIRONMENTMAPPINGNAVSECHNO:
      oe_uloz_undo(p_cnf);
      oe_prikaz_env_mapping_all(p_cnf);
      break;
    case ID_K_SVETLA_DXSVETLA:
      oe_uloz_undo(p_cnf);
      oe_prikaz_gourand_maping_kont(p_cnf);
      break;
    case ID_K_KONTEJNER_NORMALOVEVEKTORYNAVSECHNO:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kont_norm_vect_all(p_cnf);
      break;
    case ID_K_SVETLA_KONFIGURACEGOURANDA:
      oe_uloz_undo(p_cnf);
      doe_prikaz_konfigurace_table(p_cnf,hwnd_aplikace,3);
      break;
    case ID_K_ZEMEOBJEKTY_VYROBITKRYCHLI:
      oe_uloz_undo(p_cnf);
      doe_prikaz_vyrob_krychli(p_cnf);
      break;
    case ID_PROGRAM_MESHEDITOR: // prepne se do mesh editoru
      doe_prepni_do_mesh_editor();
      break;
    case ID_PROGRAM_GAMEEDITOR: // prepne se do game editoru
      doe_prepni_do_game_editor();
      break;
    case ID_SOUBOR_NAHRATBEZMATERIALU:
      oe_uloz_undo(p_cnf);
      oe_prikaz_nahraj_kontejner(p_cnf, LOAD_KONT_BTX, FALSE);
      break;
    case ID_MESH_KONTEJNER_POSUNOUTNASTRED:
      oe_uloz_undo(p_cnf);
      oe_prikaz_init_matrix_kontejner(p_cnf);
      break;

    case ID_SOUBOR_NOVYLEVEL: // new level
      oe_uloz_undo(p_cnf);
      be_prikaz_novy_level(&p_cnf->bnf);
      break;
    case ID_SOUBOR_NAHRATLEVEL:
      oe_uloz_undo(p_cnf);
      be_prikaz_nahraj_level(p_cnf,FALSE);
      break;
    case ID_SOUBOR_ULOZITLEVEL:      
      be_prikaz_uloz_level(p_cnf);
      break;
    case ID_SOUBOR_ULOZITLEVELJAKO:  
      be_prikaz_uloz_level_jako(p_cnf);
      break;
    case ID_SOUBOR_SMAZATLEVEL:
      oe_uloz_undo(p_cnf);
      be_prikaz_smaz_level(p_cnf,TRUE,TRUE,FALSE);
      break;
    case ID_SOUBOR_SMAZATOKOLILEVELU:
      oe_uloz_undo(p_cnf);
      be_prikaz_smaz_level(p_cnf,FALSE,TRUE,FALSE);
      break;
    case ID_SOUBOR_NAHRATLEVELBEZPOZADI_LV6:
      oe_uloz_undo(p_cnf);
      be_prikaz_nahraj_level(p_cnf,TRUE);
      break;
    case ID_SOUBOR_ULOZITLEVELBEZPOZADI:
      be_prikaz_uloz_level_jako(p_cnf);
      break;
    case ID_SOUBOR_ULOZITLEVEL2:
      be_save_level(&p_cnf->bnf,p_cnf->bnf.jmeno_levelu,p_cnf->dir.level_dir);
      break;

    case ID_HELP_OPROGRAMU:
      doe_prikaz_abount();
      break;
    case ID_HELP_NOVINKYVEDITORU:
      doe_prikaz_help_news();
      break;
    case ID_HELP_NPOVDAPROZOUFALUIVATELE:
      doe_prikaz_help_ovladani();
      break;
    case ID_K_VLASTNOSTIPRVKU:
      be_prikaz_uprav_prvek_levelu(p_cnf);
      break;
    case ID_K_OBJEKT_VLASTNOSTI:
      prikaz(doe_edit_jmeno_objektu(hwnd_aplikace));  
      break;
    case ID_K_KONTEJNER_VLASTNOSTI:
      prikaz(doe_edit_jmeno_kontejneru(hwnd_aplikace));      
      break;
    /*
      Kreslit/nekreslit level
    */
    case ID_SCENA_KRESLITLEVEL:
      doe_toogle_kreslit_level(p_cnf);
      break;
    case ID_SCENA_KRESLITSCENU:
      doe_toogle_kreslit_scenu(p_cnf);
      break;
    case ID_NASTROJE_UNIFIKATORKONTEJNERU:  
      doe_prevod_kontejneru(p_cnf);
      break;
    /*
      Flare
    */
    case ID_K_FLARE_NOVY:
      oe_uloz_undo(p_cnf);
      oe_prikaz_pridej_flare(p_cnf);
      break;
    case ID_K_FLARE_UPRAVIT:
      oe_uloz_undo(p_cnf);
      oe_uprav_flare(p_cnf, p_cnf->flare+p_cnf->flareakt);
      break;
    case ID_K_FLARE_POSUNOUT:
      oe_uloz_undo(p_cnf);
      oe_prikaz_posun_flare(p_cnf);
      break;
    case ID_K_FLARE_POSUNKOPII:
      oe_uloz_undo(p_cnf);
      key[K_CTRL] = TRUE;
      oe_prikaz_posun_flare(p_cnf);
      break;

    /* 
      Svetla
    */
    case ID_K_SVETLA_NOVE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_pridej_staticke_svetlo(p_cnf);
      break;
    case ID_K_SVETLA_UPRAVIT:
      oe_uloz_undo(p_cnf);
      oe_uprav_staticke_svetlo(p_cnf, p_cnf->light+p_cnf->lightakt);
      break;
    case ID_K_SVETLA_POSUNOUT:
      oe_uloz_undo(p_cnf);
      key[K_CTRL] = FALSE;
      cekej_stisk_mysi();
      oe_prikaz_posun_staticke_svetlo(p_cnf);
      break;
    case ID_K_SVETLA_POSUNOUTKOPII:
      oe_uloz_undo(p_cnf);
      key[K_CTRL] = TRUE;
      cekej_stisk_mysi();
      oe_prikaz_posun_staticke_svetlo(p_cnf);
      break;

    case ID_SCENA_STATISTIKA:
      oe_uloz_undo(p_cnf);
      doe_statistika_sceny(p_cnf);
      break;

    case ID_MESH_ZEME_VYROBITOKOLI:
      oe_uloz_undo(p_cnf);
      oe_vyrob_okoli(p_cnf);
      break;
    case ID_K_OKOLI_UPRAVITOKOLI:
      oe_uloz_undo(p_cnf);
      oe_uprav_okoli(p_cnf);
      break;
    case ID_K_ROTACEPRVKU:
      be_prikaz_rotace_prvku(p_cnf);
      break;
    case ID_NASTROJE_POZVEDNUTIKONTEJNERU:
      doe_prevod_kontejneru_nahoru(p_cnf);
      break;
    case ID_K_OBJEKT_VYPIS:
      oe_tiskni_akt_objekt(p_cnf);
      break;
    case ID_K_BODY_VLOZOBJEKTY:
      oe_prikaz_pridej_akt_objekt(p_cnf,FALSE);
      break;
    case ID_K_OBJEKT_PRESUNDOKONTEJNERU:
      oe_uloz_undo(p_cnf);
      oe_prikaz_pridej_akt_objekt(p_cnf,TRUE);
      break;
    case ID_K_SCENA_VYPICHNIOBJEKT:
      oe_prikaz_selekce_objektu(p_cnf);
      break;
    case ID_K_SCENA_VYPICHNIKONTEJNER:
      oe_prikaz_selekce_kontejneru(p_cnf);
      break;
    case ID_K_ZRCADLO_AKTOBJEKTJAKOZRCADLO:
      oe_zrcadlo_aktivuj_body(p_cnf);
      break;
    case ID_K_ZRCADLO_AKTIVUJZRCADLOBODY:
      oe_zrcadlo_aktivuj_plocha(p_cnf);
      break;      
    case ID_K_ZRCADLO_ZRUZZRCADLO:
      oe_zrcadlo_zrus(p_cnf);
      break;
    case ID_SCENA_KRESLITVSECHNO:
      doe_prikaz_kresli_vsechno(p_cnf);
      break;
    case ID_MESH_FACE_ROZDROBIT:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_rozdrob_objekt(p_cnf,TRUE);
      break;
    case ID_SOUBOR_POSUNOUTLEVEL:
      cekej_stisk_mysi();
      oe_uloz_undo(p_cnf);
      be_prikaz_posun_levelu(&p_cnf->bnf);
      break;
    case ID_K_DEBUG_WORDMATRIXAKTKONTEJNERU:      
      doe_word_matrix_kont(p_cnf);
      break;
    case ID_K_DEBUG_WORDMATRIXOBJEKTU:
      doe_word_matrix_objektu(p_cnf);
      break;
    case ID_K_DEBUG_MPMATRIXKONTEJNERU:
      doe_mp_matrix_kont(p_cnf);
      break;
    case ID_NASTROJE_SEZNAMTEXTUR:
      doe_texture_list(p_cnf,1);
      break;
    case ID_NASTROJE_SEZNAMLIGHTMAP:
      doe_texture_list(p_cnf, 0);
      break;
    case ID_K_KONTEJNER_DEINTEGRATOR:
      oe_uloz_undo(p_cnf);
      oe_prikaz_desintegrator(p_cnf);
      break;     
    case ID_K_ZRCADLO_VYRIZNIOBRAZOVEFACE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_vyrizni_face(p_cnf,TRUE);
      break;
    case ID_K_ZRCADLO_KOPIRUJZRCADLO:
      oe_uloz_undo(p_cnf);
      oe_prikaz_vyrizni_face(p_cnf,FALSE);
      break;

    /* 
      Horni prikaz menu
    */
    case ID_BERUSKYII_PREVOD:
      oe_scena_to_game(p_cnf);
      break;
    case ID_RENDERING_RENDERING:
      ray_prepocitej_staticke_osvetleni(p_cnf,FALSE);
      break;
    case ID_BERUSKYII_ZRUSBERUSCISCENU:
      oe_poly_to_scena(p_cnf);
      break;
    case ID_SOUBOR_ULOZITHERNISCENU:
      oe_prikaz_uloz_berusci_level(p_cnf);
      break;
    case ID_K_OBJEKT_OSAMOSTATNIT:
      oe_uloz_undo(p_cnf);
      oe_prikaz_vertex_objekt(p_cnf);
      break;
    case ID_K_OBJEKT_VERTEXOVYRAYTRACER:
      oe_uloz_undo(p_cnf);
      oe_prikaz_osamostatnit_objekt(p_cnf);
      break;
    case ID_K_KONZOLE_VELKAKONZOLE:
      doe_konzole_max(p_cnf);
      break;
    case ID_K_KONZOLE_SMAZATKONZOLI:
      konzole_smaz(p_cnf);
      break;
    case ID_K_DEBUG_RELOADTEXTUR:
      oe_reload_textur(p_cnf);
      break;
    case ID_BERUSKYII_HREJLEVEL:
      oe_prikaz_hrej_berusci_level(p_cnf);
      break;
    case ID_K_KONTEJNER_VYCISTIKONTEJNER:
      oe_uloz_undo(p_cnf);
      oe_prikaz_vycisti_kontejner(p_cnf);
      break;

    case ID_K_KAMERY_NOVAKAMERA:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kamera_nova(p_cnf);
      break;
    case ID_K_KAMERY_UPRAVITKAMERU:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kamera_upravit(p_cnf);
      break;
    case ID_K_KAMERY_SMAZATKAMERU:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kamera_smazat(p_cnf);
      break;
    case ID_K_KAMERA_NOVAANIMACE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kamera_animace_nova(p_cnf);
      break;
    case ID_K_KAMERA_UPRAVITANIMACI:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kamera_animace_upravit(p_cnf);
      break;
    case ID_K_KAMERA_SMAZATANIMACI:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kamera_animace_smazat(p_cnf);
      break;
    case ID_K_KAMERY_POSUNPOZICE:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_posun_kameru(p_cnf,2);
      break;
    case ID_K_KAMERY_POSUNTARGET:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_posun_kameru(p_cnf,1);
      break;
    case ID_K_KAMERA_POSUNPOZICETARGET:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_posun_kameru(p_cnf,3);
      break;
    case ID_K_KAMERY_ROTACEROLL:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_roll_kameru(p_cnf);
      break;
    case ID_K_KAMERY_VELIKOSTFOV:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_fov_kameru(p_cnf);
      break;
    case ID_K_DEBUG_VYPISVERTEXU:
      doe_prikaz_vypis_group(p_cnf);
      break;
    case ID_K_KONTEJNER_AUTOSCALE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_autoscale_kontejneru(p_cnf);
      break;
    case ID_NASTROJE_FLAFOVACMATERIALU:
      oe_uloz_undo(p_cnf);
      doe_editor_materialu_flagovac(p_cnf);
      break;
    case ID_K_KONTEJNER_KOPIE_2:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kopiruj_kontejner_vyber(p_cnf);
      break;
    case ID_NASTROJE_MNOZICSTACICITY:      
      oe_uloz_undo(p_cnf);
      oe_rozmnoz_staticke(p_cnf);
      break;
    case ID_K_VYBERFACE_ODEBRATPLOSKU:
      oe_ungroup_face_vybrany(p_cnf);
      break;
    case ID_K_VYBERFACE_ODEBRATVSECHNY:
      oe_ungroup_face_all(p_cnf);
      break;
    case ID_K_VYBERFACE_VYBRATFACECTVERCEM:
      cekej_stisk_mysi();
      oe_group_face_vyber(p_cnf);
      break;
    case ID_K_VYBERFACE_PRIDATFACECTVERCEM:
      cekej_stisk_mysi();
      oe_group_face_pridej(p_cnf);
      break;
    case ID_K_VYBERFACE_ODEBRATCTVERCEM:
      oe_ungroup_face_ctverec(p_cnf);
      break;
    case ID_K_STREDLEVELU_POSUNSTREDU:
      cekej_stisk_mysi();
      oe_prikaz_posun_stredu_levelu(p_cnf);
      break;
    case ID_K_MIX_POSUNMLHOKOSTKY:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_posun_mlhokostky(p_cnf);
      break;
    case ID_K_MIX_SCALEMLHOKOSTKY:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_scale_mlhokostky(p_cnf);
      break;
    case ID_K_DEBUG_VYPISFACE:
      oe_prikaz_vypis_face(p_cnf);
      break;
    case ID_K_KONTEJNER_INVERZEPLOSEK:
      oe_uloz_undo(p_cnf);
      oe_prikaz_face_invert_kontejner(p_cnf);
      break;
    case ID_K_OBJEKT_INVERZEPLOSEK:
      oe_uloz_undo(p_cnf);
      oe_prikaz_face_invert_objekt(p_cnf);
      break;
    case ID_K_FACE_INVERZEPLOSEK:
      oe_uloz_undo(p_cnf);
      oe_prikaz_face_invert_vyber(p_cnf);
      break;
    case ID_K_STATICKESVETLA_SMAZAT:
      oe_uloz_undo(p_cnf);
      if(p_cnf->lightakt < MAX_EDIT_SVETEL) {
        oe_smaz_staticke_bodove_svetlo(p_cnf, p_cnf->light+p_cnf->lightakt);
        p_cnf->lightakt = lo_najdi_plne_stat_svetlo(p_cnf->light,MAX_EDIT_SVETEL);
        if(p_cnf->lightakt == K_CHYBA)
          p_cnf->lightakt = 0;
      }
      break;
    case ID_K_DYNAMICKESVETLA_NOVE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_dsvetlo_nove(p_cnf);
      break;
    case ID_K_DYNAMICKESVETLA_UPRAVIT:
      oe_uloz_undo(p_cnf);
      oe_prikaz_dsvetlo_uprav(p_cnf);
      break;
    case ID_K_DYNAMICKESVETLA_SMAZAT:
      oe_uloz_undo(p_cnf);
      oe_prikaz_dsvetlo_smaz(p_cnf);
      break;
    case ID_K_DYNAMICKESVETLA_POSUNOUT:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_dsvetlo_posun(p_cnf);
      break;
    case ID_K_DYNAMICKESVETLA_POSUNOUTKOPII:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      key[K_CTRL] = TRUE;
      oe_prikaz_dsvetlo_posun(p_cnf);
      break;
    case ID_K_EFEKTOVESVETLA_NOVE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_extra_svetlo_nove(p_cnf);
      break;
    case ID_K_EFEKTOVESVETLA_UPRAVIT:
      oe_uloz_undo(p_cnf);
      oe_prikaz_extra_svetlo_uprav(p_cnf);
      break;
    case ID_K_EFEKTOVESVETLA_SMAZAT:
      oe_uloz_undo(p_cnf);
      oe_prikaz_extra_svetlo_smaz(p_cnf);
      break;
    case ID_K_EFEKTOVESVETLA_POSUN:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_extra_svetlo_posun(p_cnf);
      break;
    case ID_K_EFEKTOVESVETLA_POSUNKOPIE:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      key[K_CTRL] = TRUE;
      oe_prikaz_extra_svetlo_posun(p_cnf);
      break;
    case ID_NASTROJE_MNOZICSTATICITY:
      oe_rozmnoz_staticke(p_cnf);
      break;
    case ID_K_KONTEJNER_SLEPITVYBRANEKONTEJNERY:
      oe_uloz_undo(p_cnf);
      oe_prikaz_slep_kontejnery(p_cnf);
      break;
    case ID_NASTROJE_LEVELENVIRONMENT:
      kof_Set_Level_Environment(&p_cnf->env, hinst, hwnd_aplikace);
      break;
    case ID_SOUBOR_IMPORTUJASESOUBOR:      
      oe_uloz_undo(p_cnf);
      prikaz(oe_prikaz_nahraj_kontejner(p_cnf,LOAD_KONT_ASE,TRUE));
      break;
    case ID_K_FACE_VYRIZNIOBJEKT:
      oe_uloz_undo(p_cnf);
      oe_prikaz_vyrizni_objekt(p_cnf,FALSE);
      break;
    case ID_K_FACE_VYRIZNIKONTEJNER:
      oe_uloz_undo(p_cnf);
      oe_prikaz_vyrizni_kontejner(p_cnf,FALSE);
      break;
    case ID_K_FACE_VYRIZNIOBJEKT_CROP:
      oe_uloz_undo(p_cnf);
      oe_prikaz_vyrizni_objekt(p_cnf,TRUE);
      break;
    case ID_K_FACE_VYRIZNIKONTEJNER_CROP:
      oe_uloz_undo(p_cnf);
      oe_prikaz_vyrizni_kontejner(p_cnf,TRUE);
      break;      
    case ID_K_BODY_SLIJBODY:
      oe_uloz_undo(p_cnf);
      oe_prikaz_slij_group_vertex(p_cnf);
      break;
    case ID_K_FACE_DECIMACETRSU:
      oe_uloz_undo(p_cnf);
      oe_prikaz_decimace_trsu(p_cnf);
      break;
    case ID_K_FACE_DECIMACEPRUHU:
      oe_uloz_undo(p_cnf);
      oe_prikaz_decimace_pruhu(p_cnf);
      break;
    case ID_K_FACE_DECIMACETRSUBODY:
      oe_uloz_undo(p_cnf);
      oe_prikaz_decimace_pruhu_body(p_cnf);
      break;
    case ID_K_BODY_SLIVACKA:
      oe_uloz_undo(p_cnf);
      oe_prikaz_slivacka_group_vertex(p_cnf);
      break;
    case ID_K_FACE_VYRIZNIFACE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_vyrizni_plosky(p_cnf);
      break;      
    case ID_K_KAMERA_POSUNBBKAMERY:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_posun_bb_kamery(p_cnf);
      break;
    case ID_K_KAMERA_SCALEBBKAMERY:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_scale_bb_kamery(p_cnf);
      break;
    case ID_K_VLOZITPRVEK:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      be_prikaz_vloz_prvek(p_cnf);
      break;
    case ID_K_SMAZATPRVEK:      
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      be_prikaz_smaz_prvek(p_cnf);
      break;
    case ID_K_PATRONAHORU:
      be_prikaz_patro_nahoru(p_cnf);
      break;
    case ID_K_PATRODOLU:
      be_prikaz_patro_dolu(p_cnf);
      break;
    case ID_SOUBOR_SMAZATLEVELBEZPOZADI:
      oe_uloz_undo(p_cnf);
      be_prikaz_smaz_level(p_cnf,TRUE,FALSE,FALSE);
      break;      
    case ID_NASTROJE_OBRUBALEVELU:
      oe_uloz_undo(p_cnf);      
      be_prikaz_vyrob_obrubu_levelu(p_cnf,be_najdi_obruba_material(p_cnf),FALSE);
      break;
    case ID_SCENA_SCALESCENY:
      oe_uloz_undo(p_cnf);
      oe_prikaz_scale_sceny(p_cnf);
      break;
    case ID_SCENA_POSUNSCENY:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_posun_scenu(p_cnf);
      break;
    case ID_NASTROJE_POUZITEMATERIALY:
      oe_prikaz_vypis_pouzite_materialy(p_cnf);
      break;
    case ID_NASTROJE_PODLOZKALEVELU:
      oe_uloz_undo(p_cnf);
      be_prikaz_vyrob_podlozku_levelu(p_cnf,p_cnf->km.vybr_mat);
      break;
    case ID_K_HROMADNENAHRAZENI:
      oe_uloz_undo(p_cnf);
      be_prikaz_nahrad_aktualni_prvek(p_cnf);
      break;
    case ID_K_OBJEKT_TESELACEOBJEKTU:
      oe_uloz_undo(p_cnf);
      oe_prikaz_teselace_objektu(p_cnf);
      break;
    case ID_K_ZRCADLO_PREHODPORADIBODU:
      oe_zrcadlo_prehod_poradi_bodu(p_cnf);
      break;
    case ID_K_KONTEJNER_KOORDINATY12:
      oe_prikaz_rozmnoz_texture_koord(p_cnf, 0, 1);
      break;
    case ID_K_KONTEJNER_KOORDINATY13:
      oe_prikaz_rozmnoz_texture_koord(p_cnf, 0, 2);
      break;
    case ID_K_KONTEJNER_KOORDINATY14:
      oe_prikaz_rozmnoz_texture_koord(p_cnf, 0, 3);
      break;
    case ID_K_KONTEJNER_PREINDEXACE:
      oe_prikaz_preindexuj_kontejner(p_cnf);
      break;
    case ID_K_OBJEKT_POSUNPIVOTBODU:
      oe_uloz_undo(p_cnf);
      oe_prikaz_posun_objekt_pivot_point(p_cnf);
      break;
    case ID_K_OBJEKT_CENTRPIVOTBODU:
      oe_uloz_undo(p_cnf);
      oe_prikaz_centr_pivot_point(p_cnf);
      break;
    case ID_SOUBOR_ULOZITVYBRANEKONTEJNERY:
      oe_prikaz_uloz_vybrane_kontejnery_jako(p_cnf);
      break;
    case ID_SOUBOR_NAHRATOBJEKT:
      oe_prikaz_nahraj_objekt(p_cnf);
      break;
    case ID_K_OBJEKT_KOPIEGEOMETRIE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kopiruj_objekt_geometrie(p_cnf);
      break;
    case ID_K_OBJEKT_SCHOVEJMATICI:
      doe_schovej_word_matrix_objektu(p_cnf);
      break;
    case ID_K_OBJEKT_VLOZWORLDMATRIX:
      oe_uloz_undo(p_cnf);
      doe_vloz_word_matrix_objektu(p_cnf);
      break;
    case ID_SOUBOR_EXPORTUJ3DSSOUBOR:
      oe_prikaz_uloz_3ds_kontejner_jako(p_cnf);
      break;
    case ID_SOUBOR_EXPORTUJ3DSSCENU:
      oe_prikaz_uloz_3ds_scenu_jako(p_cnf);
      break;
    case ID_SOUBOR_EXPORTSCENY:
      oe_prikaz_exportuj_scenu(p_cnf);
      break;
    case ID_K_KONTEJNER_VYPISMATERIALU:
      oe_prikaz_vypis_pouzite_materialy_kontejner(p_cnf);
      break;
    case ID_K_KOSTI_VLOZJOINTMEZIKOZY: // nova joint animace
      oe_uloz_undo(p_cnf);
      oe_prikaz_kost_pridej_start(p_cnf);
      break;
    case ID_K_KOSTI_PRIPOJJOINTMEZIKOZY: // navaze na joint dalsi
      oe_uloz_undo(p_cnf);
      oe_prikaz_kost_pridej_navaz(p_cnf);
      break;
    case ID_K_KOSTI_PRIDEJVERTEXY:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kost_pridej_vertexy(p_cnf);
      break;
    case ID_K_KOSTI_UBERVERTEXY:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kost_uber_vertexy(p_cnf);
      break;
    case ID_K_KOSTI_SMAZJOINT:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kost_smaz(p_cnf);
      break;
    case ID_K_KOSTI_JOINTPOSUN:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_kost_posun(p_cnf,FALSE);
      break;
    case ID_K_KOSTI_POSUNPIVOT:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_kost_posun(p_cnf,TRUE);
      break;
    case ID_K_KOSTI_JOINTROTACE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kost_rotace(p_cnf);
      break;
    case ID_K_KOSTI_SCALE:
      oe_uloz_undo(p_cnf);
      cekej_stisk_mysi();
      oe_prikaz_kost_scale(p_cnf);
      break;
    case ID_K_FLARE_GENERUJFLARE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_generuj_flare(p_cnf);      
      break;
    case ID_K_FLARE_EXPORTTEXTUR:
      oe_prikaz_flare_export_textur(p_cnf);
      break;
    case ID_K_BODY_SJEDNOTVYSKU:
      oe_uloz_undo(p_cnf);
      oe_prikaz_vyrovnej_group_vertex(p_cnf, FALSE);
      break;
    case ID_K_BODY_SJEDNOTVYSKU2: // - level
      oe_uloz_undo(p_cnf);
      oe_prikaz_vyrovnej_group_vertex(p_cnf, TRUE);
      break;
    case ID_K_KOSTI_KOPIEROOTJOINTANIMACE:
      oe_uloz_undo(p_cnf);
      oe_prikaz_kost_kopiruj(p_cnf);
      break;
    case ID_K_OBJEKT_SJEDNOCENIVERTEXU:
      oe_uloz_undo(p_cnf);
      oe_prikaz_sjednoceni_vertexu_obj(p_cnf);
      break;
    case ID_SCENA_OBALKY:
      oe_obb_calc_scena(p_cnf);
      break;
    case ID_K_KONTEJNER_FLAGOVAC:
      oe_prikaz_flagovac(p_cnf);
      break;
    case ID_K_OBJEKT_CENTRDOWNPIVOTBODU:
      oe_prikaz_down_centr_pivot(p_cnf);
      break;
    case ID_SCENA_VYCISTISCENU:
      oe_prikaz_vycisti_scenu(p_cnf);
      break;
    case ID_K_ZRCADLO_ZPRUHLEDNOVACVYBROBJEKTU:
      oe_prikaz_zrcadelnik(p_cnf);
      break;
    case ID_KONFIGURACE_LOADFLAGY:
      doe_prikaz_load_flags(p_cnf);
      break;
    case ID_K_VYREZANILEVELU:
      oe_uloz_undo(p_cnf);
      be_prikaz_vyrezani_levelu(p_cnf);
      break;
    case ID_NASTROJE_BMP_PREVADEC:
      oe_uloz_undo(p_cnf);
      oe_prikaz_prevadec_bmp(p_cnf);
      break;
    case ID_K_KONTEJNER_EXPORTDOHRY:
      oe_uloz_undo(p_cnf);
      oe_kontejner_to_game(p_cnf);
      break;
    case ID_K_KONTEJNER_ZVEDNI:
      oe_uloz_undo(p_cnf);
      oe_prikaz_zvedni_kontejner(p_cnf);
      break;
    default:      
      return(FALSE);
      break;
  }
  return(TRUE);
}

void doe_toogle_kreslit_level(K_EDITOR *p_cnf)
{
  HMENU m_hMenu = GetMenu(hwnd_aplikace);
  dword ch;
  p_cnf->kreslit_level = !p_cnf->kreslit_level;
  ch = (p_cnf->kreslit_level) ? MF_CHECKED : MF_UNCHECKED;
  CheckMenuItem(  m_hMenu, ID_SCENA_KRESLITLEVEL, ch);
}

void doe_toogle_kreslit_scenu(K_EDITOR *p_cnf)
{
  HMENU m_hMenu = GetMenu(hwnd_aplikace);
  dword ch;
  p_cnf->kreslit_scenu = !p_cnf->kreslit_scenu;
  ch = (p_cnf->kreslit_scenu) ? MF_CHECKED : MF_UNCHECKED;
  CheckMenuItem(  m_hMenu, ID_SCENA_KRESLITSCENU, ch);
}

void doe_zaskrkni_menu(K_EDITOR *p_cnf)
{
  HMENU m_hMenu = GetMenu(hwnd_aplikace);
  dword ch;
  ch = (p_cnf->kreslit_scenu) ? MF_CHECKED : MF_UNCHECKED;
  CheckMenuItem(  m_hMenu, ID_SCENA_KRESLITSCENU, ch);
  ch = (p_cnf->kreslit_level) ? MF_CHECKED : MF_UNCHECKED;
  CheckMenuItem(  m_hMenu, ID_SCENA_KRESLITLEVEL, ch);
}

/*
  log = 0 - obrazovka
      = 1 - obrazovka & disk
      = 2 -             disk
*/
void kprintf(byte log, byte *p_text,...)
{
  B_KONFIG *p_bnf = &cnf.bnf;
  HWND      hwnd = GetDlgItem(hwnd_konzole_dialog, IDC_LIST2);
  byte      text[2000];  
  va_list   argumenty;

  va_start(argumenty,p_text);
  vsprintf(text,p_text,argumenty);
  va_end(argumenty);

  if(log != 2) {
    SendMessage( hwnd, LB_SETCURSEL,SendMessage( hwnd, LB_ADDSTRING, 0,(LPARAM)text),0);
  }

  if(log&&p_bnf->debug_file) {
    fputs(text,p_bnf->debug_file);
    fputc('\n',p_bnf->debug_file);
    fflush(p_bnf->debug_file);
  }
  
  spracuj_spravy(TRUE);
}

void kgprintf(FILE *f, byte *p_text,...)
{
  HWND      hwnd = GetDlgItem(hwnd_konzole_dialog, IDC_LIST2);
  byte      text[2000];  
  va_list   argumenty;

  va_start(argumenty,p_text);
  vsprintf(text,p_text,argumenty);
  va_end(argumenty);
  SendMessage( hwnd, LB_SETCURSEL,SendMessage( hwnd, LB_ADDSTRING, 0,(LPARAM)text),0);
  
  if(f) {
    fputs(text,f);
    fputc('\n',f);
    fflush(f);
  }
}

void kprintfl(byte log, byte *p_text,...)
{ 
  B_KONFIG *p_bnf = &cnf.bnf;
  HWND      hwnd = GetDlgItem(hwnd_konzole_dialog, IDC_LIST1);
  byte      text[2000];  
  va_list   argumenty;

  va_start(argumenty,p_text);
  vsprintf(text,p_text,argumenty);
  va_end(argumenty);
  
  SendMessage( hwnd, LB_DELETESTRING, SendMessage( hwnd, LB_GETCURSEL,0,0),0);  
  SendMessage( hwnd, LB_SETCURSEL,SendMessage( hwnd, LB_ADDSTRING, 0,(LPARAM)text),0);

  if(log&&p_bnf->debug_file) {
    fputs(text,p_bnf->debug_file);
    fputc('\n',p_bnf->debug_file);
    fflush(p_bnf->debug_file);
  }
}

/*
  Smazne konzoli
*/
void konzole_smaz(K_EDITOR *p_cnf)
{
  HWND hwnd = GetDlgItem(hwnd_konzole_dialog, IDC_LIST2);
  oe_wrlog(p_cnf,"==================== Mazba konzole =====================");
  SendMessage(hwnd, LB_RESETCONTENT, 0, 0);
}

BOOL CALLBACK doe_callback_konzole_max(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {          
          EndDialog( hDlg, FALSE );
          break;
        }
        default:
          return TRUE;
      }
      break;    
    case WM_INITDIALOG: {      
      HWND hwnds = GetDlgItem(hwnd_konzole_dialog, IDC_LIST2);
      HWND hwndd = GetDlgItem(hDlg, IDC_LIST1);
      byte pom[200];
      int  i,max;
      
      max = SendMessage( hwnds, LB_GETCOUNT, 0, 0);
      for(i = 0; i < max; i++) {
        SendMessage( hwnds, LB_GETTEXT, i,(LPARAM)pom);
        SendMessage( hwndd, LB_ADDSTRING, 0,(LPARAM)pom);
      } 
      SendMessage( hwndd, LB_SETCURSEL, i-1,0);

      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_konzole_max(K_EDITOR *p_cnf)
{
  DialogBox( hinst, MAKEINTRESOURCE(IDD_KONZOLE_MAXI), hwnd_aplikace, doe_callback_konzole_max);
}

void doe_updatuj_konzoli_mod(K_EDITOR *p_cnf)
{
 dword x,y;

 x = 0; y = 27;
 if(p_cnf->mod == MOD_GAME) {
   kprintf(TRUE,"Kamera 4 - Herni pohled");
 }
 else if(p_cnf->mod == MOD_XY) {
   kprintf(TRUE,"Kamera 2 - Pohled X / Y");  
 }                               
 else if(p_cnf->mod == MOD_XZ) {
   kprintf(TRUE,"Kamera 1 - Pohled X / Z");  
 }
 else if(p_cnf->mod == MOD_ZY) {
   kprintf(TRUE,"Kamera 3 - Pohled Y / Z");
 }
 else {
   kprintf(TRUE,"Volny pohled");  
 }
 kprintf(TRUE,"vzdal %.2f metru", *p_cnf->p_vzdal);
 kprintf(TRUE,"uhel  %.2f stupnu",*p_cnf->p_fi/PI*180);
}


void doe_updatuj_konzoli_posun(K_EDITOR *p_cnf)
{
 float x = 0, y = 0, z = 0;

 transformuj_bod_matici(&x, &y, &z, p_cnf->p_r_bod);
 kprintf(TRUE,"Stred obrazovky X = %.3f Y = %.3f Z = %.3f",x,y,z);
}

void doe_updatuj_konzoli_kamera(K_EDITOR *p_cnf)
{
 float x = 0, y = 0, z = 0;
 GLMATRIX m;
  
 transformuj_bod_matici(&x, &y, &z, invert_matrix(p_cnf->p_camera,&m));
 kprintf(TRUE,"Kamera X = %.3f Y = %.3f Z = %.3f",x,y,z);
}

void doe_updatuj_konzoli(K_EDITOR *p_cnf)
{
// doe_updatuj_konzoli_mod(p_con, p_cnf);
}

void doe_updatuj_konzoli_mys(K_EDITOR *p_cnf)
{
 B_KONFIG *p_bnf;
 float x,y,z;
 int   lx,ly,lz,i1,i2;
 byte  pom[100];

 transf_2d_3d_z((float)mi.x, (float)mi.y, *p_cnf->p_vzdal,
              &x, &y, &z,
              p_cnf->p_camera,
              p_cnf->p_project, 
              OXRES, OYRES, OXSTART, OYSTART,
              p_cnf->kam.near_plane,p_cnf->kam.far_plane);
 
 sprintf(pom,"%f",x);
 SetWindowText(GetDlgItem(hwnd_dolni_lista_dialog,IDC_STATIC1),pom);
 sprintf(pom,"%f",y); 
 SetWindowText(GetDlgItem(hwnd_dolni_lista_dialog,IDC_STATIC2),pom);
 sprintf(pom,"%f",z); 
 SetWindowText(GetDlgItem(hwnd_dolni_lista_dialog,IDC_STATIC3),pom);

 SetWindowText(GetDlgItem(hwnd_dolni_lista_dialog,IDC_STATIC4),itoa(mi.x,pom,10));
 SetWindowText(GetDlgItem(hwnd_dolni_lista_dialog,IDC_STATIC5),itoa(mi.y,pom,10));
 
 sprintf(pom,"%.4f",p_cnf->fps);
 SetWindowText(GetDlgItem(hwnd_dolni_lista_dialog,IDC_STATIC6),pom);

 p_bnf = &p_cnf->bnf;
 lx = p_bnf->grid.x_kurzor;
 ly = p_bnf->grid.z_kurzor;
 lz = p_bnf->grid.y_kurzor;
 SetWindowText(GetDlgItem(hwnd_dolni_lista_dialog,IDC_STATIC24),itoa(lx,pom,10));
 SetWindowText(GetDlgItem(hwnd_dolni_lista_dialog,IDC_STATIC25),itoa(ly,pom,10));
 SetWindowText(GetDlgItem(hwnd_dolni_lista_dialog,IDC_STATIC26),itoa(lz,pom,10));

 i1 = be_pozice_v_levelu(p_bnf, lx, ly, lz, 0);
 i2 = be_pozice_v_levelu(p_bnf, lx, ly, lz, 1);
 SetWindowText(GetDlgItem(hwnd_dolni_lista_dialog,IDC_STATIC28),itoa(i1,pom,10));
 SetWindowText(GetDlgItem(hwnd_dolni_lista_dialog,IDC_STATIC29),itoa(i2,pom,10));
}

void aplikuj_zmeny_konfigurace(K_EDITOR *p_cnf)
{
  nastav_konfig(&hwconf,&txconf);
      
  light_ambient_d(p_cnf->svetlost_sceny);
  light_on_off(p_cnf->cnf.svetla);  
}

// Konfigurace -------

TABLE_LIST tk;
#define nastav_pozici() SetWindowPos(hDlg, HWND_TOP, tk.rc.left, tk.rc.top, 0, 0, SWP_NOSIZE)

BOOL CALLBACK doe_calback_konfigurace_konfig(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON5: // smazani konzole
          oe_prikaz_smazani_konzole(p_cnf);
          break;      
        case IDC_BUTTON6: // back-face
          oe_prikaz_zmena_cul(p_cnf);
          break;      
        default:
          break;
      }
      break;
    case WM_NOTIFY: {
      break;
    }
    case WM_INITDIALOG: {
        K_EDITOR *p_cnf = (K_EDITOR *)tk.p_sv;
        HWND hSlider = GetDlgItem( hDlg, IDC_SLIDER1);
        SendMessage(hSlider, TBM_SETRANGE,(WPARAM)(BOOL)TRUE,(LPARAM)MAKELONG(0,1)); 
        hSlider = GetDlgItem( hDlg, IDC_SLIDER2);            
        SendMessage(hSlider, TBM_SETRANGE,(WPARAM)(BOOL)TRUE,(LPARAM)MAKELONG(0,4)); 
        SendMessage(hSlider, TBM_SETTICFREQ,(WPARAM)1,0);
        
        nastav_pozici();
        CheckDlgButton( hDlg, IDC_CHECK1, p_cnf->cnf.barvy_z_materialu);
        CheckDlgButton( hDlg, IDC_CHECK2, p_cnf->cnf.objekt_origin);
        CheckDlgButton( hDlg, IDC_CHECK3, p_cnf->group_param);
        CheckDlgButton( hDlg, IDC_CHECK5, p_cnf->cnf.kamera_kreslit);
        CheckDlgButton( hDlg, IDC_CHECK6, p_cnf->cnf.textury);

        CheckDlgButton( hDlg, IDC_CHECK15, p_cnf->cnf.dratove);

        CheckDlgButton( hDlg, IDC_CHECK20, p_cnf->cnf.undo);
        CheckDlgButton( hDlg, IDC_CHECK22, p_cnf->cnf.objekt_stred);

        CheckDlgButton( hDlg, IDC_CHECK12, p_cnf->cnf.grid);

        CheckDlgButton( hDlg, IDC_CHECK14, p_cnf->cnf.mazat_dummy);
        CheckDlgButton( hDlg, IDC_CHECK8,  p_cnf->cnf.kdtree_render_bunky);
        
        CheckDlgButton( hDlg, IDC_CHECK16, p_cnf->cnf.norm_vect);
        CheckDlgButton( hDlg, IDC_CHECK17, p_cnf->lp.aktivni);
        
        CheckDlgButton( hDlg, IDC_CHECK4,  p_cnf->cnf.precizni_export_obb);
        
        if(p_cnf->cul == 0)
          CheckDlgButton( hDlg, IDC_RADIO4, BST_CHECKED);
        else if(p_cnf->cul == 1)
          CheckDlgButton( hDlg, IDC_RADIO2, BST_CHECKED);
        else if(p_cnf->cul == 2)
          CheckDlgButton( hDlg, IDC_RADIO3, BST_CHECKED);
        
        CheckDlgButton( hDlg, IDC_CHECK21, p_cnf->cnf.obalky_objekt);
        CheckDlgButton( hDlg, IDC_CHECK26, p_cnf->cnf.obalky_kontejner);

        CheckDlgButton( hDlg, IDC_CHECK9, p_cnf->cnf.keyframe_load);        
        CheckDlgButton( hDlg, IDC_CHECK13, p_cnf->cnf.render_zrcadla);

        CheckDlgButton( hDlg, IDC_CHECK19, p_cnf->cnf.delit_materialy);

        doe_printf(hDlg,IDC_EDIT1,"%f",p_cnf->cnf.norm_vect_velikost);
      } 
      break;
    case WM_DESTROY: {
      K_EDITOR *p_cnf = (K_EDITOR *)tk.p_sv;
      
      p_cnf->cnf.barvy_z_materialu = IsDlgButtonChecked( hDlg, IDC_CHECK1);
      p_cnf->cnf.objekt_origin = IsDlgButtonChecked( hDlg, IDC_CHECK2);
      p_cnf->group_param = IsDlgButtonChecked( hDlg, IDC_CHECK3);
      p_cnf->cnf.kamera_kreslit = IsDlgButtonChecked( hDlg, IDC_CHECK5);
      p_cnf->cnf.textury = IsDlgButtonChecked( hDlg, IDC_CHECK6);

      p_cnf->cnf.dratove = IsDlgButtonChecked( hDlg, IDC_CHECK15);

      p_cnf->cnf.undo = IsDlgButtonChecked( hDlg, IDC_CHECK20);
      p_cnf->cnf.objekt_stred = IsDlgButtonChecked( hDlg, IDC_CHECK22);

      p_cnf->cnf.grid = IsDlgButtonChecked( hDlg, IDC_CHECK12);

      p_cnf->cnf.mazat_dummy = IsDlgButtonChecked( hDlg, IDC_CHECK14);
      p_cnf->cnf.kdtree_render_bunky = IsDlgButtonChecked( hDlg, IDC_CHECK8);
      p_cnf->cnf.render_zrcadla = IsDlgButtonChecked( hDlg, IDC_CHECK13);
      
      p_cnf->cnf.precizni_export_obb = IsDlgButtonChecked( hDlg, IDC_CHECK4);
      p_cnf->cnf.norm_vect = IsDlgButtonChecked( hDlg, IDC_CHECK16);
      
      if(IsDlgButtonChecked( hDlg, IDC_RADIO4)) { // none
        p_cnf->cul = 0;        
        cull_nastav(p_cnf->cul);
      } else if(IsDlgButtonChecked( hDlg, IDC_RADIO2)) { // CW
        p_cnf->cul = 1;
        cull_nastav(p_cnf->cul);
      } else if(IsDlgButtonChecked( hDlg, IDC_RADIO3)) { // CCW
        p_cnf->cul = 2;
        cull_nastav(p_cnf->cul);
      }
      
      p_cnf->cnf.obalky_kontejner = IsDlgButtonChecked( hDlg, IDC_CHECK26);
      p_cnf->cnf.obalky_objekt = IsDlgButtonChecked( hDlg, IDC_CHECK21 );

      p_cnf->cnf.keyframe_load = IsDlgButtonChecked( hDlg, IDC_CHECK9 );      
      p_cnf->lp.aktivni = IsDlgButtonChecked( hDlg, IDC_CHECK17 );      
      
      p_cnf->cnf.norm_vect_velikost = doe_vyber_kolonku_float(hDlg,IDC_EDIT1);      
      p_cnf->cnf.delit_materialy = IsDlgButtonChecked(hDlg, IDC_CHECK19);
      
      EndDialog( hDlg, FALSE );
      break; 
    }
    default:
      return FALSE;
  }
  return TRUE;
}

BOOL CALLBACK doe_calback_konfigurace_dir(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_INITDIALOG: {
      K_EDITOR *p_cnf = (K_EDITOR *)tk.p_sv;
      HWND hwnd = GetDlgItem( hDlg, IDC_COMBO1 );
      
      nastav_pozici();
      SetDlgItemText(hDlg, IDC_EDIT1,  p_cnf->dir.prvky_dir);
      SetDlgItemText(hDlg, IDC_EDIT11, p_cnf->dir.level_dir);
      SetDlgItemText(hDlg, IDC_EDIT10, p_cnf->dir.out_dir);
      SetDlgItemText(hDlg, IDC_EDIT12, p_cnf->tdir.texture_dir[0]);
      SetDlgItemText(hDlg, IDC_EDIT13, p_cnf->dir.material_dir);
      SetDlgItemText(hDlg, IDC_EDIT20, p_cnf->dir.data_dir);
      SetDlgItemText(hDlg, IDC_EDIT21, p_cnf->dir.scene_dir);
      SetDlgItemText(hDlg, IDC_EDIT22, p_cnf->dir.game_data_dir);

      } 
      break;
    case WM_DESTROY: {
      K_EDITOR *p_cnf = (K_EDITOR *)tk.p_sv;
      HWND  hwnd = GetDlgItem( hDlg, IDC_COMBO1 );


      GetDlgItemText(hDlg,IDC_EDIT1,p_cnf->dir.prvky_dir,99);
      GetDlgItemText(hDlg,IDC_EDIT11,p_cnf->dir.level_dir,99);
      GetDlgItemText(hDlg,IDC_EDIT10,p_cnf->dir.out_dir,99);
      GetDlgItemText(hDlg,IDC_EDIT12,p_cnf->tdir.texture_dir[0],99);
      GetDlgItemText(hDlg,IDC_EDIT13,p_cnf->dir.material_dir,99);
      GetDlgItemText(hDlg,IDC_EDIT20,p_cnf->dir.data_dir,99);
      GetDlgItemText(hDlg,IDC_EDIT21,p_cnf->dir.scene_dir,99);
      GetDlgItemText(hDlg,IDC_EDIT22,p_cnf->dir.game_data_dir,99);
      
      EndDialog( hDlg, FALSE );
      break; 
    }
    default:
      return FALSE;
  }
  return TRUE;
}

DLGTEMPLATE * load_dialog_resource(byte *p_res) 
{ 
  HRSRC hrsrc = FindResource(NULL, p_res, RT_DIALOG); 
  HGLOBAL hglb = LoadResource(hinst, hrsrc); 
  return(LockResource(hglb));
} 

BOOL CALLBACK doe_calback_konfigurace_table( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON1:
          aplikuj_zmeny_konfigurace(p_cnf);
          break;
        case IDCANCEL:
  				EndDialog( hDlg, FALSE);
          break;
        case IDOK:
          EndDialog( hDlg, TRUE);
          break;
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {      
      TCITEM it;
      int i;
      tk.hwndTab = GetDlgItem(hDlg, IDC_TAB1);
      it.mask = TCIF_TEXT;
      for(i = 0; i < tk.polozek; i++) {
        it.pszText = tk.jmena[i]; 
        SendMessage(tk.hwndTab,TCM_INSERTITEM,i,(LPARAM) (const LPTCITEM)&it);
        tk.p_template[i] = load_dialog_resource(MAKEINTRESOURCE(tk.dlg[i]));
      }
      TabCtrl_AdjustRect(tk.hwndTab, FALSE, &tk.rc);
      tk.rc.top += 5;
      tk.rc.left += 10;
      TabCtrl_SetCurSel(tk.hwndTab,tk.aktivni);
      tk.hwndDlg = CreateDialogIndirect(hinst, tk.p_template[tk.aktivni], hDlg, tk.call[tk.aktivni]); 
      break;
    }
    case WM_NOTIFY: {
      NMHDR *p_nmhdr = (LPNMHDR) lParam;
      // zmen vyber - prepni na jinou kartu
      if(p_nmhdr->code == TCN_SELCHANGE) {
        int vybr = TabCtrl_GetCurSel(tk.hwndTab);
        if(tk.hwndTab != NULL) DestroyWindow(tk.hwndDlg); 
        tk.hwndDlg = CreateDialogIndirect(hinst, tk.p_template[vybr], hDlg, tk.call[vybr]); 
      } 
      break;
    }
    case WM_DESTROY: {
      // tady se musi uvolnit resource
/*
      for(i = 0; i < tk.polozek; i++) {
        DeleteObject(tk.p_template[i]);
      }
*/
      EndDialog( hDlg, FALSE );
      break; 
    }
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_prikaz_konfigurace_table(K_EDITOR *p_cnf, HWND hwnd, int strana)
{
  K_EDITOR tmpconfig = *p_cnf;

  memset(&tk,0,sizeof(tk));
  
  tk.polozek = 2;

  tk.p_sv = &tmpconfig;

  strcpy(tk.jmena[0],"Editor");
  strcpy(tk.jmena[1],"System");

  tk.dlg[0] = IDD_KONFIGURACE_KONFIG; // std konfig editoru
  tk.dlg[1] = IDD_KONFIGURACE_DIR; // adresare
  
  tk.call[0] = doe_calback_konfigurace_konfig;
  tk.call[1] = doe_calback_konfigurace_dir;

  tk.aktivni = strana;
  tk.t_dlg = IDC_TAB1;
      
  if(DialogBox( hinst, MAKEINTRESOURCE(IDD_KONFIGURACE_TABLE), hwnd,  doe_calback_konfigurace_table)) {
    *p_cnf = tmpconfig;
    aplikuj_zmeny_konfigurace(p_cnf);
    oe_uloz_konfiguraci(p_cnf,INI_DIR,INI_FILE,TRUE);
  }  
}

void _updatuj_combo_objekty(K_EDITOR *p_cnf, HWND hDlg, int k, int o)
{
  HWND hwnd1 = GetDlgItem( hDlg, IDC_COMBO1);
  HWND hwnd2 = GetDlgItem( hDlg, IDC_COMBO4);   
  EDIT_KONTEJNER *p_kont;
  int  ak,i,list;  
  byte pom[100];
  
  SendMessage( hwnd1, CB_RESETCONTENT, 0, 0 );
  ak = 0;
  list = 0;
  for(i = 0; i < p_cnf->max_kont; i++) {
    if(i == k)
      ak = list;
    if(p_cnf->p_kont[i]) {
      if(p_cnf->p_kont[i]->jmeno[0])
        strcpy(pom,p_cnf->p_kont[i]->jmeno);
      else
        sprintf(pom,"Bezejmenec %d",i);
      SendMessage( hwnd1, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) pom);        
      SendMessage( hwnd1, CB_SETITEMDATA, list, i);
      list++;
    }
  }
  if(p_cnf->p_kont[ak])
    SendMessage( hwnd1, CB_SETCURSEL, ak, 0 );
  
  p_kont = p_cnf->p_kont[p_cnf->vybr_kont];
  SendMessage( hwnd2, CB_RESETCONTENT, 0, 0 );
  if(p_kont) {
    ak = 0;
    list = 0;
    for(i = 0; i < MAX_KONT_OBJEKTU; i++) {
      if(i == o)
        ak = list;
      if(p_kont->p_obj[i]) {
        if(p_kont->p_obj[i]->jmeno[0])
          strcpy(pom,p_kont->p_obj[i]->jmeno);            
        else
          sprintf(pom,"Bezejmenec %d",i);
        SendMessage( hwnd2, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) pom);
        SendMessage( hwnd2, CB_SETITEMDATA, list, i);
        list++;
      }
    }
    if(p_kont->p_obj[ak])
      SendMessage( hwnd2, CB_SETCURSEL, ak, 0 );
  }
}

void _cti_combo_objekty(K_EDITOR *p_cnf, HWND hDlg, int *p_k, int *p_o)
{
  HWND hwnd1 = GetDlgItem( hDlg, IDC_COMBO1);
  HWND hwnd2 = GetDlgItem( hDlg, IDC_COMBO4);
 
  *p_k = SendMessage( hwnd1, CB_GETITEMDATA, SendMessage( hwnd1, CB_GETCURSEL, 0, 0 ), 0);
  *p_o = SendMessage( hwnd2, CB_GETITEMDATA, SendMessage( hwnd2, CB_GETCURSEL, 0, 0 ), 0);
}

BOOL CALLBACK doe_calback_bodove_svetlo(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON2: { //vyber barvy svetla
          STATIC_LIGHT *p_light = (STATIC_LIGHT *)tmp.p_pom1;
          dword barva = doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT5);
          rgb_float(barva,&p_light->r);
          break;
        }
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {                 
          STATIC_LIGHT *p_light = (STATIC_LIGHT *)tmp.p_pom1;
          byte  pom[60];
          dword barva;
          
          // dosah
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT4),pom,59);
          p_light->dosah = (float)atof(pom);
          
          // barva
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT5),pom,59);
          sscanf(pom,"%x",&barva);
          rgb_float(barva,&p_light->r);
          
          // k1
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT7),pom,59);
          p_light->k1 = (float)atof(pom);

          // k2
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT15),pom,59);
          p_light->k2 = (float)atof(pom);

          // k3
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT16),pom,59);
          p_light->k3 = (float)atof(pom);

          // x,y,z
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT18),pom,59);
          p_light->p.x = (float)atof(pom);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT19),pom,59);
          p_light->p.y = (float)atof(pom);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT24),pom,59);
          p_light->p.z = (float)atof(pom);

          GetWindowText(GetDlgItem(hDlg,IDC_EDIT17),pom,49);

          p_light->utlum = IsDlgButtonChecked(hDlg,IDC_CHECK1);
          p_light->uhel = IsDlgButtonChecked(hDlg,IDC_CHECK2);
          //p_light->plosne = IsDlgButtonChecked(hDlg,IDC_RADIO2);
          //_cti_combo_objekty(p_cnf, hDlg, &p_light->kontejner, &p_light->objekt);

          p_light->flag2 = IsDlgButtonChecked(hDlg,IDC_CHECK3) ?
                           p_light->flag2|SL2_DYNAMIC :
                           p_light->flag2&~SL2_DYNAMIC;
                    
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    
    case WM_INITDIALOG: {
      #define FLOAT_FORMAT "%.3f"
      byte  pom[60];
      STATIC_LIGHT *p_light = (STATIC_LIGHT *)tmp.p_pom1;
      SetWindowText(hDlg,tmp.p_pom);

      // dosah
      sprintf(pom,FLOAT_FORMAT,p_light->dosah);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT4),pom);

      // barva
      sprintf(pom,"%x",float_rgb(&p_light->r));
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT5),pom);
      
      // K1
      sprintf(pom,FLOAT_FORMAT,p_light->k1);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT7),pom);

      // K2
      sprintf(pom,FLOAT_FORMAT,p_light->k2);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT15),pom);
      
      // K3
      sprintf(pom,FLOAT_FORMAT,p_light->k3);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT16),pom);

      // x,y,z
      sprintf(pom,FLOAT_FORMAT,p_light->p.x);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT18),pom);
      sprintf(pom,FLOAT_FORMAT,p_light->p.y);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT19),pom);
      sprintf(pom,FLOAT_FORMAT,p_light->p.z);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT24),pom);
           
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT17),p_light->jmeno);

      CheckDlgButton( hDlg, IDC_CHECK1, p_light->utlum);
      CheckDlgButton( hDlg, IDC_CHECK2, p_light->uhel);

//      CheckDlgButton( hDlg, IDC_RADIO1, !p_light->plosne);
//      CheckDlgButton( hDlg, IDC_RADIO2, p_light->plosne);

//      _updatuj_combo_objekty(p_cnf, hDlg, p_light->kontejner, p_light->objekt);
      
      doe_check_stlac( hDlg, IDC_CHECK3, p_light->flag2&SL2_DYNAMIC);

      itoa((int)(p_light->p_flare),pom,10);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT6),pom);
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_prikaz_bodove_svetlo(HWND hwnd, byte *p_nadpis, STATIC_LIGHT *p_light)
{
  tmp.p_pom = p_nadpis;
  tmp.p_pom1 = p_light;
  return(DialogBox(hinst,MAKEINTRESOURCE(IDD_STATICKE_BODOVE_SVETLO),hwnd,doe_calback_bodove_svetlo));
}

BOOL CALLBACK doe_calback_dynamicke_svetlo(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON1: { //vyber diffusni barvy svetla          
          DYN_LIGHT *p_light = (DYN_LIGHT *)tmp.p_pom1;
          doe_vyber_barvu_do_kolonek(hDlg, IDC_EDIT1, IDC_EDIT9, IDC_EDIT10, &p_light->dr);
          break;
        }
        case IDC_BUTTON2: { //vyber diffusni barvy svetla          
          DYN_LIGHT *p_light = (DYN_LIGHT *)tmp.p_pom1;
          doe_vyber_barvu_do_kolonek(hDlg, IDC_EDIT12, IDC_EDIT13, IDC_EDIT20, &p_light->sr);
          break;
        }
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {                 
          DYN_LIGHT *p_light = (DYN_LIGHT *)tmp.p_pom1;

          p_light->dr = doe_vyber_kolonku_float(hDlg,IDC_EDIT1);
          p_light->dg = doe_vyber_kolonku_float(hDlg,IDC_EDIT9);
          p_light->db = doe_vyber_kolonku_float(hDlg,IDC_EDIT10);

          p_light->sr = doe_vyber_kolonku_float(hDlg,IDC_EDIT12);
          p_light->sg = doe_vyber_kolonku_float(hDlg,IDC_EDIT13);
          p_light->sb = doe_vyber_kolonku_float(hDlg,IDC_EDIT20);

          p_light->ktrida = doe_vyber_kolonku_int(hDlg,IDC_EDIT11);
          
          p_light->np.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT2);
          p_light->np.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT3);
          p_light->np.z = doe_vyber_kolonku_float(hDlg,IDC_EDIT7);

          p_light->dosah = doe_vyber_kolonku_float(hDlg,IDC_EDIT4);
          p_light->min = doe_vyber_kolonku_float(hDlg,IDC_EDIT5);
          p_light->max = doe_vyber_kolonku_float(hDlg,IDC_EDIT15);

          GetDlgItemText(hDlg,IDC_EDIT8,p_light->jmeno,MAX_JMENO);

          p_light->flag = doe_get_check_flag(hDlg,IDC_RADIO4,p_light->flag,SDL_SUB);
          p_light->flag = doe_get_check_flag(hDlg,IDC_RADIO1,p_light->flag,SDL_UTLUM_LINEAR);
          p_light->flag = doe_get_check_flag(hDlg,IDC_RADIO5,p_light->flag,SDL_ADD_SUB);

          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    
    case WM_INITDIALOG: {
      #define FLOAT_FORMAT "%.3f"
      DYN_LIGHT *p_light = (DYN_LIGHT *)tmp.p_pom1;

      SetWindowText(hDlg,tmp.p_pom);

      doe_printf(hDlg,IDC_EDIT1,RGBF,p_light->dr);
      doe_printf(hDlg,IDC_EDIT9,RGBF,p_light->dg);
      doe_printf(hDlg,IDC_EDIT10,RGBF,p_light->db);

      doe_printf(hDlg,IDC_EDIT12,RGBF,p_light->sr);
      doe_printf(hDlg,IDC_EDIT13,RGBF,p_light->sg);
      doe_printf(hDlg,IDC_EDIT20,RGBF,p_light->sb);

      doe_printf(hDlg,IDC_EDIT11,"%d",p_light->ktrida);
      
      doe_printf(hDlg,IDC_EDIT2,FLOAT_FORMAT,p_light->np.x);
      doe_printf(hDlg,IDC_EDIT3,FLOAT_FORMAT,p_light->np.y);
      doe_printf(hDlg,IDC_EDIT7,FLOAT_FORMAT,p_light->np.z);

      doe_printf(hDlg,IDC_EDIT4, FLOAT_FORMAT,p_light->dosah);
      doe_printf(hDlg,IDC_EDIT5, FLOAT_FORMAT,p_light->min);
      doe_printf(hDlg,IDC_EDIT15,FLOAT_FORMAT,p_light->max);
      
      doe_printf(hDlg,IDC_EDIT8,p_light->jmeno);

      doe_check_stlac( hDlg, IDC_RADIO3,!(p_light->flag&(SDL_SUB|SDL_ADD_SUB)));
      doe_check_stlac( hDlg, IDC_RADIO4, (p_light->flag&SDL_SUB));
      doe_check_stlac( hDlg, IDC_RADIO5, (p_light->flag&SDL_ADD_SUB));

      doe_check_stlac( hDlg, IDC_RADIO1, (p_light->flag&SDL_UTLUM_LINEAR));
      doe_check_stlac( hDlg, IDC_RADIO2,!(p_light->flag&SDL_UTLUM_LINEAR));
      
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_prikaz_dynamicke_svetlo(HWND hwnd, byte *p_nadpis, DYN_LIGHT *p_light)
{
  tmp.p_pom = p_nadpis;
  tmp.p_pom1 = p_light;
  return(DialogBox(hinst,MAKEINTRESOURCE(IDD_SVETLO_DYNAMICKE),hwnd,doe_calback_dynamicke_svetlo));
}

// nastavi flag
void doe_lop_set_combo(HWND hDlg, int res, int akt)
{
  HWND combo = GetDlgItem(hDlg,res);
  int  vyber;

  if(akt&0x1)
    vyber = 0;
  else if(akt&0x2)
    vyber = 1;
  else if(akt&0x4)
    vyber = 2;
  else if(akt&0x8)
    vyber = 3;

  SendMessage( combo, CB_RESETCONTENT, 0, 0 );
  SendMessage( combo, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"Modulace");
  SendMessage( combo, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"Prepsani");
  SendMessage( combo, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"Pricteni");
  SendMessage( combo, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"Odecteni");
  SendMessage( combo, CB_SETCURSEL, vyber, 0 );
}

// vraci flag zalozeny na 0 zaklade
int doe_lop_get_combo(HWND hDlg, int res)
{
  HWND combo = GetDlgItem(hDlg,res);
  int  vyber = SendMessage( combo, CB_GETCURSEL, 0, 0 );

  if(!vyber)
    return(0x1);    
  else if(vyber == 1)
    return(0x2);
  else if(vyber == 2)
    return(0x4);
  else if(vyber == 3)
    return(0x8);
  return(0);
}

BOOL CALLBACK doe_calback_extra_dynamicke_svetlo(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON1: { //vyber barvy svetla
          EXTRA_DYN_LIGHT *p_light = (EXTRA_DYN_LIGHT *)tmp.p_pom1;
          /*
          dword barva = float_rgb(&p_light->dr);          
          if(doe_prikaz_vyber_barvu(p_cnf,hDlg,&barva)) {          
            rgb_float(barva,&p_light->dr);
            doe_printf(hDlg,IDC_EDIT1,RGBF,p_light->dr*255.0f);
            doe_printf(hDlg,IDC_EDIT13,RGBF,p_light->dg*255.0f);
            doe_printf(hDlg,IDC_EDIT23,RGBF,p_light->db*255.0f);
          }
          */
          break;
        }
        case IDC_BUTTON2: { //vyber spekularni barvy svetla
          EXTRA_DYN_LIGHT *p_light = (EXTRA_DYN_LIGHT *)tmp.p_pom1;
          dword barva;
          barva = float_rgb(&p_light->sr);
          if(doe_prikaz_vyber_barvu(p_cnf,hDlg,&barva)) {          
            rgb_float(barva,&p_light->sr);
            doe_printf(hDlg,IDC_EDIT8,RGBF,p_light->sr*255.0f);
            doe_printf(hDlg,IDC_EDIT20,RGBF,p_light->sg*255.0f);
            doe_printf(hDlg,IDC_EDIT41,RGBF,p_light->sb*255.0f);
          }
          break;
        }
        case IDC_RADIO3:
          {
            int check = IsDlgButtonChecked(hDlg,IDC_RADIO3);
            EnableDlgItem(hDlg,IDC_CHECK1,check);
            EnableDlgItem(hDlg,IDC_CHECK2,check);
            EnableDlgItem(hDlg,IDC_CHECK3,check);
          }
          break;
        case IDC_RADIO1:
          {
            int check = IsDlgButtonChecked(hDlg,IDC_RADIO3);
            EnableDlgItem(hDlg,IDC_CHECK1,check);
            EnableDlgItem(hDlg,IDC_CHECK2,check);
            EnableDlgItem(hDlg,IDC_CHECK3,check);
          }
          break;
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {
          EXTRA_DYN_LIGHT *p_light = (EXTRA_DYN_LIGHT *)tmp.p_pom1;

          GetDlgItemText(hDlg,IDC_EDIT5,p_light->jmeno,MAX_JMENO);
          if(IsDlgButtonChecked(hDlg,IDC_RADIO1)) {
            p_light->flag|=EDL_BODOVE;
            p_light->flag&=~(EDL_PLOSNE_X|EDL_PLOSNE_Y|EDL_PLOSNE_Z);
          } else {
            p_light->flag&=~EDL_BODOVE;
            p_light->flag = doe_get_check_flag(hDlg,IDC_CHECK1,p_light->flag,EDL_PLOSNE_X);
            p_light->flag = doe_get_check_flag(hDlg,IDC_CHECK2,p_light->flag,EDL_PLOSNE_Y);
            p_light->flag = doe_get_check_flag(hDlg,IDC_CHECK3,p_light->flag,EDL_PLOSNE_Z);
          }

          p_light->ktrida = doe_vyber_kolonku_int(hDlg,IDC_EDIT15);
/*
          p_light->flag = doe_get_check_flag(hDlg,IDC_CHECK11,p_light->flag,EDL_MESH_LIGHT);
          p_light->flag = doe_get_check_flag(hDlg,IDC_CHECK26,p_light->flag,EDL_SCENA);
          p_light->flag = doe_get_check_flag(hDlg,IDC_CHECK12,p_light->flag,EDL_PRUHL_LIGHT);
*/
          /*
          if(IsDlgButtonChecked(hDlg,IDC_CHECK5)) {
            int vybr = doe_lop_get_combo(hDlg, IDC_COMBO1);            
            p_light->flag &= ~(EDL_DIFF_MOD|EDL_DIFF_SET|EDL_DIFF_ADD|EDL_DIFF_SUB);
            p_light->flag |= vybr<<16;
            p_light->dr = doe_vyber_kolonku_float(hDlg,IDC_EDIT1)/255.0f;
            p_light->dg = doe_vyber_kolonku_float(hDlg,IDC_EDIT13)/255.0f;
            p_light->db = doe_vyber_kolonku_float(hDlg,IDC_EDIT23)/255.0f;            
          } else {
            p_light->flag&=~(EDL_DIFF_MOD|EDL_DIFF_SET|EDL_DIFF_ADD|EDL_DIFF_SUB);
          }
          */
          if(IsDlgButtonChecked(hDlg,IDC_CHECK28)) {
            int vybr = doe_lop_get_combo(hDlg, IDC_COMBO2);
            p_light->flag &= ~(EDL_ALFA_MOD|EDL_ALFA_SET|EDL_ALFA_ADD|EDL_ALFA_SUB);
            p_light->flag |= vybr<<20;
            p_light->da = doe_vyber_kolonku_float(hDlg,IDC_EDIT9)/255.0f;
          } else {
            p_light->flag&=~(EDL_ALFA_MOD|EDL_ALFA_SET|EDL_ALFA_ADD|EDL_ALFA_SUB);
          }
          if(IsDlgButtonChecked(hDlg,IDC_CHECK25)) {
            int vybr = doe_lop_get_combo(hDlg, IDC_COMBO4);
            p_light->flag &= ~(EDL_SPEC_MOD|EDL_SPEC_SET|EDL_SPEC_ADD|EDL_SPEC_SUB);
            p_light->flag |= vybr<<24;
            p_light->sr = doe_vyber_kolonku_float(hDlg,IDC_EDIT8)/255.0f;
            p_light->sg = doe_vyber_kolonku_float(hDlg,IDC_EDIT20)/255.0f;
            p_light->sb = doe_vyber_kolonku_float(hDlg,IDC_EDIT41)/255.0f;
          } else {
            p_light->flag&=~(EDL_SPEC_MOD|EDL_SPEC_SET|EDL_SPEC_ADD|EDL_SPEC_SUB);
          }
          

          p_light->np.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT2);
          p_light->np.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT3);
          p_light->np.z = doe_vyber_kolonku_float(hDlg,IDC_EDIT7);

          p_light->flag = doe_get_check_flag(hDlg,IDC_CHECK8, p_light->flag,EDL_DOSAH);
          p_light->dosah = doe_vyber_kolonku_float(hDlg,IDC_EDIT8);

          p_light->flag = doe_get_check_flag(hDlg,IDC_CHECK9, p_light->flag,EDL_GOURAND);
          p_light->gourand = doe_vyber_kolonku_float(hDlg,IDC_EDIT11);
          
          if(IsDlgButtonChecked(hDlg,IDC_CHECK10)) {            
            p_light->utlum = doe_vyber_kolonku_float(hDlg,IDC_EDIT12);
            p_light->flag &= ~(EDL_UTLUM_LIN|EDL_UTLUM_KVAD);
            if(IsDlgButtonChecked(hDlg,IDC_RADIO7)) {
              p_light->flag |= EDL_UTLUM_LIN;
            } else {
              p_light->flag |= EDL_UTLUM_KVAD;
            }
          }
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    
    case WM_INITDIALOG: {
      #define FLOAT_FORMAT "%.3f"
      EXTRA_DYN_LIGHT *p_light = (EXTRA_DYN_LIGHT *)tmp.p_pom1;
      
      SetWindowText(hDlg,tmp.p_pom);

      doe_printf(hDlg,IDC_EDIT5,p_light->jmeno);
      doe_printf(hDlg,IDC_EDIT15,"%d",p_light->ktrida);

      doe_check_stlac(hDlg,IDC_RADIO1,p_light->flag&EDL_BODOVE);
      doe_check_stlac(hDlg,IDC_RADIO3,p_light->flag&(EDL_PLOSNE_X|EDL_PLOSNE_Y|EDL_PLOSNE_Z));

      if(p_light->flag&(EDL_PLOSNE_X|EDL_PLOSNE_Y|EDL_PLOSNE_Z)) {
        doe_check_stlac(hDlg,IDC_CHECK1,p_light->flag&EDL_PLOSNE_X);
        doe_check_stlac(hDlg,IDC_CHECK2,p_light->flag&EDL_PLOSNE_Y);
        doe_check_stlac(hDlg,IDC_CHECK3,p_light->flag&EDL_PLOSNE_Z);
      } else {
        EnableDlgItem(hDlg,IDC_CHECK1,FALSE);
        EnableDlgItem(hDlg,IDC_CHECK2,FALSE);
        EnableDlgItem(hDlg,IDC_CHECK3,FALSE);
      }

/*
      doe_check_stlac(hDlg,IDC_CHECK11,p_light->flag&EDL_MESH_LIGHT);
      doe_check_stlac(hDlg,IDC_CHECK26,p_light->flag&EDL_SCENA);

      doe_check_stlac(hDlg,IDC_CHECK5, p_light->flag&(EDL_DIFF_MOD|EDL_DIFF_SET|EDL_DIFF_ADD|EDL_DIFF_SUB));
      doe_check_stlac(hDlg,IDC_CHECK25,p_light->flag&(EDL_SPEC_MOD|EDL_SPEC_SET|EDL_SPEC_ADD|EDL_SPEC_SUB));
      doe_check_stlac(hDlg,IDC_CHECK28,p_light->flag&(EDL_ALFA_MOD|EDL_ALFA_SET|EDL_ALFA_ADD|EDL_ALFA_SUB));      
*/
/*
      doe_printf(hDlg,IDC_EDIT1,RGBF,p_light->dr*255.0f);
      doe_printf(hDlg,IDC_EDIT13,RGBF,p_light->dg*255.0f);
      doe_printf(hDlg,IDC_EDIT23,RGBF,p_light->db*255.0f);
*/
      doe_printf(hDlg,IDC_EDIT8,RGBF,p_light->sr*255.0f);
      doe_printf(hDlg,IDC_EDIT20,RGBF,p_light->sg*255.0f);
      doe_printf(hDlg,IDC_EDIT41,RGBF,p_light->sb*255.0f);

      doe_printf(hDlg,IDC_EDIT9,RGBF,p_light->da*255.0f);

      doe_printf(hDlg,IDC_EDIT2,"%f",p_light->np.x);
      doe_printf(hDlg,IDC_EDIT3,"%f",p_light->np.y);
      doe_printf(hDlg,IDC_EDIT7,"%f",p_light->np.z);

      doe_check_stlac(hDlg,IDC_CHECK8,p_light->flag&EDL_DOSAH);
      doe_printf(hDlg,IDC_EDIT4,"%f",p_light->dosah);

      doe_check_stlac(hDlg,IDC_CHECK9,p_light->flag&EDL_GOURAND);
      doe_printf(hDlg,IDC_EDIT11,"%f",p_light->gourand);

      doe_check_stlac(hDlg,IDC_CHECK10,p_light->flag&(EDL_UTLUM_LIN|EDL_UTLUM_KVAD));
      doe_printf(hDlg,IDC_EDIT12,"%f",p_light->utlum);
      
      doe_check_stlac(hDlg,IDC_CHECK12,p_light->flag&EDL_PRUHL_LIGHT);

      doe_check_stlac(hDlg,IDC_RADIO7,p_light->flag&EDL_UTLUM_LIN);
      doe_check_stlac(hDlg,IDC_RADIO8,p_light->flag&EDL_UTLUM_KVAD);

      doe_lop_set_combo(hDlg, IDC_COMBO1, p_light->flag>>16);
      doe_lop_set_combo(hDlg, IDC_COMBO4, p_light->flag>>20);
      doe_lop_set_combo(hDlg, IDC_COMBO2, p_light->flag>>24);

      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_prikaz_extra_dynamicke_svetlo(HWND hwnd, byte *p_nadpis, EXTRA_DYN_LIGHT *p_light)
{
  tmp.p_pom = p_nadpis;
  tmp.p_pom1 = p_light;
  return(DialogBox(hinst,MAKEINTRESOURCE(IDD_SVETLO_EFEKTOVE),hwnd,doe_calback_extra_dynamicke_svetlo));
}


void _updatuj_combo_materialy(K_EDITOR *p_cnf, HWND hDlg, int mat, int combo)
{
  HWND hwnd1 = GetDlgItem( hDlg, combo);
  int  ak,i,list;  
  byte pom[100];
  
  SendMessage( hwnd1, CB_RESETCONTENT, 0, 0 );
  ak = 0;
  list = 0;
  for(i = 0; i < MAX_EDIT_MATERIALU; i++) {
    if(i == mat)
      ak = list;
    if(p_cnf->p_mat[i]) {
      if(p_cnf->p_mat[i]->jmeno[0])
        strcpy(pom,p_cnf->p_mat[i]->jmeno);
      else
        sprintf(pom,"Bezejmenec %d",i);
      SendMessage( hwnd1, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) pom);
      SendMessage( hwnd1, CB_SETITEMDATA, list, i);
      list++;
    }
  }
  if(p_cnf->p_mat[ak])
    SendMessage( hwnd1, CB_SETCURSEL, ak, 0 );
}

void _cti_combo_materialy(K_EDITOR *p_cnf, HWND hDlg, int *p_mat, int com2)
{  
  HWND hwnd2 = GetDlgItem( hDlg, com2);
  *p_mat = SendMessage( hwnd2, CB_GETITEMDATA, SendMessage( hwnd2, CB_GETCURSEL, 0, 0 ), 0);
}

static void doe_multi_flare_enable(HWND hDlg, int stav)
{
  
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT1),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_COMBO6),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT15),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT16),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT10),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON3),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON4),stav);
  
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT61),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_COMBO8),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT17),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT18),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT11),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON7),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON8),stav);

  EnableWindow(GetDlgItem(hDlg,IDC_EDIT62),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_COMBO10),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT19),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT24),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT12),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON10),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON11),stav);

  EnableWindow(GetDlgItem(hDlg,IDC_EDIT63),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_COMBO13),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT25),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT26),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT13),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON13),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON14),stav);

  EnableWindow(GetDlgItem(hDlg,IDC_EDIT64),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_COMBO15),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT44),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT45),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT20),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON15),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON16),stav);

  EnableWindow(GetDlgItem(hDlg,IDC_EDIT65),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_COMBO17),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT46),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT47),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT21),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON17),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON26),stav);

  EnableWindow(GetDlgItem(hDlg,IDC_EDIT66),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_COMBO19),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT48),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT49),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT22),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON24),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON27),stav);

  EnableWindow(GetDlgItem(hDlg,IDC_EDIT67),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_COMBO21),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT50),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT51),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT23),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON29),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON28),stav);

  EnableWindow(GetDlgItem(hDlg,IDC_EDIT68),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_COMBO23),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT52),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT53),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT41),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON30),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON31),stav);

  EnableWindow(GetDlgItem(hDlg,IDC_EDIT69),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_COMBO25),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT54),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT55),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT28),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON32),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON33),stav);

  EnableWindow(GetDlgItem(hDlg,IDC_EDIT70),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_COMBO27),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT8),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT29),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT42),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON34),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON35),stav);

  EnableWindow(GetDlgItem(hDlg,IDC_EDIT71),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_COMBO29),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT56),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT57),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT58),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON6),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON23),stav);

  EnableWindow(GetDlgItem(hDlg,IDC_EDIT72),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_COMBO31),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT59),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT60),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT31),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON36),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON37),stav);

  EnableWindow(GetDlgItem(hDlg,IDC_EDIT73),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_COMBO33),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT35),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT36),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_EDIT38),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON38),stav);
  EnableWindow(GetDlgItem(hDlg,IDC_BUTTON39),stav);
}

void doe_set_multi_flare_default(HWND hDlg, int edv, int com2, int ed1, int ed2, int ed3)
{
  byte pom[200];

  sprintf(pom,FLOAT_FORMAT,0.0f);
  SetWindowText(GetDlgItem(hDlg,edv),pom);
   
  _updatuj_combo_materialy(p_cnf, hDlg, 0,com2);

  // dx
  sprintf(pom,FLOAT_FORMAT,1.0f);
  SetWindowText(GetDlgItem(hDlg,ed1),pom);
  
  // dy
  sprintf(pom,FLOAT_FORMAT,1.0f);
  SetWindowText(GetDlgItem(hDlg,ed2),pom);
  
  // barva      
  SetDlgItemText( hDlg, ed3, itoa(0x50ffffff,pom,16));
}

void doe_set_multi_flare(HWND hDlg, LENS_FLARE_SLOZ *p_sloz, int edv, int com2, int ed1, int ed2, int ed3)
{  
  byte pom[200];  
    
  sprintf(pom,FLOAT_FORMAT,p_sloz->vzdal);
  SetDlgItemText(hDlg,edv,pom);

  _updatuj_combo_materialy(p_cnf, hDlg, p_sloz->material,com2);  

  // dx
  sprintf(pom,FLOAT_FORMAT,p_sloz->dx);
  SetWindowText(GetDlgItem(hDlg,ed1),pom);
  
  // dy
  sprintf(pom,FLOAT_FORMAT,p_sloz->dy);
  SetWindowText(GetDlgItem(hDlg,ed2),pom);
  
  // barva      
  SetDlgItemText( hDlg, ed3, itoa(float_rgba(&p_sloz->r),pom,16));
}

void doe_get_multi_flare(HWND hDlg, LENS_FLARE_SLOZ *p_sloz, int edv, int com2, int ed1, int ed2, int ed3)
{  
  dword barva;
  byte  pom[200];
  
  GetWindowText(GetDlgItem(hDlg,edv),pom,59);
  p_sloz->vzdal = (float)atof(pom);

  _cti_combo_materialy(p_cnf, hDlg, &p_sloz->material,com2);
  p_sloz->p_mat = p_cnf->p_mat[p_sloz->material];

  // dx
  GetWindowText(GetDlgItem(hDlg,ed1),pom,59);
  p_sloz->dx = (float)atof(pom);  
  
  // dy
  GetWindowText(GetDlgItem(hDlg,ed2),pom,59);
  p_sloz->dy = (float)atof(pom);
  
  // barva
  GetWindowText(GetDlgItem(hDlg,ed3),pom,59);
  sscanf(pom,"%x",&barva);
  rgba_float(barva,&p_sloz->r);
}

static void doe_flare_vloz_barvu_svetla(HWND hDlg, int com1, int ed1)
{
  HWND  hwnd = GetDlgItem( hDlg, com1);
  byte  pom[200];     
  SetDlgItemText( hDlg, ed1, itoa(float_rgb(&p_cnf->light[SendMessage(hwnd,CB_GETCURSEL,0,0)].r),pom,16));
}

BOOL CALLBACK doe_calback_flare(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON1: { // vyber barvy
          LENS_FLARE *p_flare = (LENS_FLARE *)tmp.p_pom1;
          rgba_float(doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT9),&p_flare->r);
          break;
        }
        case IDC_BUTTON2: { // vloz barvu ze svetla
          LENS_FLARE *p_flare = (LENS_FLARE *)tmp.p_pom1;
          HWND hwnd = GetDlgItem( hDlg, IDC_COMBO1 );
          int  ind = SendMessage(hwnd,CB_GETCURSEL,0,0);
          p_flare->r = p_cnf->light[ind].r;
          p_flare->g = p_cnf->light[ind].g;
          p_flare->b = p_cnf->light[ind].b;
          break;
        }
        case IDC_BUTTON4:
          doe_flare_vloz_barvu_svetla(hDlg, IDC_COMBO1, IDC_EDIT10);
          break;
        case IDC_BUTTON8:
          doe_flare_vloz_barvu_svetla( hDlg, IDC_COMBO1, IDC_EDIT11);
          break;
        case IDC_BUTTON11:
          doe_flare_vloz_barvu_svetla( hDlg, IDC_COMBO1, IDC_EDIT12);
          break;
        case IDC_BUTTON14:
          doe_flare_vloz_barvu_svetla( hDlg, IDC_COMBO1, IDC_EDIT13);
          break;
        case IDC_BUTTON16:
          doe_flare_vloz_barvu_svetla( hDlg, IDC_COMBO1, IDC_EDIT20);
          break;
        case IDC_BUTTON26:
          doe_flare_vloz_barvu_svetla( hDlg, IDC_COMBO1, IDC_EDIT21);
          break;
        case IDC_BUTTON27:
          doe_flare_vloz_barvu_svetla( hDlg, IDC_COMBO1, IDC_EDIT22);
          break;
        case IDC_BUTTON28:
          doe_flare_vloz_barvu_svetla( hDlg, IDC_COMBO1, IDC_EDIT23);
          break;
        case IDC_BUTTON31:
          doe_flare_vloz_barvu_svetla( hDlg, IDC_COMBO1, IDC_EDIT41);
          break;
        case IDC_BUTTON33:
          doe_flare_vloz_barvu_svetla( hDlg, IDC_COMBO1, IDC_EDIT28);
          break;
        case IDC_BUTTON35:
          doe_flare_vloz_barvu_svetla( hDlg, IDC_COMBO1, IDC_EDIT42);
          break;
        case IDC_BUTTON23:
          doe_flare_vloz_barvu_svetla( hDlg, IDC_COMBO1, IDC_EDIT58);
          break;
        case IDC_BUTTON37:
          doe_flare_vloz_barvu_svetla( hDlg, IDC_COMBO1, IDC_EDIT31);
          break;
        case IDC_BUTTON39:
          doe_flare_vloz_barvu_svetla( hDlg, IDC_COMBO1, IDC_EDIT38);
          break;


        case IDC_BUTTON3:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT10);
          break;
        case IDC_BUTTON7:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT11);
          break;
        case IDC_BUTTON10:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT12);
          break;
        case IDC_BUTTON13:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT13);
          break;
        case IDC_BUTTON15:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT20);
          break;
        case IDC_BUTTON17:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT21);
          break;
        case IDC_BUTTON24:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT22);
          break;
        case IDC_BUTTON29:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT23);
          break;
        case IDC_BUTTON30:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT41);
          break;
        case IDC_BUTTON32:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT28);
          break;
        case IDC_BUTTON34:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT42);
          break;
        case IDC_BUTTON6:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT58);
          break;
        case IDC_BUTTON36:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT31);
          break;
        case IDC_BUTTON38:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT38);
          break;
        
        case IDC_CHECK21:
          doe_multi_flare_enable(hDlg,IsDlgButtonChecked( hDlg, IDC_CHECK21));
          if(IsDlgButtonChecked( hDlg, IDC_CHECK21)) {
            doe_set_multi_flare_default(hDlg, IDC_EDIT1,  IDC_COMBO6,  IDC_EDIT15, IDC_EDIT16, IDC_EDIT10);
            doe_set_multi_flare_default(hDlg, IDC_EDIT61,  IDC_COMBO8,  IDC_EDIT17, IDC_EDIT18, IDC_EDIT11);
            doe_set_multi_flare_default(hDlg, IDC_EDIT62, IDC_COMBO10, IDC_EDIT19, IDC_EDIT24, IDC_EDIT12);
            doe_set_multi_flare_default(hDlg, IDC_EDIT63, IDC_COMBO13, IDC_EDIT25, IDC_EDIT26, IDC_EDIT13);
            doe_set_multi_flare_default(hDlg, IDC_EDIT64, IDC_COMBO15, IDC_EDIT44, IDC_EDIT45, IDC_EDIT20);
            doe_set_multi_flare_default(hDlg, IDC_EDIT65, IDC_COMBO17, IDC_EDIT46, IDC_EDIT47, IDC_EDIT21);
            doe_set_multi_flare_default(hDlg, IDC_EDIT66, IDC_COMBO19, IDC_EDIT48, IDC_EDIT49, IDC_EDIT22);
            doe_set_multi_flare_default(hDlg, IDC_EDIT67, IDC_COMBO21, IDC_EDIT50, IDC_EDIT51, IDC_EDIT23);
            doe_set_multi_flare_default(hDlg, IDC_EDIT68, IDC_COMBO23, IDC_EDIT52, IDC_EDIT53, IDC_EDIT41);
            doe_set_multi_flare_default(hDlg, IDC_EDIT69, IDC_COMBO25, IDC_EDIT54, IDC_EDIT55, IDC_EDIT28);
            doe_set_multi_flare_default(hDlg, IDC_EDIT70, IDC_COMBO27, IDC_EDIT8,  IDC_EDIT29, IDC_EDIT42);
            doe_set_multi_flare_default(hDlg, IDC_EDIT71, IDC_COMBO29, IDC_EDIT56, IDC_EDIT57, IDC_EDIT58);
            doe_set_multi_flare_default(hDlg, IDC_EDIT72, IDC_COMBO31, IDC_EDIT59, IDC_EDIT60, IDC_EDIT31);
            doe_set_multi_flare_default(hDlg, IDC_EDIT73, IDC_COMBO33, IDC_EDIT35, IDC_EDIT36, IDC_EDIT38);
          }
          break;
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {
          LENS_FLARE *p_flare = (LENS_FLARE *)tmp.p_pom1;
          HWND hwnd = GetDlgItem( hDlg, IDC_COMBO1 );
          int  sv;
          byte pom[60];
          dword barva;

          // dx
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT2),pom,59);
          p_flare->dx = (float)atof(pom);

          // dy
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT3),pom,59);
          p_flare->dy = (float)atof(pom);

          // x,y,z
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT4),pom,59);
          p_flare->p.x = (float)atof(pom);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT5),pom,59);
          p_flare->p.y = (float)atof(pom);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT7),pom,59);
          p_flare->p.z = (float)atof(pom);

          // barva
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT9),pom,59);
          sscanf(pom,"%x",&barva);
          rgba_float(barva,&p_flare->r);

          if(IsDlgButtonChecked( hDlg, IDC_RADIO2)) {            
            sv = SendMessage(hwnd,CB_GETITEMDATA,SendMessage(hwnd,CB_GETCURSEL,0,0),0);
            p_flare->p_svetlo = p_cnf->light+sv;
            p_cnf->light[sv].p_flare = p_cnf->flare+p_flare->index;
          } else {
            p_flare->p_svetlo = NULL;
          }
          
          _cti_combo_materialy(p_cnf, hDlg, &p_flare->material,IDC_COMBO4);
          p_flare->p_mat = p_cnf->p_mat[p_flare->material];
          
          if(IsDlgButtonChecked( hDlg, IDC_CHECK21)) {
            if(!p_flare->p_sloz) { // naalokuj na to pamet
              if(!(p_flare->p_sloz = malloc(sizeof(p_flare->p_sloz[0])*SLOZ_FLARE)))
                chyba("Pamet");
              memset(p_flare->p_sloz,0,sizeof(p_flare->p_sloz[0])*SLOZ_FLARE);
            }
            // nastrkej to tam
            doe_get_multi_flare(hDlg, p_flare->p_sloz,    IDC_EDIT1, IDC_COMBO6, IDC_EDIT15, IDC_EDIT16, IDC_EDIT10);
            doe_get_multi_flare(hDlg, p_flare->p_sloz+1,  IDC_EDIT61, IDC_COMBO8, IDC_EDIT17, IDC_EDIT18, IDC_EDIT11);
            doe_get_multi_flare(hDlg, p_flare->p_sloz+2,  IDC_EDIT62, IDC_COMBO10, IDC_EDIT19, IDC_EDIT24, IDC_EDIT12);
            doe_get_multi_flare(hDlg, p_flare->p_sloz+3,  IDC_EDIT63, IDC_COMBO13, IDC_EDIT25, IDC_EDIT26, IDC_EDIT13);
            doe_get_multi_flare(hDlg, p_flare->p_sloz+4,  IDC_EDIT64, IDC_COMBO15, IDC_EDIT44, IDC_EDIT45, IDC_EDIT20);
            doe_get_multi_flare(hDlg, p_flare->p_sloz+5,  IDC_EDIT65, IDC_COMBO17, IDC_EDIT46, IDC_EDIT47, IDC_EDIT21);
            doe_get_multi_flare(hDlg, p_flare->p_sloz+6,  IDC_EDIT66, IDC_COMBO19, IDC_EDIT48, IDC_EDIT49, IDC_EDIT22);
            doe_get_multi_flare(hDlg, p_flare->p_sloz+7,  IDC_EDIT67, IDC_COMBO21, IDC_EDIT50, IDC_EDIT51, IDC_EDIT23);
            doe_get_multi_flare(hDlg, p_flare->p_sloz+8,  IDC_EDIT68, IDC_COMBO23, IDC_EDIT52, IDC_EDIT53, IDC_EDIT41);
            doe_get_multi_flare(hDlg, p_flare->p_sloz+9,  IDC_EDIT69, IDC_COMBO25, IDC_EDIT54, IDC_EDIT55, IDC_EDIT28);
            doe_get_multi_flare(hDlg, p_flare->p_sloz+10, IDC_EDIT70, IDC_COMBO27, IDC_EDIT8,  IDC_EDIT29, IDC_EDIT42);
            doe_get_multi_flare(hDlg, p_flare->p_sloz+11, IDC_EDIT71, IDC_COMBO29, IDC_EDIT56, IDC_EDIT57, IDC_EDIT58);
            doe_get_multi_flare(hDlg, p_flare->p_sloz+12, IDC_EDIT72, IDC_COMBO31, IDC_EDIT59, IDC_EDIT60, IDC_EDIT31);
            doe_get_multi_flare(hDlg, p_flare->p_sloz+13, IDC_EDIT73, IDC_COMBO33, IDC_EDIT35, IDC_EDIT36, IDC_EDIT38);
          } else {
            null_free(&p_flare->p_sloz);            
          }

          p_flare->alfa = IsDlgButtonChecked( hDlg, IDC_CHECK22) ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA;
          p_flare->zflag = doe_get_check_flag(hDlg, IDC_CHECK41, p_flare->zflag, FLR_NO_ZTEST);
          
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    
    case WM_INITDIALOG: {
      #define FLOAT_FORMAT "%.3f"
      byte pom[60];
      LENS_FLARE *p_flare = (LENS_FLARE *)tmp.p_pom1;
      SetWindowText(hDlg,tmp.p_pom);
            
      // dx
      sprintf(pom,FLOAT_FORMAT,p_flare->dx);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT2),pom);

      // dy
      sprintf(pom,FLOAT_FORMAT,p_flare->dy);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT3),pom);

      // x,y,z
      sprintf(pom,FLOAT_FORMAT,p_flare->p.x);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT4),pom);
      sprintf(pom,FLOAT_FORMAT,p_flare->p.y);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT5),pom);
      sprintf(pom,FLOAT_FORMAT,p_flare->p.z);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT7),pom);

      // barva      
      SetDlgItemText( hDlg, IDC_EDIT9, itoa(float_rgba(&p_flare->r),pom,16));

      CheckDlgButton( hDlg, IDC_RADIO3,(int)!p_flare->p_svetlo);
      CheckDlgButton( hDlg, IDC_RADIO2, (int)p_flare->p_svetlo);
      CheckDlgButton( hDlg, IDC_CHECK21,p_flare->p_sloz ? TRUE : FALSE);

      CheckDlgButton( hDlg, IDC_CHECK22, p_flare->alfa == GL_ONE ? TRUE : FALSE);
      CheckDlgButton( hDlg, IDC_CHECK41, p_flare->zflag&FLR_NO_ZTEST ? BST_CHECKED : BST_UNCHECKED);
      
      // IDC_COMBO4 - seznam svetel
      doe_vloz_stat_svetla_combo(hDlg,p_flare->p_svetlo ? p_flare->p_svetlo->index : FALSE);

      // vlozi seznam matrosu
      _updatuj_combo_materialy(p_cnf, hDlg, p_flare->material,IDC_COMBO4);
      
      doe_multi_flare_enable(hDlg,(int)p_flare->p_sloz);
      if(p_flare->p_sloz) {
        // nastrkej to tam
        doe_set_multi_flare(hDlg, p_flare->p_sloz,    IDC_EDIT1, IDC_COMBO6, IDC_EDIT15, IDC_EDIT16, IDC_EDIT10);
        doe_set_multi_flare(hDlg, p_flare->p_sloz+1,  IDC_EDIT61, IDC_COMBO8, IDC_EDIT17, IDC_EDIT18, IDC_EDIT11);
        doe_set_multi_flare(hDlg, p_flare->p_sloz+2,  IDC_EDIT62, IDC_COMBO10, IDC_EDIT19, IDC_EDIT24, IDC_EDIT12);
        doe_set_multi_flare(hDlg, p_flare->p_sloz+3,  IDC_EDIT63, IDC_COMBO13, IDC_EDIT25, IDC_EDIT26, IDC_EDIT13);
        doe_set_multi_flare(hDlg, p_flare->p_sloz+4,  IDC_EDIT64, IDC_COMBO15, IDC_EDIT44, IDC_EDIT45, IDC_EDIT20);
        doe_set_multi_flare(hDlg, p_flare->p_sloz+5,  IDC_EDIT65, IDC_COMBO17, IDC_EDIT46, IDC_EDIT47, IDC_EDIT21);
        doe_set_multi_flare(hDlg, p_flare->p_sloz+6,  IDC_EDIT66, IDC_COMBO19, IDC_EDIT48, IDC_EDIT49, IDC_EDIT22);
        doe_set_multi_flare(hDlg, p_flare->p_sloz+7,  IDC_EDIT67, IDC_COMBO21, IDC_EDIT50, IDC_EDIT51, IDC_EDIT23);
        doe_set_multi_flare(hDlg, p_flare->p_sloz+8,  IDC_EDIT68, IDC_COMBO23, IDC_EDIT52, IDC_EDIT53, IDC_EDIT41);
        doe_set_multi_flare(hDlg, p_flare->p_sloz+9,  IDC_EDIT69, IDC_COMBO25, IDC_EDIT54, IDC_EDIT55, IDC_EDIT28);
        doe_set_multi_flare(hDlg, p_flare->p_sloz+10, IDC_EDIT70, IDC_COMBO27, IDC_EDIT8,  IDC_EDIT29, IDC_EDIT42);
        doe_set_multi_flare(hDlg, p_flare->p_sloz+11, IDC_EDIT71, IDC_COMBO29, IDC_EDIT56, IDC_EDIT57, IDC_EDIT58);
        doe_set_multi_flare(hDlg, p_flare->p_sloz+12, IDC_EDIT72, IDC_COMBO31, IDC_EDIT59, IDC_EDIT60, IDC_EDIT31);
        doe_set_multi_flare(hDlg, p_flare->p_sloz+13, IDC_EDIT73, IDC_COMBO33, IDC_EDIT35, IDC_EDIT36, IDC_EDIT38);
      }
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_prikaz_flare(HWND hwnd, byte *p_nadpis, LENS_FLARE *p_flare)
{
  tmp.p_pom = p_nadpis;
  tmp.p_pom1 = p_flare;
  return(DialogBox(hinst,MAKEINTRESOURCE(IDD_LENS_FLARES),hwnd,doe_calback_flare));
}


BOOL CALLBACK doe_calback_texturace_2d(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {                 
          byte pom[10];
          mmp.i[0] = IsDlgButtonChecked(hDlg,IDC_RADIO1);
          mmp.i[1] = IsDlgButtonChecked(hDlg,IDC_RADIO2);
          mmp.i[2] = IsDlgButtonChecked(hDlg,IDC_RADIO3);
          mmp.i[3] = IsDlgButtonChecked(hDlg,IDC_RADIO4);
          GetDlgItemText(hDlg,IDC_EDIT1,pom,10);
          mmp.i[5] = atoi(pom);
          GetDlgItemText(hDlg,IDC_EDIT2,pom,10);
          mmp.i[6] = atoi(pom);
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      byte pom[30];
      CheckDlgButton( hDlg, IDC_RADIO1, mmp.i[0]);
      CheckDlgButton( hDlg, IDC_RADIO2, mmp.i[1]);
      CheckDlgButton( hDlg, IDC_RADIO3, mmp.i[2]);
      CheckDlgButton( hDlg, IDC_RADIO4, mmp.i[3]);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT1),itoa(mmp.i[5],pom,10));
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT2),itoa(mmp.i[6],pom,10));
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}


void doe_prikaz_texturace_2d(K_EDITOR *p_cnf, HWND hwnd, int fofr)
{
  static first_run = 1;
  EDIT_KONTEJNER *p_kont;
  int i;
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;

  if(first_run) {
    memset(&mmp,0,sizeof(mmp));
    mmp.i[1] = TRUE;
    mmp.i[5] = 0;
    first_run = 0;
  }
 
  if(fofr || DialogBox( hinst, MAKEINTRESOURCE(IDD_2DTEXT_TRANSF), hwnd, doe_calback_texturace_2d)) {
    if(mmp.i[0]) { // je to na vybrane body
      oe_transformuj_2d_texture_tagy_body(p_cnf, (byte)mmp.i[5]);
    } else if(mmp.i[1]) { // je to na vybrany objekt
      n_objekt(k,o);
      oe_transformuj_2d_texture_tagy(p_cnf, k, o, (byte)mmp.i[5]);
    } else if(mmp.i[2]) { // je to na vybrany kontejner
      n_kont(k);
      p_kont = p_cnf->p_kont[k];
      for(i = 0; i < p_kont->max_objektu; i++) {
        if(p_kont->p_obj[i])
          oe_transformuj_2d_texture_tagy(p_cnf, k, i, (byte)mmp.i[5]);
      }
    } else if(mmp.i[3]) { // je to na vybrany face
      n_objekt(k,o);
      oe_prikaz_vyber_vertex_face(p_cnf);
      oe_transformuj_2d_texture_tagy_body(p_cnf, (byte)mmp.i[5]);
    }
  }
}

BOOL CALLBACK doe_calback_load_flags(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {                 
          int stav = tmp.i[0];
          
          stav = doe_get_check_flag(hDlg,IDC_CHECK1, stav, LOAD_GEOM);
          stav = doe_get_check_flag(hDlg,IDC_CHECK2, stav, LOAD_MATERIALY);
          stav = doe_get_check_flag(hDlg,IDC_CHECK3, stav, LOAD_TEXTURY);
          stav = doe_get_check_flag(hDlg,IDC_CHECK4, stav, LOAD_STAT_LIGHT);
          stav = doe_get_check_flag(hDlg,IDC_CHECK5, stav, LOAD_DYN_LIGHT);
          stav = doe_get_check_flag(hDlg,IDC_CHECK25,stav, LOAD_EXTRA_DYN_LIGHT);
          stav = doe_get_check_flag(hDlg,IDC_CHECK28,stav, LOAD_AMBIENT);
          stav = doe_get_check_flag(hDlg,IDC_CHECK35,stav, LOAD_MLHA);
          stav = doe_get_check_flag(hDlg,IDC_CHECK36,stav, LOAD_ZRCADLO);
          stav = doe_get_check_flag(hDlg,IDC_CHECK37,stav, LOAD_FLARE);
          stav = doe_get_check_flag(hDlg,IDC_CHECK38,stav, LOAD_LEVEL_ENV);
          stav = doe_get_check_flag(hDlg,IDC_CHECK39,stav, LOAD_LEVEL_CONF);
          stav = doe_get_check_flag(hDlg,IDC_CHECK40,stav, LOAD_KAMERY);
          stav = doe_get_check_flag(hDlg,IDC_CHECK12,stav, SAVE_GEOM_VYBR);
          
          tmp.i[0] = stav;

          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      int stav = tmp.i[0];
      
      doe_check_stlac(hDlg,IDC_CHECK1, stav&LOAD_GEOM);
      doe_check_stlac(hDlg,IDC_CHECK12,stav&SAVE_GEOM_VYBR);
      doe_check_stlac(hDlg,IDC_CHECK2, stav&LOAD_MATERIALY);
      doe_check_stlac(hDlg,IDC_CHECK3, stav&LOAD_TEXTURY);
      doe_check_stlac(hDlg,IDC_CHECK4, stav&LOAD_STAT_LIGHT);
      doe_check_stlac(hDlg,IDC_CHECK5, stav&LOAD_DYN_LIGHT);
      doe_check_stlac(hDlg,IDC_CHECK25,stav&LOAD_EXTRA_DYN_LIGHT);
      doe_check_stlac(hDlg,IDC_CHECK28,stav&LOAD_AMBIENT);
      doe_check_stlac(hDlg,IDC_CHECK35,stav&LOAD_MLHA);
      doe_check_stlac(hDlg,IDC_CHECK36,stav&LOAD_ZRCADLO);
      doe_check_stlac(hDlg,IDC_CHECK37,stav&LOAD_FLARE);
      doe_check_stlac(hDlg,IDC_CHECK38,stav&LOAD_LEVEL_ENV);
      doe_check_stlac(hDlg,IDC_CHECK39,stav&LOAD_LEVEL_CONF);
      doe_check_stlac(hDlg,IDC_CHECK40,stav&LOAD_KAMERY);

      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_prikaz_load_flags(K_EDITOR *p_cnf)
{
  tmp.i[0] = p_cnf->cnf.load;
  if(DialogBox( hinst, MAKEINTRESOURCE(IDD_LOAD_FLAGS), hwnd_aplikace, doe_calback_load_flags)) {
    p_cnf->cnf.load = tmp.i[0];
  }
}


static EDIT_MATERIAL *p_lmm;

void doe_prikaz_posun_material_lista(K_EDITOR *p_cnf, int smer)
{
 // -1 == dolu
 //  1 == nahoru
  int max;

  if(!p_cnf->km.aktivni)
    return;

  max = SendMessage(p_cnf->km.hwnd_list,LB_GETCOUNT,0,0);

  if(smer > 0) { // nahoru          
    if(p_cnf->km.vybr_polozka+1 < max)
      p_cnf->km.vybr_polozka += 1;
    else
      return;
  }
  else { //dolu
    if(p_cnf->km.vybr_polozka-1 >= 0)
      p_cnf->km.vybr_polozka -= 1;
    else
      return;
  }
  SendMessage(p_cnf->km.hwnd_list,LB_SETCURSEL,p_cnf->km.vybr_polozka,0);
  p_cnf->km.vybr_mat = SendMessage((HWND)p_cnf->km.hwnd_list,LB_GETITEMDATA,p_cnf->km.vybr_polozka,0);
  if(p_lmm)
    zrus_material(&p_lmm);
  p_lmm = kopiruj_material(p_cnf->p_mat[p_cnf->km.vybr_mat]);  
  zobraz_akt_material(p_cnf->km.hwnd_dialog, p_lmm);
}

void doe_updatuj_lista_objekty_last(K_EDITOR *p_cnf)
{
  HWND hDlg = p_cnf->kk.hwnd_dialog;  
  byte pom[100];

  if(p_cnf->kk.aktivni) {
    SetDlgItemText( hDlg, IDC_EDIT1, itoa(p_cnf->vybr_kont_last,pom,10));
    SetDlgItemText( hDlg, IDC_EDIT9, itoa(p_cnf->vybr_objekt_last,pom,10));
    if(p_cnf->p_kont[p_cnf->vybr_kont_last]) {
      SetDlgItemText( hDlg, IDC_EDIT8, p_cnf->p_kont[p_cnf->vybr_kont_last]->jmeno);
      if(p_cnf->p_kont[p_cnf->vybr_kont_last]->p_obj[p_cnf->vybr_objekt_last])
        SetDlgItemText( hDlg, IDC_EDIT13, p_cnf->p_kont[p_cnf->vybr_kont_last]->p_obj[p_cnf->vybr_objekt_last]->jmeno);
      else
        SetDlgItemText( hDlg, IDC_EDIT13, "Nic");
    } else {
      SetDlgItemText( hDlg, IDC_EDIT8, "Nic");
      SetDlgItemText( hDlg, IDC_EDIT13, "Nic");
    }
  }
}

void doe_updatuj_lista_objekty(K_EDITOR *p_cnf)
{
  HWND hDlg = p_cnf->kk.hwnd_dialog;
  HWND hwnd1 = GetDlgItem( hDlg, IDC_COMBO1);
  HWND hwnd2 = GetDlgItem( hDlg, IDC_COMBO4);   
  EDIT_KONTEJNER *p_kont;
  int  ak,i,list;  
  byte pom[100];

  if(p_cnf->kk.aktivni) {
    SendMessage( hwnd1, CB_RESETCONTENT, 0, 0 );
    ak = 0;
    list = 0;
    for(i = 0; i < p_cnf->max_kont; i++) {
      if(i == p_cnf->vybr_kont)
        ak = list;
      if(p_cnf->p_kont[i]) {
        if(p_cnf->p_kont[i]->jmeno[0])
          strcpy(pom,p_cnf->p_kont[i]->jmeno);
        else
          sprintf(pom,"Bezejmenec %d",i);
        SendMessage( hwnd1, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) pom);        
        SendMessage( hwnd1, CB_SETITEMDATA, list, i);
        list++;
      }
    }
    if(p_cnf->p_kont[ak])
      SendMessage( hwnd1, CB_SETCURSEL, ak, 0 );
    
    p_kont = p_cnf->p_kont[p_cnf->vybr_kont];
    SendMessage( hwnd2, CB_RESETCONTENT, 0, 0 );
    if(p_kont) {
      ak = 0;
      list = 0;
      for(i = 0; i < MAX_KONT_OBJEKTU; i++) {
        if(i == p_cnf->vybr_objekt)
          ak = list;
        if(p_kont->p_obj[i]) {
          if(p_kont->p_obj[i]->jmeno[0])
            strcpy(pom,p_kont->p_obj[i]->jmeno);            
          else
            sprintf(pom,"Bezejmenec %d",i);
          SendMessage( hwnd2, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) pom);
          SendMessage( hwnd2, CB_SETITEMDATA, list, i);
          list++;
        }
      }
      if(p_kont->p_obj[ak])
        SendMessage( hwnd2, CB_SETCURSEL, ak, 0 );
    }
    doe_printf(hDlg,IDC_EDIT2,"%d",p_cnf->vybr_face/3);

    /* Last objekty
    */   
    doe_updatuj_lista_objekty_last(p_cnf);
  }  
}


TABLE_LIST bl;
KRESLIT_VECI kres = {1,1,1,1,1};

#define nastav_pozici_bl() SetWindowPos(hDlg, HWND_TOP, bl.rc.left, bl.rc.top, 0, 0, SWP_NOSIZE)

BOOL CALLBACK doe_calback_konfigurace_objekty(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON1:
          doe_dialog_objekty();
          break;
        case IDC_BUTTON2:
          doe_statistika_sceny(p_cnf);
          break;
        case IDC_BUTTON6:
          oe_prikaz_rozdrob_kontejner(p_cnf);
          break;
        case IDC_BUTTON5:
          oe_prikaz_rozdrob_objekt(p_cnf,FALSE);
          break;
        case IDC_BUTTON4:
          oe_prikaz_rozdrob_objekt(p_cnf,TRUE);
          break;
        case IDC_CHECK1:
          p_cnf->cnf.animace = IsDlgButtonChecked(hDlg,IDC_CHECK1);
          if(p_cnf->cnf.animace)
            oe_init_animace_materialy(p_cnf);
          break;
        case IDC_CHECK2:
          p_cnf->deka.aktivni = IsDlgButtonChecked(hDlg,IDC_CHECK2);
          break;
        case IDC_BUTTON3:
          oe_animuj_materialy(p_cnf, TRUE);
          break;        
        case IDC_COMBO1: // zmena vyberu kontejneru
          if(HIWORD(wParam) ==  CBN_SELCHANGE) {
            HWND hCombo1 = GetDlgItem(hDlg,IDC_COMBO1);            
            p_cnf->vybr_kont_last = p_cnf->vybr_kont;
            p_cnf->vybr_kont = SendMessage( hCombo1, CB_GETITEMDATA, SendMessage( hCombo1, CB_GETCURSEL, 0, 0 ), 0 );
            doe_updatuj_lista_objekty_last(p_cnf);
            updatuj_obalku_kontejner(p_cnf->p_kont[p_cnf->vybr_kont]);
          }
          break;
        case IDC_COMBO4:
          if(HIWORD(wParam) ==  CBN_SELCHANGE) {
            HWND hCombo2 = GetDlgItem(hDlg,IDC_COMBO4);            
            p_cnf->vybr_objekt_last = p_cnf->vybr_objekt;
            p_cnf->vybr_objekt = SendMessage( hCombo2, CB_GETITEMDATA, SendMessage( hCombo2, CB_GETCURSEL, 0, 0 ), 0 );
            doe_updatuj_lista_objekty_last(p_cnf);
          }
          break;
        case IDC_EDIT10: {
          byte pom[100];
          GetDlgItemText(hDlg,IDC_EDIT10,pom,100);
          p_cnf->cnf.kdtree_bunka = p_cnf->drobic_hloubka = atoi(pom);
          if(p_cnf->drobic_hloubka < 1) {
            p_cnf->drobic_hloubka = 1;            
            SetDlgItemText(hDlg,IDC_EDIT10,"1");
          }
          break;
        }
        case IDC_EDIT2:
          if(HIWORD(wParam) == EN_CHANGE) {
             p_cnf->vybr_face = doe_vyber_kolonku_int(hDlg,IDC_EDIT2)*3;
          }
          break;
        case IDC_RADIO1:
          p_cnf->groupflag = GROUP_KONT;
          oe_ungroup_face_all(p_cnf);
          break;
        case IDC_RADIO2:
          p_cnf->groupflag = GROUP_KONT|GROUP_OBJ;
          oe_ungroup_face_all(p_cnf);
          break;
        case IDC_RADIO3:
          p_cnf->groupflag = GROUP_KONT|GROUP_OBJ|GROUP_FACE;
          oe_ungroup_face_all(p_cnf);
          break;
        case IDC_CHECK3:
          p_cnf->cnf.oznacovat = doe_get_check_flag(hDlg,IDC_CHECK3,p_cnf->cnf.oznacovat,OZNACOVAT_MESH);
          break;
        case IDC_CHECK4:
          p_cnf->cnf.oznacovat = doe_get_check_flag(hDlg,IDC_CHECK4,p_cnf->cnf.oznacovat,OZNACOVAT_KOSTI);
          break;
        case IDC_CHECK5:
          p_cnf->cnf.oznacovat = doe_get_check_flag(hDlg,IDC_CHECK5,p_cnf->cnf.oznacovat,OZNACOVAT_SVETLA);
          break;
        default:
          return FALSE;
      }
      break;
    case WM_VSCROLL: {
      byte pom[100];
      if(LOWORD(wParam) == SB_LINEUP) {
        p_cnf->drobic_hloubka++;
        itoa(p_cnf->drobic_hloubka,pom,10);
        SetDlgItemText(hDlg,IDC_EDIT10,pom);
      }      
      if(LOWORD(wParam) == SB_LINEDOWN) {        
        if(--p_cnf->drobic_hloubka < 1)
          p_cnf->drobic_hloubka = 1;
        itoa(p_cnf->drobic_hloubka,pom,10);
        SetDlgItemText(hDlg,IDC_EDIT10,pom);
      }
      break;
    }
    case WM_INITDIALOG: {
      byte pom[100];
      nastav_pozici_bl();
      if(!p_cnf)
        break;
      p_cnf->kk.aktivni = TRUE;
      p_cnf->kk.hwnd_dialog = hDlg;      
      CheckDlgButton(hDlg,IDC_CHECK1,p_cnf->cnf.animace);
      doe_updatuj_lista_objekty(p_cnf);
      if(p_cnf->drobic_hloubka < 1)
        p_cnf->drobic_hloubka = 1;
      itoa(p_cnf->drobic_hloubka,pom,10);      
      CheckDlgButton(hDlg,IDC_CHECK5,kres.staticke);
      CheckDlgButton(hDlg,IDC_CHECK25,kres.dynamicke);
      
      CheckDlgButton(hDlg,IDC_CHECK3,p_cnf->cnf.oznacovat&OZNACOVAT_MESH);
      CheckDlgButton(hDlg,IDC_CHECK4,p_cnf->cnf.oznacovat&OZNACOVAT_KOSTI);
      CheckDlgButton(hDlg,IDC_CHECK5,p_cnf->cnf.oznacovat&OZNACOVAT_SVETLA);
      
      SetDlgItemText(hDlg,IDC_EDIT10,pom);
      
      CheckDlgButton(hDlg,IDC_CHECK2,p_cnf->deka.aktivni);
      
      doe_printf(hDlg,IDC_EDIT11,"%f",p_cnf->deka.radius);
      doe_printf(hDlg,IDC_EDIT12,"%f",p_cnf->deka.sila);      

      if(p_cnf->groupflag&GROUP_KONT) {
        if(p_cnf->groupflag&GROUP_OBJ) {
          if(p_cnf->groupflag&GROUP_FACE) {
            doe_check_stlac(hDlg,IDC_RADIO3,TRUE);
          } else {
            doe_check_stlac(hDlg,IDC_RADIO2,TRUE);
          }
        } else {
          doe_check_stlac(hDlg,IDC_RADIO1,TRUE);
        }
      }
      doe_printf(hDlg,IDC_STATIC1,"%d",p_cnf->groupfacenum);
      break;
    }
    case WM_DESTROY:
      if(!p_cnf)
        break;
      p_cnf->kk.aktivni = FALSE;
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

BOOL CALLBACK doe_calback_konfigurace_eliminator(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON1:
          oe_prikaz_zjednodus_objekty(p_cnf,0);
          break;
        case IDC_BUTTON10:
          oe_stack_zaloha_obnov(p_cnf);
          break;
        case IDC_BUTTON11: // vybrane
          oe_prikaz_zjednodus_objekty(p_cnf,VERT_SMAZ);
          break;
        case IDC_BUTTON13: // nevybrane
          oe_prikaz_zjednodus_objekty(p_cnf,VERT_NECHEJ);
          break;
        case IDC_BUTTON4:
          oe_prikaz_decimace_vyberu(p_cnf);
          break;
        case IDC_BUTTON2:
          oe_prikaz_decimace_kontejneru(p_cnf);
          break;
        case IDC_BUTTON3:
          oe_prikaz_decimace_sceny(p_cnf);
          break;
        case IDC_BUTTON5:
          oe_prikaz_decimace_trsu(p_cnf);
          break;
        case IDC_BUTTON6:
          oe_prikaz_decimace_pruhu(p_cnf);
          break;
        case IDC_BUTTON12:
          oe_prikaz_decimace_pruhu_body(p_cnf);
          break;
        case IDC_BUTTON9:
          oe_prikaz_slivacka_group_vertex(p_cnf);
          break;
        case IDC_BUTTON7:
          oe_objekt_izolovane_vertexy(p_cnf,p_cnf->vybr_kont,p_cnf->vybr_objekt);
          break;
        case IDC_BUTTON8:
          oe_face_izolovane_vertexy(p_cnf);
          break;
        case IDC_CHECK1:          
          p_cnf->elm.flag = doe_get_check_flag(hDlg,IDC_CHECK1, p_cnf->elm.flag, ELIM_STRED);
          break;
        case IDC_CHECK2:
          p_cnf->elm.flag = doe_get_check_flag(hDlg,IDC_CHECK2, p_cnf->elm.flag, ELIM_MAZAT);
          break;
        case IDC_CHECK3:
          p_cnf->elm.flag = doe_get_check_flag(hDlg,IDC_CHECK3, p_cnf->elm.flag, ELIM_NORMAL);
          break;
        case IDC_CHECK4:
          p_cnf->elm.flag = doe_get_check_flag(hDlg,IDC_CHECK4, p_cnf->elm.flag, ELIM_INTELIGENT_1);
          break;
        case IDC_CHECK6:
          p_cnf->elm.flag = doe_get_check_flag(hDlg,IDC_CHECK6, p_cnf->elm.flag, ELIM_INTELIGENT_2);
          break;
        case IDC_CHECK10:
          p_cnf->elm.flag = doe_get_check_flag(hDlg,IDC_CHECK10, p_cnf->elm.flag, ELIM_INTELIGENT_2_BRUTAL);
          break;
        case IDC_CHECK5:
          p_cnf->elm.flag = doe_get_check_flag(hDlg,IDC_CHECK5, p_cnf->elm.flag, ELIM_STATIC);
          break;
        case IDC_CHECK25:
          p_cnf->elm.flag = doe_get_check_flag(hDlg,IDC_CHECK25,p_cnf->elm.flag, ELIM_DYNAMIC);
          break;
        case IDC_CHECK11:
          p_cnf->elm.flag = doe_get_check_flag(hDlg,IDC_CHECK11,p_cnf->elm.flag, ELIM_PRVKY);
          break;
        case IDC_CHECK12:
          p_cnf->elm.flag = doe_get_check_flag(hDlg,IDC_CHECK12,p_cnf->elm.flag, ELIM_OKOLI);
          break;
        case IDC_EDIT1:
          {
            byte pom[200];
            GetDlgItemText(hDlg,IDC_EDIT1,pom,199);
            p_cnf->elm.p.x = (float)atof(pom);
            break;
          }
        case IDC_EDIT8:
          {
            byte pom[200];
            GetDlgItemText(hDlg,IDC_EDIT8,pom,199);
            p_cnf->elm.p.y = (float)atof(pom);
            break;
          }
        case IDC_EDIT9:
          {
            byte pom[200];
            GetDlgItemText(hDlg,IDC_EDIT9,pom,199);
            p_cnf->elm.p.z = (float)atof(pom);
            break;
          }
        case IDC_EDIT10:
          {
            byte pom[200];
            GetDlgItemText(hDlg,IDC_EDIT10,pom,199);
            p_cnf->elm.norm.x = (float)atof(pom);
            break;
          }
        case IDC_EDIT11:
          {
            byte pom[200];
            GetDlgItemText(hDlg,IDC_EDIT11,pom,199);
            p_cnf->elm.norm.y = (float)atof(pom);
            break;
          }
        case IDC_EDIT12:
          {
            byte pom[200];
            GetDlgItemText(hDlg,IDC_EDIT12,pom,199);
            p_cnf->elm.norm.z = (float)atof(pom);
            break;
          }
        case IDC_EDIT2:
          {
            byte pom[200];
            GetDlgItemText(hDlg,IDC_EDIT2,pom,199);
            p_cnf->elm.uhel = (float)DEG2RAD(atof(pom));
            break;
          }
        case IDC_EDIT3:
          {
            byte pom[200];
            GetDlgItemText(hDlg,IDC_EDIT3,pom,199);
            p_cnf->elm.spol_bodu_1 = atoi(pom);
            break;
          }
        case IDC_EDIT7:
          {
            byte pom[200];
            GetDlgItemText(hDlg,IDC_EDIT7,pom,199);
            p_cnf->elm.spol_bodu_2 = atoi(pom);
            break;
          }
        case IDC_EDIT4:
          {
            p_cnf->cnf.slicka_delta = doe_vyber_kolonku_float(hDlg,IDC_EDIT4);
            break;
          }          
        case IDC_EDIT15:
          {
            p_cnf->elm.delta_vzdalenost = doe_vyber_kolonku_float(hDlg,IDC_EDIT15);
            break;
          }
        case IDC_EDIT5:
          {
            p_cnf->ds.stupen = doe_vyber_kolonku_float(hDlg,IDC_EDIT5)*0.01f;
            break;
          }
        case IDC_EDIT16:
          p_cnf->ds.vertexu = doe_vyber_kolonku_int(hDlg,IDC_EDIT16);
          break;
        case IDC_CHECK13:
          p_cnf->ds.vaha = IsDlgButtonChecked(hDlg,IDC_CHECK13);
          break;
        case IDC_CHECK26:
          p_cnf->ds.hrany = IsDlgButtonChecked(hDlg,IDC_CHECK26);
          break;
        case IDC_RADIO1:
          p_cnf->ds.vertexy = !IsDlgButtonChecked(hDlg,IDC_RADIO1);
          break;
        case IDC_RADIO2:
          p_cnf->ds.vertexy =  IsDlgButtonChecked(hDlg,IDC_RADIO2);
          break;
        case IDC_CHECK29:
          p_cnf->ds.prah = IsDlgButtonChecked(hDlg,IDC_CHECK29);
          break;          
        case IDC_EDIT13:
          p_cnf->ds.vaha_flt = doe_vyber_kolonku_float(hDlg,IDC_EDIT13);
          break;
        case IDC_EDIT20:
          p_cnf->ds.hrany_flt = doe_vyber_kolonku_float(hDlg,IDC_EDIT20);
          break;
        case IDC_EDIT21:
          p_cnf->ds.prah_flt = doe_vyber_kolonku_float(hDlg,IDC_EDIT21);
          break;
        default:
          return FALSE;
      }
      break;
    case WM_INITDIALOG: {
      nastav_pozici_bl();
      if(!p_cnf)
        break;
      p_cnf->elm.dlg_aktivni = TRUE;
      p_cnf->elm.hwnd_dialog = hDlg;
      doe_check_stlac(hDlg,IDC_CHECK1,p_cnf->elm.flag&ELIM_STRED);
      doe_check_stlac(hDlg,IDC_CHECK2,p_cnf->elm.flag&ELIM_MAZAT);
      doe_check_stlac(hDlg,IDC_CHECK3,p_cnf->elm.flag&ELIM_NORMAL);
      doe_check_stlac(hDlg,IDC_CHECK4,p_cnf->elm.flag&ELIM_INTELIGENT_1);
      doe_check_stlac(hDlg,IDC_CHECK6,p_cnf->elm.flag&ELIM_INTELIGENT_2);
      doe_check_stlac(hDlg,IDC_CHECK10,p_cnf->elm.flag&ELIM_INTELIGENT_2_BRUTAL);
      doe_check_stlac(hDlg,IDC_CHECK5,p_cnf->elm.flag&ELIM_STATIC);
      doe_check_stlac(hDlg,IDC_CHECK25,p_cnf->elm.flag&ELIM_DYNAMIC);
      doe_check_stlac(hDlg,IDC_CHECK11,p_cnf->elm.flag&ELIM_PRVKY);
      doe_check_stlac(hDlg,IDC_CHECK12,p_cnf->elm.flag&ELIM_OKOLI);
      doe_printf(hDlg,IDC_EDIT1, "%.3f",p_cnf->elm.p.x);
      doe_printf(hDlg,IDC_EDIT8, "%.3f",p_cnf->elm.p.y);
      doe_printf(hDlg,IDC_EDIT9, "%.3f",p_cnf->elm.p.z);
      doe_printf(hDlg,IDC_EDIT2, "%.3f",RAD2DEG(p_cnf->elm.uhel));
      doe_printf(hDlg,IDC_EDIT10,"%.3f",p_cnf->elm.norm.x);
      doe_printf(hDlg,IDC_EDIT11,"%.3f",p_cnf->elm.norm.y);
      doe_printf(hDlg,IDC_EDIT12,"%.3f",p_cnf->elm.norm.z);
      doe_printf(hDlg,IDC_EDIT3, "%d",p_cnf->elm.spol_bodu_1);
      doe_printf(hDlg,IDC_EDIT7, "%d",p_cnf->elm.spol_bodu_2);
      doe_printf(hDlg,IDC_EDIT4, "%f",p_cnf->cnf.slicka_delta);
      doe_printf(hDlg,IDC_EDIT15,"%f",p_cnf->elm.delta_vzdalenost);      
      doe_printf(hDlg,IDC_EDIT5, "%.2f",p_cnf->ds.stupen*100.0f);
      doe_printf(hDlg,IDC_EDIT16,"%d",p_cnf->ds.vertexu);
      
      doe_check_stlac(hDlg,IDC_RADIO2, p_cnf->ds.vertexy);
      doe_check_stlac(hDlg,IDC_RADIO1,!p_cnf->ds.vertexy);

      doe_check_stlac(hDlg,IDC_CHECK13,p_cnf->ds.vaha);
      doe_check_stlac(hDlg,IDC_CHECK26,p_cnf->ds.hrany);
      doe_check_stlac(hDlg,IDC_CHECK29,p_cnf->ds.prah);

      doe_printf(hDlg,IDC_EDIT13,"%f",p_cnf->ds.vaha_flt);
      doe_printf(hDlg,IDC_EDIT20,"%f",p_cnf->ds.hrany_flt);
      doe_printf(hDlg,IDC_EDIT21,"%f",p_cnf->ds.prah_flt);

      break;
    }
    case WM_DESTROY:
      if(!p_cnf)
        break;
      p_cnf->elm.dlg_aktivni = FALSE;
      break;
    default:
      return FALSE;
  }
  return TRUE;
}


void doe_smaz_material(K_EDITOR *p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int matnum = p_cnf->km.vybr_mat;
  int k,o;
  
  if(p_cnf->p_mat[matnum]) {
    for(k = 0; k < KONT_NUM; k++) {
      if(p_kont = p_cnf->p_kont[k]) {
        for(o = 0; o < p_kont->max_objektu; o++) {    
          if(p_kont->p_obj[o] && p_kont->p_obj[o]->material == matnum) {
            p_kont->p_obj[o]->material = FALSE;            
          }
        }
      }
    }
    zrus_material(p_cnf->p_mat+matnum);
  }
}


static int vyber_volny_material(K_EDITOR *p_cnf);

BOOL CALLBACK doe_calback_konfigurace_materialy(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_LIST7: 
          /* Vyber materialu */
          if(HIWORD(wParam) == LBN_SELCHANGE) {
            p_cnf->km.vybr_polozka = SendMessage((HWND)lParam,LB_GETCURSEL,0,0);
            p_cnf->km.vybr_mat = SendMessage((HWND)lParam,LB_GETITEMDATA,p_cnf->km.vybr_polozka,0);
            if(p_lmm)
              zrus_material(&p_lmm);
            p_lmm = kopiruj_material(p_cnf->p_mat[p_cnf->km.vybr_mat]);  
            zobraz_akt_material(hDlg, p_lmm);
          }
          
          /* Uprava materialu - material editor*/
          if(HIWORD(wParam) == LBN_DBLCLK) {
            p_cnf->km.vybr_polozka = SendMessage((HWND)lParam,LB_GETCURSEL,0,0);
            p_cnf->km.vybr_mat = SendMessage((HWND)lParam,LB_GETITEMDATA,p_cnf->km.vybr_polozka,0);
            doe_editor_materialu(p_cnf);
            if(p_lmm)
              zrus_material(&p_lmm);
            p_lmm = kopiruj_material(p_cnf->p_mat[p_cnf->km.vybr_mat]);
            zobraz_akt_material(hDlg, p_lmm);
            doe_init_dialog_materialy(hDlg,SendMessage((HWND)GetDlgItem( hDlg, IDC_LIST7 ),LB_GETCURSEL,0,0));
          }
          break;
        case IDC_BUTTON1: // /
          oe_prikaz_prirad_material_objekt(p_cnf);
          break;
        case IDC_BUTTON2: // /+CTRL
          oe_prikaz_prirad_material_kontejner(p_cnf);
          break;
        case IDC_BUTTON3: // material editor
          doe_editor_materialu(p_cnf);
          doe_init_dialog_materialy(hDlg,SendMessage((HWND)GetDlgItem( hDlg, IDC_LIST7 ),LB_GETCURSEL,0,0));            
          if(p_lmm)
            zrus_material(&p_lmm);
          p_lmm = kopiruj_material(p_cnf->p_mat[p_cnf->km.vybr_mat]);  
          zobraz_akt_material(hDlg, p_lmm);
          break;
        case IDC_BUTTON7: // vymazat vse
          lo_vymaz_materialy(p_cnf->p_mat,MAX_EDIT_MATERIALU,p_cnf->p_text,MAX_EDIT_TEXTUR);
          if(p_lmm)
            zrus_material(&p_lmm);
          doe_init_dialog_materialy(hDlg,0);
          zobraz_akt_material(hDlg, p_lmm);
          break;
        case IDC_BUTTON4: // load
          oe_prikaz_nahraj_materialy(p_cnf,FALSE);
          doe_init_dialog_materialy(hDlg,SendMessage((HWND)GetDlgItem( hDlg, IDC_LIST7 ),LB_GETCURSEL,0,0));
          if(p_lmm)
            zrus_material(&p_lmm);
          p_lmm = kopiruj_material(p_cnf->p_mat[p_cnf->km.vybr_mat]);  
          zobraz_akt_material(hDlg, p_lmm);          
          SendMessage(p_cnf->km.hwnd_list,LB_SETCURSEL,p_cnf->km.vybr_polozka,0);
          break;
        case IDC_BUTTON5: // add
          break;
        case IDC_BUTTON8: // save
          oe_prikaz_uloz_materialy(p_cnf);
          break;
        case IDC_BUTTON10:// save akt.
          break;        
        case IDC_BUTTON20: // smaz material
          doe_smaz_material(p_cnf);
          doe_init_dialog_materialy(hDlg,0);
          if(p_lmm)
            zrus_material(&p_lmm);
          p_lmm = kopiruj_material(p_cnf->p_mat[p_cnf->km.vybr_mat]);  
          zobraz_akt_material(hDlg, p_lmm);
          break;
        case IDC_BUTTON12: // Brutal load textur
          doe_nahraj_list_textur(p_cnf);
          doe_init_dialog_materialy(hDlg,SendMessage((HWND)GetDlgItem( hDlg, IDC_LIST7 ),LB_GETCURSEL,0,0));          
          if(p_lmm)
            zrus_material(&p_lmm);
          p_lmm = kopiruj_material(p_cnf->p_mat[p_cnf->km.vybr_mat]);  
          zobraz_akt_material(hDlg, p_lmm);
          break;

        case IDC_BUTTON6: // novy material
          if((p_cnf->km.vybr_mat = vyber_volny_material(p_cnf)) == K_CHYBA) {
            ddw("Neni volny material !");
            break;
          }
          p_cnf->p_mat[p_cnf->km.vybr_mat] = vyrob_material();
          strcpy(p_cnf->p_mat[p_cnf->km.vybr_mat]->jmeno,"NMat");
          if(p_lmm)
            zrus_material(&p_lmm);
          p_lmm = kopiruj_material(p_cnf->p_mat[p_cnf->km.vybr_mat]);  
          doe_init_dialog_materialy(hDlg,SendMessage((HWND)GetDlgItem( hDlg, IDC_LIST7 ),LB_GETCURSEL,0,0));
          break;

        case IDC_BUTTON19: // reload textur
          oe_smaz_textury(p_cnf);
          oe_reload_textur(p_cnf);
          break;

        case IDC_BUTTON21:
          oe_prikaz_nahraj_vybrany_material(p_cnf);
          doe_init_dialog_materialy(hDlg,SendMessage((HWND)GetDlgItem( hDlg, IDC_LIST7 ),LB_GETCURSEL,0,0));
          break;
        
        case IDC_BUTTON22:
          oe_prikaz_uloz_vybrany_material(p_cnf);
          doe_init_dialog_materialy(hDlg,SendMessage((HWND)GetDlgItem( hDlg, IDC_LIST7 ),LB_GETCURSEL,0,0));
          break;
        
        // kopie vybraneho materialu
        case IDC_BUTTON23: {
          int n;
          if((n = vyber_volny_material(p_cnf)) == K_CHYBA) {
            ddw("Neni volny material !");
            break;
          }
          p_cnf->p_mat[n] = kopiruj_material(p_cnf->p_mat[p_cnf->km.vybr_mat]);
          sprintf(p_cnf->p_mat[n]->jmeno,"Kopie_%s",p_cnf->p_mat[p_cnf->km.vybr_mat]->jmeno);
          p_cnf->km.vybr_mat = n;
          doe_init_dialog_materialy(hDlg,n);
          if(p_lmm)
            zrus_material(&p_lmm);
          p_lmm = kopiruj_material(p_cnf->p_mat[p_cnf->km.vybr_mat]);
          break;
        }
        case IDC_BUTTON40: // Seznam nenahranych textur
          oe_prikaz_vypis_nenahrane_textury(p_cnf);
          break;
        default:
          return FALSE;
      }
      break;

    case WM_INITDIALOG:
      nastav_pozici_bl();
      doe_init_dialog_materialy(hDlg,0);      
      if(p_lmm)
        zrus_material(&p_lmm);
      p_lmm = kopiruj_material(p_cnf->p_mat[p_cnf->km.vybr_mat]);
      zobraz_akt_material(hDlg, p_lmm);
      
      p_cnf->km.aktivni = TRUE;
      p_cnf->km.hwnd_list = GetDlgItem(hDlg,IDC_LIST7);
      p_cnf->km.hwnd_dialog = hDlg;
      SendMessage(p_cnf->km.hwnd_list,LB_SETCURSEL,p_cnf->km.vybr_polozka,0);
      break;
    case WM_DESTROY: {
      p_cnf->km.aktivni = FALSE;
      EndDialog( hDlg, FALSE );
      break; 
    }
    default:
      return FALSE;
  }
  return TRUE;
}

static void doe_init_anim_list(HWND hDlg, EDIT_KONTEJNER *p_kont)
{
  HWND hwnd = GetDlgItem(hDlg,IDC_LIST1);
  byte pom[100];
  int i;
  
  SendMessage(hwnd, LB_RESETCONTENT, 0, 0);
  
  for(i = 0; i < KONT_MAX_ANIM; i++) {
    if(p_kont->sim[i].keynum) {
      sprintf(pom,"%d %s",i,p_kont->sim[i].jmeno);
      SendMessage( hwnd, LB_ADDSTRING, 0,(LPARAM)pom);
      //j++;
    } else {
      sprintf(pom,"%d %s",i,"....");
      SendMessage( hwnd, LB_ADDSTRING, 0,(LPARAM)pom);
    }
  } 
  
  //if(!j) SendMessage( hwnd, LB_ADDSTRING, 0,(LPARAM)"Nicevo animacevo");
  
  SendMessage(hwnd,LB_SETCURSEL,p_kont->akt_animace,0);
}

static void doe_init_animace(HWND hDlg, EDIT_KONTEJNER *p_kont)
{ 
  HIERARCHY_SIM *p_sim;
  int  va = p_kont->akt_animace;
  byte pom[200];
 
  p_sim = p_kont->sim+va;  
  SetDlgItemText( hDlg, IDC_STATIC2, p_sim->jmeno);
  SetDlgItemText( hDlg, IDC_STATIC3, itoa(p_sim->keynum,pom,10));
    
  ftoa(time2sec(p_sim->time),pom,3);
  SetDlgItemText( hDlg, IDC_EDIT1, pom);
  
  sprintf(pom,"%d",p_sim->time);
  SetDlgItemText( hDlg, IDC_EDIT2, pom);
    
  SetDlgItemText( hDlg, IDC_STATIC8, "---");
  sprintf(pom,"%f",frame2sec(p_sim->keynum)); // delka v sekundach
  SetDlgItemText( hDlg, IDC_STATIC27, pom);
  
  CheckDlgButton(hDlg,IDC_CHECK1,p_sim->flag&GK_LOOP);
 
  doe_hs_nastav(0,p_sim->keynum-1,time2frame(p_sim->time),
                "Animace %d kontejneru '%s'",va,p_kont->jmeno);
}

static void doe_init_ani_kontejner(HWND hDlg, K_EDITOR *p_cnf)
{
  int v = p_cnf->vybr_kont;
  
  if(!p_cnf->p_kont[v])
    return;
  
  SetDlgItemText( hDlg, IDC_STATIC1, p_cnf->p_kont[v]->jmeno);
  
  doe_init_anim_list(hDlg, p_cnf->p_kont[v]); //list animaci
  doe_init_animace(hDlg, p_cnf->p_kont[v]); //info o animaci
}

__inline static void doe_nastav_slider(HWND hDlg, int time)
{ 
  doe_hs_setpos(time2frame(time));
}

BOOL CALLBACK doe_callback_upravit_animaci( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {      
        case IDOK: {
          HIERARCHY_SIM *p_sim = tmp.p_pom;
          GetDlgItemText(hDlg,IDC_EDIT1,p_sim->jmeno,99); //jmeno animace
          EndDialog( hDlg, 1 );
          break;
        }
        case IDCANCEL:
  		    EndDialog( hDlg, 0 );
          break;
        default:
          return FALSE;
      }
      break;
      
    case WM_INITDIALOG :{
      HIERARCHY_SIM *p_sim = tmp.p_pom;
      SetDlgItemText(hDlg, IDC_EDIT1, p_sim->jmeno);
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }  
  return TRUE;
}


static int doe_upravit_animaci(HWND hDlg, HIERARCHY_SIM *p_sim)
{
  if(p_sim->keynum) {
    tmp.p_pom = p_sim;
    return(DialogBox( hinst, MAKEINTRESOURCE(IDD_ANIMACE_UPRAVIT), hwnd_aplikace, doe_callback_upravit_animaci));
  } else
    return(FALSE);
}


void doe_animace_updatuj(K_EDITOR *p_cnf)
{
  HIERARCHY_SIM *p_sim;
  HWND hDlg = p_cnf->ak.hwnd_dialog;
  int  frame,k = p_cnf->vybr_kont;
  byte pom[200];

  if(p_cnf->p_kont[k]) {
    int va = p_cnf->p_kont[k]->akt_animace;
    if(p_cnf->p_kont[k]->sim[va].keynum) {
      p_sim = p_cnf->p_kont[k]->sim+va;
      frame = time2frame(p_sim->time);
      SetDlgItemText( hDlg, IDC_EDIT1, itoa(frame,pom,10));
      SetDlgItemText( hDlg, IDC_EDIT2, itoa(p_sim->time,pom,10));
      doe_hs_setpos(frame);
    }
  }
}

void doe_animace_updatuj_materialy(K_EDITOR *p_cnf)
{
  ANIM_TEXT *p_text;
  HWND hDlg = p_cnf->ak.hwnd_dialog;
  int  frame, m = p_cnf->km.vybr_mat;  
  int  am;
  byte pom[200];

  if(p_cnf->p_mat[m]) {
    am = p_cnf->p_mat[m]->atextakt[p_cnf->akmat.par1];
    p_text = p_cnf->p_mat[m]->p_atext[p_cnf->akmat.par1][am];
    
    if(p_text->endtime) {
      frame = time2frame(p_text->time);
      SetDlgItemText( hDlg, IDC_EDIT1, itoa(frame,pom,10));
      SetDlgItemText( hDlg, IDC_EDIT2, itoa(p_text->time,pom,10));
      doe_hs_setpos(frame);
    }
  }
}

int doe_animace_updatuj_materialy_slider(int frame)
{
  HWND hDlg = p_cnf->akmat.hwnd_dialog;
  int  m = p_cnf->km.vybr_mat;
  EDIT_MATERIAL *p_mat = p_cnf->p_mat[m];
  ANIM_TEXT *p_text;
  byte pom[200];

  if(p_mat) {
    p_text = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
    if(p_text && p_text->endtime) {
      p_text->time = frame2time(frame);
      SetDlgItemText( hDlg, IDC_EDIT1, itoa(frame,pom,10));
    }
  }
  return(FALSE);
}

// Nacitat to z klicu !
void doe_animace_updatuj_materialy_text(K_EDITOR *p_cnf)
{
  HWND hDlg = p_cnf->akmat.hwnd_dialog;
  EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
  GLMATRIX      *p_ttext;
  ANIM_TEXT *p_track = NULL;
  byte  pom[200];
  int   frame,klic,v;
  int   des,sec,min;
  BOD   p;

  if(p_mat) {
    p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
    p_ttext = p_mat->ttext+p_cnf->akmat.par1;
  }

  if(p_mat && p_track && p_track->endtime) {
    frame = time2frame(p_track->time);
    
    // sec je pocet sekund od zacatku
    doe_sec_to_time(time2sec(p_track->time),&des, &sec, &min);
    sprintf(pom,"%.2d:%.2d:%.2d",min,sec,des);
    SetDlgItemText( hDlg, IDC_EDIT2, pom);  
    
    CheckDlgButton( hDlg, IDC_CHECK5, p_track->flag&GK_LOOP ? BST_CHECKED : BST_UNCHECKED);

    // pivot klice
    klic = key_je_frame_klic(p_track->p_vkeys, p_track->piv_keys,frame);
    v = (klic != K_CHYBA);
    if(v) {
      SetDlgItemText( hDlg, IDC_EDIT51, ftoa(p_track->p_vkeys[klic].continuity,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT52, ftoa(p_track->p_vkeys[klic].tension,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT53, ftoa(p_track->p_vkeys[klic].bias,pom,0));
      p = p_track->p_piv[klic];
    } else {
      // ... interpolace hodnot - pred tim
      p = p_track->pivot;
    }
    SetDlgItemText( hDlg, IDC_EDIT15, ftoa(p.x,pom,0));
    SetDlgItemText( hDlg, IDC_EDIT16, ftoa(p.y,pom,0));
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT15),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT16),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT51),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT52),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT53),v);
    doe_check_stlac(hDlg,IDC_CHECK4,v);

    // pozicni klice
    klic = key_je_frame_klic(p_track->p_pkeys, p_track->pos_keys,frame);
    v = (klic != K_CHYBA);
    if(v) {
      SetDlgItemText( hDlg, IDC_EDIT25, ftoa(p_track->p_pkeys[klic].continuity,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT26, ftoa(p_track->p_pkeys[klic].tension,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT44, ftoa(p_track->p_pkeys[klic].bias,pom,0));
      p = p_track->p_pos[klic];
    } else {
      // ... interpolace hodnot - pred tim
      matrix_to_pos_2d(p_ttext,&p);
    }
    SetDlgItemText( hDlg, IDC_EDIT3, ftoa(p.x,pom,0));
    SetDlgItemText( hDlg, IDC_EDIT7, ftoa(p.y,pom,0));
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT3),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT7),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT25),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT26),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT44),v);
    doe_check_stlac(hDlg,IDC_CHECK1,v);

    //scale klice    
    klic = key_je_frame_klic(p_track->p_skeys, p_track->scs_keys, frame);
    v = (klic != K_CHYBA);
    if(v) {
      SetDlgItemText( hDlg, IDC_EDIT48, ftoa(p_track->p_skeys[klic].continuity,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT49, ftoa(p_track->p_skeys[klic].tension,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT50, ftoa(p_track->p_skeys[klic].bias,pom,0));
      p = p_track->p_scale[klic];
    } else {
      matrix_to_scale_2d(p_ttext,&p);
    }
    SetDlgItemText( hDlg, IDC_EDIT19, ftoa(p.x,pom,0));
    SetDlgItemText( hDlg, IDC_EDIT24, ftoa(p.y,pom,0));
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT19),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT24),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT48),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT49),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT50),v);
    doe_check_stlac(hDlg,IDC_CHECK2,v);

    // rot-keys        
    klic = key_je_frame_klic(p_track->p_rkeys, p_track->rot_keys, frame);
    v = (klic != K_CHYBA);
    if(v) {
      SetDlgItemText( hDlg, IDC_EDIT45, ftoa(p_track->p_rkeys[klic].continuity,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT46, ftoa(p_track->p_rkeys[klic].tension,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT47, ftoa(p_track->p_rkeys[klic].bias,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT18, ftoa(RAD2DEG(p_track->p_rot[klic]),pom,0));
    } else {
      SetDlgItemText( hDlg, IDC_EDIT18, ftoa(RAD2DEG(p_track->rot),pom,0));
    }
    
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT18),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT45),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT46),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT47),v);
    doe_check_stlac(hDlg, IDC_CHECK3, v);
  } else {
    SetFocus(GetDlgItem(hDlg,IDC_LIST1));

    SetDlgItemText( hDlg, IDC_EDIT2, "00:00:00");
    
    // pozicni klice
    SetDlgItemText( hDlg, IDC_EDIT3, "0");
    SetDlgItemText( hDlg, IDC_EDIT7, "0");    
    SetDlgItemText( hDlg, IDC_EDIT25,"0");
    SetDlgItemText( hDlg, IDC_EDIT26,"0");
    SetDlgItemText( hDlg, IDC_EDIT44,"0");
    v = FALSE;
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT3),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT7),v);    
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT25),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT26),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT44),v);
    doe_check_stlac(hDlg, IDC_CHECK1, v);

    // pivot klice
    SetDlgItemText( hDlg, IDC_EDIT15,"0");
    SetDlgItemText( hDlg, IDC_EDIT16,"0");    
    SetDlgItemText( hDlg, IDC_EDIT51,"0");
    SetDlgItemText( hDlg, IDC_EDIT52,"0");
    SetDlgItemText( hDlg, IDC_EDIT53,"0");
    v = FALSE;
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT15),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT16),v);    
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT51),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT52),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT53),v);
    doe_check_stlac(hDlg, IDC_CHECK4, v);

    // target-klice
    SetDlgItemText( hDlg, IDC_EDIT18, "0");
    SetDlgItemText( hDlg, IDC_EDIT45, "0");
    SetDlgItemText( hDlg, IDC_EDIT46, "0");
    SetDlgItemText( hDlg, IDC_EDIT47, "0");    
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT18),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT45),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT46),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT47),v);
    doe_check_stlac(hDlg, IDC_CHECK2, v);
    
    SetDlgItemText( hDlg, IDC_EDIT19, "1");
    SetDlgItemText( hDlg, IDC_EDIT24, "1");
    SetDlgItemText( hDlg, IDC_EDIT48, "0");
    SetDlgItemText( hDlg, IDC_EDIT49, "0");
    SetDlgItemText( hDlg, IDC_EDIT50, "0");
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT19),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT24),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT48),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT49),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT50),v);
    doe_check_stlac(hDlg, IDC_CHECK3, v);
  }  
}

/*
  Vybere hodnoty z editboxu a vlozi je do tracku
*/
void doe_animace_nacti_materialy_text(K_EDITOR *p_cnf)
{
  HWND hDlg = p_cnf->akmat.hwnd_dialog;
  EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
  ANIM_TEXT *p_track = NULL;
  int   frame,klic,p,s;

  if(p_mat) {
    p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
  }

  if(p_mat && p_track && p_track->endtime) {
    frame = time2frame(p_track->time);
        
    if((p = IsDlgButtonChecked(hDlg,IDC_CHECK1))) {
      klic = key_je_frame_klic(p_track->p_pkeys, p_track->pos_keys,frame);
      if(klic != K_CHYBA) {
        p_track->pos.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT3);
        p_track->pos.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT7);
        p_track->p_pos[klic] = p_track->pos;
      }
    } 
    
    // pivot klice
    if((p = IsDlgButtonChecked(hDlg,IDC_CHECK4))) {
      klic = key_je_frame_klic(p_track->p_vkeys, p_track->piv_keys,frame);
      if(klic != K_CHYBA) {
        p_track->pivot.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT15);
        p_track->pivot.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT16);
        p_track->p_piv[klic] = p_track->pivot;
      }
    } 
    //scale klice
    if((s = IsDlgButtonChecked(hDlg,IDC_CHECK2))) {
      klic = key_je_frame_klic(p_track->p_skeys, p_track->scs_keys, frame);
      if(klic != K_CHYBA) {
        p_track->scs.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT19);
        p_track->scs.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT24);
        p_track->p_scale[klic] = p_track->scs;
      }
    } 
    
    // rot-keys    
    if(IsDlgButtonChecked(hDlg,IDC_CHECK3)) {
      klic = key_je_frame_klic(p_track->p_rkeys, p_track->rot_keys, frame);
      if(klic != K_CHYBA) {
        p_track->rot = DEG2RAD(doe_vyber_kolonku_float(hDlg,IDC_EDIT18));
        p_track->p_rot[klic] = p_track->rot;
      }
    } 
  }
}


int doe_animace_updatuj_slider(int frame)
{
  byte  pom[200];  
  HWND  hDlg = p_cnf->ak.hwnd_dialog;
  int   va,k = p_cnf->vybr_kont;
  
  if(!p_cnf->p_kont[k])
    return(FALSE);
  
  va = p_cnf->p_kont[k]->akt_animace;
  if(!p_cnf->p_kont[k]->sim[va].keynum) 
    return(FALSE);

  p_cnf->p_kont[k]->sim[va].time = frame2time(frame);
  itoa(p_cnf->p_kont[k]->sim[va].time, pom, 10);
  SetDlgItemText(hDlg, IDC_EDIT2, pom);
  itoa(frame,pom,10);
  SetDlgItemText(hDlg, IDC_EDIT1, pom);
  
  ani_animuj_scenu_vybr_animace(p_cnf, p_cnf->p_kont[k], 
                                p_cnf->p_kont[k]->sim[va].time);
  return(FALSE);
}


void doe_anim_start(int next_time, int framenum,
                    int *p_time_start, int *p_time_stop, 
                    int *p_time_delka, int *p_time, 
                    int *p_start, int *p_stop)
{
  *p_start = 0;
  *p_stop = 0;
  *p_time_delka = calc_endtime(framenum);
  *p_time_start = next_time;
  *p_time_stop  = next_time+calc_endtime(framenum);
  *p_time = 0;
}

void doe_anim_end(int next_time, int framenum,
                  int *p_time_start, int *p_time_stop, 
                  int *p_time_delka, int *p_time, 
                  int *p_start, int *p_stop)
{
  *p_start = 0;
  *p_stop = 0;
  *p_time_delka = calc_endtime(framenum);
  *p_time_start = next_time;
  *p_time_stop  = next_time+calc_endtime(framenum);
  *p_time = calc_endtime(framenum);
}

void doe_anim_start_et(int next_time, int endtime,
                       int *p_time_start, int *p_time_stop, 
                       int *p_time_delka, int *p_time, 
                       int *p_start, int *p_stop)
{
  *p_start = 0;
  *p_stop = 0;
  *p_time_delka = endtime;
  *p_time_start = next_time;
  *p_time_stop  = next_time+endtime;
  *p_time = 0;
}

void doe_anim_end_et(int next_time, int endtime,
                     int *p_time_start, int *p_time_stop, 
                     int *p_time_delka, int *p_time, 
                     int *p_start, int *p_stop)
{
  *p_start = 0;
  *p_stop = 0;
  *p_time_delka = endtime;
  *p_time_start = next_time;
  *p_time_stop  = next_time+endtime;
  *p_time = endtime;
}


BOOL CALLBACK doe_callback_pridej_low_kontejner( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDOK: {
          HWND combo = GetDlgItem(hDlg,IDC_LIST1);                   
          tmp.i[0] = SendMessage(combo, LB_GETITEMDATA, SendMessage(combo,LB_GETCURSEL,0,0),0);
          EndDialog( hDlg, 1 );
          break;
        }
        case IDCANCEL:
  		    EndDialog( hDlg, 0 );
          break;
        default:
          return FALSE;
      }
      break;
      
    case WM_INITDIALOG :
      {
        EDIT_KONTEJNER *p_kont;
        HWND combo = GetDlgItem(hDlg,IDC_LIST1);
        byte pom[200];
        int  i,index,p;

        SendMessage(combo, LB_RESETCONTENT, 0, 0);        
        oe_klist_reset(&i);
        while((p_kont = oe_klist_next(p_cnf,&i))) {
          index = i;
          if(index != tmp.i[1]) {
            sprintf(pom,"%d %s",index,p_kont->jmeno);
            p = SendMessage(combo, LB_ADDSTRING, 0, (LPARAM)pom);
            SendMessage(combo, LB_SETITEMDATA, p, index);
          }
        }
        SendMessage(combo,LB_SETCURSEL,0,0);
        break;
      }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }  
  return TRUE;
}

void doe_pridej_low_kontejner(K_EDITOR *p_cnf, EDIT_KONTEJNER **p_kont_top, int k)
{
  EDIT_KONTEJNER *p_kont;
  tmp.i[1] = k;
  if(DialogBox( hinst, MAKEINTRESOURCE(IDD_SEZNAM_KONTEJNERU), hwnd_aplikace, doe_callback_pridej_low_kontejner)) {
    p_kont = p_cnf->p_kont[tmp.i[0]]; // vybrany low-kontejner - bude top
    p_kont->p_next = *p_kont_top;
    *p_kont_top = p_kont;
    p_cnf->p_kont[tmp.i[0]] = NULL;
  }
}

void doe_init_low_kontejnery(HWND hDlg, K_EDITOR *p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  HWND combo = GetDlgItem(hDlg,IDC_LIST2);
  int v = p_cnf->vybr_kont;
  
  if(!p_cnf->p_kont[v])
    return;
  
  SetDlgItemText( hDlg, IDC_STATIC24, p_cnf->p_kont[v]->jmeno);
  doe_printf(hDlg,IDC_EDIT3,"%d",p_cnf->p_kont[v]->low_id);
  SendMessage(combo, LB_RESETCONTENT, 0, 0);
  SendMessage(combo, LB_ADDSTRING, 0, (LPARAM)p_cnf->p_kont[v]->jmeno);  
  p_kont = p_cnf->p_kont[v]->p_next;
  while(p_kont) {    
    SendMessage(combo, LB_ADDSTRING, 0, (LPARAM)p_kont->jmeno);
    p_kont = p_kont->p_next;
  }
  SendMessage(combo,LB_SETCURSEL,0,0);
}

void oe_aktivuj_kontejner_list(EDIT_KONTEJNER **p_kont_top, int vybrany)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_KONTEJNER *p_prev;
  int i;

  p_prev = *p_kont_top;
  p_kont =  p_prev->p_next;
  
  if(!vybrany || !p_kont) // aktivace nulteho meshe
    return;

  i = 1;
  while(p_kont) {
    if(i == vybrany) {
      p_prev->p_next = p_kont->p_next;
      p_kont->p_next = *p_kont_top;
      *p_kont_top = p_kont;
      return;
    }
    p_prev = p_kont;
    p_kont = p_kont->p_next;
    i++;
  }
}

void doe_vyhod_low_kontejner(EDIT_KONTEJNER **p_kont_top)
{
  int i;

  if((*p_kont_top)->p_next) {
    i = lo_najdi_volny_kontejner(p_cnf->p_kont,KONT_NUM);
    p_cnf->p_kont[i] = *p_kont_top;
    *p_kont_top = (*p_kont_top)->p_next;
  } 
}

BOOL CALLBACK doe_calback_konfigurace_animace(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_LIST2: // list sub-kontejneru
          if(HIWORD(wParam) == LBN_SELCHANGE || HIWORD(wParam) == LBN_DBLCLK) {
            int k = p_cnf->vybr_kont;
            if(p_cnf->p_kont[k]) {
              oe_aktivuj_kontejner_list(p_cnf->p_kont+k,SendMessage((HWND)lParam,LB_GETCURSEL,0,0));
              doe_init_low_kontejnery(hDlg,p_cnf);
              doe_init_ani_kontejner(hDlg, p_cnf);
            }
          }
          break;
        case IDC_LIST1: // list podtrid
          if(HIWORD(wParam) == LBN_SELCHANGE) {
            int k = p_cnf->vybr_kont;
            if(p_cnf->p_kont[k]) {
              p_cnf->p_kont[k]->akt_animace = SendMessage((HWND)lParam,LB_GETCURSEL,0,0);
              doe_init_animace(hDlg, p_cnf->p_kont[k]); //info o animaci
            }
          }          
          if(HIWORD(wParam) == LBN_DBLCLK) {
            int k = p_cnf->vybr_kont;
            if(p_cnf->p_kont[k]) {
              doe_upravit_animaci(hDlg,p_cnf->p_kont[k]->sim+p_cnf->p_kont[k]->akt_animace);
              doe_init_ani_kontejner(hDlg, p_cnf);
            }
          }
          
          break;
        case IDC_EDIT3:
          if(HIWORD(wParam) == EN_CHANGE) {
            int k = p_cnf->vybr_kont;
            if(p_cnf->p_kont[k]) {              
              p_cnf->p_kont[k]->low_id = doe_vyber_kolonku_int(hDlg,IDC_EDIT3);
            }
          }
          break;
        case IDC_EDIT1:
        case IDC_EDIT2: {
          int k = p_cnf->vybr_kont;
          int fnum;
          byte pom[100];
          /* Zadava frame animace
          */
          if(!p_cnf->p_kont[k])
            break;

          if(HIWORD(wParam) == EN_CHANGE) {
            int va = p_cnf->p_kont[k]->akt_animace;

            fnum = p_cnf->p_kont[k]->sim[va].keynum;
            if(!fnum)
              break;
            
            // nactu frame
            GetDlgItemText( hDlg, LOWORD(wParam), pom,99);
            
            if(LOWORD(wParam) == IDC_EDIT1) {
              p_cnf->p_kont[k]->sim[va].time = frame2time(atoi(pom));
              itoa(p_cnf->p_kont[k]->sim[va].time,pom,10);
              SetDlgItemText(hDlg, IDC_EDIT2, pom);
            }
            else {
              p_cnf->p_kont[k]->sim[va].time = atoi(pom);
            }
                        
            doe_nastav_slider(hDlg,p_cnf->p_kont[k]->sim[va].time);
            ani_animuj_scenu_vybr_animace(p_cnf, p_cnf->p_kont[k], p_cnf->p_kont[k]->sim[va].time);
          }
          break;
        }
        
        case IDC_BUTTON15: { // Load low-kontejner
          int k = p_cnf->vybr_kont;
          if(p_cnf->p_kont[k]) {
            doe_pridej_low_kontejner(p_cnf,p_cnf->p_kont+k,k);
            doe_init_low_kontejnery(hDlg,p_cnf);
          }                   
          break;
        }
        case IDC_BUTTON16: { // Smazat (vyhodit se seznamu) low-kontejner
          int k = p_cnf->vybr_kont;
          if(p_cnf->p_kont[k]) {
            doe_vyhod_low_kontejner(p_cnf->p_kont+k);
            doe_init_low_kontejnery(hDlg,p_cnf);
          }
          break;
        }

        case IDC_BUTTON1: { // Load          
          oe_prikaz_nahraj_b2m_animaci_dialog(p_cnf);
          doe_init_ani_kontejner(hDlg, p_cnf);
          break;
        }
        case IDC_BUTTON17: // load sim-animace
          oe_prikaz_nahraj_sim_animaci_dialog(p_cnf);
          doe_init_ani_kontejner(hDlg, p_cnf);
          break;
        case IDC_BUTTON3: { // Upravit
          int k = p_cnf->vybr_kont;
          if(p_cnf->p_kont[k]) {
            doe_upravit_animaci(hDlg,p_cnf->p_kont[k]->sim+p_cnf->p_kont[k]->akt_animace);
            doe_init_ani_kontejner(hDlg, p_cnf);
          }
          break;
        }
        case IDC_BUTTON2: { // Smazat
          int k = p_cnf->vybr_kont;
          if(p_cnf->p_kont[k]) {
            int va = p_cnf->p_kont[k]->akt_animace;
            if(p_cnf->p_kont[k]->sim[va].keynum) {
              key_sim_root_zrus(p_cnf->p_kont[k]->sim+va);
              doe_init_ani_kontejner(hDlg, p_cnf);
            }
            key_kont_updatuj_flagy(p_cnf->p_kont[k]);
          }
          break;
        }
        case IDC_BUTTON14: { // Export animace
          int k = p_cnf->vybr_kont;
          if(p_cnf->p_kont[k]) {
            int va = p_cnf->p_kont[k]->akt_animace;
            if(p_cnf->p_kont[k]->sim[va].keynum) {
              byte file[200];
              chdir(p_cnf->dir.data_dir);
              if(doe_dialog_save(file, 200, "Ulozit animaci...", "*.sim", "Simple animace")) {
                key_sim_uloz(p_cnf->p_kont[k]->sim[va].p_child, file, ".");
              }
            }
          }
          break;
        }          
        case IDC_CHECK1: { // zaskrkavani animaci
          int k = p_cnf->vybr_kont;
          HIERARCHY_SIM *p_sim;
          if(p_cnf->p_kont[k]) {
            int va = p_cnf->p_kont[k]->akt_animace;
            p_sim = p_cnf->p_kont[k]->sim+va;
            if(p_sim->keynum) {
              p_sim->flag = IsDlgButtonChecked(hDlg,IDC_CHECK1) ? p_sim->flag|GK_LOOP : p_sim->flag&~GK_LOOP;
            }
          }
          break;
        }

        case IDC_BUTTON8: { // []
          int k = p_cnf->vybr_kont;
          HIERARCHY_SIM *p_sim;
          if(p_cnf->p_kont[k]) {
            int va = p_cnf->p_kont[k]->akt_animace;
            p_sim = p_cnf->p_kont[k]->sim+va;
            if(p_sim->keynum) {
              doe_anim_start(p_cnf->TimeEndLastFrame,p_sim->keynum,&p_sim->time_start,&p_sim->time_stop,&p_sim->time_delka,&p_sim->time,&p_sim->start,&p_sim->stop);
              p_sim->stav = 0; // stop animace
            }
          }
          break;
        }
        case IDC_BUTTON11: {// >
          int k = p_cnf->vybr_kont;
          HIERARCHY_SIM *p_sim;
          if(p_cnf->p_kont[k]) {
            int va = p_cnf->p_kont[k]->akt_animace;
            p_sim = p_cnf->p_kont[k]->sim+va;
            if(p_sim->keynum) {              
              if(!p_sim->stav) {
                doe_anim_start(p_cnf->TimeEndLastFrame,
                  p_sim->keynum,
                  &p_sim->time_start,
                  &p_sim->time_stop,
                  &p_sim->time_delka,
                  &p_sim->time,
                  &p_sim->start,
                  &p_sim->stop);
              }
              p_sim->stav = !p_sim->stav;
            }
          }
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_VSCROLL:
      if(LOWORD(wParam) == SB_LINEUP|| LOWORD(wParam) == SB_LINEDOWN) {
        byte  pom[200];
        int   frame;
        int   va,k = p_cnf->vybr_kont;
        int   fnum;

        if(!p_cnf->p_kont[k])
          break;
        
        GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
        frame = atoi(pom);
        va = p_cnf->p_kont[k]->akt_animace;
        fnum = p_cnf->p_kont[k]->sim[va].keynum;
        if(!fnum) break;

        if(LOWORD(wParam) == SB_LINEUP)
          frame += 1;
        if(LOWORD(wParam) == SB_LINEDOWN)
          frame -= 1;

        if(frame > fnum || frame < 0)
          frame = 0;
        
        p_cnf->p_kont[k]->sim[va].time = frame2time(frame);
        itoa(frame2time(frame),pom,10);
        SetDlgItemText(hDlg, IDC_EDIT2, pom);
        itoa(frame,pom,10);
        SetDlgItemText(hDlg, IDC_EDIT1, pom);
        
        doe_nastav_slider(hDlg, p_cnf->p_kont[k]->sim[va].time);
        ani_animuj_scenu_vybr_animace(p_cnf, p_cnf->p_kont[k], p_cnf->p_kont[k]->sim[va].time);
      }
      break;

    case WM_INITDIALOG: 
      {
        nastav_pozici_bl();
        doe_init_low_kontejnery(hDlg, p_cnf);
        doe_init_ani_kontejner(hDlg, p_cnf);
        doe_hs_posfce(doe_animace_updatuj_slider);
        p_cnf->ak.aktivni = TRUE;
        p_cnf->ak.hwnd_dialog = hDlg;
      } 
      break;
    case WM_DESTROY: {
      p_cnf->ak.aktivni = FALSE;
      p_cnf->ak.hwnd_dialog = NULL;
      doe_hs_zrus();
      EndDialog( hDlg, FALSE );
      break; 
    }
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_joint_pridej_animaci(HWND hcombo, EDIT_OBJEKT *p_obj)
{
  JOINT_ANIMACE *p_anim = p_obj->p_joit_animace;

  while(p_anim) {
    SendMessage(hcombo,CB_SETITEMDATA,SendMessage(hcombo,CB_ADDSTRING,0,(LPARAM)(LPCTSTR)p_anim->jmeno),(long)p_anim);
    p_anim = p_anim->p_next;
  }  
}

void doe_joint_vloz_animace(HWND combo, K_EDITOR *p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT    *p_obj;
  int   k,o;
  
  SendMessage(combo,CB_RESETCONTENT,0,0);
  oe_klist_reset(&k);
  while((p_kont = oe_klist_next(p_cnf,&k))) {
    oe_olist_reset(&o);
    while((p_obj = oe_olist_next(p_kont, &o))) {
      doe_joint_pridej_animaci(combo, p_obj);
    }
  }
  SendMessage(combo,CB_SETCURSEL,0,0);
}


JOINT_ANIMACE * doe_joint_vloz_animace_vybr(HWND hcombo, EDIT_OBJEKT *p_obj, byte *p_jmeno)
{
  JOINT_ANIMACE *p_anim = p_obj->p_joit_animace;
  while(p_anim) {
    if(!strcmp(p_anim->jmeno,p_jmeno))
      return(p_anim);
    p_anim = p_anim->p_next;
  }
  return(NULL);
}


void doe_joint_vloz_animace_vybrane(HWND combo, K_EDITOR *p_cnf, JOINT_ANIM_GENERATOR *p_jgen)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT    *p_obj;
  JOINT_ANIMACE  *p_anim;
  int   k,o;
    
  p_jgen->p_anim = NULL;
  
  oe_klist_reset(&k);
  while((p_kont = oe_klist_next(p_cnf,&k))) {
    oe_olist_reset(&o);
    while((p_obj = oe_olist_next(p_kont, &o))) {
      p_anim = doe_joint_vloz_animace_vybr(combo, p_obj, p_jgen->jmeno);
      if(p_anim) {
        p_anim->p_next_gen = p_jgen->p_anim;
        p_jgen->p_anim = p_anim;
      }
    }
  }  
}

BOOL CALLBACK doe_callback_prikaz_generator_animaci( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDOK: {
          JOINT_ANIM_GENERATOR *p_jgen = tmp.p_pom;
          JOINT_ANIMACE *p_anim;
          HWND combo = GetDlgItem( hDlg, IDC_COMBO1);

          p_anim = (JOINT_ANIMACE *)SendMessage(combo,CB_GETITEMDATA,SendMessage(combo,CB_GETCURSEL,0,0),0);
          if(p_anim && p_anim != (JOINT_ANIMACE *)-1) {
            strcpy(p_jgen->jmeno,p_anim->jmeno);
            doe_joint_vloz_animace_vybrane(combo, p_cnf, p_jgen);
          }

          p_jgen->framu_min = doe_vyber_kolonku_int(hDlg,IDC_EDIT1);
          p_jgen->framu_max = doe_vyber_kolonku_int(hDlg,IDC_EDIT8);
          p_jgen->scale_0 = doe_vyber_kolonku_float(hDlg,IDC_EDIT2);
          p_jgen->scale_1 = doe_vyber_kolonku_float(hDlg,IDC_EDIT3);

          p_jgen->udr_rozsah = doe_vyber_kolonku_float(hDlg,IDC_EDIT44);
          p_jgen->udr_sila = doe_vyber_kolonku_float(hDlg,IDC_EDIT46);
          p_jgen->udr_frame_min = doe_vyber_kolonku_int(hDlg,IDC_EDIT16);
          p_jgen->udr_frame_max = doe_vyber_kolonku_int(hDlg,IDC_EDIT15);
          
          p_jgen->ud_rozsah = doe_vyber_kolonku_float(hDlg,IDC_EDIT7);
          p_jgen->ud_frame_min = doe_vyber_kolonku_int(hDlg,IDC_EDIT48);
          p_jgen->ud_frame_max = doe_vyber_kolonku_int(hDlg,IDC_EDIT47);
          
          p_jgen->vt_rozsah = doe_vyber_kolonku_float(hDlg,IDC_EDIT19);
          p_jgen->vt_frame_min = doe_vyber_kolonku_int(hDlg,IDC_EDIT25);
          p_jgen->vt_frame_max = doe_vyber_kolonku_int(hDlg,IDC_EDIT24);
          p_jgen->vt_smer.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT17);
          p_jgen->vt_smer.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT18);
          p_jgen->vt_smer.z = doe_vyber_kolonku_float(hDlg,IDC_EDIT26);

          p_jgen->vtr_rozsah = doe_vyber_kolonku_float(hDlg,IDC_EDIT54);
          p_jgen->vtr_sila = doe_vyber_kolonku_float(hDlg,IDC_EDIT55);
          p_jgen->vtr_frame_min = doe_vyber_kolonku_int(hDlg,IDC_EDIT80);
          p_jgen->vtr_frame_max = doe_vyber_kolonku_int(hDlg,IDC_EDIT53);

          p_jgen->sila_vetru = doe_vyber_kolonku_float(hDlg,IDC_EDIT9);

          if(p_anim && p_anim != (JOINT_ANIMACE *)-1) {
            oe_generator_animaci(p_cnf);
          }

          EndDialog( hDlg, 1 );
          break;
        }
        case IDCANCEL:
  		    EndDialog( hDlg, 0 );
          break;
        default:
          return FALSE;
      }
      break;
      
    case WM_INITDIALOG :
      {
        JOINT_ANIM_GENERATOR *p_jgen = tmp.p_pom;        
        HWND combo = GetDlgItem( hDlg, IDC_COMBO1);

        doe_joint_vloz_animace(combo, p_cnf);

        doe_printf(hDlg,IDC_EDIT1,"%d", p_jgen->framu_min);
        doe_printf(hDlg,IDC_EDIT8,"%d", p_jgen->framu_max);
        doe_printf(hDlg,IDC_EDIT2,"%f", p_jgen->scale_0);
        doe_printf(hDlg,IDC_EDIT3,"%f", p_jgen->scale_1);

        doe_printf(hDlg,IDC_EDIT7,"%f", p_jgen->ud_rozsah);
        doe_printf(hDlg,IDC_EDIT48,"%d",p_jgen->ud_frame_min);
        doe_printf(hDlg,IDC_EDIT47,"%d",p_jgen->ud_frame_max);

        doe_printf(hDlg,IDC_EDIT44,"%f",p_jgen->udr_rozsah);
        doe_printf(hDlg,IDC_EDIT46,"%f",p_jgen->udr_sila);
        doe_printf(hDlg,IDC_EDIT16,"%d",p_jgen->udr_frame_min);
        doe_printf(hDlg,IDC_EDIT15,"%d",p_jgen->udr_frame_max);

        doe_printf(hDlg,IDC_EDIT54,"%f",p_jgen->vtr_rozsah);
        doe_printf(hDlg,IDC_EDIT55,"%f",p_jgen->vtr_sila);
        doe_printf(hDlg,IDC_EDIT80,"%d",p_jgen->vtr_frame_min);
        doe_printf(hDlg,IDC_EDIT53,"%d",p_jgen->vtr_frame_max);
        
        doe_printf(hDlg,IDC_EDIT17,"%f",p_jgen->vt_smer.x);
        doe_printf(hDlg,IDC_EDIT18,"%f",p_jgen->vt_smer.y);
        doe_printf(hDlg,IDC_EDIT26,"%f",p_jgen->vt_smer.z);
        doe_printf(hDlg,IDC_EDIT19,"%f",p_jgen->vt_rozsah);
        doe_printf(hDlg,IDC_EDIT25,"%d",p_jgen->vt_frame_min);
        doe_printf(hDlg,IDC_EDIT24,"%d",p_jgen->vt_frame_max);
        
        doe_printf(hDlg,IDC_EDIT9,"%f",p_jgen->sila_vetru);

        break;
      }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }  
  return TRUE;
}

int doe_prikaz_generator_animaci(JOINT_ANIM_GENERATOR *p_jgen)
{
  tmp.p_pom = p_jgen;
  return(DialogBox(hinst, MAKEINTRESOURCE(IDD_JOINT_ANIMACE_GENERATOR), hwnd_aplikace, doe_callback_prikaz_generator_animaci));
}



// vraci time noveho klice
int doe_callback_joint_key_edit_cti(HWND hDlg)
{
  JOINT_KEYS *p_keys = tmp.p_pom;
  int pos = tmp.i[0],
      rot = tmp.i[1],
      scs = tmp.i[2],
      frame;  
  
  tmp.i[5] = frame = doe_vyber_kolonku_int(hDlg,IDC_EDIT1);
  frame = frame2time(frame);

  // pos-keys
  if(p_keys->p_pos && pos != -1) {    
    p_keys->p_pos[pos].x = doe_vyber_kolonku_float(hDlg,IDC_EDIT2);
    p_keys->p_pos[pos].y = doe_vyber_kolonku_float(hDlg,IDC_EDIT3);
    p_keys->p_pos[pos].z = doe_vyber_kolonku_float(hDlg,IDC_EDIT7);
    
    p_keys->p_pkeys[pos].tension = doe_vyber_kolonku_float(hDlg,IDC_EDIT15);
    p_keys->p_pkeys[pos].continuity = doe_vyber_kolonku_float(hDlg,IDC_EDIT16);
    p_keys->p_pkeys[pos].bias = doe_vyber_kolonku_float(hDlg,IDC_EDIT17);
    p_keys->p_pkeys[pos].time = frame;
  } 
  
  if(p_keys->p_rot && rot != -1) {
    p_keys->p_rot[rot].x = doe_vyber_kolonku_float(hDlg,IDC_EDIT18);
    p_keys->p_rot[rot].y = doe_vyber_kolonku_float(hDlg,IDC_EDIT19);
    p_keys->p_rot[rot].z = doe_vyber_kolonku_float(hDlg,IDC_EDIT24);
    p_keys->p_rot[rot].w = doe_vyber_kolonku_float(hDlg,IDC_EDIT45);
    
    p_keys->p_rkeys[rot].tension = doe_vyber_kolonku_float(hDlg,IDC_EDIT25);
    p_keys->p_rkeys[rot].continuity = doe_vyber_kolonku_float(hDlg,IDC_EDIT26);
    p_keys->p_rkeys[rot].bias = doe_vyber_kolonku_float(hDlg,IDC_EDIT44);
    p_keys->p_rkeys[pos].time = frame;
  } 
  
  if(p_keys->p_scale && scs != -1) {
    p_keys->p_scale[scs].x = doe_vyber_kolonku_float(hDlg,IDC_EDIT46);
    p_keys->p_scale[scs].y = doe_vyber_kolonku_float(hDlg,IDC_EDIT47);
    p_keys->p_scale[scs].z = doe_vyber_kolonku_float(hDlg,IDC_EDIT48);
    
    p_keys->p_skeys[scs].tension = doe_vyber_kolonku_float(hDlg,IDC_EDIT49);
    p_keys->p_skeys[scs].continuity = doe_vyber_kolonku_float(hDlg,IDC_EDIT50);
    p_keys->p_skeys[scs].bias = doe_vyber_kolonku_float(hDlg,IDC_EDIT51);
    p_keys->p_rkeys[scs].time = frame;
  } 
  
  return(frame);
}

/*
  joint - dialog
*/
BOOL CALLBACK doe_callback_joint_key_edit( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDC_BUTTON1:  				
          break;
        case IDOK: {
          doe_callback_joint_key_edit_cti(hDlg);
          EndDialog( hDlg, TRUE );
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG:
      {
        JOINT_KEYS *p_keys = tmp.p_pom;
        int pos = tmp.i[0],
            rot = tmp.i[1],
            scs = tmp.i[2],
            frame = tmp.i[3];        

        doe_printf(hDlg,IDC_EDIT1,"%d",frame);
        
        // pos-keys
        if(p_keys->p_pos && pos != -1) {
          doe_printf(hDlg,IDC_EDIT2,"%f",p_keys->p_pos[pos].x);
          doe_printf(hDlg,IDC_EDIT3,"%f",p_keys->p_pos[pos].y);
          doe_printf(hDlg,IDC_EDIT7,"%f",p_keys->p_pos[pos].z);

          doe_printf(hDlg,IDC_EDIT15,"%f",p_keys->p_pkeys[pos].tension);
          doe_printf(hDlg,IDC_EDIT16,"%f",p_keys->p_pkeys[pos].continuity);
          doe_printf(hDlg,IDC_EDIT17,"%f",p_keys->p_pkeys[pos].bias);
        } else {
          EnableDlgItem(hDlg, IDC_EDIT2, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT3, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT7, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT15,FALSE);
          EnableDlgItem(hDlg, IDC_EDIT16,FALSE);
          EnableDlgItem(hDlg, IDC_EDIT17,FALSE);
        }

        if(p_keys->p_rot && rot != -1) {
          doe_printf(hDlg,IDC_EDIT18,"%f",p_keys->p_rot[rot].x);
          doe_printf(hDlg,IDC_EDIT19,"%f",p_keys->p_rot[rot].y);
          doe_printf(hDlg,IDC_EDIT24,"%f",p_keys->p_rot[rot].z);
          doe_printf(hDlg,IDC_EDIT45,"%f",p_keys->p_rot[rot].w);

          doe_printf(hDlg,IDC_EDIT25,"%f",p_keys->p_rkeys[rot].tension);
          doe_printf(hDlg,IDC_EDIT26,"%f",p_keys->p_rkeys[rot].continuity);
          doe_printf(hDlg,IDC_EDIT44,"%f",p_keys->p_rkeys[rot].bias);
        } else {
          EnableDlgItem(hDlg, IDC_EDIT18, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT19, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT24, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT45, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT25, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT26, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT44, FALSE);
        }

        if(p_keys->p_scale && scs != -1) {
          doe_printf(hDlg,IDC_EDIT46,"%f",p_keys->p_scale[scs].x);
          doe_printf(hDlg,IDC_EDIT47,"%f",p_keys->p_scale[scs].y);
          doe_printf(hDlg,IDC_EDIT38,"%f",p_keys->p_scale[scs].z);

          doe_printf(hDlg,IDC_EDIT49,"%f",p_keys->p_skeys[scs].tension);
          doe_printf(hDlg,IDC_EDIT50,"%f",p_keys->p_skeys[scs].continuity);
          doe_printf(hDlg,IDC_EDIT51,"%f",p_keys->p_skeys[scs].bias);
        } else {
          EnableDlgItem(hDlg, IDC_EDIT46, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT47, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT48, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT49, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT50, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT51, FALSE);          
        }

        break;   
      }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

// vraci cas noveho klice
int doe_joint_key_edit(HWND hwnd, JOINT_KEYS *p_keys, int pos, int rot, int scs, int frame)
{
  tmp.p_pom = p_keys;
  tmp.i[0] = pos;
  tmp.i[1] = rot;
  tmp.i[2] = scs;
  tmp.i[3] = frame;
  return(DialogBox( hinst, MAKEINTRESOURCE(IDD_JOINT_FRAME), hwnd, doe_callback_joint_key_edit));    
}

int doe_joint_animace_updatuj_slider(int frame)
{
  EDIT_JOINT *p_jc = &p_cnf->jcf;
  HWND  hDlg = p_jc->hDlg;
  byte  pom[200];
  int   time;
  
  if(!p_jc->ak)
    return(FALSE);
  
  time = frame2time(frame);
  itoa(time, pom, 10);
  SetDlgItemText(hDlg, IDC_EDIT2, pom);
  itoa(frame,pom,10);
  SetDlgItemText(hDlg, IDC_EDIT1, pom);
  
  return(FALSE);
}

void doe_joint_animace_vloz_frame(K_EDITOR *p_cnf)
{ 
  EDIT_JOINT *p_jc = &p_cnf->jcf;  
  JOINT *p_joint = p_jc->p_joint_akt;
  JOINT_ANIMACE *p_anim = p_jc->p_joint_animace_akt;
  HWND hDlg = p_jc->hDlg;
  byte pom[200];
 
  if(p_jc->ak && p_anim) {
    SetDlgItemText( hDlg, IDC_STATIC2, p_anim->jmeno);
    SetDlgItemText( hDlg, IDC_STATIC3, itoa(p_anim->framenum,pom,10));
    
    itoa(time2frame(p_anim->time),pom,10); // vlozi frame
    SetDlgItemText( hDlg, IDC_EDIT1, pom);
    
    sprintf(pom,"%d",p_anim->time);       // vlozi cas
    SetDlgItemText( hDlg, IDC_EDIT2, pom);
    
    SetDlgItemText( hDlg, IDC_STATIC8, "---"); // delka animace
    sprintf(pom,"%f",frame2sec(p_anim->framenum)); // delka v sekundach
    SetDlgItemText( hDlg, IDC_STATIC27, pom);
    
    CheckDlgButton(hDlg,IDC_CHECK1,p_anim->flag&GK_LOOP);
  }
}

/*
  1) child animace
  2) next animace
*/
void doe_joint_animace_vloz_objekt_rec(HWND hDlg, JOINT *p_joint, int *p_por, int hloubka, JOINT *p_joint_akt)
{  
  byte pos[500];

  sprintf(pos,"%*s[%.2f %.2f %.2f]",2*hloubka,"",p_joint->pivot.x,p_joint->pivot.y,p_joint->pivot.z);
  SendMessage(hDlg, LB_SETITEMDATA, SendMessage( hDlg, LB_ADDSTRING, 0,(LPARAM)pos),(long)p_joint);
  
  if(p_joint == p_joint_akt)
    SendMessage(hDlg, LB_SETCURSEL,(WPARAM)(*p_por),0);
  (*p_por)++;

  if(p_joint->p_child)
    doe_joint_animace_vloz_objekt_rec(hDlg, p_joint->p_child, p_por, hloubka+1, p_joint_akt);
  if(p_joint->p_next)
    doe_joint_animace_vloz_objekt_rec(hDlg, p_joint->p_next, p_por, hloubka, p_joint_akt);
}

// Vlozi objekt do dialogu (zmena objektu a tak)
void doe_joint_animace_vloz_objekt(K_EDITOR *p_cnf)
{
  JOINT_ANIMACE  *p_anim, *p_animace_akt;
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT    *p_obj;
  HWND ha,hj,hdlg = p_cnf->jcf.hDlg;
  int  handle,v = 0;
  int  curr;

  if(p_cnf->jcf.ak && p_cnf->jcf.hDlg) {
  
    ha = GetDlgItem( hdlg, IDC_LIST1);
    SendMessage(ha, LB_RESETCONTENT, 0, 0);
    
    hj = GetDlgItem( hdlg, IDC_LIST10);
    SendMessage(hj, LB_RESETCONTENT, 0, 0);
    
    handle = oe_vybr_obj_reset();
    if(!handle) {
      return;
    }

    p_obj = oe_vybr_obj_get_ind(&handle,&p_cnf->jcf.k,&p_cnf->jcf.o);
    p_kont = oe_vybr_obj_get_kont(&handle);
    
    doe_printf(p_cnf->jcf.hDlg,IDC_STATIC1, p_kont->jmeno);
    doe_printf(p_cnf->jcf.hDlg,IDC_STATIC24,p_obj->jmeno);

    p_animace_akt = p_cnf->jcf.p_joint_animace_akt;
    if(!p_animace_akt)
      p_animace_akt = p_obj->p_joit_animace;
    
    if(p_animace_akt) {
      key_kosti_interpoluj(p_animace_akt);
      key_kosti_updatuj_vertexy(p_obj,p_animace_akt);
    }

    curr = FALSE;
    p_anim = p_obj->p_joit_animace;
    while(p_anim) {
      SendMessage(ha, LB_SETITEMDATA, SendMessage( ha, LB_ADDSTRING, 0,(LPARAM)p_anim->jmeno),(long)p_anim);
      if(p_anim == p_animace_akt) {
        SendMessage(ha, LB_SETCURSEL,(WPARAM)v,0);
        curr = TRUE;
      }
      p_anim = p_anim->p_next;
      v++;
    }
    
    if(curr && p_animace_akt) {
      v = 0;
      doe_joint_animace_vloz_objekt_rec(hj, p_animace_akt->p_child, &v, 0, p_cnf->jcf.p_joint_akt);
    }
  }
}

// doplni klice na slide-bar
void doe_joint_animace_vloz_tiky(K_EDITOR *p_cnf)
{
  JOINT_ANIMACE *p_anim = p_cnf->jcf.p_joint_animace_akt;
  JOINT *p_joint = p_cnf->jcf.p_joint_akt;
  HWND   hDlg = p_cnf->jcf.hDlg;
  JOINT_KEYS *p_track;
  int    frame;
  word   i;    
  
  if(p_anim) {
    frame = time2frame(p_anim->time);
    doe_hs_nastav(0,p_anim->framenum-1,frame,"Joint animace");
  }

  doe_hs_delmark();

  /* Seznam animaci aktivni kamery
  */
  if(p_joint && p_joint->p_keys)   {
    p_track = p_joint->p_keys;    
    
    for(i = 0; i < p_track->pos_keys; i++) {
      assert(p_track->p_pkeys);
      doe_hs_setmark(time2frame(p_track->p_pkeys[i].time));
    }
    for(i = 0; i < p_track->rot_keys; i++) {
      assert(p_track->p_rkeys);
      doe_hs_setmark(time2frame(p_track->p_rkeys[i].time));
    }
    for(i = 0; i < p_track->scs_keys; i++) {
      assert(p_track->p_skeys);
      doe_hs_setmark(time2frame(p_track->p_skeys[i].time));
    }
  }
}

// vlozi hodnoty klicu
void doe_joint_animace_vloz_text(K_EDITOR *p_cnf)
{
  JOINT_ANIMACE *p_anim = p_cnf->jcf.p_joint_animace_akt;
  JOINT_KEYS *p_track;
  JOINT *p_joint = p_cnf->jcf.p_joint_akt;
  HWND hDlg = p_cnf->jcf.hDlg;
  byte pom[200];
  int  frame,klic,v;

  if(p_anim && p_joint && p_joint->p_keys) {
    p_track = p_joint->p_keys;
    
    frame = time2frame(p_anim->time);

    // pozicni klice
    klic = key_je_frame_klic(p_track->p_pkeys, p_track->pos_keys, frame);
    v = (klic != K_CHYBA);
    if(v) {
      SetDlgItemText( hDlg, IDC_EDIT3,  ftoa(p_track->p_pos[klic].x,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT7,  ftoa(p_track->p_pos[klic].y,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT15, ftoa(p_track->p_pos[klic].z,pom,0));
    } else {
      SetDlgItemText( hDlg, IDC_EDIT3,  ftoa(p_joint->p.x,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT7,  ftoa(p_joint->p.y,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT15, ftoa(p_joint->p.z,pom,0));
    }
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT3),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT7),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT15),v);
    doe_check_stlac(hDlg, IDC_CHECK3, v);

    // rotacni klice
    klic = key_je_frame_klic(p_track->p_rkeys, p_track->rot_keys, frame);
    v = (klic != K_CHYBA);
    if(v) {
      SetDlgItemText( hDlg, IDC_EDIT19, ftoa(p_track->p_rot[klic].x,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT24, ftoa(p_track->p_rot[klic].y,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT25, ftoa(p_track->p_rot[klic].z,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT26, ftoa(p_track->p_rot[klic].w,pom,0));
    } else {
      SetDlgItemText( hDlg, IDC_EDIT19, ftoa(p_joint->r.x,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT24, ftoa(p_joint->r.y,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT25, ftoa(p_joint->r.z,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT26, ftoa(p_joint->r.w,pom,0));
    }
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT19),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT24),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT25),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT26),v);
    doe_check_stlac(hDlg, IDC_CHECK4, v);

    // scale klice
    klic = key_je_frame_klic(p_track->p_skeys, p_track->scs_keys, frame);
    v = (klic != K_CHYBA);
    if(v) {
      SetDlgItemText( hDlg, IDC_EDIT46, ftoa(p_track->p_scale[klic].x,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT47, ftoa(p_track->p_scale[klic].y,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT48, ftoa(p_track->p_scale[klic].z,pom,0));
    } else {
      SetDlgItemText( hDlg, IDC_EDIT46, ftoa(p_joint->s.x,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT47, ftoa(p_joint->s.y,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT48, ftoa(p_joint->s.z,pom,0));
    }
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT46),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT47),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT48),v);
    doe_check_stlac(hDlg, IDC_CHECK5, v);
  } else {
    v = FALSE;

        // pozicni klice    
    SetDlgItemText( hDlg, IDC_EDIT3, "0");
    SetDlgItemText( hDlg, IDC_EDIT7, "0");
    SetDlgItemText( hDlg, IDC_EDIT15,"0");
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT3),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT7),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT15),v);
    doe_check_stlac(hDlg, IDC_CHECK3, v);

    // rotacni
    SetDlgItemText( hDlg, IDC_EDIT19,"0");
    SetDlgItemText( hDlg, IDC_EDIT24,"0");
    SetDlgItemText( hDlg, IDC_EDIT25,"0");
    SetDlgItemText( hDlg, IDC_EDIT26,"0");
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT19),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT24),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT25),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT26),v);
    doe_check_stlac(hDlg, IDC_CHECK4, v);

    // scale
    SetDlgItemText( hDlg, IDC_EDIT46,"0");
    SetDlgItemText( hDlg, IDC_EDIT47,"0");
    SetDlgItemText( hDlg, IDC_EDIT48,"0");
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT46),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT47),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT48),v);
    doe_check_stlac(hDlg, IDC_CHECK5, v);
  }
}

// 1 - animace
//10 - uzly
BOOL CALLBACK doe_calback_konfigurace_animace_joint(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_LIST10: // zmena jointu
          if(HIWORD(wParam) == LBN_SELCHANGE || HIWORD(wParam) == LBN_DBLCLK) {
            p_cnf->jcf.p_joint_akt = (JOINT *)SendMessage((HWND)lParam, LB_GETITEMDATA, SendMessage((HWND)lParam,LB_GETCURSEL,0,0),0);
            doe_joint_animace_vloz_objekt(p_cnf);
            doe_joint_animace_vloz_frame(p_cnf);
            doe_joint_animace_vloz_text(p_cnf);
            doe_joint_animace_vloz_tiky(p_cnf);
          }
          break;
        case IDC_LIST1: // zmena animace
          if(HIWORD(wParam) == LBN_SELCHANGE) {
            p_cnf->jcf.p_joint_animace_akt = (JOINT_ANIMACE *)SendMessage((HWND)lParam, LB_GETITEMDATA, SendMessage((HWND)lParam,LB_GETCURSEL,0,0),0);
            doe_joint_animace_vloz_objekt(p_cnf);
            doe_joint_animace_vloz_frame(p_cnf);
            doe_joint_animace_vloz_tiky(p_cnf);
            doe_joint_animace_vloz_text(p_cnf);
          }
          break;
        case IDC_BUTTON29: // delka animace
          {
            byte pom[200] = "100";
            if(p_cnf->jcf.p_joint_animace_akt && doe_cti_jmeno_ext(hwnd_aplikace,pom,"Delka animace")) {
              p_cnf->jcf.p_joint_animace_akt->framenum = atoi(pom);
              doe_joint_animace_vloz_frame(p_cnf);
              doe_joint_animace_vloz_tiky(p_cnf);
            }
          }
          break;
        case IDC_EDIT1:
          {
          JOINT_ANIMACE *p_anim = p_cnf->jcf.p_joint_animace_akt;
          EDIT_OBJEKT *p_obj = oe_cti_joint_objekt(p_cnf);
          byte pom[100];
          /* Zadava frame animace
          */
          if(!p_anim || !p_anim->framenum || !p_obj)
            break;

          if(HIWORD(wParam) == EN_CHANGE) {            
            GetDlgItemText( hDlg, LOWORD(wParam), pom,99);            
            p_anim->time = frame2time(atoi(pom));
            itoa(p_anim->time,pom,10);
            SetDlgItemText(hDlg, IDC_EDIT2, pom);
            doe_nastav_slider(hDlg,p_anim->time);
            
            // nastaveni animace (interpolace) na tuto pozici        
            key_kosti_animuj(p_anim);
            key_kosti_updatuj_vertexy(p_obj,p_anim);
            doe_joint_animace_vloz_text(p_cnf);
          }
          break;
        }
        
        case IDC_BUTTON1: // nova animace
          oe_prikaz_kost_pridej_start(p_cnf);
          break;
        case IDC_BUTTON3: // kopie
          oe_prikaz_kost_kopiruj(p_cnf);
          break;
        case IDC_BUTTON2: // smazat
          oe_prikaz_kost_smaz_animaci(p_cnf);
          break;
        case IDC_BUTTON14: // uprava
          {
            JOINT_ANIMACE *p_anim = p_cnf->jcf.p_joint_animace_akt;
            if(p_anim) {
              doe_cti_jmeno_ext(hDlg,p_anim->jmeno,"Jmeno animace");
            }
          }
          break;
        case IDC_BUTTON4: // novy joint
          oe_prikaz_kost_pridej_navaz(p_cnf);
          break;
        case IDC_BUTTON13:// smazat joint
          oe_prikaz_kost_smaz(p_cnf);
          break;
        case IDC_BUTTON5:          
          doe_prikaz_generator_animaci(&p_cnf->jgen);
          break;
        case IDC_CHECK2:
          p_cnf->jgen.anim_vse = IsDlgButtonChecked(hDlg,IDC_CHECK2);
          break;
        case IDC_CHECK1:
          { // zaskrkavani animaci
            JOINT_ANIMACE *p_anim = p_cnf->jcf.p_joint_animace_akt;
            if(p_anim) {
              p_anim->flag = IsDlgButtonChecked(hDlg,IDC_CHECK1) ? p_anim->flag|GK_LOOP : p_anim->flag&~GK_LOOP;
            }
            break;
          }
        case IDC_CHECK3: // pos
          {
            JOINT *p_joint = p_cnf->jcf.p_joint_akt;
            JOINT_KEYS *p_track = key_kost_spline_vyrob(p_joint);
            JOINT_ANIMACE *p_anim = p_cnf->jcf.p_joint_animace_akt;
            KEY_POINT_BRS key = {0,0,0,0,0,0};
            byte pom[200];
            
            if(p_anim && p_anim->framenum && p_track) {
              GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
              key.time = frame2time(atoi(pom));
              
              if(key_je_time_klic(p_track->p_pkeys, p_track->pos_keys, key.time) != K_CHYBA) { // smazat
                key_track_klic_smaz((byte **)&p_track->p_pos, &p_track->p_pkeys, &p_track->pos_keys, sizeof(p_track->p_pos[0]),key.time);
              } else { // vlozit
                p_track->pos_keys = key_track_klic_vloz((byte **)&p_track->p_pos, &p_track->p_pkeys, p_track->pos_keys, sizeof(p_track->p_pos[0]), (byte *)&p_joint->p, &key);
              }

              doe_joint_animace_vloz_text(p_cnf);
              doe_joint_animace_vloz_tiky(p_cnf);              
            }
            break;
          }
        case IDC_CHECK4: // rot
          {
            JOINT *p_joint = p_cnf->jcf.p_joint_akt;
            JOINT_KEYS *p_track = key_kost_spline_vyrob(p_joint);
            JOINT_ANIMACE *p_anim = p_cnf->jcf.p_joint_animace_akt;
            KEY_POINT_BRS key = {0,0,0,0,0,0};
            byte pom[200];
            
            if(p_anim && p_anim->framenum && p_track) {
              GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
              key.time = frame2time(atoi(pom));
              
              if(key_je_time_klic(p_track->p_rkeys, p_track->rot_keys, key.time) != K_CHYBA) { // smazat
                key_track_klic_smaz((byte **)&p_track->p_rot, &p_track->p_rkeys, &p_track->rot_keys, sizeof(p_track->p_rot[0]),key.time);
              } else { // vlozit
                p_track->rot_keys = key_track_klic_vloz((byte **)&p_track->p_rot, &p_track->p_rkeys, p_track->rot_keys, sizeof(p_track->p_rot[0]), (byte *)&p_joint->r, &key);
              }
              doe_joint_animace_vloz_text(p_cnf);
              doe_joint_animace_vloz_tiky(p_cnf);
            }
            break;
          }          
        case IDC_CHECK5:// scs
          {
            JOINT *p_joint = p_cnf->jcf.p_joint_akt;
            JOINT_KEYS *p_track = key_kost_spline_vyrob(p_joint);
            JOINT_ANIMACE *p_anim = p_cnf->jcf.p_joint_animace_akt;
            KEY_POINT_BRS key = {0,0,0,0,0,0};
            byte pom[200];
            
            if(p_anim && p_anim->framenum && p_track) {
              GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
              key.time = frame2time(atoi(pom));
              
              if(key_je_time_klic(p_track->p_skeys, p_track->scs_keys, key.time) != K_CHYBA) { // smazat
                key_track_klic_smaz((byte **)&p_track->p_scale, &p_track->p_skeys, &p_track->scs_keys, sizeof(p_track->p_scale[0]),key.time);
              } else { // vlozit
                p_track->scs_keys = key_track_klic_vloz((byte **)&p_track->p_scale, &p_track->p_skeys, p_track->scs_keys, sizeof(p_track->p_scale[0]), (byte *)&p_joint->s, &key);              
              }
              doe_joint_animace_vloz_text(p_cnf);
              doe_joint_animace_vloz_tiky(p_cnf);
            }
            break;
          }
        case IDC_BUTTON9: // upravit klic
          {
            JOINT_ANIMACE *p_anim = p_cnf->jcf.p_joint_animace_akt;
            JOINT_KEYS *p_key;
            JOINT *p_joint = p_cnf->jcf.p_joint_akt;
            EDIT_OBJEKT *p_obj = oe_cti_joint_objekt(p_cnf);
            int frame,pos,rot,scs,time = -1;

            if(!p_anim || !p_anim->framenum || !p_joint || !p_joint->p_keys)
              break;
            
            frame = time2frame(p_anim->time);
            p_key = p_joint->p_keys;
            
            pos = key_je_frame_klic(p_key->p_pkeys, p_key->pos_keys, frame);
            rot = key_je_frame_klic(p_key->p_rkeys, p_key->rot_keys, frame);
            scs = key_je_frame_klic(p_key->p_skeys, p_key->scs_keys, frame);

            if(doe_joint_key_edit(hDlg,p_key,pos,rot,scs,frame)) {
              key_kosti_serad_klice((byte *)p_key->p_pos, p_key->p_pkeys, p_key->pos_keys, sizeof(p_key->p_pos[0]));
              key_kosti_serad_klice((byte *)p_key->p_rot, p_key->p_rkeys, p_key->rot_keys, sizeof(p_key->p_rot[0]));
              key_kosti_serad_klice((byte *)p_key->p_scale, p_key->p_skeys, p_key->scs_keys, sizeof(p_key->p_scale[0]));
              p_anim->time = frame2time(tmp.i[5]);
              key_kosti_interpoluj(p_anim);
              key_kosti_updatuj_vertexy(p_obj,p_anim);
              doe_joint_animace_vloz_tiky(p_cnf);
              doe_joint_animace_vloz_frame(p_cnf);
            }
            break;
          }
        case IDC_BUTTON8:  // []
          {
            JOINT_ANIMACE *p_anim = p_cnf->jcf.p_play;
            while(p_anim) {              
              p_anim->stav = FALSE;
              p_anim = p_anim->p_game_next;
            }
            p_cnf->jcf.p_play = NULL;            
            break;
          }
        case IDC_BUTTON11: // >
          {
            if(p_cnf->jgen.anim_vse) {
              EDIT_KONTEJNER *p_kont;
              EDIT_OBJEKT *p_obj;
              int k,o;
              
              p_cnf->jcf.p_play = NULL;

              oe_klist_reset(&k);
              while((p_kont = oe_klist_next(p_cnf,&k))) {
                oe_olist_reset(&o);
                while((p_obj = oe_olist_next(p_kont, &o))) {
                  if(p_obj->p_joit_animace) {
                    p_obj->p_joit_animace->p_game_next = p_cnf->jcf.p_play;
                    if(p_cnf->jcf.p_play) {
                      p_cnf->jcf.p_play->p_game_prev = p_obj->p_joit_animace;
                    }
                    p_obj->p_joit_animace->p_game_prev = NULL;
                    p_cnf->jcf.p_play = p_obj->p_joit_animace;
                    p_cnf->jcf.p_play->p_obj = p_obj;
                  }
                }
              }
            } else {
              p_cnf->jcf.p_play = p_cnf->jcf.p_joint_animace_akt;
              if(p_cnf->jcf.p_play) {
                p_cnf->jcf.p_play->p_game_next = NULL;
                p_cnf->jcf.p_play->p_game_prev = NULL;
              }
            }

            {
              JOINT_ANIMACE *p_anim = p_cnf->jcf.p_play;
              while(p_anim) {
                if(p_anim->framenum) {
                  doe_anim_start_et(p_cnf->TimeEndLastFrame,calc_endtime(p_anim->framenum),
                    &p_anim->time_start,&p_anim->time_stop,
                    &p_anim->time_delka,&p_anim->time,
                    &p_anim->start,&p_anim->stop);
                  p_anim->stav = TRUE;
                }
                p_anim = p_anim->p_game_next;
              }
            }

            break;
          }
        default:
          return FALSE;
      }
      break;    
    case WM_VSCROLL:
      if(LOWORD(wParam) == SB_LINEUP|| LOWORD(wParam) == SB_LINEDOWN) {
        JOINT_ANIMACE *p_anim = p_cnf->jcf.p_joint_animace_akt;
        EDIT_OBJEKT *p_obj = oe_cti_joint_objekt(p_cnf);
        byte  pom[200];
        int   frame;
        int   k = p_cnf->vybr_kont;
        int   fnum;

        if(!p_anim || !p_anim->framenum || !p_obj)
          break;
        
        GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
        frame = atoi(pom);
        
        fnum = p_anim->framenum;
        if(!fnum) break;

        if(LOWORD(wParam) == SB_LINEUP)
          frame += 1;
        if(LOWORD(wParam) == SB_LINEDOWN)
          frame -= 1;

        if(frame > fnum || frame < 0)
          frame = 0;
        
        p_anim->time = frame2time(frame);
        itoa(frame2time(frame),pom,10);
        SetDlgItemText(hDlg, IDC_EDIT2, pom);
        itoa(frame,pom,10);
        SetDlgItemText(hDlg, IDC_EDIT1, pom);        
      }
      break;

    case WM_INITDIALOG: {
        nastav_pozici_bl();
        p_cnf->jcf.ak = TRUE;
        p_cnf->jcf.hDlg = hDlg;        
        doe_hs_posfce(doe_joint_animace_updatuj_slider);
        doe_joint_animace_vloz_objekt(p_cnf);
        if(p_cnf->jcf.p_joint_animace_akt) { // delka animace
          doe_printf(hDlg,IDC_EDIT52,"%d",p_cnf->jcf.p_joint_animace_akt->framenum);
        }
        doe_joint_animace_vloz_frame(p_cnf);
        doe_joint_animace_vloz_tiky(p_cnf);
        doe_joint_animace_vloz_text(p_cnf);

        doe_check_stlac(hDlg, IDC_CHECK2, p_cnf->jgen.anim_vse);
      } 
      break;
    case WM_DESTROY: {
      p_cnf->jcf.ak = FALSE;
      p_cnf->jcf.hDlg = NULL;
      doe_hs_zrus();
      EndDialog( hDlg, FALSE );
      break; 
    }
    default:
      return FALSE;
  }
  return TRUE;
}

static void doe_init_kamery(HWND hDlg, K_EDITOR *p_cnf)
{
  GLMATRIX *p_cam = p_cnf->p_camera;
  HWND    hwnd = GetDlgItem(hDlg,IDC_LIST1);  
  byte    pom[200];
  int     i;

  /* Seznam kamer
  */
  SendMessage(hwnd, LB_RESETCONTENT, 0, 0);
  
  doe_printf(hDlg,IDC_EDIT4, "%f",p_cam->_11);
  doe_printf(hDlg,IDC_EDIT5, "%f",p_cam->_12);
  doe_printf(hDlg,IDC_EDIT25,"%f",p_cam->_13);

  doe_printf(hDlg,IDC_EDIT26,"%f",p_cam->_21);
  doe_printf(hDlg,IDC_EDIT44,"%f",p_cam->_22);
  doe_printf(hDlg,IDC_EDIT45,"%f",p_cam->_23);

  doe_printf(hDlg,IDC_EDIT46,"%f",p_cam->_31);
  doe_printf(hDlg,IDC_EDIT47,"%f",p_cam->_32);
  doe_printf(hDlg,IDC_EDIT48,"%f",p_cam->_33);

  if(!p_cnf->kamnum)
    return;
  if(p_cnf->kamakt > p_cnf->kamnum)
    p_cnf->kamakt = 0;
  
  for(i = 0; i < p_cnf->kamnum; i++) {
    sprintf(pom,"%d %s",i,p_cnf->kamery[i].jmeno);
    SendMessage( hwnd, LB_ADDSTRING, 0,(LPARAM)pom);
  }
  
  SendMessage(hwnd,LB_SETCURSEL,p_cnf->kamakt,0);
}

static void doe_init_kamery_animace(HWND hDlg, K_EDITOR *p_cnf)
{
  HWND    ahwnd = GetDlgItem(hDlg,IDC_LIST9);
  KAMERA *p_kam;
  byte    pom[200];
  int     i;

  /* Seznam kamer
  */
  if(!p_cnf->kamnum)
    return;
  if(p_cnf->kamakt > p_cnf->kamnum)
    p_cnf->kamakt = 0;

  /* Seznam animaci aktivni kamery
  */
  p_kam = p_cnf->kamery+p_cnf->kamakt;
  SendMessage(ahwnd, LB_RESETCONTENT, 0, 0);
  
  for(i = 0; i < KAM_MAX_ANIM; i++) {
    if(p_kam->tracky[i].endtime) {
      sprintf(pom,"%d frm %d '%s'",i,calc_framenum(p_kam->tracky[i].endtime),p_kam->tracky[i].jmeno);
    } else {
      strcpy(pom,"...");
    }
    SendMessage( ahwnd, LB_ADDSTRING, 0,(LPARAM)pom);
  }
  SendMessage(ahwnd,LB_SETCURSEL,p_kam->akt_animace,0);
}

void doe_kamera_slide_vloz_tiky(HWND hDlg, K_EDITOR *p_cnf)
{
  KAMERA_TRACK_INFO *p_track;
  KAMERA *p_kam;
  word    i;

  doe_hs_delmark();

  /* Seznam kamer
  */
  if(!p_cnf->kamnum)
    return;
  if(p_cnf->kamakt > p_cnf->kamnum)
    p_cnf->kamakt = 0;
  
  /* Seznam animaci aktivni kamery
  */
  p_kam = p_cnf->kamery+p_cnf->kamakt;
  p_track = p_kam->tracky+p_kam->akt_animace;
  
  for(i = 0; i < p_track->fov_keys; i++)
    doe_hs_setmark(time2frame(p_track->p_fkeys[i].time));
  for(i = 0; i < p_track->pos_keys; i++)
    doe_hs_setmark(time2frame(p_track->p_pkeys[i].time));
  for(i = 0; i < p_track->roll_keys; i++)
    doe_hs_setmark(time2frame(p_track->p_rlkeys[i].time));
  for(i = 0; i < p_track->trg_keys; i++)
    doe_hs_setmark(time2frame(p_track->p_tkeys[i].time));
}

static void doe_init_kamery_animace_dialog(HWND hDlg, K_EDITOR *p_cnf)
{
  KAMERA_TRACK_INFO *p_track;
  KAMERA *p_kam;
  byte    pom[200];
  int     des,sec,min;

  /* Seznam kamer
  */
  if(p_cnf->kamnum) {
    if(p_cnf->kamakt > p_cnf->kamnum)
      p_cnf->kamakt = 0;
    
    /* Seznam animaci aktivni kamery
    */
    p_kam = p_cnf->kamery+p_cnf->kamakt;
    p_track = p_kam->tracky+p_kam->akt_animace;
    
    SetDlgItemText(hDlg, IDC_STATIC2, p_track->jmeno);
    SetDlgItemText(hDlg, IDC_STATIC3, itoa(calc_framenum(p_track->endtime),pom,10)); // framu
    sprintf(pom,"%f",(float)calc_framenum(p_track->endtime)/(float)FRAMU_PER_SEC);
    SetDlgItemText(hDlg, IDC_STATIC8, pom); // sec
    SetDlgItemText(hDlg, IDC_STATIC27,itoa(FRAMU_PER_SEC,pom,10));// fps
    
    // akt frame
    SetDlgItemText(hDlg, IDC_EDIT1, itoa(time2frame(p_track->time),pom,10));
    
    // akt time  
    doe_sec_to_time(time2sec(p_track->time), &des, &sec, &min);
    sprintf(pom,"%.2d:%.2d:%.2d",min,sec,des);
    SetDlgItemText( hDlg, IDC_EDIT2, pom);  

    CheckDlgButton(hDlg, IDC_CHECK2, p_track->flag&GK_LOOP ?
                         BST_CHECKED : BST_UNCHECKED);
    
    // nastaveni slideru
    doe_hs_nastav(0,calc_keynum(p_track->endtime)-1,0,"Kamera %s",p_kam->jmeno);
  } else {    
    SetDlgItemText(hDlg, IDC_STATIC2, "Velky kulovy");
    SetDlgItemText(hDlg, IDC_STATIC3, "0"); // framu    
    SetDlgItemText(hDlg, IDC_STATIC8, "0"); // sec
    SetDlgItemText(hDlg, IDC_STATIC27,"0");// fps
    
    SetDlgItemText(hDlg, IDC_EDIT1, "0");    
    SetDlgItemText(hDlg, IDC_EDIT2, "00:00:00");
    CheckDlgButton(hDlg, IDC_CHECK2, BST_UNCHECKED);
    doe_hs_nastav(0,0,0,"Zadna kamera");
  }
  doe_kamera_slide_vloz_tiky(hDlg, p_cnf);
}

// vraci time noveho klice
int doe_callback_kamera_key_edit_cti(HWND hDlg)
{
  KAMERA_TRACK_INFO *p_keys = tmp.p_pom;  
  int pos  = tmp.i[0],
      trg  = tmp.i[1],
      roll = tmp.i[2],
      fov  = tmp.i[3],
      frame;
  
  tmp.i[5] = frame = doe_vyber_kolonku_int(hDlg,IDC_EDIT1);
  frame = frame2time(frame);

  // pos-keys
  if(p_keys->p_pos && pos != -1) {    
    p_keys->p_pos[pos].x = doe_vyber_kolonku_float(hDlg,IDC_EDIT2);
    p_keys->p_pos[pos].y = doe_vyber_kolonku_float(hDlg,IDC_EDIT3);
    p_keys->p_pos[pos].z = doe_vyber_kolonku_float(hDlg,IDC_EDIT7);
    
    p_keys->p_pkeys[pos].tension = doe_vyber_kolonku_float(hDlg,IDC_EDIT25);
    p_keys->p_pkeys[pos].continuity = doe_vyber_kolonku_float(hDlg,IDC_EDIT26);
    p_keys->p_pkeys[pos].bias = doe_vyber_kolonku_float(hDlg,IDC_EDIT44);
    p_keys->p_pkeys[pos].time = frame;
  } 

  if(p_keys->p_trg && trg != -1) {
    p_keys->p_trg[trg].x = doe_vyber_kolonku_float(hDlg,IDC_EDIT15);
    p_keys->p_trg[trg].y = doe_vyber_kolonku_float(hDlg,IDC_EDIT16);
    p_keys->p_trg[trg].z = doe_vyber_kolonku_float(hDlg,IDC_EDIT17);
    
    p_keys->p_tkeys[trg].tension = doe_vyber_kolonku_float(hDlg,IDC_EDIT45);
    p_keys->p_tkeys[trg].continuity = doe_vyber_kolonku_float(hDlg,IDC_EDIT46);
    p_keys->p_tkeys[trg].bias = doe_vyber_kolonku_float(hDlg,IDC_EDIT47);
    p_keys->p_pkeys[trg].time = frame;
  }

  if(p_keys->p_roll && roll != -1) {
    p_keys->p_roll[roll] = doe_vyber_kolonku_float(hDlg,IDC_EDIT18);
    
    p_keys->p_rlkeys[roll].tension = doe_vyber_kolonku_float(hDlg,IDC_EDIT48);
    p_keys->p_rlkeys[roll].continuity = doe_vyber_kolonku_float(hDlg,IDC_EDIT49);
    p_keys->p_rlkeys[roll].bias = doe_vyber_kolonku_float(hDlg,IDC_EDIT50);
  }
  
  if(p_keys->p_fov && fov != -1) {
    p_keys->p_fov[fov] = doe_vyber_kolonku_float(hDlg,IDC_EDIT19);
    
    p_keys->p_fkeys[fov].tension = doe_vyber_kolonku_float(hDlg,IDC_EDIT51);
    p_keys->p_fkeys[fov].continuity = doe_vyber_kolonku_float(hDlg,IDC_EDIT52);
    p_keys->p_fkeys[fov].bias = doe_vyber_kolonku_float(hDlg,IDC_EDIT53);
  }

  return(frame);
}

/*
  joint - dialog
*/
BOOL CALLBACK doe_callback_animace_kamery_uprav_frame( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDC_BUTTON1:  				
          break;
        case IDOK: {
          doe_callback_kamera_key_edit_cti(hDlg);
          EndDialog( hDlg, TRUE );
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG:
      {
        KAMERA_TRACK_INFO *p_keys = tmp.p_pom;
        int pos = tmp.i[0],
            trg = tmp.i[1],
            roll = tmp.i[2],
            fov = tmp.i[3],
            frame = tmp.i[4];

        doe_printf(hDlg,IDC_EDIT1,"%d",frame);

        // pos-keys
        if(p_keys->p_pos && pos != -1) {
          doe_printf(hDlg,IDC_EDIT2,"%f",p_keys->p_pos[pos].x);
          doe_printf(hDlg,IDC_EDIT3,"%f",p_keys->p_pos[pos].y);
          doe_printf(hDlg,IDC_EDIT7,"%f",p_keys->p_pos[pos].z);

          doe_printf(hDlg,IDC_EDIT25,"%f",p_keys->p_pkeys[pos].tension);
          doe_printf(hDlg,IDC_EDIT26,"%f",p_keys->p_pkeys[pos].continuity);
          doe_printf(hDlg,IDC_EDIT44,"%f",p_keys->p_pkeys[pos].bias);
        } else {
          EnableDlgItem(hDlg, IDC_EDIT2, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT3, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT7, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT25,FALSE);
          EnableDlgItem(hDlg, IDC_EDIT26,FALSE);
          EnableDlgItem(hDlg, IDC_EDIT44,FALSE);
        }

        if(p_keys->p_trg && trg != -1) {
          doe_printf(hDlg,IDC_EDIT15,"%f",p_keys->p_trg[trg].x);
          doe_printf(hDlg,IDC_EDIT16,"%f",p_keys->p_trg[trg].y);
          doe_printf(hDlg,IDC_EDIT17,"%f",p_keys->p_trg[trg].z);

          doe_printf(hDlg,IDC_EDIT45,"%f",p_keys->p_tkeys[trg].tension);
          doe_printf(hDlg,IDC_EDIT46,"%f",p_keys->p_tkeys[trg].continuity);
          doe_printf(hDlg,IDC_EDIT47,"%f",p_keys->p_tkeys[trg].bias);
        } else {
          EnableDlgItem(hDlg, IDC_EDIT15, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT16, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT17, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT45, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT46, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT47, FALSE);
        }

        if(p_keys->p_fov && fov != -1) {
          doe_printf(hDlg,IDC_EDIT18,"%f",p_keys->p_fov[fov]);

          doe_printf(hDlg,IDC_EDIT48,"%f",p_keys->p_fkeys[fov].tension);
          doe_printf(hDlg,IDC_EDIT49,"%f",p_keys->p_fkeys[fov].continuity);
          doe_printf(hDlg,IDC_EDIT50,"%f",p_keys->p_fkeys[fov].bias);
        } else {
          EnableDlgItem(hDlg, IDC_EDIT18, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT48, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT49, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT50, FALSE);
        }

        if(p_keys->p_roll && roll != -1) {
          doe_printf(hDlg,IDC_EDIT19,"%f",p_keys->p_roll[roll]);

          doe_printf(hDlg,IDC_EDIT51,"%f",p_keys->p_rlkeys[roll].tension);
          doe_printf(hDlg,IDC_EDIT52,"%f",p_keys->p_rlkeys[roll].continuity);
          doe_printf(hDlg,IDC_EDIT53,"%f",p_keys->p_rlkeys[roll].bias);
        } else {
          EnableDlgItem(hDlg, IDC_EDIT19, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT51, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT52, FALSE);
          EnableDlgItem(hDlg, IDC_EDIT53, FALSE);
        }
        break;   
      }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

// vraci cas noveho klice
int doe_animace_kamery_uprav_frame(HWND hwnd, KAMERA_TRACK_INFO *p_keys, int pos, int trg, int roll, int fov, int frame)
{
  tmp.p_pom = p_keys;
  tmp.i[0] = pos;
  tmp.i[1] = trg;
  tmp.i[2] = roll;
  tmp.i[3] = fov;
  tmp.i[4] = frame;
  return(DialogBox( hinst, MAKEINTRESOURCE(IDD_KAMERA_FRAME), hwnd, doe_callback_animace_kamery_uprav_frame));
}

//IDC_EDIT11 - spodni konzole
void doe_animace_updatuj_kamery_text(K_EDITOR *p_cnf, KAMERA *p_kam)
{
  HWND hDlg = p_cnf->akkam.hwnd_dialog;
  KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
  byte pom[200];
  int  frame,klic,v;
  int  des,sec,min;

  if(p_track->endtime) {

    frame = time2frame(p_track->time);            
    doe_sec_to_time(frame2sec(p_track->time), &des, &sec, &min);
    sprintf(pom,"%.2d:%.2d:%.2d",min,sec,des);
    SetDlgItemText( hDlg, IDC_EDIT2, pom);  

    // pozicni klice
    SetDlgItemText( hDlg, IDC_EDIT3,  ftoa(p_kam->p.x,pom,0));
    SetDlgItemText( hDlg, IDC_EDIT7,  ftoa(p_kam->p.y,pom,0));
    SetDlgItemText( hDlg, IDC_EDIT15, ftoa(p_kam->p.z,pom,0));    
    klic = key_je_frame_klic(p_track->p_pkeys, p_track->pos_keys,frame);
    v = (klic != K_CHYBA);
    if(v) {
      SetDlgItemText( hDlg, IDC_EDIT25, ftoa(p_track->p_pkeys[klic].continuity,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT26, ftoa(p_track->p_pkeys[klic].tension,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT44, ftoa(p_track->p_pkeys[klic].bias,pom,0));
    }
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT3),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT7),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT15),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT25),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT26),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT44),v);
    doe_check_stlac(hDlg, IDC_CHECK3, v);

    // target-klice
    SetDlgItemText( hDlg, IDC_EDIT16, ftoa(p_kam->t.x,pom,0));
    SetDlgItemText( hDlg, IDC_EDIT17, ftoa(p_kam->t.y,pom,0));
    SetDlgItemText( hDlg, IDC_EDIT18, ftoa(p_kam->t.z,pom,0));
    klic = key_je_frame_klic(p_track->p_tkeys, p_track->trg_keys, frame);
    v = (klic != K_CHYBA);
    if(v) {
      SetDlgItemText( hDlg, IDC_EDIT45, ftoa(p_track->p_tkeys[klic].continuity,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT46, ftoa(p_track->p_tkeys[klic].tension,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT47, ftoa(p_track->p_tkeys[klic].bias,pom,0));
    }
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT16),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT17),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT18),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT45),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT46),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT47),v);
    doe_check_stlac(hDlg, IDC_CHECK4, v);

    // roll klice
    SetDlgItemText( hDlg, IDC_EDIT19, ftoa(RAD2DEG(p_kam->roll),pom,0));
    klic = key_je_frame_klic(p_track->p_rlkeys, p_track->roll_keys, frame);
    v = (klic != K_CHYBA);
    if(v) {
      SetDlgItemText( hDlg, IDC_EDIT48, ftoa(p_track->p_rlkeys[klic].continuity,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT49, ftoa(p_track->p_rlkeys[klic].tension,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT50, ftoa(p_track->p_rlkeys[klic].bias,pom,0));
    }
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT19),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT48),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT49),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT50),v);
    doe_check_stlac(hDlg, IDC_CHECK5, v);

    // fov-keys
    SetDlgItemText( hDlg, IDC_EDIT24, ftoa(RAD2DEG(p_kam->fov),pom,0));
    klic = key_je_frame_klic(p_track->p_fkeys, p_track->fov_keys, frame);
    v = (klic != K_CHYBA);
    if(v) {
      SetDlgItemText( hDlg, IDC_EDIT51, ftoa(p_track->p_fkeys[klic].continuity,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT52, ftoa(p_track->p_fkeys[klic].tension,pom,0));
      SetDlgItemText( hDlg, IDC_EDIT53, ftoa(p_track->p_fkeys[klic].bias,pom,0));
    }
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT24),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT51),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT52),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT53),v);
    doe_check_stlac(hDlg, IDC_CHECK25, v);
  } else {

    SetDlgItemText( hDlg, IDC_EDIT2, "00:00:00");

    // pozicni klice
    SetDlgItemText( hDlg, IDC_EDIT3, "0");
    SetDlgItemText( hDlg, IDC_EDIT7, "0");
    SetDlgItemText( hDlg, IDC_EDIT15,"0");
    SetDlgItemText( hDlg, IDC_EDIT25,"0");
    SetDlgItemText( hDlg, IDC_EDIT26,"0");
    SetDlgItemText( hDlg, IDC_EDIT44,"0");
    v = FALSE;
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT3),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT7),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT15),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT25),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT26),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT44),v);    
    doe_check_stlac(hDlg, IDC_CHECK3, v);

    // target-klice
    SetDlgItemText( hDlg, IDC_EDIT16, "0");
    SetDlgItemText( hDlg, IDC_EDIT17, "0");
    SetDlgItemText( hDlg, IDC_EDIT18, "0");        
    SetDlgItemText( hDlg, IDC_EDIT45, "0");
    SetDlgItemText( hDlg, IDC_EDIT46, "0");
    SetDlgItemText( hDlg, IDC_EDIT47, "0");    
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT16),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT17),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT18),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT45),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT46),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT47),v);
    doe_check_stlac(hDlg, IDC_CHECK4, v);
    
    // roll-keys
    SetDlgItemText( hDlg, IDC_EDIT19, "0");
    SetDlgItemText( hDlg, IDC_EDIT48, "0");
    SetDlgItemText( hDlg, IDC_EDIT49, "0");
    SetDlgItemText( hDlg, IDC_EDIT50, "0");
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT19),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT48),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT49),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT50),v);
    doe_check_stlac(hDlg, IDC_CHECK5, v);
    

    // fov-keys
    SetDlgItemText( hDlg, IDC_EDIT24, "0");
    SetDlgItemText( hDlg, IDC_EDIT51, "0");
    SetDlgItemText( hDlg, IDC_EDIT52, "0");
    SetDlgItemText( hDlg, IDC_EDIT53, "0");
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT24),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT51),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT52),v);
    EnableWindow(GetDlgItem(hDlg,IDC_EDIT53),v);    
    doe_check_stlac(hDlg, IDC_CHECK25, v);
  }
}

void doe_animace_updatuj_kamery(K_EDITOR *p_cnf)
{
  HWND hDlg = p_cnf->akkam.hwnd_dialog;
  int  k = p_cnf->kamakt;
  KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;  
  KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
  byte pom[200];

  if(p_track->endtime && p_cnf->akkam.aktivni) {
    SetDlgItemText(hDlg, IDC_EDIT1, itoa(time2frame(p_track->time),pom,10)); 
  }
}

int doe_animace_updatuj_kamery_slider(int frame)
{
  HWND hDlg = p_cnf->akkam.hwnd_dialog;
  int  k = p_cnf->kamakt;
  KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
  KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
  byte pom[200];

  if(p_track->endtime) {
    p_track->time = frame2time(frame);
    SetDlgItemText( hDlg, IDC_EDIT1, itoa(frame,pom,10));
  }
  return(FALSE);
}

BOOL CALLBACK doe_calback_konfigurace_kamery(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_LIST1: // list kamer
          if(HIWORD(wParam) == LBN_SELCHANGE || HIWORD(wParam) == LBN_DBLCLK) {
            p_cnf->kamakt = SendMessage((HWND)lParam,LB_GETCURSEL,0,0);
            doe_init_kamery_animace(hDlg, p_cnf);
            doe_init_kamery_animace_dialog(hDlg, p_cnf);
          }
          break;
        case IDC_LIST9: // list kamer
          if(HIWORD(wParam) == LBN_SELCHANGE || HIWORD(wParam) == LBN_DBLCLK) {
            KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
            p_kam->akt_animace = SendMessage((HWND)lParam,LB_GETCURSEL,0,0);
            doe_init_kamery_animace_dialog(hDlg, p_cnf);
          }
          break;
        
        case IDC_CHECK1: { // Aktivace kamery
          p_cnf->cnf.kamera_aktivni = IsDlgButtonChecked(hDlg,IDC_CHECK1);
          if(p_cnf->kamery[p_cnf->kamakt].cislo != K_CHYBA) {
            if(p_cnf->cnf.kamera_aktivni)
              oe_kamera_to_view(p_cnf, p_cnf->kamakt);
            else
              oe_kamera_to_view(p_cnf,K_CHYBA);
          } else
            kprintf(TRUE,"Neni vybrana zadna kamera!");
          break;
        }
        case IDC_CHECK2:
          {
            KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
            KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
            p_track->flag = IsDlgButtonChecked(hDlg,IDC_CHECK2) ?
                            p_track->flag|GK_LOOP:p_track->flag&~GK_LOOP;
            break;
          }
        case IDC_BUTTON4: { // <<
          KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
          KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
          if(p_track->endtime) {
            doe_anim_start_et(p_cnf->TimeEndLastFrame,p_track->endtime,&p_track->time_start,&p_track->time_stop,&p_track->time_delka,&p_track->time,&p_track->start,&p_track->stop);
          }
          break;
                          }
          
        case IDC_BUTTON8: { // []
          KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
          KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
          if(p_track->endtime) {
            doe_anim_start_et(p_cnf->TimeEndLastFrame,p_track->endtime,&p_track->time_start,&p_track->time_stop,&p_track->time_delka,&p_track->time,&p_track->start,&p_track->stop);
            p_track->stav = 0; // stop animace
          }
          break;
                          }
          
        case IDC_BUTTON10: {// || // pauza
          KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
          KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
          if(p_track->endtime) {
            doe_anim_start_et(p_cnf->TimeEndLastFrame,p_track->endtime,&p_track->time_start,&p_track->time_stop,&p_track->time_delka,&p_track->time,&p_track->start,&p_track->stop);
            p_track->stav = 0; // stop animace
          }
          break;
                           }
          
        case IDC_BUTTON11: {// >
          KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
          KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
          if(p_track->endtime) {
            if(!p_track->stav) {
              doe_anim_start_et(p_cnf->TimeEndLastFrame,p_track->endtime,&p_track->time_start,&p_track->time_stop,&p_track->time_delka,&p_track->time,&p_track->start,&p_track->stop);
            }
            p_track->stav = !p_track->stav;
          }
          break;
          }
          
        case IDC_BUTTON13: {// >>
          KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
          KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
          if(p_track->endtime)
            doe_anim_end_et(p_cnf->TimeEndLastFrame,p_track->endtime,&p_track->time_start,&p_track->time_stop,&p_track->time_delka,&p_track->time,&p_track->start,&p_track->stop);
          break;
          }

        case IDC_CHECK3: { // vloz pos_key
          KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
          KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
          KEY_POINT_BRS key = {0,0,0,0,0,0};
          byte pom[200];
          int v = IsDlgButtonChecked(hDlg,IDC_CHECK3);
          
          GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
          key.time = frame2time(atoi(pom));

          /* Test vlozeni/zruseni klice tracku
          */
          if(!v && key_je_time_klic(p_track->p_pkeys, p_track->pos_keys, key.time) != K_CHYBA) { // smazat
            key_track_klic_smaz((byte **)&p_track->p_pos, &p_track->p_pkeys, &p_track->pos_keys, 
                                sizeof(p_track->p_pos[0]),key.time);
            doe_kamera_slide_vloz_tiky(hDlg, p_cnf);
          } else if(v) { // vlozit
            p_track->pos_keys = key_track_klic_vloz((byte **)&p_track->p_pos, 
              &p_track->p_pkeys, p_track->pos_keys, 
              sizeof(p_track->p_pos[0]), 
              (byte *)&p_kam->p, &key);             
            doe_hs_setmark(time2frame(key.time));
          }
          doe_animace_updatuj_kamery_text(p_cnf,p_kam);
          break;
        }

        case IDC_CHECK4: { // vloz target_key
          KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
          KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
          KEY_POINT_BRS key = {0,0,0,0,0,0};
          byte pom[200];
          int v = IsDlgButtonChecked(hDlg,IDC_CHECK4);
          
          GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
          key.time = frame2time(atoi(pom));
          
          if(!v && key_je_time_klic(p_track->p_tkeys, p_track->trg_keys, key.time) != K_CHYBA) { // smazat
            key_track_klic_smaz((byte **)&p_track->p_trg, &p_track->p_tkeys, &p_track->trg_keys, 
                                sizeof(p_track->p_trg[0]),key.time);
            doe_kamera_slide_vloz_tiky(hDlg, p_cnf);
          } else if(v) { // vlozit
            p_track->trg_keys = key_track_klic_vloz((byte **)&p_track->p_trg,
              &p_track->p_tkeys, p_track->trg_keys, sizeof(p_track->p_trg[0]), 
              (byte *)&p_kam->t, &key);
            doe_hs_setmark(time2frame(key.time));
          }
          doe_animace_updatuj_kamery_text(p_cnf,p_kam);
          break;
        }

        case IDC_CHECK5: { // vloz roll_key
          KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
          KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
          KEY_POINT_BRS key = {0,0,0,0,0,0};
          byte pom[200];
          int v = IsDlgButtonChecked(hDlg,IDC_CHECK5);

          GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
          key.time = frame2time(atoi(pom));
          
          if(!v && key_je_time_klic(p_track->p_rlkeys, p_track->roll_keys, key.time) != K_CHYBA) { // smazat
            key_track_klic_smaz((byte **)&p_track->p_roll, &p_track->p_rlkeys, &p_track->roll_keys,
              sizeof(p_track->p_roll[0]),key.time);
            doe_kamera_slide_vloz_tiky(hDlg, p_cnf);
          } else if(v) { // vlozit
            p_track->roll_keys = key_track_klic_vloz((byte **)&p_track->p_roll,
              &p_track->p_rlkeys, p_track->roll_keys, 
              sizeof(p_track->p_roll[0]), 
              (byte *)&p_kam->roll, &key);            
            doe_hs_setmark(time2frame(key.time));
          }
          doe_animace_updatuj_kamery_text(p_cnf,p_kam);
          break;
        }

        case IDC_CHECK25: { // vloz fov_key
          KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
          KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
          KEY_POINT_BRS key = {0,0,0,0,0,0};
          byte pom[200];
          int v = IsDlgButtonChecked(hDlg,IDC_CHECK25);

          GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
          key.time = frame2time(atoi(pom));
          
          if(!v && key_je_time_klic(p_track->p_fkeys, p_track->fov_keys, key.time) != K_CHYBA) { // smazat
            key_track_klic_smaz((byte **)&p_track->p_fov, &p_track->p_fkeys, &p_track->fov_keys,
              sizeof(p_track->p_fov[0]),key.time);
            doe_kamera_slide_vloz_tiky(hDlg, p_cnf);
          } else if(v) { // vlozit
            p_track->fov_keys = key_track_klic_vloz((byte **)&p_track->p_fov,
              &p_track->p_fkeys, p_track->fov_keys,
              sizeof(p_track->p_fov[0]), 
              (byte *)&p_kam->fov, &key);            
            doe_hs_setmark(time2frame(key.time));
          }
          doe_animace_updatuj_kamery_text(p_cnf,p_kam);
          break;
        }
          // upravit klic
        case IDC_BUTTON6:
          {
            KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
            KAMERA_TRACK_INFO *p_key = p_kam->tracky+p_kam->akt_animace;            
            int  frame,pos,trg,roll,fov,time = -1;
                        
            if(!p_key->endtime)
              break;
             
            frame = time2frame(p_key->time);
            
            pos = key_je_frame_klic(p_key->p_pkeys, p_key->pos_keys, frame);
            trg = key_je_frame_klic(p_key->p_tkeys, p_key->trg_keys, frame);
            roll = key_je_frame_klic(p_key->p_rlkeys, p_key->roll_keys, frame);
            fov = key_je_frame_klic(p_key->p_fkeys, p_key->fov_keys, frame);

            if(doe_animace_kamery_uprav_frame(hDlg,p_key,pos,trg,roll,fov,frame)) {
              key_kosti_serad_klice((byte *)p_key->p_pos, p_key->p_pkeys, p_key->pos_keys, sizeof(p_key->p_pos[0]));
              key_kosti_serad_klice((byte *)p_key->p_trg, p_key->p_tkeys, p_key->trg_keys, sizeof(p_key->p_trg[0]));
              key_kosti_serad_klice((byte *)p_key->p_roll, p_key->p_rlkeys, p_key->roll_keys, sizeof(p_key->p_roll[0]));
              key_kosti_serad_klice((byte *)p_key->p_fov, p_key->p_fkeys, p_key->fov_keys, sizeof(p_key->p_fov[0]));
              
              p_key->time = frame2time(tmp.i[5]);              

              key_kan_to_kamera(p_kam, p_key, p_key->time);
              kameru_dratovou_reanimuj(p_kam);
              if(p_cnf->cnf.kamera_aktivni)
                oe_kamera_to_view(p_cnf,p_cnf->kamakt);              
              doe_animace_updatuj_kamery_text(p_cnf,p_kam);
              doe_kamera_slide_vloz_tiky(hDlg,p_cnf);

              doe_printf(hDlg,IDC_EDIT1,"%d",tmp.i[5]);
            }
            break;
          }

        case IDC_EDIT1: {// editace framu
          KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
          KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
          byte pom[200];
          int  frame;
          int  framenum;
          
          if(!p_track->endtime)
            break;
          
          GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
          frame = atoi(pom);                        
          
          framenum = calc_framenum(p_track->endtime);
          if(frame >= framenum || !frame)
            frame = 0;
          
          p_track->time = frame2time(frame);
          SetDlgItemText(hDlg, IDC_EDIT2, itoa(p_track->time,pom,10));
          
          key_kan_to_kamera(p_kam, p_track, p_track->time);
          kameru_dratovou_reanimuj(p_kam);
          if(p_cnf->cnf.kamera_aktivni)
            oe_kamera_to_view(p_cnf,p_cnf->kamakt);
          doe_hs_setpos(frame);

          doe_animace_updatuj_kamery_text(p_cnf,p_kam);
          break;
        }

        case IDC_BUTTON16: 
          {
            oe_prikaz_kamera_nova(p_cnf);
            doe_init_kamery(hDlg, p_cnf);
            doe_init_kamery_animace(hDlg, p_cnf);
            doe_init_kamery_animace_dialog(hDlg, p_cnf);
          }
          break;
        case IDC_BUTTON3: 
          {
            oe_prikaz_kamera_upravit(p_cnf);
            doe_init_kamery(hDlg, p_cnf);
            doe_init_kamery_animace(hDlg, p_cnf);
            doe_init_kamery_animace_dialog(hDlg, p_cnf);
          }
          break;
        case IDC_BUTTON17:
          {
            oe_prikaz_kamera_smazat(p_cnf);
            doe_init_kamery(hDlg, p_cnf);
            doe_init_kamery_animace(hDlg, p_cnf);
            doe_init_kamery_animace_dialog(hDlg, p_cnf);
          }
          break;

        case IDC_BUTTON9:
          {
            oe_prikaz_kamera_animace_nova(p_cnf);
            doe_init_kamery_animace(hDlg, p_cnf);
            doe_init_kamery_animace_dialog(hDlg, p_cnf);
          }
          break;        
        case IDC_BUTTON5:
          {
            oe_prikaz_kamera_animace_upravit(p_cnf);
            doe_init_kamery_animace(hDlg, p_cnf);
            doe_init_kamery_animace_dialog(hDlg, p_cnf);
          }
          break;
        case IDC_BUTTON25:
          {
            oe_prikaz_kamera_animace_smazat(p_cnf);
            doe_init_kamery_animace(hDlg, p_cnf);
            doe_init_kamery_animace_dialog(hDlg, p_cnf);
          }
          break;
        case IDC_BUTTON24: // load          
          oe_prikaz_kamera_nahrat(p_cnf);
          doe_init_kamery(hDlg, p_cnf);
          doe_init_kamery_animace(hDlg, p_cnf);
          doe_init_kamery_animace_dialog(hDlg, p_cnf);
          break;
        case IDC_BUTTON27: // save
          oe_prikaz_kamera_ulozit(p_cnf);
          break;
        case IDC_BUTTON28: // save all
          oe_prikaz_kamera_ulozit_vse(p_cnf);
          break;

        /* Editace pozice/target/roll/fov
        */
        //EDIT2EDIT2EDIT2
        default:
          return FALSE;
      }
      break;    
    
    case WM_VSCROLL: // slider framu
      if(LOWORD(wParam) == SB_LINEUP|| LOWORD(wParam) == SB_LINEDOWN) {
        KAMERA *p_kam = p_cnf->kamery+p_cnf->kamakt;
        KAMERA_TRACK_INFO *p_track = p_kam->tracky+p_kam->akt_animace;
        byte pom[200];
        int  frame;
        int  framenum;

        framenum = calc_framenum(p_track->endtime);
        if(!p_track->endtime)
          break;
                
        // frame-num
        GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
        frame = atoi(pom);                        
        if(LOWORD(wParam) == SB_LINEUP)
          frame += 1;
        if(LOWORD(wParam) == SB_LINEDOWN)
          frame -= 1;
      
        if(frame > framenum)
          frame = framenum;
        else if(frame < 0.0f)
          frame = 0;

        SetDlgItemText(hDlg, IDC_EDIT1, itoa(frame,pom,10));
      }
      break;
    
    case WM_INITDIALOG: {
        nastav_pozici_bl();
        p_cnf->akkam.aktivni = TRUE;
        p_cnf->akkam.hwnd_dialog = hDlg;
        doe_init_kamery(hDlg, p_cnf);
        doe_init_kamery_animace(hDlg, p_cnf);
        doe_init_kamery_animace_dialog(hDlg, p_cnf);
        doe_hs_posfce(doe_animace_updatuj_kamery_slider);
        SetFocus(GetDlgItem(hDlg,IDC_LIST1));        
      } 
      break;
    case WM_DESTROY: {
      p_cnf->akkam.aktivni = FALSE;
      p_cnf->akkam.hwnd_dialog = NULL;
      doe_hs_zrus();
      EndDialog( hDlg, FALSE );
      break; 
    }
    default:
      return FALSE;
  }
  return TRUE;
}

BOOL CALLBACK doe_calback_konfigurace_kamery_omezeni(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_EDIT2:
          if(HIWORD(wParam) == EN_CHANGE) {
            p_cnf->kam.far_plane = doe_vyber_kolonku_float(hDlg,IDC_EDIT2);
            projection_matrix(p_cnf->p_project,p_cnf->kam.fov,(float)OXRES/(float)OYRES,p_cnf->kam.near_plane,p_cnf->kam.far_plane);
            set_matrix_project(p_cnf->p_project);            
          }
          break;
        case IDC_EDIT3:
          if(HIWORD(wParam) == EN_CHANGE) {
            p_cnf->kam.fov = DEG2RAD(doe_vyber_kolonku_float(hDlg,IDC_EDIT3));
            if(p_cnf->kam.fov > 0.0f) {
              projection_matrix(p_cnf->p_project,p_cnf->kam.fov,(float)OXRES/(float)OYRES,p_cnf->kam.near_plane,p_cnf->kam.far_plane);
              set_matrix_project(p_cnf->p_project);
            } else {
              p_cnf->kam.fov = DEG2RAD(60.0f);
            }
          }
          break;
        case IDC_EDIT1:
          if(HIWORD(wParam) == EN_CHANGE) {
            p_cnf->kam.min_uhel = DEG2RAD(doe_vyber_kolonku_float(hDlg,IDC_EDIT1));
          }
          break;
        case IDC_EDIT8:
          if(HIWORD(wParam) == EN_CHANGE) {
            p_cnf->kam.max_uhel = DEG2RAD(doe_vyber_kolonku_float(hDlg,IDC_EDIT8));
          }
          break;
        case IDC_EDIT9:
          if(HIWORD(wParam) == EN_CHANGE) {
            p_cnf->kam.min_vzdal = doe_vyber_kolonku_float(hDlg,IDC_EDIT9);
          }
          break;
        case IDC_EDIT10:
          if(HIWORD(wParam) == EN_CHANGE) {
            p_cnf->kam.max_vzdal = doe_vyber_kolonku_float(hDlg,IDC_EDIT10);
          }
          break;
        case IDC_CHECK1:
          p_cnf->cnf.kamera_kostka_kreslit = IsDlgButtonChecked(hDlg,IDC_CHECK1);
          break;
        case IDC_EDIT4:
          if(HIWORD(wParam) == EN_CHANGE) {
            p_cnf->kam.min.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT4);
          }
          break;
        case IDC_EDIT5:
          if(HIWORD(wParam) == EN_CHANGE) {
            p_cnf->kam.min.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT5);
          }
          break;
        case IDC_EDIT7:
          if(HIWORD(wParam) == EN_CHANGE) {
            p_cnf->kam.min.z = doe_vyber_kolonku_float(hDlg,IDC_EDIT7);
          }
          break;
        case IDC_EDIT15:
          if(HIWORD(wParam) == EN_CHANGE) {
            p_cnf->kam.max.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT15);
          }
          break;
        case IDC_EDIT16:
          if(HIWORD(wParam) == EN_CHANGE) {
            p_cnf->kam.max.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT16);
          }
          break;
        case IDC_EDIT17:
          if(HIWORD(wParam) == EN_CHANGE) {
            p_cnf->kam.max.z = doe_vyber_kolonku_float(hDlg,IDC_EDIT17);
          }
          break;
        case IDC_BUTTON1:
          if(p_cnf->bnf.p_lev) {
            float dx = (float)(p_cnf->bnf.x_num*X_PRVEK2),
                  dy = (float)(p_cnf->bnf.y_num*Y_PRVEK2),
                  dz = (float)(p_cnf->bnf.z_num*Z_PRVEK2),
                  sx = p_cnf->bnf.x_start+(float)(p_cnf->bnf.x_num*X_PRVEK2),
                  sy = p_cnf->bnf.y_start,
                  sz = p_cnf->bnf.z_start+(float)(p_cnf->bnf.z_num*Z_PRVEK2);
            
            p_cnf->kam.min.x = sx-dx;
            p_cnf->kam.min.y = sy;
            p_cnf->kam.min.z = sz-dz;

            p_cnf->kam.max.x = sx+dx;
            p_cnf->kam.max.y = sy+2*dy;
            p_cnf->kam.max.z = sz+dz;

            doe_printf(hDlg,IDC_EDIT4,"%.3f",p_cnf->kam.min.x);
            doe_printf(hDlg,IDC_EDIT5,"%.3f",p_cnf->kam.min.y);
            doe_printf(hDlg,IDC_EDIT7,"%.3f",p_cnf->kam.min.z);
            
            doe_printf(hDlg,IDC_EDIT15,"%.3f",p_cnf->kam.max.x);
            doe_printf(hDlg,IDC_EDIT16,"%.3f",p_cnf->kam.max.y);
            doe_printf(hDlg,IDC_EDIT17,"%.3f",p_cnf->kam.max.z);
          } else
            kprintf(TRUE,"Neni aktivni level...");
          break;
        default:
          break;
      }
      break;
    case WM_INITDIALOG: {
        nastav_pozici_bl();
        doe_printf(hDlg,IDC_EDIT2,"%.3f",p_cnf->kam.far_plane);
        doe_printf(hDlg,IDC_EDIT3,"%.3f",RAD2DEG(p_cnf->kam.fov));

        doe_printf(hDlg,IDC_EDIT1,"%.3f",RAD2DEG(p_cnf->kam.min_uhel));
        doe_printf(hDlg,IDC_EDIT8,"%.3f",RAD2DEG(p_cnf->kam.max_uhel));

        doe_printf(hDlg,IDC_EDIT9, "%.3f", p_cnf->kam.min_vzdal);
        doe_printf(hDlg,IDC_EDIT10,"%.3f",p_cnf->kam.max_vzdal);

        doe_printf(hDlg,IDC_EDIT4,"%.3f",p_cnf->kam.min.x);
        doe_printf(hDlg,IDC_EDIT5,"%.3f",p_cnf->kam.min.y);
        doe_printf(hDlg,IDC_EDIT7,"%.3f",p_cnf->kam.min.z);

        doe_printf(hDlg,IDC_EDIT15,"%.3f",p_cnf->kam.max.x);
        doe_printf(hDlg,IDC_EDIT16,"%.3f",p_cnf->kam.max.y);
        doe_printf(hDlg,IDC_EDIT17,"%.3f",p_cnf->kam.max.z);

        doe_check_stlac(hDlg,IDC_CHECK1,p_cnf->cnf.kamera_kostka_kreslit);
      }
      break;
    case WM_DESTROY: {
      EndDialog( hDlg, FALSE );
      break; 
    }
    default:
      return FALSE;
  }
  return TRUE;
}

BOOL CALLBACK doe_calback_konfigurace_gourand(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        
        // Prepnuti mezi diffusni/spekularni barvou
        case IDC_RADIO7: {
          if(IsDlgButtonChecked( hDlg, IDC_RADIO7))
            p_cnf->bar.mod = 0;
          break;
        }
        case IDC_RADIO8: {
          if(IsDlgButtonChecked( hDlg, IDC_RADIO8))
            p_cnf->bar.mod = 1;
          break;
        }

        // vyber barvu
        case IDC_BUTTON1:  
          doe_prikaz_vyber_barvu_float(p_cnf, hDlg, &p_cnf->bar.gr);
          doe_printf(hDlg,IDC_EDIT15,"%d",ftoi(p_cnf->bar.gr*MAX_BYTE_F));
          doe_printf(hDlg,IDC_EDIT16,"%d",ftoi(p_cnf->bar.gg*MAX_BYTE_F));
          doe_printf(hDlg,IDC_EDIT17,"%d",ftoi(p_cnf->bar.gb*MAX_BYTE_F));
          break;        
        
        case IDC_EDIT15:
          p_cnf->bar.gr = (float)doe_vyber_kolonku_int(hDlg,IDC_EDIT15)/MAX_BYTE_F;
          break;
        case IDC_EDIT16:
          p_cnf->bar.gg = (float)doe_vyber_kolonku_int(hDlg,IDC_EDIT16)/MAX_BYTE_F;
          break;
        case IDC_EDIT17:
          p_cnf->bar.gb = (float)doe_vyber_kolonku_int(hDlg,IDC_EDIT17)/MAX_BYTE_F;
          break;
        case IDC_EDIT5:
          p_cnf->bar.ga = (float)doe_vyber_kolonku_int(hDlg,IDC_EDIT5)/MAX_BYTE_F;
          break;

        // +- barvy
        case IDC_BUTTON5: {
          add_mult_color_f(&p_cnf->bar.gr,&p_cnf->bar.gr,p_cnf->bar.set_krok);
          
          doe_printf(hDlg,IDC_EDIT15,"%d",ftoi(p_cnf->bar.gr*MAX_BYTE_F));
          doe_printf(hDlg,IDC_EDIT16,"%d",ftoi(p_cnf->bar.gg*MAX_BYTE_F));
          doe_printf(hDlg,IDC_EDIT17,"%d",ftoi(p_cnf->bar.gb*MAX_BYTE_F));
          doe_printf(hDlg,IDC_EDIT5, "%d",ftoi(p_cnf->bar.ga*MAX_BYTE_F));
          break;
        }
        case IDC_BUTTON6: {
          sub_mult_color_f(&p_cnf->bar.gr,&p_cnf->bar.gr,p_cnf->bar.set_krok);
          
          doe_printf(hDlg,IDC_EDIT15,"%d",ftoi(p_cnf->bar.gr*MAX_BYTE_F));
          doe_printf(hDlg,IDC_EDIT16,"%d",ftoi(p_cnf->bar.gg*MAX_BYTE_F));
          doe_printf(hDlg,IDC_EDIT17,"%d",ftoi(p_cnf->bar.gb*MAX_BYTE_F));
          doe_printf(hDlg,IDC_EDIT5, "%d",ftoi(p_cnf->bar.ga*MAX_BYTE_F));
          break;
        }

        // Scale stinovace
        case IDC_EDIT2: {
          byte pom[100];
          cti_polozku_float(IDC_EDIT2,p_cnf->bar.gourand_scale);
          break;
        }
        case IDC_EDIT3: {
          byte pom[100];
          cti_polozku_float(IDC_EDIT3,p_cnf->bar.gourand_pridat1);
          break;
        }
        case IDC_EDIT7: {
          byte pom[100];
          cti_polozku_float(IDC_EDIT7,p_cnf->bar.gourand_pridat2);
          break;
        }

        // Mod cinnosti stinovace
        case IDC_RADIO1: {
          if(IsDlgButtonChecked( hDlg, IDC_RADIO1))
            p_cnf->bar.gourand_cop = COLOR_SET;
          break;
        }
        case IDC_RADIO2: {
          if(IsDlgButtonChecked( hDlg, IDC_RADIO2))
            p_cnf->bar.gourand_cop = COLOR_ADD;
          break;
        }
        case IDC_RADIO3: {
          if(IsDlgButtonChecked( hDlg, IDC_RADIO3))
            p_cnf->bar.gourand_cop = COLOR_SUB;
          break;
        }

        // Funkce stinovace
        case IDC_BUTTON11: { // na vybrane body
          oe_prikaz_gourand_maping_body(p_cnf);
          break;
        }
        case IDC_BUTTON2: { // na objekt
          oe_prikaz_gourand_maping_obj(p_cnf);
          break;
        }
        case IDC_BUTTON3: { // na kontejner
          oe_prikaz_gourand_maping_kont(p_cnf);
          break;
        }
        case IDC_CHECK2: { // oboustrane
          p_cnf->bar.multi_mesh = IsDlgButtonChecked( hDlg, IDC_CHECK2);
          break;
        }

        // Normalove vektory - na celou scenu
        case IDC_BUTTON4:
          oe_prikaz_kont_norm_vect_all(p_cnf);
          break;

        // Operace s nastavenim nastavovace
        case IDC_RADIO4: {
          if(IsDlgButtonChecked( hDlg, IDC_RADIO4))
            p_cnf->bar.set_operace = COLOR_SET;
          break;
        }          
        case IDC_RADIO5: {
          if(IsDlgButtonChecked( hDlg, IDC_RADIO5))
            p_cnf->bar.set_operace = COLOR_ADD;
          break;
        }
        case IDC_RADIO6: {         
          if(IsDlgButtonChecked( hDlg, IDC_RADIO6))
            p_cnf->bar.set_operace = COLOR_SUB;
          break;
        }

        // Prikazy nastavovace
        case IDC_BUTTON13: // nastav body
          oe_prikaz_set_barva_body(p_cnf);
          break;
        case IDC_BUTTON8:  // nastav objekt
          oe_prikaz_set_barva_objekt(p_cnf);
          break;
        case IDC_BUTTON14: // nastav kontejner
          oe_prikaz_set_barva_kontejner(p_cnf);
          break;
        case IDC_BUTTON17: // nastav scenu
          oe_prikaz_set_barva_scena(p_cnf);
          break;
        case IDC_BUTTON26: // nastav guidy          
          oe_prikaz_set_barva_scena_guids(p_cnf,p_cnf->bar.lo,p_cnf->bar.hi,IsDlgButtonChecked(hDlg,IDC_CHECK3));
          break;          
        case IDC_BUTTON10: // resetuj objekt
          oe_prikaz_reset_barva_objekt(p_cnf);
          break;
        case IDC_BUTTON15: // resetuj kontejner
          oe_prikaz_reset_barva_kontejner(p_cnf);
          break;
        case IDC_CHECK1:
          p_cnf->bar.oboustrany_stinovac = IsDlgButtonChecked(hDlg,IDC_CHECK1);
          break;
        case IDC_EDIT1:
          p_cnf->bar.lo = doe_vyber_kolonku_int(hDlg,IDC_EDIT1);
          break;
        case IDC_EDIT8:
          p_cnf->bar.hi = doe_vyber_kolonku_int(hDlg,IDC_EDIT8);      
          break;
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
        byte pom[100];
        nastav_pozici_bl();

        p_cnf->bar.hDlg = hDlg;

        if(!p_cnf->bar.mod)
          CheckDlgButton( hDlg, IDC_RADIO7, TRUE);
        else
          CheckDlgButton( hDlg, IDC_RADIO8, TRUE);

        doe_printf(hDlg,IDC_EDIT15,"%d",ftoi(p_cnf->bar.gr*MAX_BYTE_F));
        doe_printf(hDlg,IDC_EDIT16,"%d",ftoi(p_cnf->bar.gg*MAX_BYTE_F));
        doe_printf(hDlg,IDC_EDIT17,"%d",ftoi(p_cnf->bar.gb*MAX_BYTE_F));
        doe_printf(hDlg,IDC_EDIT5, "%d",ftoi(p_cnf->bar.ga*MAX_BYTE_F));
        doe_printf(hDlg,IDC_EDIT1, "%d",p_cnf->bar.lo);
        doe_printf(hDlg,IDC_EDIT8, "%d",p_cnf->bar.hi);
                
        pridej_string_matrix(p_cnf->bar.gourand_scale,IDC_EDIT2);
        pridej_string_matrix(p_cnf->bar.gourand_pridat1,IDC_EDIT3);
        pridej_string_matrix(p_cnf->bar.gourand_pridat2,IDC_EDIT7);
        
        if(p_cnf->bar.gourand_cop == COLOR_SET)
          CheckDlgButton( hDlg, IDC_RADIO1, TRUE);
        if(p_cnf->bar.gourand_cop == COLOR_ADD)
          CheckDlgButton( hDlg, IDC_RADIO2, TRUE);
        if(p_cnf->bar.gourand_cop == COLOR_SUB)
          CheckDlgButton( hDlg, IDC_RADIO3, TRUE);
        
        if(p_cnf->bar.set_operace == COLOR_SET)
          CheckDlgButton( hDlg, IDC_RADIO4, TRUE);
        if(p_cnf->bar.set_operace == COLOR_ADD)
          CheckDlgButton( hDlg, IDC_RADIO5, TRUE);
        if(p_cnf->bar.set_operace == COLOR_SUB)
          CheckDlgButton( hDlg, IDC_RADIO6, TRUE);

        doe_check_stlac(hDlg,IDC_CHECK1,p_cnf->bar.oboustrany_stinovac);
        doe_check_stlac(hDlg,IDC_CHECK2,p_cnf->bar.multi_mesh);
      } 
      break;
    case WM_DESTROY: {
      p_cnf->bar.hDlg = FALSE;
      EndDialog( hDlg, FALSE );
      break; 
    }
    default:
      return FALSE;
  }
  return TRUE;
}

BOOL CALLBACK doe_callback_mlhokostka_jmeno( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {
          MLZNA_KOSTKA *p_mlha = tmp.p_pom;
          GetDlgItemText(hDlg,IDC_EDIT1,p_mlha->jmeno,49);
          p_mlha->min.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT2);
          p_mlha->min.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT3);
          p_mlha->min.z = doe_vyber_kolonku_float(hDlg,IDC_EDIT7);
          p_mlha->max.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT15);
          p_mlha->max.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT16);
          p_mlha->max.z = doe_vyber_kolonku_float(hDlg,IDC_EDIT17);
          p_mlha->priorita = doe_vyber_kolonku_int(hDlg,IDC_EDIT18);
          EndDialog( hDlg, TRUE );
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      MLZNA_KOSTKA *p_mlha = tmp.p_pom;
      doe_printf(hDlg,IDC_EDIT1,p_mlha->jmeno);
      doe_printf(hDlg,IDC_EDIT2,"%.3f",p_mlha->min.x);
      doe_printf(hDlg,IDC_EDIT3,"%.3f",p_mlha->min.y);
      doe_printf(hDlg,IDC_EDIT7,"%.3f",p_mlha->min.z);
      doe_printf(hDlg,IDC_EDIT15,"%.3f",p_mlha->max.x);
      doe_printf(hDlg,IDC_EDIT16,"%.3f",p_mlha->max.y);
      doe_printf(hDlg,IDC_EDIT17,"%.3f",p_mlha->max.z);
      doe_printf(hDlg,IDC_EDIT18,"%d",p_mlha->priorita);
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_mlhokostka_jmeno(HWND hwnd, MLZNA_KOSTKA *p_mlha)
{
  tmp.p_pom = p_mlha;  
  return(DialogBox( hinst, MAKEINTRESOURCE(IDD_MLHOKOSTKA), hwnd, doe_callback_mlhokostka_jmeno));
}


static void konfigurace_mlha_init_combo(HWND hDlg)
{
  HWND combo = GetDlgItem(hDlg, IDC_COMBO1);
  MLZNA_KOSTKA *p_mlha = p_cnf->p_mlha;
  int  i,sel = K_CHYBA;

  SendMessage( combo, CB_RESETCONTENT, 0, 0 );
  
  i = 0;
  while(p_mlha) {
    SendMessage(combo, CB_SETITEMDATA, 
                SendMessage(combo, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)p_mlha->jmeno),
                (long)p_mlha);
    if(p_mlha == p_cnf->p_mlha_akt)
      sel = i;
    i++;
    p_mlha = p_mlha->p_next;
  }
  
  if(sel == K_CHYBA) {
    p_cnf->p_mlha_akt = p_cnf->p_mlha;
    sel = 0;
  } 
  
  SendMessage( combo, CB_SETCURSEL, sel, 0 );
  
  doe_check_stlac(hDlg, IDC_CHECK11, p_cnf->cnf.mlhokostku_kreslit);
  doe_check_stlac(hDlg, IDC_CHECK1 , p_cnf->cnf.mlhokostka_aktivni);
  doe_check_stlac(hDlg, IDC_CHECK2 , p_cnf->cnf.mlhokostka_testy);
  
  if(p_cnf->p_mlha_akt) {
    doe_printf(hDlg, IDC_EDIT4, "%.3f", p_cnf->p_mlha_akt->min.x);
    doe_printf(hDlg, IDC_EDIT5, "%.3f", p_cnf->p_mlha_akt->min.y);
    doe_printf(hDlg, IDC_EDIT15,"%.3f", p_cnf->p_mlha_akt->min.z);

    doe_printf(hDlg, IDC_EDIT16,"%.3f", p_cnf->p_mlha_akt->max.x);
    doe_printf(hDlg, IDC_EDIT17,"%.3f", p_cnf->p_mlha_akt->max.y);
    doe_printf(hDlg, IDC_EDIT18,"%.3f", p_cnf->p_mlha_akt->max.z);

    doe_printf(hDlg, IDC_EDIT8, "%d", p_cnf->p_mlha_akt->priorita);
  }
}

static void konfigurace_mlha_init_combo_kont(HWND hDlg)
{ 
  HWND combo = GetDlgItem(hDlg, IDC_COMBO2);    
  EDIT_KONTEJNER *p_kont;
  int k,p = 0,cur = 0;

  SendMessage( combo, CB_RESETCONTENT, 0, 0 );
    
  oe_klist_reset(&k);
  while((p_kont = oe_klist_next(p_cnf,&k))) {
    if(!p_kont->p_mlha || p_cnf->p_mlha_akt == p_kont->p_mlha) {
      if(p_cnf->p_mlha_akt == p_kont->p_mlha)
        cur = p;
      SendMessage(combo, CB_SETITEMDATA, 
                  SendMessage(combo, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)p_kont->jmeno),
                  (long)p_kont);
      p++;
    }
  }
  SendMessage( combo, CB_SETCURSEL, cur, 0 );
}

static EDIT_KONTEJNER * konfigurace_mlha_init_combo_kont_vrat(HWND hDlg)
{ 
  HWND combo = GetDlgItem(hDlg, IDC_COMBO2);    
  return((EDIT_KONTEJNER *)SendMessage(combo,CB_GETITEMDATA,SendMessage(combo,CB_GETCURSEL,0,0),0));
}

static void konfigurace_mlha_init_dialog(HWND hDlg)
{
 MLZNA_KOSTKA *p_mlha = p_cnf->p_mlha_akt;
 ZEMNI_MLHA   *p_zm = &p_cnf->zm; // zemni mlha

 if(p_mlha) {
   doe_check_stlac(hDlg, IDC_RADIO1, p_mlha->mod == GL_LINEAR);
   doe_check_stlac(hDlg, IDC_RADIO2, p_mlha->mod == GL_EXP);
   doe_check_stlac(hDlg, IDC_RADIO3, p_mlha->mod == GL_EXP2);
   doe_check_stlac(hDlg, IDC_CHECK3, (int)p_mlha->p_kont);
   EnableDlgItem(hDlg,IDC_COMBO2, (int)p_mlha->p_kont);
   if(p_mlha->p_kont) {
     konfigurace_mlha_init_combo_kont(hDlg);
   }
   
   doe_printf(hDlg, IDC_EDIT1, "%x",  float_rgba(&p_mlha->r));
   doe_printf(hDlg, IDC_EDIT2, "%.2f",p_mlha->start);
   doe_printf(hDlg, IDC_EDIT3, "%.2f",p_mlha->stop);
   doe_printf(hDlg, IDC_EDIT7, "%.2f",p_mlha->intenzita);
 }

 doe_printf(hDlg, IDC_EDIT19,"%.2f",p_zm->v_start);
 doe_printf(hDlg, IDC_EDIT24,"%.2f",p_zm->v_stop);
 doe_printf(hDlg, IDC_EDIT25,"%.2f",p_zm->h_start);
 doe_printf(hDlg, IDC_EDIT26,"%.2f",p_zm->h_stop);
 doe_printf(hDlg, IDC_EDIT9, "%x",p_zm->barva);
 doe_check_stlac(hDlg, IDC_CHECK4, p_zm->aktivni);
}

void konfigurace_mlha_aktivuj(void)
{
  MLZNA_KOSTKA *p_mlha = p_cnf->p_mlha_akt;
  if(p_mlha && p_cnf->cnf.mlhokostka_aktivni) {
    nastav_fog(p_mlha);
    if(p_cnf->cnf.mlhokostka_testy && !p_mlha->p_kont) {
      if(kd_je_bod_v_kostce((BOD *)&p_cnf->p_invcam->_41, &p_mlha->min, &p_mlha->max))
        enable_fog();
      else
        disable_fog();
    } else {
      enable_fog();
    }
  } else {
    disable_fog();
  }
}

BOOL CALLBACK doe_calback_konfigurace_mlha(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{ 
  byte pom[100];
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_COMBO1:
          if(HIWORD(wParam) ==  CBN_SELCHANGE) {
            HWND hCombo1 = GetDlgItem(hDlg,IDC_COMBO1);
            p_cnf->p_mlha_akt = (MLZNA_KOSTKA *)SendMessage( hCombo1, CB_GETITEMDATA, SendMessage( hCombo1, CB_GETCURSEL, 0, 0 ), 0 );
            konfigurace_mlha_init_combo(hDlg);
            konfigurace_mlha_init_dialog(hDlg);
            konfigurace_mlha_aktivuj();
          }
          break;
        case IDC_BUTTON2: // nova kostka - insert first
          {
            MLZNA_KOSTKA *p_mlha,mlha;
            byte jmeno[MAX_JMENO];
            memset(&mlha,0,sizeof(mlha));
            mlha.max.x = mlha.max.y = mlha.max.z = 10.0f;
            if(doe_mlhokostka_jmeno(hDlg, &mlha)) {
              p_mlha = vyrob_mlhokostku(jmeno);
              p_mlha->p_next = p_cnf->p_mlha;
              p_cnf->p_mlha_akt = p_cnf->p_mlha = p_mlha;              
              strcpy(p_mlha->jmeno,mlha.jmeno);
              p_mlha->min = mlha.min;
              p_mlha->max = mlha.max;              
              konfigurace_mlha_init_combo(hDlg);
              konfigurace_mlha_init_dialog(hDlg);
              konfigurace_mlha_aktivuj();
            }
            break;
          }
        case IDC_BUTTON3: // smazat kostku
          if(p_cnf->p_mlha_akt) {
            if(ddwqueto(hwnd_aplikace,"Smazat mlho-kostku '%s'?",p_cnf->p_mlha_akt->jmeno)) {
              zrus_mlhokostku(&p_cnf->p_mlha, p_cnf->p_mlha_akt);
              konfigurace_mlha_init_combo(hDlg);
              konfigurace_mlha_init_dialog(hDlg);
              konfigurace_mlha_aktivuj();
            }
          }
          break;
        case IDC_BUTTON4:
          {
            if(p_cnf->p_mlha_akt) {
              doe_mlhokostka_jmeno(hDlg,p_cnf->p_mlha_akt);
              konfigurace_mlha_init_combo(hDlg);
              konfigurace_mlha_init_dialog(hDlg);
              konfigurace_mlha_aktivuj();
            }
          }
          break;
        case IDC_CHECK11:
          {
            p_cnf->cnf.mlhokostku_kreslit = IsDlgButtonChecked(hDlg,IDC_CHECK11)?TRUE:FALSE;            
            break;
          }
        case IDC_CHECK1:
          {
            p_cnf->cnf.mlhokostka_aktivni = IsDlgButtonChecked(hDlg,IDC_CHECK1)?TRUE:FALSE;
            konfigurace_mlha_aktivuj();
            break;
          }
        case IDC_CHECK2:
          {
            p_cnf->cnf.mlhokostka_testy = IsDlgButtonChecked(hDlg,IDC_CHECK2)?TRUE:FALSE;
            konfigurace_mlha_aktivuj();
            break;
          }
        case IDC_BUTTON1: { // vyber barvy
          if(p_cnf->p_mlha_akt) {
            dword barva;
            GetDlgItemText(hDlg,IDC_EDIT1,pom,49);
            sscanf(pom,"%x",&barva);
            if(doe_prikaz_vyber_barvu(p_cnf,hDlg,&barva)) {
              SetDlgItemText( hDlg, IDC_EDIT1, itoa(barva,pom,16));
              rgb_float(barva,&p_cnf->p_mlha_akt->r);
              p_cnf->p_mlha_akt->a = 1.0f;
              konfigurace_mlha_aktivuj();
            }
          }
          break;
        }
        case IDC_BUTTON7: // vyber barvy
          p_cnf->zm.barva = doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT9);
          break;
        case IDC_CHECK4:
          p_cnf->zm.aktivni = IsDlgButtonChecked(hDlg,IDC_CHECK4) ? TRUE : FALSE;
          break;
        case IDC_EDIT19:
          p_cnf->zm.v_start = doe_vyber_kolonku_float(hDlg,IDC_EDIT19);
          break;
        case IDC_EDIT24:
          p_cnf->zm.v_stop = doe_vyber_kolonku_float(hDlg,IDC_EDIT24);
          break;
        case IDC_EDIT25:
          p_cnf->zm.h_start = doe_vyber_kolonku_float(hDlg,IDC_EDIT25);
          break;
        case IDC_EDIT26:
          p_cnf->zm.h_stop = doe_vyber_kolonku_float(hDlg,IDC_EDIT26);
          break;
        case IDC_EDIT1:
          {
            if(p_cnf->p_mlha_akt) {
              dword barva;
              GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
              sscanf(pom,"%x",&barva);
              rgba_float(barva,&p_cnf->p_mlha_akt->r);
              konfigurace_mlha_aktivuj();
            }
            break;
          }
        case IDC_EDIT2:
          if(p_cnf->p_mlha_akt) {
            GetDlgItemText(hDlg,IDC_EDIT2,pom,99);
            p_cnf->p_mlha_akt->start = (float)atof(pom);
            konfigurace_mlha_aktivuj();
          }
          break;
        case IDC_EDIT3:
          if(p_cnf->p_mlha_akt) {
            GetDlgItemText(hDlg,IDC_EDIT3,pom,99);
            p_cnf->p_mlha_akt->stop = (float)atof(pom);
            konfigurace_mlha_aktivuj();
          }
          break;
        case IDC_EDIT7:
          if(p_cnf->p_mlha_akt) {
            GetDlgItemText(hDlg,IDC_EDIT7,pom,99);
            p_cnf->p_mlha_akt->intenzita = (float)atof(pom);
            konfigurace_mlha_aktivuj();
          }
          break;
        case IDC_RADIO1:
          if(p_cnf->p_mlha_akt && IsDlgButtonChecked(hDlg,IDC_RADIO1)) {
            p_cnf->p_mlha_akt->mod = GL_LINEAR;
            konfigurace_mlha_aktivuj();
          }          
          break;
        case IDC_RADIO2:
          if(p_cnf->p_mlha_akt && IsDlgButtonChecked(hDlg,IDC_RADIO2)) {
            p_cnf->p_mlha_akt->mod = GL_EXP;
            konfigurace_mlha_aktivuj();
          }          
          break;
        case IDC_RADIO3:
          if(p_cnf->p_mlha_akt && IsDlgButtonChecked(hDlg,IDC_RADIO3)) {
            p_cnf->p_mlha_akt->mod = GL_EXP2;
            konfigurace_mlha_aktivuj();
          }          
          break;
        case IDC_CHECK3:
          {            
            // Aktivni mlho-kostka je kontejnerova/nekontejnerova
            if(p_cnf->p_mlha_akt) {
              // podle kont/nekont pridela kostku k vybranemu v combu
              if(IsDlgButtonChecked(hDlg,IDC_CHECK3)) {
                EDIT_KONTEJNER *p_kont;
                
                // vyber kontejneru
                konfigurace_mlha_init_combo_kont(hDlg);
                p_kont = konfigurace_mlha_init_combo_kont_vrat(hDlg);
                if(p_kont) {
                  p_kont->p_mlha = p_cnf->p_mlha_akt; 
                  p_cnf->p_mlha_akt->p_kont = p_kont;
                }                
              } else { // vymazni to
                if(p_cnf->p_mlha_akt->p_kont) {
                  ((EDIT_KONTEJNER *)(p_cnf->p_mlha_akt->p_kont))->p_mlha = NULL;
                }
                p_cnf->p_mlha_akt->p_kont = NULL;                
              }
              konfigurace_mlha_init_dialog(hDlg);              
              konfigurace_mlha_aktivuj();
            }
          }
          break;
        case IDC_COMBO2:
          if(HIWORD(wParam) ==  CBN_SELCHANGE) {
            EDIT_KONTEJNER *p_kont;
            if(p_cnf->p_mlha_akt && p_cnf->p_mlha_akt->p_kont)
              ((EDIT_KONTEJNER *)(p_cnf->p_mlha_akt->p_kont))->p_mlha = NULL;
            p_kont = konfigurace_mlha_init_combo_kont_vrat(hDlg);
            if(p_kont) {
              p_kont->p_mlha = p_cnf->p_mlha_akt; 
              p_cnf->p_mlha_akt->p_kont = p_kont;
            }
          }
          break;
        default:
          break;
      }
      break;
    case WM_INITDIALOG: {
      nastav_pozici_bl();
      konfigurace_mlha_init_combo(hDlg);
      konfigurace_mlha_init_dialog(hDlg);
      konfigurace_mlha_aktivuj();
      break;
    }
    case WM_DESTROY: {     
      EndDialog( hDlg, FALSE );
      break; 
    }
    default:
      return FALSE;
  }
  return TRUE;
}


BOOL CALLBACK doe_callback_seznam_svetel( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {                    
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      HWND combo = GetDlgItem(hDlg,IDC_LIST1);
      byte pom[200];
      int  i;
      
      SendMessage(combo, LB_RESETCONTENT, 0, 0);      
      for(i = 0; i < MAX_EDIT_SVETEL; i++) {
        if(p_cnf->light[i].akt) {
          if(p_cnf->light[i].flag2&SL2_MESH) {
            sprintf(pom,"SL '%d' zavisle na '%s'",i,((EDIT_KONTEJNER *)(p_cnf->light[i].p_mesh_data))->jmeno);
          } else {
            sprintf(pom,"SL '%d'",i);
          }          
          SendMessage( combo, LB_ADDSTRING, 0,(LPARAM)pom);
        }
      }
      for(i = 0; i < p_cnf->dl_lightnum; i++) {
        if(p_cnf->p_dlight[i].akt) {
          EDIT_KONTEJNER *p_kont = p_cnf->p_dlight[i].p_mesh_data;
          if(p_cnf->p_dlight[i].flag&SDL_MESH && p_kont) {
            sprintf(pom,"DL '%s' zavisle na '%s'",p_cnf->p_dlight[i].jmeno,p_kont->jmeno);
          } else {
            sprintf(pom,"DL '%s'",p_cnf->p_dlight[i].jmeno);
          }
          SendMessage( combo, LB_ADDSTRING, 0,(LPARAM)pom);
        }
      }      
      for(i = 0; i < p_cnf->edl_lightnum; i++) {
        if(p_cnf->p_edlight[i].akt) {
          sprintf(pom,"EL '%s'",p_cnf->p_edlight[i].jmeno);
          SendMessage( combo, LB_ADDSTRING, 0,(LPARAM)pom);
        }
      }      
      break;
    }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

// Seznam svetel
void doe_seznam_svetel(K_EDITOR *p_cnf)
{
  DialogBox(hinst,MAKEINTRESOURCE(IDD_SEZNAM_SVETEL),hwnd_aplikace,doe_callback_seznam_svetel);
}

// flag -> extra/obyc svetlo
void doe_svetla_dyn_vloz_anim(HWND hDlg, dword time, DYN_LIGHT_ANIM *p_anim, int loop, int extra_light)
{
  BOD  p;
  RGBA b;
  int klic;

  /* Pozice - klice
  */
  klic = key_je_time_klic(p_anim->p_pkeys, p_anim->pos_keys, time);
  if(klic != K_CHYBA) {
    EnableDlgItem(hDlg, IDC_CHECK3, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT3,  TRUE);
    EnableDlgItem(hDlg, IDC_EDIT7,  TRUE);
    EnableDlgItem(hDlg, IDC_EDIT15, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT25, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT26, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT44, TRUE);

    doe_printf(hDlg,IDC_EDIT3,"%f",p_anim->p_pos[klic].x);
    doe_printf(hDlg,IDC_EDIT7,"%f",p_anim->p_pos[klic].y);
    doe_printf(hDlg,IDC_EDIT15,"%f",p_anim->p_pos[klic].z);

    doe_printf(hDlg,IDC_EDIT25,"%f",p_anim->p_pkeys[klic].continuity);
    doe_printf(hDlg,IDC_EDIT26,"%f",p_anim->p_pkeys[klic].tension);
    doe_printf(hDlg,IDC_EDIT27,"%f",p_anim->p_pkeys[klic].bias);

    doe_check_stlac(hDlg,IDC_CHECK3,TRUE);
  } else {
    // interpolace hodnot + disable

    EnableDlgItem(hDlg, IDC_CHECK3, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT3,  FALSE);
    EnableDlgItem(hDlg, IDC_EDIT7,  FALSE);
    EnableDlgItem(hDlg, IDC_EDIT15, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT25, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT26, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT44, FALSE);

    doe_printf(hDlg,IDC_EDIT25,"-");
    doe_printf(hDlg,IDC_EDIT26,"-");
    doe_printf(hDlg,IDC_EDIT27,"-");

    key_track_interpolace_bod(&p,p_anim->p_pos,p_anim->p_pkeys,
                               time,p_anim->endtime,p_anim->pos_keys,loop);

    doe_printf(hDlg,IDC_EDIT25,"%f",p.x);
    doe_printf(hDlg,IDC_EDIT26,"%f",p.y);
    doe_printf(hDlg,IDC_EDIT27,"%f",p.z);

    doe_check_stlac(hDlg,IDC_CHECK3,FALSE);
  }

  /* Disable cile - nemam to hotovy
  */
  EnableDlgItem(hDlg, IDC_CHECK4,  FALSE);
  EnableDlgItem(hDlg, IDC_EDIT16,  FALSE);
  EnableDlgItem(hDlg, IDC_EDIT17,  FALSE);
  EnableDlgItem(hDlg, IDC_EDIT18,  FALSE);
  EnableDlgItem(hDlg, IDC_EDIT45,  FALSE);
  EnableDlgItem(hDlg, IDC_EDIT46,  FALSE);
  EnableDlgItem(hDlg, IDC_EDIT47,  FALSE);
  doe_check_stlac(hDlg,IDC_CHECK4,FALSE);

  /* Dosah - klice
  */
  klic = key_je_time_klic(p_anim->p_dskeys, p_anim->dos_keys, time);
  if(klic != K_CHYBA) {
    EnableDlgItem(hDlg, IDC_CHECK5, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT19, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT24, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT51, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT48, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT49, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT50, TRUE);
    
    doe_printf(hDlg,IDC_EDIT19,"%f",p_anim->p_dosah[klic].y);
    doe_printf(hDlg,IDC_EDIT24,"%f",p_anim->p_dosah[klic].z);
    doe_printf(hDlg,IDC_EDIT51,"%f",p_anim->p_dosah[klic].x);

    doe_printf(hDlg,IDC_EDIT48,"%f",p_anim->p_dskeys[klic].continuity);
    doe_printf(hDlg,IDC_EDIT49,"%f",p_anim->p_dskeys[klic].tension);
    doe_printf(hDlg,IDC_EDIT50,"%f",p_anim->p_dskeys[klic].bias);
    
    doe_check_stlac(hDlg,IDC_CHECK5,TRUE);
  } else {
    // interpolace hodnot + disable

    EnableDlgItem(hDlg, IDC_CHECK5, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT19, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT24, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT51, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT48, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT49, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT50, FALSE);

    doe_printf(hDlg,IDC_EDIT48,"-");
    doe_printf(hDlg,IDC_EDIT49,"-");
    doe_printf(hDlg,IDC_EDIT50,"-");

    key_track_interpolace_bod(&p,p_anim->p_dosah,p_anim->p_dskeys,
                               time,p_anim->endtime,p_anim->dos_keys,loop);

    doe_printf(hDlg,IDC_EDIT19,"%f",p.y);
    doe_printf(hDlg,IDC_EDIT24,"%f",p.z);
    doe_printf(hDlg,IDC_EDIT51,"%f",p.x);

    doe_check_stlac(hDlg,IDC_CHECK5,FALSE);
  }

  if(extra_light) {
    EnableDlgItem(hDlg, IDC_EDIT19, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT24, FALSE);
  }

  /* Diffusni barvy - klice
  */
  klic = key_je_time_klic(p_anim->p_dfkeys, p_anim->diff_keys, time);
  if(klic != K_CHYBA) {
    EnableDlgItem(hDlg, IDC_CHECK25,TRUE);
    EnableDlgItem(hDlg, IDC_EDIT55, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT74, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT75, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT79, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT81, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT82, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT83, TRUE);

    doe_printf(hDlg,IDC_EDIT55,"%f",p_anim->p_diff[klic].r);
    doe_printf(hDlg,IDC_EDIT74,"%f",p_anim->p_diff[klic].g);
    doe_printf(hDlg,IDC_EDIT75,"%f",p_anim->p_diff[klic].b);
    doe_printf(hDlg,IDC_EDIT79,"%f",p_anim->p_diff[klic].a);

    doe_printf(hDlg,IDC_EDIT81,"%f",p_anim->p_dfkeys[klic].continuity);
    doe_printf(hDlg,IDC_EDIT82,"%f",p_anim->p_dfkeys[klic].tension);
    doe_printf(hDlg,IDC_EDIT83,"%f",p_anim->p_dfkeys[klic].bias);

    doe_check_stlac(hDlg,IDC_CHECK25,TRUE);
  } else {
    // interpolace hodnot + disable
    EnableDlgItem(hDlg, IDC_CHECK25,FALSE);
    EnableDlgItem(hDlg, IDC_EDIT55, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT74, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT75, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT79, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT81, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT82, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT83, FALSE);

    doe_printf(hDlg,IDC_EDIT81,"-");
    doe_printf(hDlg,IDC_EDIT82,"-");
    doe_printf(hDlg,IDC_EDIT83,"-");

    key_track_interpolace_wbod((WBOD *)&b,(WBOD *)p_anim->p_diff,p_anim->p_dfkeys,
                               time,p_anim->endtime,p_anim->diff_keys,loop);

    doe_printf(hDlg,IDC_EDIT19,"%f",b.r);
    doe_printf(hDlg,IDC_EDIT24,"%f",b.g);
    doe_printf(hDlg,IDC_EDIT51,"%f",b.b);
    doe_printf(hDlg,IDC_EDIT51,"%f",b.a);

    doe_check_stlac(hDlg,IDC_CHECK25,FALSE);
  }

  if(extra_light) {
    /* Spekularni barvy - klice
    */
    klic = key_je_time_klic(p_anim->p_spkeys, p_anim->spec_keys, time);
    if(klic != K_CHYBA) {
      EnableDlgItem(hDlg, IDC_CHECK28, TRUE);
      EnableDlgItem(hDlg, IDC_EDIT76, TRUE);
      EnableDlgItem(hDlg, IDC_EDIT77, TRUE);
      EnableDlgItem(hDlg, IDC_EDIT78, TRUE);
      EnableDlgItem(hDlg, IDC_EDIT84, TRUE);
      EnableDlgItem(hDlg, IDC_EDIT85, TRUE);
      EnableDlgItem(hDlg, IDC_EDIT86, TRUE);
      
      doe_printf(hDlg,IDC_EDIT76,"%f",p_anim->p_spec[klic].r);
      doe_printf(hDlg,IDC_EDIT77,"%f",p_anim->p_spec[klic].g);
      doe_printf(hDlg,IDC_EDIT78,"%f",p_anim->p_spec[klic].b);
      
      doe_printf(hDlg,IDC_EDIT84,"%f",p_anim->p_spkeys[klic].continuity);
      doe_printf(hDlg,IDC_EDIT85,"%f",p_anim->p_spkeys[klic].tension);
      doe_printf(hDlg,IDC_EDIT86,"%f",p_anim->p_spkeys[klic].bias);
      
      doe_check_stlac(hDlg,IDC_CHECK28,TRUE);
    } else {
      // interpolace hodnot + disable
    
      EnableDlgItem(hDlg, IDC_CHECK28, FALSE);
      EnableDlgItem(hDlg, IDC_EDIT25, FALSE);
      EnableDlgItem(hDlg, IDC_EDIT74, FALSE);
      EnableDlgItem(hDlg, IDC_EDIT75, FALSE);
      EnableDlgItem(hDlg, IDC_EDIT79, FALSE);
      EnableDlgItem(hDlg, IDC_EDIT81, FALSE);
      EnableDlgItem(hDlg, IDC_EDIT82, FALSE);
      EnableDlgItem(hDlg, IDC_EDIT83, FALSE);
      
      doe_printf(hDlg,IDC_EDIT84,"-");
      doe_printf(hDlg,IDC_EDIT85,"-");
      doe_printf(hDlg,IDC_EDIT86,"-");
      
      key_track_interpolace_bod(&p,(BOD *)p_anim->p_spec,p_anim->p_spkeys,
        time,p_anim->endtime,p_anim->spec_keys,loop);
      
      doe_printf(hDlg,IDC_EDIT76,"%f",p.x);
      doe_printf(hDlg,IDC_EDIT77,"%f",p.y);
      doe_printf(hDlg,IDC_EDIT78,"%f",p.z);
      
      doe_check_stlac(hDlg,IDC_CHECK28,FALSE);
    }
  } else {
    EnableDlgItem(hDlg, IDC_EDIT76, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT77, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT78, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT84, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT85, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT86, FALSE);
    EnableDlgItem(hDlg, IDC_CHECK28,FALSE);
    doe_printf(hDlg,IDC_EDIT84,"-");
    doe_printf(hDlg,IDC_EDIT85,"-");
    doe_printf(hDlg,IDC_EDIT86,"-");
  }
}

void doe_svetla_dyn_vloz(HWND hDlg, DYN_LIGHT *p_dlight)
{
  if(p_dlight->an.endtime) {    
    EnableDlgItem(hDlg, IDC_EDIT4, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT1, TRUE);
    EnableDlgItem(hDlg, IDC_EDIT2, TRUE);
    EnableDlgItem(hDlg, IDC_CHECK7, TRUE);
    doe_check_stlac(hDlg,IDC_CHECK1,TRUE);
    doe_check_stlac(hDlg,IDC_CHECK7,p_dlight->an_flag&GK_LOOP);
    doe_printf(hDlg,IDC_EDIT4,"%d",endtime2framenum(p_dlight->an.endtime));
    doe_printf(hDlg,IDC_STATIC3,"%d",endtime2framenum(p_dlight->an.endtime));
    doe_printf(hDlg,IDC_STATIC8,"%d",p_dlight->an.endtime);
    doe_printf(hDlg,IDC_EDIT1,"%d",time2frame(p_dlight->time));
    doe_printf(hDlg,IDC_EDIT2,"%d",p_dlight->time);
  } else {
    EnableDlgItem(hDlg, IDC_EDIT4, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT1, FALSE);
    EnableDlgItem(hDlg, IDC_EDIT2, FALSE);
    EnableDlgItem(hDlg, IDC_CHECK7, FALSE);
    doe_check_stlac(hDlg,IDC_CHECK1,FALSE);
    doe_check_stlac(hDlg,IDC_CHECK7,FALSE);
    doe_printf(hDlg,IDC_EDIT4,"0");
    doe_printf(hDlg,IDC_STATIC3,"0");
    doe_printf(hDlg,IDC_STATIC8,"0");
    doe_printf(hDlg,IDC_EDIT1,"0");
    doe_printf(hDlg,IDC_EDIT2,"0");
  }
  // ... Disable/enable podle animaci (dyn/extra)
  doe_svetla_dyn_vloz_anim(hDlg, p_dlight->time, &p_dlight->an, p_dlight->an_flag&GK_LOOP, FALSE);
}

void doe_svetla_dyn_vloz_combo(HWND hDlg)
{
  DYN_LIGHT *p_light;
  HWND combo = GetDlgItem(hDlg,IDC_COMBO4);
  int  k,i,p,akt = K_CHYBA;

  SendMessage(combo, CB_RESETCONTENT, 0, 0);

  if(p_cnf->lc.scena_light) {
    for(i = 0; i < MAX_FLARE_SVETEL; i++) {
      if(p_cnf->p_dlight[i].akt) {
        if(!(p_cnf->p_dlight[i].flag&SDL_MESH)) {          
          p = SendMessage( combo, CB_ADDSTRING, 0,(LPARAM)p_cnf->p_dlight[i].jmeno); 
          SendMessage( combo, CB_SETITEMDATA,p,i);
          if(i == p_cnf->dl_akt)
            akt = p;
        }
      }
    } 
    if(akt == K_CHYBA)
      akt = 0;
  } else {
    k = p_cnf->vybr_kont;
    if(p_cnf->p_kont[k]) {
      p_light = p_cnf->p_kont[k]->p_dlight;
      while(p_light) {
        p = SendMessage( combo, CB_ADDSTRING, 0,(LPARAM)p_light->jmeno);
        SendMessage( combo, CB_SETITEMDATA,p,p_light->index);
        if(p_cnf->dl_akt == p_light->index)
          akt = p;
        p_light = p_light->p_next;
      }
    }
    if(akt == K_CHYBA) {
      akt = 0;
      if(p_cnf->p_kont[k] && p_cnf->p_kont[k]->p_dlight) {
        p_cnf->dl_akt = p_cnf->p_kont[k]->p_dlight->index;
      }
    }
  }
  SendMessage(combo, CB_SETCURSEL, akt, 0);
}

void doe_svetla_exdyn_vloz(HWND hDlg, EXTRA_DYN_LIGHT *p_dlight)
{
  if(p_dlight->an.endtime) {    
    doe_check_stlac(hDlg,IDC_CHECK1,TRUE);
    doe_check_stlac(hDlg,IDC_CHECK7,p_dlight->an_flag&GK_LOOP);
    doe_printf(hDlg,IDC_EDIT4,"%d",endtime2framenum(p_dlight->an.endtime));
    doe_printf(hDlg,IDC_STATIC3,"%d",endtime2framenum(p_dlight->an.endtime));
    doe_printf(hDlg,IDC_STATIC8,"%d",p_dlight->an.endtime);
    doe_printf(hDlg,IDC_EDIT1,"%d",time2frame(p_dlight->time));
    doe_printf(hDlg,IDC_EDIT2,"%d",p_dlight->time);
  } else {
    doe_check_stlac(hDlg,IDC_CHECK1,FALSE);
    doe_check_stlac(hDlg,IDC_CHECK7,FALSE);
    doe_printf(hDlg,IDC_EDIT4,"0");
    doe_printf(hDlg,IDC_STATIC3,"0");
    doe_printf(hDlg,IDC_STATIC8,"0");
    doe_printf(hDlg,IDC_EDIT1,"0");
    doe_printf(hDlg,IDC_EDIT2,"0");
  }
  // ... Disable/enable podle animaci (dyn/extra)
  doe_svetla_dyn_vloz_anim(hDlg, p_dlight->time, &p_dlight->an, p_dlight->an_flag&GK_LOOP, TRUE);
}

void doe_svetla_exdyn_vloz_combo(HWND hDlg)
{
  EXTRA_DYN_LIGHT *p_light;
  HWND combo = GetDlgItem(hDlg,IDC_COMBO4);
  int  k,i,p,akt = K_CHYBA;

  SendMessage(combo, CB_RESETCONTENT, 0, 0);

  if(p_cnf->lc.scena_light) {
    for(i = 0; i < MAX_FLARE_SVETEL; i++) {
      if(p_cnf->p_edlight[i].akt) {
        if(!(p_cnf->p_edlight[i].flag&EDL_MESH)) {
          p = SendMessage( combo, CB_ADDSTRING, 0,(LPARAM)p_cnf->p_edlight[i].jmeno); 
          SendMessage( combo, CB_SETITEMDATA,p,i);
          if(i == p_cnf->edl_akt)
            akt = p;
        }
      }
    } 
    if(akt == K_CHYBA)
      akt = 0;
  } else {
    k = p_cnf->vybr_kont;
    if(p_cnf->p_kont[k]) {
      p_light = p_cnf->p_kont[k]->p_edlight;
      while(p_light) {
        p = SendMessage( combo, CB_ADDSTRING, 0,(LPARAM)p_light->jmeno);
        SendMessage( combo, CB_SETITEMDATA,p,p_light->index);
        if(p_cnf->edl_akt == p_light->index)
          akt = p;
        p_light = p_light->p_next;
      }
    }
    if(akt == K_CHYBA) {
      akt = 0;
      if(p_cnf->p_kont[k] && p_cnf->p_kont[k]->p_edlight) {
        p_cnf->edl_akt = p_cnf->p_kont[k]->p_edlight->index;
      }
    }
  }
  SendMessage(combo, CB_SETCURSEL, akt, 0);
}

void doe_svetla_prepis(HWND hDlg)
{
  if(p_cnf->lc.extra_svetlo) {
    doe_svetla_exdyn_vloz_combo(hDlg);
    doe_svetla_exdyn_vloz(hDlg, p_cnf->p_edlight+p_cnf->edl_akt);
  } else {
    doe_svetla_dyn_vloz_combo(hDlg);
    doe_svetla_dyn_vloz(hDlg, p_cnf->p_dlight+p_cnf->dl_akt);
  }
}

void doe_svetla_najdi_aktivni(void)
{
  if(p_cnf->lc.extra_svetlo) {
    p_cnf->edl_akt = lo_najdi_plne_extra_svetlo(p_cnf->p_edlight, MAX_FLARE_SVETEL);
    if(p_cnf->edl_akt == K_CHYBA)
      p_cnf->edl_akt = 0;
  } else {
    p_cnf->dl_akt = lo_najdi_plne_dsvetlo(p_cnf->p_dlight, MAX_FLARE_SVETEL);
    if(p_cnf->dl_akt == K_CHYBA)
      p_cnf->dl_akt = 0;
  }
}

BOOL CALLBACK doe_calback_konfigurace_svetla_dyn(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {       
        case IDC_COMBO4:
          if(HIWORD(wParam) ==  CBN_SELCHANGE) {
            HWND combo = (HWND)lParam;
            if(p_cnf->lc.extra_svetlo) {
              p_cnf->edl_akt = SendMessage(combo,CB_GETITEMDATA,SendMessage(combo,CB_GETCURSEL,0,0),0);
              doe_svetla_exdyn_vloz(hDlg, p_cnf->p_edlight+p_cnf->edl_akt);
            } else {
              p_cnf->dl_akt = SendMessage(combo,CB_GETITEMDATA,SendMessage(combo,CB_GETCURSEL,0,0),0);
              doe_svetla_dyn_vloz(hDlg, p_cnf->p_dlight+p_cnf->dl_akt);
            }
          }
          break;
        case IDC_BUTTON8: // nove svetlo
          if(p_cnf->lc.extra_svetlo) {
            oe_prikaz_extra_svetlo_nove(p_cnf);
          } else {
            oe_prikaz_dsvetlo_nove(p_cnf);
          }
          doe_svetla_prepis(hDlg);
          break;
        case IDC_BUTTON7: // smazat svetlo
          if(p_cnf->lc.extra_svetlo) {
            oe_prikaz_extra_svetlo_smaz(p_cnf);
          } else {
            oe_prikaz_dsvetlo_smaz(p_cnf);
          }
          doe_svetla_prepis(hDlg);
          break;
        case IDC_BUTTON4: // upravit svetlo
          if(p_cnf->lc.extra_svetlo) {
            oe_prikaz_extra_svetlo_uprav(p_cnf);
          } else {
            oe_prikaz_dsvetlo_uprav(p_cnf);
          }
          doe_svetla_prepis(hDlg);
          break;                
        case IDC_BUTTON16: // posunout svetlo
          if(p_cnf->lc.extra_svetlo) {
            cekej_stisk_mysi_br();
            oe_prikaz_extra_svetlo_posun(p_cnf);
          } else {
            cekej_stisk_mysi_br();
            oe_prikaz_dsvetlo_posun(p_cnf);
          }
          doe_svetla_prepis(hDlg);
          break;
        case IDC_BUTTON17: // kompletni seznam svetel
          doe_seznam_svetel(p_cnf);
          break;
        case IDC_RADIO1:
          p_cnf->lc.extra_svetlo = !IsDlgButtonChecked(hDlg,IDC_RADIO1);
          doe_svetla_prepis(hDlg);
          break;
        case IDC_RADIO2:
          p_cnf->lc.extra_svetlo =  IsDlgButtonChecked(hDlg,IDC_RADIO2);
          doe_svetla_prepis(hDlg);
          break;
        case IDC_RADIO4:
          p_cnf->lc.scena_light = IsDlgButtonChecked(hDlg,IDC_RADIO4);
          doe_svetla_najdi_aktivni();
          doe_svetla_prepis(hDlg);
          break;
        case IDC_RADIO5:
          p_cnf->lc.scena_light = !IsDlgButtonChecked(hDlg,IDC_RADIO5);
          doe_svetla_najdi_aktivni();
          doe_svetla_prepis(hDlg);
          break;        
/*
        case IDC_CHECK1:
          {
            DYN_LIGHT_ANIM *p_anim;
            int i;
            if(p_cnf->lc.extra_svetlo) {
              if(p_cnf->edl_akt < MAX_FLARE_SVETEL && p_cnf->p_edlight[p_cnf->edl_akt].akt)
                p_anim = &p_cnf->p_edlight[p_cnf->edl_akt].an;
              else
                break;
            } else {
              if(p_cnf->dl_akt < MAX_FLARE_SVETEL && p_cnf->p_dlight[p_cnf->dl_akt].akt)
                p_anim = &p_cnf->p_dlight[p_cnf->dl_akt].an;
              else
                break;
            }
            if(IsDlgButtonChecked(hDlg,IDC_CHECK1)) {
              // vyrobit animaci
              i = 100;
              doe_animace_uprav(hDlg, &i);
              p_anim->endtime = calc_endtime(i);
              doe_svetla_prepis(hDlg);
            } else {
              // zrusit animaci
              sdla_animaci_zrus(p_anim);
              doe_svetla_prepis(hDlg);              
            }
          }
          break;
*/

        default:
          break;
      }
      break;
    case WM_INITDIALOG: 
      {
        nastav_pozici_bl();
        p_cnf->lc.hDlg = hDlg;
        
        doe_check_stlac(hDlg, IDC_RADIO1,!p_cnf->lc.extra_svetlo);
        doe_check_stlac(hDlg, IDC_RADIO2, p_cnf->lc.extra_svetlo);
        doe_check_stlac(hDlg, IDC_RADIO4, p_cnf->lc.scena_light);
        doe_check_stlac(hDlg, IDC_RADIO5,!p_cnf->lc.scena_light);

        doe_svetla_prepis(hDlg);

        break;
      }
    case WM_DESTROY: 
      {           
        p_cnf->lc.hDlg = FALSE;
        EndDialog( hDlg, FALSE );
        break; 
      }
    default:
      return FALSE;
  }
  return TRUE;
}

static void doe_text_slide_vloz_tiky(HWND hDlg, K_EDITOR *p_cnf)
{
  EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
  ANIM_TEXT *p_track;  
  int i;
  
  doe_hs_delmark();

  if(!p_mat) 
    return;
  p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
  if(!p_track)
    return;
     
  for(i = 0; i < p_track->pos_keys; i++)
    doe_hs_setmark(time2frame(p_track->p_pkeys[i].time));
  for(i = 0; i < p_track->rot_keys; i++)
    doe_hs_setmark(time2frame(p_track->p_rkeys[i].time));
  for(i = 0; i < p_track->scs_keys; i++)
    doe_hs_setmark(time2frame(p_track->p_skeys[i].time));
  for(i = 0; i < p_track->piv_keys; i++)
    doe_hs_setmark(time2frame(p_track->p_vkeys[i].time));
}

static void doe_init_textury_animace(HWND hDlg, EDIT_MATERIAL *p_mat)
{
  ANIM_TEXT *p_track;
  byte    pom[200];
  int     des,sec,min;
  int     framenum, akt;

  if(!p_mat)
    return;

  akt = p_mat->atextakt[p_cnf->akmat.par1];
  p_track = p_mat->p_atext[p_cnf->akmat.par1][akt];
  
  if(p_track) {   
    framenum = calc_framenum(p_track->endtime);
    SetDlgItemText(hDlg, IDC_STATIC2, "Aktivni");
    SetDlgItemText(hDlg, IDC_STATIC3, itoa(framenum,pom,10)); // framu
    sprintf(pom,"%f",(float)framenum/(float)FRAMU_PER_SEC);
    SetDlgItemText(hDlg, IDC_STATIC8, pom); // sec
    SetDlgItemText(hDlg, IDC_STATIC27,itoa(FRAMU_PER_SEC,pom,10));// fps
    
    SetDlgItemText(hDlg, IDC_EDIT1, itoa(time2frame(p_track->time),pom,10));
    
    doe_sec_to_time(time2sec(p_track->time), &des, &sec, &min);
    sprintf(pom,"%.2d:%.2d:%.2d",min,sec,des);
    SetDlgItemText( hDlg, IDC_EDIT2, pom);  
        
    doe_hs_nastav(0,framenum-1,0,"Text animace");
    doe_text_slide_vloz_tiky(hDlg, p_cnf);

    doe_check_stlac(hDlg,IDC_CHECK9,p_track->flag&ANIM_TEXT_DEFAULT);
  } else {
    SetDlgItemText(hDlg, IDC_STATIC2, "Neaktivni");
    SetDlgItemText(hDlg, IDC_STATIC3, "0");
    SetDlgItemText(hDlg, IDC_STATIC8, "0");
    SetDlgItemText(hDlg, IDC_STATIC27,"0");    
    SetDlgItemText(hDlg, IDC_EDIT1, "0");    
    SetDlgItemText( hDlg, IDC_EDIT2, "00:00:00");          
    doe_hs_nastav(0,0,0,"Text animace");  
    doe_text_slide_vloz_tiky(hDlg, p_cnf);
  }
}

void doe_text_animace_povol(HWND hDlg, K_EDITOR *p_cnf)
{
  EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
  int stav,akt;  
  
  if(p_mat) {
    akt = p_cnf->akmat.par1;
    stav = p_mat->flag&glstav_posun_flagy2[akt] && p_mat->p_atext[akt];
  } else
    stav = FALSE;
  
  EnableDlgItem(hDlg,IDC_EDIT1,stav);
  EnableDlgItem(hDlg,IDC_EDIT2,stav);
  EnableDlgItem(hDlg,IDC_CHECK1,stav);
  EnableDlgItem(hDlg,IDC_CHECK2,stav);
  EnableDlgItem(hDlg,IDC_CHECK3,stav);
  EnableDlgItem(hDlg,IDC_CHECK4,stav);
  EnableDlgItem(hDlg,IDC_CHECK5,stav);
  EnableDlgItem(hDlg,IDC_LIST1,stav);
}

#define ANIM_TEXT_1 0
#define ANIM_TEXT_2 1
#define ANIM_TEXT_3 2
#define ANIM_TEXT_4 3

void doe_text_animace_pln_listbox(HWND hDlg, K_EDITOR *p_cnf)
{
  EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
  ANIM_TEXT **p_track;
  HWND hCombo;
  int  i;
  byte pom[200];

  hCombo = GetDlgItem( hDlg, IDC_LIST1 );
  SendMessage(hCombo, LB_RESETCONTENT, 0, 0);

  if(!p_mat)
    return;

  p_track = p_mat->p_atext[p_cnf->akmat.par1];
  
  for(i = 0; i < MAX_TEXT_ANIM; i++) {
    if(p_track[i]) {
      sprintf(pom,"%d Aktivni",i);
    } else {
      strcpy(pom,"...");
    }      
    SendMessage( hCombo, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) pom);
  }   
  SendMessage(hCombo,LB_SETCURSEL,(WPARAM)p_mat->atextakt[p_cnf->akmat.par1],0);
}

void doe_text_animace_material_combo(HWND hDlg, K_EDITOR *p_cnf)
{
  HWND hCombo;
  int  i,s,v = 0;
  byte pom[200];

  hCombo = GetDlgItem( hDlg, IDC_COMBO1 );
  SendMessage(hCombo, CB_RESETCONTENT, 0, 0);  
  
  for(i = 0, s = 0; i < MAX_EDIT_MATERIALU; i++) {
    if(p_cnf->p_mat[i]) {
      sprintf(pom,"%d %s",i,p_cnf->p_mat[i]->jmeno);      
      SendMessage(hCombo, CB_SETITEMDATA, 
                  SendMessage( hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) pom),
                  i);
      if(i == p_cnf->km.vybr_mat)
        v = s;
      s++;
    }
  }   
  SendMessage(hCombo,CB_SETCURSEL,v,0);
}

BOOL CALLBACK doe_calback_konfigurace_text_animace(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_COMBO1:
          {
            if(HIWORD(wParam) == CBN_SELCHANGE) {
              p_cnf->km.vybr_mat = SendMessage((HWND)lParam, CB_GETITEMDATA,
                                   SendMessage((HWND)lParam, CB_GETCURSEL,0,0),
                                   0);              
              
              CheckDlgButton( hDlg, IDC_RADIO1, p_cnf->akmat.par1 == ANIM_TEXT_1);
              CheckDlgButton( hDlg, IDC_RADIO2, p_cnf->akmat.par1 == ANIM_TEXT_2);
              CheckDlgButton( hDlg, IDC_RADIO3, p_cnf->akmat.par1 == ANIM_TEXT_3);
              CheckDlgButton( hDlg, IDC_RADIO4, p_cnf->akmat.par1 == ANIM_TEXT_4);              

              doe_text_animace_pln_listbox(hDlg, p_cnf);
              doe_init_textury_animace(hDlg,p_cnf->p_mat[p_cnf->km.vybr_mat]);
              doe_animace_updatuj_materialy_text(p_cnf);
              doe_text_animace_povol(hDlg, p_cnf);
            }
          }
          break;
        case IDC_LIST1:
          if(HIWORD(wParam) == LBN_SELCHANGE) {
            EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            HWND hlist = GetDlgItem( hDlg, IDC_LIST1);            
            if(p_mat) {              
              p_mat->atextakt[p_cnf->akmat.par1] = SendMessage(hlist,LB_GETCURSEL,0,0);
              doe_init_textury_animace(hDlg,p_cnf->p_mat[p_cnf->km.vybr_mat]);
              doe_animace_updatuj_materialy_text(p_cnf);
              doe_text_animace_povol(hDlg, p_cnf);
            }
          }
          break;
        case IDC_BUTTON9: // load
        {
          EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
          ANIM_TEXT *p_track;
          byte pom[200];
          if(!p_mat)
            break;
          p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
          if(p_track) {
            if(!ddwquetot(hwnd_aplikace,"Mazba?","Smazat aktualni animaci ?"))
              break;
          }

          if(doe_dialog_load(hwnd_aplikace, pom, 200, "Vyber soubor s kamerou", "*.tam","Soubor s animacema textur (*.tam)")) {
            if(p_track) {              
              key_zrus_material_animace(p_mat->p_atext[p_cnf->akmat.par1]+p_mat->atextakt[p_cnf->akmat.par1]);
              p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]] = key_text_nahraj(pom,".");
            }
            doe_text_animace_pln_listbox(hDlg, p_cnf);
            doe_init_textury_animace(hDlg,p_mat);
            doe_text_animace_povol(hDlg, p_cnf);
          }         
          break;
        }
        case IDC_BUTTON25: // save
          {
            EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            ANIM_TEXT *p_track;
            byte pom[200];
            if(!p_mat)
              break;
            p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
            if(!p_track)
              break;
            if(doe_dialog_save(pom, 200, "Ulozit animaci jako", "*.tam",
                              "Soubor s animacema textur (*.tam)")) {              
              if(key_text_uloz(p_track, pom, "."))
                kprintf(TRUE,"Save animace do %s OK",pom);
            }
            break;
          }
        case IDC_BUTTON5: // vyrobit 
          {
            EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            ANIM_TEXT *p_track;
            byte pom[200];
            int  framenum;
            if(!p_mat)
              break;
            p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
            if(p_track) {
              if(!ddwquetot(hwnd_aplikace,"Mazba?","Smazat aktualni animaci ?"))
                break;
            }
            pom[0] = 0;
            if(doe_cti_jmeno_ext(hDlg, pom, "Pocet framu animace")) {
              framenum = atoi(pom);
              if(framenum) {
                p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]] = key_vyrob_material_animace(0,0,0,0);
                p_track->endtime = calc_endtime(framenum);
              }
              doe_text_animace_pln_listbox(hDlg, p_cnf);
              doe_init_textury_animace(hDlg,p_mat);
              doe_text_animace_povol(hDlg, p_cnf);
            }
            break;
          }
          break;
        case IDC_BUTTON20:
          {
            EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            ANIM_TEXT *p_track;
            byte pom[200];                        
            if(!p_mat)
              break;            
            p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
            if(p_track) {
              pom[0] = 0;
              if(doe_cti_jmeno_ext(hDlg, pom, "Pocet framu animace")) {
                p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
                p_track->endtime = calc_endtime(atoi(pom));
              }                
            }            
            doe_init_textury_animace(hDlg,p_mat);            
            break;
          }
        case IDC_BUTTON12: // smazat 
          {
            EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            ANIM_TEXT *p_track;            
            if(!p_mat)
              break;
            p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];            
            if(!p_track)
              break;
            if(ddwqueto(hwnd_aplikace,"Smazat animaci textury?")) {
              
              key_zrus_material_animace(p_mat->p_atext[p_cnf->akmat.par1]+p_mat->atextakt[p_cnf->akmat.par1]);
              
              doe_text_animace_pln_listbox(hDlg, p_cnf);
              doe_init_textury_animace(hDlg,p_mat);
              doe_text_animace_povol(hDlg, p_cnf);
            }
            break;
          }          
        case IDC_RADIO1:
          {
            EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            if(IsDlgButtonChecked( hDlg, IDC_RADIO1))
              p_cnf->akmat.par1 = ANIM_TEXT_1;
            doe_text_animace_pln_listbox(hDlg, p_cnf);
            doe_init_textury_animace(hDlg,p_mat);
            doe_text_animace_povol(hDlg,p_cnf);
            break;
          }
        case IDC_RADIO2:
          {
            EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            if(IsDlgButtonChecked( hDlg, IDC_RADIO2))
              p_cnf->akmat.par1 = ANIM_TEXT_2;
            doe_text_animace_pln_listbox(hDlg, p_cnf);
            doe_init_textury_animace(hDlg,p_mat);
            doe_text_animace_povol(hDlg,p_cnf);
            break;
          }
        case IDC_RADIO3:
          {
            EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            if(IsDlgButtonChecked( hDlg, IDC_RADIO3))
              p_cnf->akmat.par1 = ANIM_TEXT_3;
            doe_text_animace_pln_listbox(hDlg, p_cnf);
            doe_init_textury_animace(hDlg,p_mat);
            doe_text_animace_povol(hDlg,p_cnf);
            break;
          }
        case IDC_RADIO4:
          {
            EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            if(IsDlgButtonChecked( hDlg, IDC_RADIO4))
              p_cnf->akmat.par1 = ANIM_TEXT_4;
            doe_text_animace_pln_listbox(hDlg, p_cnf);
            doe_init_textury_animace(hDlg,p_mat);
            doe_text_animace_povol(hDlg,p_cnf);
            break;
          }
        case IDC_CHECK9:
          {
            EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            ANIM_TEXT *p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
            if(p_track) {                 
              p_track->flag = doe_get_check_flag(hDlg,IDC_CHECK9,p_track->flag,ANIM_TEXT_DEFAULT);
            }
            break;
          }
        case IDC_BUTTON4: { // <<
          EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
          ANIM_TEXT *p_track;            
          if(!p_mat)
            break;
          p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
          if(!p_track)
            break;
          if(p_track->endtime) {
            doe_anim_start_et(p_cnf->TimeEndLastFrame,p_track->endtime,&p_track->time_start,&p_track->time_stop,&p_track->time_delka,&p_track->time,&p_track->start,&p_track->stop);
          }
          break;
        }
        case IDC_BUTTON8: { // []
          EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
          ANIM_TEXT *p_track;            
          if(!p_mat)
            break;
          p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
          if(!p_track)
            break;
          if(p_track->endtime) {
            doe_anim_start_et(p_cnf->TimeEndLastFrame,p_track->endtime,&p_track->time_start,&p_track->time_stop,&p_track->time_delka,&p_track->time,&p_track->start,&p_track->stop);
            p_track->stav = 0; // stop animace
          }
          break;
        }
        case IDC_BUTTON10: {// || // pauza
          EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
          ANIM_TEXT *p_track;            
          if(!p_mat)
            break;
          p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
          if(!p_track)
            break;
          if(p_track->endtime) {
            doe_anim_start_et(p_cnf->TimeEndLastFrame,p_track->endtime,&p_track->time_start,&p_track->time_stop,&p_track->time_delka,&p_track->time,&p_track->start,&p_track->stop);
            p_track->stav = 0; // stop animace
          }
          break;
        }
        case IDC_BUTTON11: {// >
          EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
          ANIM_TEXT *p_track;            
          if(!p_mat)
            break;
          p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
          if(!p_track)
            break;
          if(p_track->endtime) {
            if(!p_track->stav) {
              doe_anim_start_et(p_cnf->TimeEndLastFrame,p_track->endtime,&p_track->time_start,&p_track->time_stop,&p_track->time_delka,&p_track->time,&p_track->start,&p_track->stop);
            }
            p_track->stav = !p_track->stav;
          }
          break;
        }
        case IDC_BUTTON13: {// >>
          EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
          ANIM_TEXT *p_track;            
          if(!p_mat)
            break;
          p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
          if(!p_track)
            break;
          if(p_track->endtime) {
            doe_anim_end_et(p_cnf->TimeEndLastFrame,p_track->endtime,&p_track->time_start,&p_track->time_stop,&p_track->time_delka,&p_track->time,&p_track->start,&p_track->stop);
          }
          break;
        }
        case IDC_BUTTON1:
          doe_editor_materialu(p_cnf);
          doe_text_animace_povol(hDlg,p_cnf);
          break;
        case IDC_CHECK5:
          {
            EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            ANIM_TEXT *p_track;
            if(!p_mat)
              break;
            p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
            if(!p_track)
              break;
            p_track->flag = IsDlgButtonChecked(hDlg,IDC_CHECK5) ? p_track->flag|GK_LOOP : p_track->flag&~GK_LOOP;
            break;
          }
        case IDC_CHECK1: // pos key
          {
            KEY_POINT_BRS key = {0,0,0,0,0,0};
            EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            ANIM_TEXT *p_track;
            byte pom[200];
            if(!p_mat)
              break;
            p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
            if(!p_track)
              break;
            
            GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
            key.time = frame2time(atoi(pom));
            
            /* Test vlozeni/zruseni klice tracku
            */
            if(key_je_time_klic(p_track->p_pkeys, p_track->pos_keys, key.time) != K_CHYBA) {
              key_track_klic_smaz((byte **)&p_track->p_pos, &p_track->p_pkeys, &p_track->pos_keys, sizeof(p_track->p_pos[0]),key.time);
              doe_text_slide_vloz_tiky(hDlg, p_cnf);
            } else { // vlozit
              p_track->pos_keys = key_track_klic_vloz((byte **)&p_track->p_pos, 
                &p_track->p_pkeys, p_track->pos_keys, 
                sizeof(p_track->p_pos[0]), (byte *)&p_track->pos, &key);
              doe_hs_setmark(time2frame(key.time));
            }
            doe_animace_updatuj_materialy_text(p_cnf);
            break;
          }        
        case IDC_CHECK4: // pivot key
          {
            KEY_POINT_BRS key = {0,0,0,0,0,0};
            EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            ANIM_TEXT *p_track;
            byte pom[200];
            if(!p_mat)
              break;
            p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
            if(!p_track)
              break;
            
            GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
            key.time = frame2time(atoi(pom));
            
            /* Test vlozeni/zruseni klice tracku
            */
            if(key_je_time_klic(p_track->p_vkeys, p_track->piv_keys, key.time) != K_CHYBA) {
              key_track_klic_smaz((byte **)&p_track->p_piv, &p_track->p_vkeys, 
                                  &p_track->piv_keys, 
                                  sizeof(p_track->p_piv[0]),key.time);
              doe_text_slide_vloz_tiky(hDlg, p_cnf);
            } else { // vlozit
              p_track->piv_keys = key_track_klic_vloz((byte **)&p_track->p_piv, 
                &p_track->p_vkeys, p_track->piv_keys, 
                sizeof(p_track->p_piv[0]), (byte *)&p_track->pivot, &key);
              doe_hs_setmark(time2frame(key.time));
            }
            doe_animace_updatuj_materialy_text(p_cnf);
            break;
          }
        case IDC_CHECK2: { // vloz scale-key
          EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
          ANIM_TEXT *p_track;          
          KEY_POINT_BRS key = {0,0,0,0,0,0};
          byte pom[200];
          
          if(!p_mat)
            break;
          p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
          if(!p_track)
            break;

          GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
          key.time = frame2time(atoi(pom));
          
          if(key_je_time_klic(p_track->p_skeys, p_track->scs_keys, key.time) != K_CHYBA) { // smazat
            key_track_klic_smaz((byte **)&p_track->p_scale, &p_track->p_skeys, 
              &p_track->scs_keys, sizeof(p_track->p_scale[0]),key.time);
            doe_text_slide_vloz_tiky(hDlg, p_cnf);
          } else { // vlozit
            p_track->scs_keys = key_track_klic_vloz((byte **)&p_track->p_scale,
              &p_track->p_skeys, p_track->scs_keys, sizeof(p_track->p_scale[0]), 
              (byte *)&p_track->scs, &key);            
            doe_hs_setmark(time2frame(key.time));
          }          
          doe_animace_updatuj_materialy_text(p_cnf);
          break;
        }
        case IDC_CHECK3: { // vloz roll_key
          EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
          ANIM_TEXT *p_track;
          KEY_POINT_BRS key = {0,0,0,0,0,0};
          byte pom[200];

          if(!p_mat)
            break;
          p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
          if(!p_track)
            break;

          GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
          key.time = frame2time(atoi(pom));
          
          if(key_je_time_klic(p_track->p_rkeys, p_track->rot_keys, key.time) != K_CHYBA) { // smazat
             key_track_klic_smaz((byte **)&p_track->p_rot, &p_track->p_rkeys, 
                     &p_track->rot_keys, sizeof(p_track->p_rot[0]),key.time);
            doe_text_slide_vloz_tiky(hDlg, p_cnf);
          } else { // vlozit
            p_track->rot_keys = key_track_klic_vloz((byte **)&p_track->p_rot,
              &p_track->p_rkeys, p_track->rot_keys, 
              sizeof(p_track->p_rot[0]), 
              (byte *)&p_track->rot, &key);
            doe_hs_setmark(time2frame(key.time));
          }
          doe_animace_updatuj_materialy_text(p_cnf);
          break;
        }
        case IDC_EDIT3:
        case IDC_EDIT7:
        case IDC_EDIT18:
        case IDC_EDIT19:
        case IDC_EDIT24:
        case IDC_EDIT15:
        case IDC_EDIT16:
          {
            EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            ANIM_TEXT *p_track;
            GLMATRIX  *p_ttext;
            if(!p_mat)
              break;
            p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
            if(!p_track || !p_track->endtime)
              break;
            p_ttext = p_mat->ttext+p_cnf->akmat.par1;

            doe_animace_nacti_materialy_text(p_cnf);
            key_sim_to_text_matrix(p_track, p_track->time, p_ttext);
            break;      
          }
        
        case IDC_EDIT1: {// editace framu
          EDIT_MATERIAL *p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
          ANIM_TEXT *p_track;          
          GLMATRIX  *p_ttext;
          byte pom[200];
          int  frame,framenum;
          if(!p_mat)
            break;
          p_track = p_mat->p_atext[p_cnf->akmat.par1][p_mat->atextakt[p_cnf->akmat.par1]];
          if(!p_track || !p_track->endtime)
            break;
          p_ttext = p_mat->ttext+p_cnf->akmat.par1;
          
          GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
          frame = atoi(pom); 
          framenum = calc_framenum(p_track->endtime);
          
          if(frame > framenum)
            frame = framenum;
          else if(frame < 0.0f)
            frame = 0;

          p_track->time = frame2time(frame);
          SetDlgItemText(hDlg, IDC_EDIT2, itoa(p_track->time,pom,10));
          doe_hs_setpos(frame);

          /* Interpolace podle akt hodnot
          */
          key_sim_to_text_matrix(p_track, p_track->time, p_ttext);
          
          /* Nacte hodnoty z tracku do edit-boxu
          */
          doe_animace_updatuj_materialy_text(p_cnf); // nacti hodnoty s klicu

          break;
        }
        default:
          break;
      }
      break;
    case WM_INITDIALOG: {
      nastav_pozici_bl();
      p_cnf->akmat.aktivni = TRUE;
      p_cnf->akmat.hwnd_dialog = hDlg;
      doe_hs_posfce(doe_animace_updatuj_materialy_slider);
      CheckDlgButton( hDlg, IDC_RADIO1, !p_cnf->akmat.par1);
      CheckDlgButton( hDlg, IDC_RADIO2,  p_cnf->akmat.par1);      
      doe_text_animace_pln_listbox(hDlg, p_cnf);
      doe_init_textury_animace(hDlg,p_cnf->p_mat[p_cnf->km.vybr_mat]);
      doe_animace_updatuj_materialy_text(p_cnf);
      doe_text_animace_povol(hDlg, p_cnf);
      doe_text_animace_material_combo(hDlg, p_cnf);
      break;
    }
    case WM_DESTROY: {
      p_cnf->akkam.aktivni = FALSE;
      p_cnf->akkam.hwnd_dialog = NULL;
      doe_hs_zrus();
      EndDialog( hDlg, FALSE );
      break; 
    }
    default:
      return FALSE;
  }
  return TRUE;
}


void doe_vyber_chapadelnika_do_matice(HWND hDlg, GLMATRIX *p_mat)
{
  float uhel;
  BOD   p,s,piv;
  
  p.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT3);
  p.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT7);
  s.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT19);
  s.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT24);
  uhel = DEG2RAD(doe_vyber_kolonku_float(hDlg,IDC_EDIT18));
  piv.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT15);
  piv.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT16);
  if(IsDlgButtonChecked(hDlg,IDC_CHECK3)) { // mirror U
    s.x *= -1.0f;
  }
  if(IsDlgButtonChecked(hDlg,IDC_CHECK4)) { // mirror V
    s.y *= -1.0f;
  }
  key_mat_to_text_matrix(p_mat, &p, uhel, &s, &piv);
}

BOOL CALLBACK doe_calback_konfigurace_text_animace_editace(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON1:
          { // aplikace na objekt
            GLMATRIX m;
            int o = p_cnf->vybr_objekt,
                k = p_cnf->vybr_kont, flag = 0;

            doe_vyber_chapadelnika_do_matice(hDlg, &m);
            flag = doe_get_check_flag(hDlg, IDC_CHECK1, flag, 0x1);
            flag = doe_get_check_flag(hDlg, IDC_CHECK2, flag, 0x2);
            
            if(p_cnf->groupfacenum) {
              if(oe_je_group_objekt(p_cnf)) {
                int i;
                for(i = 0; i < p_cnf->groupfacenum; i++) {
                  k = p_cnf->p_group_face[i].k;
                  o = p_cnf->p_group_face[i].o;
                  transformuj_objekt_text_coord(p_cnf->p_kont[k]->p_obj[o],&m,flag);
                }
              }
            } else {
              if(!p_cnf->p_kont[k] || !p_cnf->p_kont[k]->p_obj[o])
                break;              
              transformuj_objekt_text_coord(p_cnf->p_kont[k]->p_obj[o],&m,flag);
            }
          }
          break;
        case IDC_BUTTON2:
          { // aplikace na kontejner
            GLMATRIX m;          
            int k = p_cnf->vybr_kont, flag = 0;

            doe_vyber_chapadelnika_do_matice(hDlg, &m);
            flag = doe_get_check_flag(hDlg, IDC_CHECK1, flag, 0x1);
            flag = doe_get_check_flag(hDlg, IDC_CHECK2, flag, 0x2);

            if(p_cnf->groupfacenum) {
              if(oe_je_group_kontejner(p_cnf)) {
                int i;
                for(i = 0; i < p_cnf->groupfacenum; i++) {                  
                  transformuj_kontejner_text_coord(p_cnf->p_kont[p_cnf->p_group_face[i].k],&m,flag);
                }
              }              
            } else {
              if(!p_cnf->p_kont[k])
                break;              
              transformuj_kontejner_text_coord(p_cnf->p_kont[k],&m,flag);
            }
          }
          break;
        default:
          break;
      }
      break;
    case WM_INITDIALOG: {
      nastav_pozici_bl();
      
      doe_printf(hDlg,IDC_EDIT3, "0.0");
      doe_printf(hDlg,IDC_EDIT7, "0.0");
      doe_printf(hDlg,IDC_EDIT19,"1.0");
      doe_printf(hDlg,IDC_EDIT24,"1.0");
      doe_printf(hDlg,IDC_EDIT18,"0.0");
      doe_printf(hDlg,IDC_EDIT15,"0.0");
      doe_printf(hDlg,IDC_EDIT16,"0.0");
      
      doe_check_stlac(hDlg, IDC_CHECK1, TRUE);  // text 1
      doe_check_stlac(hDlg, IDC_CHECK2, FALSE); // text 2
      doe_check_stlac(hDlg, IDC_CHECK3, FALSE); // text 1
      doe_check_stlac(hDlg, IDC_CHECK4, FALSE); // text 2
      break;
    }
    case WM_DESTROY: {
      EndDialog( hDlg, FALSE );
      break; 
    }
    default:
      return FALSE;
  }
  return TRUE;
}

static void doe_vloz_stat_svetla(HWND hDlg, int lightakt, int combo)
{
  STATIC_LIGHT *p_light = NULL;
  byte pom[100];
  HWND hwnd = GetDlgItem( hDlg, IDC_COMBO1 );
  int i,k,id,
      idv = -1,
      scena = p_cnf->cnf.svetla_sceny,
      flag;  

  if(combo) {  
    SendMessage( hwnd, CB_RESETCONTENT, 0, 0 );
    if(scena) {
      for(i = 0; i < MAX_EDIT_SVETEL; i++) {
        if(p_cnf->light[i].akt) {
          flag = p_cnf->light[i].flag2&SL2_MESH;
          if(!flag) {
            sprintf(pom,"%s = {%.2f,%.2f,%.2f}",p_cnf->light[i].jmeno,p_cnf->light[i].p.x,p_cnf->light[i].p.y,p_cnf->light[i].p.z);
            id = SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) pom);
            SendMessage( hwnd, CB_SETITEMDATA, id, i);
            if(i == lightakt)
              idv = id;
          }
        }                
      }
      if(idv < 0) {
        lightakt = K_CHYBA;
        p_light = NULL;
      }
      else {
        SendMessage( hwnd, CB_SETCURSEL, idv, 0 );
        p_light = p_cnf->light+lightakt;
      }
    } else {
      k = p_cnf->vybr_kont;
      if(p_cnf->p_kont[k]) {
        p_light = p_cnf->p_kont[k]->p_slight;
        while(p_light) {
          sprintf(pom,"%s = {%.2f,%.2f,%.2f}",p_light->jmeno,p_light->p.x,p_light->p.y,p_light->p.z);
          id = SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) pom);
          SendMessage( hwnd, CB_SETITEMDATA, id, p_light->index);
          p_light = p_light->p_next;
        }
        SendMessage( hwnd, CB_SETCURSEL, 0, 0 );
        p_light = p_cnf->p_kont[k]->p_slight;
      }
    }
  } else {
    p_light = p_cnf->light+lightakt;
  }

  if(p_light && p_light->akt) {     
    CheckDlgButton(hDlg, IDC_CHECK7, p_light->bez_prev ? BST_CHECKED : BST_UNCHECKED);

    k = ftoi(p_light->k1*10.0f);
    doe_slider_nastav(hDlg,IDC_SLIDER4, 0,100, k);
    k = ftoi(p_light->k1*100.0f)%10;
    doe_slider_nastav(hDlg,IDC_SLIDER1, 0,100, k);
    doe_printf(hDlg,IDC_STATIC1,"Konstantni %f",p_light->k1);
    
    k = ftoi(p_light->k2*1000.0f);
    doe_slider_nastav(hDlg,IDC_SLIDER5, 0,100, k);
    k = ftoi(p_light->k2*100000.0f)%10;
    doe_slider_nastav(hDlg,IDC_SLIDER2, 0,100, k);
    doe_printf(hDlg,IDC_STATIC2,"Linearni %f",p_light->k2);
    
    k = ftoi(p_light->k3*100000.0f);
    doe_slider_nastav(hDlg,IDC_SLIDER6, 0,100, k);
    k = ftoi(p_light->k3*10000000.0f)%10;
    doe_slider_nastav(hDlg,IDC_SLIDER30,0,100, k);
    doe_printf(hDlg,IDC_STATIC3,"Kvadraticky %f",p_light->k3);
  }
}

static void doe_vloz_stat_svetla_combo(HWND hDlg, int lightakt)
{  
  byte pom[100];
  HWND hwnd = GetDlgItem( hDlg, IDC_COMBO1 );
  int  i,id,
       idv = -1,
       scena = p_cnf->cnf.svetla_sceny;
//       flag;

  SendMessage( hwnd, CB_RESETCONTENT, 0, 0 );
  for(i = 0; i < MAX_EDIT_SVETEL; i++) {
    if(p_cnf->light[i].akt) {
      //flag = p_cnf->light[i].flag2&SL2_MESH;
      //if((!scena && flag) || (scena && !flag)) {
        sprintf(pom,"%s = {%.2f,%.2f,%.2f} I = %.2f",p_cnf->light[i].jmeno,p_cnf->light[i].p.x,p_cnf->light[i].p.y,p_cnf->light[i].p.z);
        id = SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) pom);
        SendMessage( hwnd, CB_SETITEMDATA, id, i);
        if(i == lightakt)
          idv = id;
      //}
    }                
  }    
  if(idv < 0)
    idv = 0;
  SendMessage( hwnd, CB_SETCURSEL, idv, 0 );
}

static void doe_vloz_flare(HWND hDlg)
{
  byte pom[100];
  HWND hwnd = GetDlgItem( hDlg, IDC_COMBO4 );
  int i;
  
  SendMessage( hwnd, CB_RESETCONTENT, 0, 0 );
  for(i = 0; i < MAX_EDIT_SVETEL; i++) {
    if(p_cnf->flare[i].akt)
      sprintf(pom,"Flare %d {%.2f,%.2f,%.2f}",i,p_cnf->flare[i].p.x,p_cnf->flare[i].p.y,p_cnf->flare[i].p.z);
    else
      strcpy(pom,"-");
    SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) pom);
  }
  SendMessage( hwnd, CB_SETCURSEL, p_cnf->flareakt, 0 );
}

BOOL CALLBACK doe_calback_konfigurace_svetla(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {        
       case IDC_COMBO1: // zmena vyberu svetla         
         if(HIWORD(wParam) ==  CBN_SELCHANGE) {
           HWND hCombo1 = GetDlgItem(hDlg,IDC_COMBO1);
           p_cnf->lightakt = SendMessage( hCombo1, CB_GETCURSEL, 0, 0 );
           p_cnf->lightakt = SendMessage( hCombo1, CB_GETITEMDATA, p_cnf->lightakt, 0 );
           doe_vloz_stat_svetla(hDlg,p_cnf->lightakt,FALSE);
         }
         break;
       case IDC_COMBO4: //zmena vyberu flare
         if(HIWORD(wParam) ==  CBN_SELCHANGE) {
           HWND hCombo1 = GetDlgItem(hDlg,IDC_COMBO4);
           p_cnf->flareakt = SendMessage( hCombo1, CB_GETCURSEL, 0, 0 );
         }
         break;
       case IDC_CHECK1:
         p_cnf->cnf.flare = IsDlgButtonChecked( hDlg, IDC_CHECK1);
         break;
       // svetla
       case IDC_BUTTON3: // nove
          oe_prikaz_pridej_staticke_svetlo(p_cnf);
          if(p_cnf->cnf.svetla_dynamicke) {            
            ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
          }
          doe_vloz_stat_svetla(hDlg,p_cnf->lightakt,TRUE);
          break;
        case IDC_BUTTON1: {// upravit
          HWND hwnd = GetDlgItem( hDlg, IDC_COMBO1 );          
          oe_uprav_staticke_svetlo(p_cnf, p_cnf->light+p_cnf->lightakt);
          if(p_cnf->cnf.svetla_dynamicke) {          
            ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
          }
          doe_vloz_stat_svetla(hDlg,p_cnf->lightakt,TRUE);
          break;
        }
        case IDC_BUTTON2: // smazat
          oe_smaz_staticke_bodove_svetlo(p_cnf, p_cnf->light+p_cnf->lightakt);
          p_cnf->lightakt = lo_najdi_plne_stat_svetlo(p_cnf->light,MAX_EDIT_SVETEL);
          if(p_cnf->lightakt == K_CHYBA)
            p_cnf->lightakt = 0;
          if(p_cnf->cnf.svetla_dynamicke) {
            oe_prepocitat_stat_svetla(p_cnf);
            ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
          }
          doe_vloz_stat_svetla(hDlg,p_cnf->lightakt,TRUE);
          break;
          // flary
        case IDC_BUTTON10: // nove
          oe_prikaz_pridej_flare(p_cnf);
          doe_vloz_flare(hDlg);
          break;
        case IDC_BUTTON7: {// upravit
          HWND hwnd = GetDlgItem( hDlg, IDC_COMBO4 );
          p_cnf->flareakt = SendMessage( hwnd, CB_GETCURSEL, 0, 0 );
          oe_uprav_flare(p_cnf, p_cnf->flare+p_cnf->flareakt);
          doe_vloz_flare(hDlg);
          break;
        }
        case IDC_BUTTON8:
          oe_smaz_flare(p_cnf,p_cnf->flare+p_cnf->flareakt);
          doe_vloz_flare(hDlg);
          break;
        case IDC_BUTTON5: // refresh
          doe_vloz_flare(hDlg);
          break;
        case IDC_EDIT2: {
          byte  pom[50];
          float barvy[4];
          GetDlgItemText(hDlg,IDC_EDIT2,pom,99);
          sscanf(pom,"%x",&p_cnf->barva_pozadi);
          rgb_float(p_cnf->barva_pozadi,barvy);
          glClearColor(barvy[0],barvy[1],barvy[2],1.0f);
          break;
        }        
        case IDC_BUTTON14: {// barva pozadi
          dword barva;
          byte  pom[50];
          float barvy[4];
          GetDlgItemText(hDlg,IDC_EDIT2,pom,49);
          sscanf(pom,"%x",&barva);
          if(doe_prikaz_vyber_barvu(p_cnf,hDlg,&barva)) {
            SetDlgItemText( hDlg, IDC_EDIT2, itoa(barva,pom,16));
            p_cnf->barva_pozadi = barva;
            rgb_float(p_cnf->barva_pozadi,barvy);
            glClearColor(barvy[0],barvy[1],barvy[2],1.0f);
          }
          break;
        }
        // ray-tracer
        case IDC_BUTTON15: { // barva ambient
          dword barva;
          byte  pom[50];
          GetDlgItemText(hDlg,IDC_EDIT5,pom,49);
          sscanf(pom,"%x",&barva);
          if(doe_prikaz_vyber_barvu(p_cnf,hDlg,&barva)) {
            SetDlgItemText( hDlg, IDC_EDIT5, itoa(barva,pom,16));
            p_cnf->ray.ambient = barva;
          }
          break;
        }
        case IDC_CHECK3: // on/off dynamicke stinovani
          p_cnf->cnf.svetla_dynamicke = IsDlgButtonChecked(hDlg,IDC_CHECK3);
          if(p_cnf->cnf.svetla_dynamicke) {
            oe_prikaz_kont_norm_vect_all(p_cnf);
            oe_prepocitat_stat_svetla(p_cnf);
            ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
          } else {
            ray_zrus_staticke_osvetleni_vertexy(p_cnf);
          }
          break;
        case IDC_CHECK5:
          p_cnf->cnf.svetla_bez_gourandu = !IsDlgButtonChecked(hDlg,IDC_CHECK5);
          if(p_cnf->cnf.svetla_dynamicke)
            ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
          break;
        case IDC_CHECK2:
          p_cnf->ray.disk = IsDlgButtonChecked(hDlg,IDC_CHECK2);
          break;
        case IDC_CHECK4:
          p_cnf->cnf.svetla_kreslit = IsDlgButtonChecked(hDlg,IDC_CHECK4);
          break;
        case IDC_CHECK7:
          p_cnf->light[p_cnf->lightakt].bez_prev = IsDlgButtonChecked(hDlg,IDC_CHECK7);
          break;
        case IDC_EDIT5: {
          byte  pom[50];
          GetDlgItemText(hDlg,IDC_EDIT5,pom,99);
          sscanf(pom,"%x",&p_cnf->ray.ambient);
          break;
        }        
        case IDC_EDIT1: {
          byte  pom[50];
          GetDlgItemText(hDlg,IDC_EDIT1,pom,99);
          p_cnf->ray.faktor = (float)atof(pom);
          break;
        }        
        case IDC_BUTTON6: {
          ray_prepocitej_staticke_osvetleni(p_cnf,FALSE);
          break;
        }
        case IDC_CHECK6:           
          p_cnf->cnf.light_maps = IsDlgButtonChecked(hDlg,IDC_CHECK6);
          break;
        case IDC_RADIO2:
          p_cnf->cnf.svetla_sceny = IsDlgButtonChecked(hDlg,IDC_RADIO2);
          p_cnf->lightakt = lo_najdi_plne_stat_svetlo(p_cnf->light, MAX_EDIT_SVETEL);
          if(p_cnf->lightakt == K_CHYBA)
            p_cnf->lightakt = 0;
          doe_vloz_stat_svetla(hDlg,p_cnf->lightakt,TRUE);
          break;
        case IDC_RADIO3:
          p_cnf->cnf.svetla_sceny = !IsDlgButtonChecked(hDlg,IDC_RADIO3);
          p_cnf->lightakt = lo_najdi_plne_stat_svetlo(p_cnf->light, MAX_EDIT_SVETEL);
          if(p_cnf->lightakt == K_CHYBA)
            p_cnf->lightakt = 0;
          doe_vloz_stat_svetla(hDlg,p_cnf->lightakt,TRUE);
          break;
        case IDC_CHECK11:
          p_cnf->cnf.aktivni_pozadi = IsDlgButtonChecked(hDlg,IDC_CHECK11);
          break;
        default:
          break;
      }
      break;
    
    case WM_HSCROLL:
      {
        word pozice;
        
        if(LOWORD(wParam) == TB_THUMBPOSITION || LOWORD(wParam) == TB_THUMBTRACK) {
          pozice = HIWORD(wParam);
        } else  {
          pozice = (word)SendMessage((HWND)lParam,TBM_GETPOS,0,0);
        }                       

        if((HWND)lParam == GetDlgItem(hDlg,IDC_SLIDER4)) {
          p_cnf->light[p_cnf->lightakt].k1 = (float)pozice*0.1f;
          p_cnf->light[p_cnf->lightakt].k1 += (float)doe_slider_getpos(hDlg,IDC_SLIDER1)*0.01f;
          doe_printf(hDlg,IDC_STATIC1,"Konstantni %f",p_cnf->light[p_cnf->lightakt].k1);
          if(p_cnf->cnf.svetla_dynamicke)
            ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
        } else if((HWND)lParam == GetDlgItem(hDlg,IDC_SLIDER1)) {
          p_cnf->light[p_cnf->lightakt].k1 = (float)doe_slider_getpos(hDlg,IDC_SLIDER4)*0.1f;
          p_cnf->light[p_cnf->lightakt].k1 += (float)pozice*0.01f;
          doe_printf(hDlg,IDC_STATIC1,"Konstantni %f",p_cnf->light[p_cnf->lightakt].k1);
          if(p_cnf->cnf.svetla_dynamicke)
            ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
        } else if((HWND)lParam == GetDlgItem(hDlg,IDC_SLIDER5)) {
          p_cnf->light[p_cnf->lightakt].k2 = (float)pozice*0.001f;
          p_cnf->light[p_cnf->lightakt].k2 += (float)doe_slider_getpos(hDlg,IDC_SLIDER2)*0.0001f;
          doe_printf(hDlg,IDC_STATIC2,"Linearni %f",p_cnf->light[p_cnf->lightakt].k2);
          if(p_cnf->cnf.svetla_dynamicke)
            ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
        } else if((HWND)lParam == GetDlgItem(hDlg,IDC_SLIDER2)) {
          p_cnf->light[p_cnf->lightakt].k2 = (float)doe_slider_getpos(hDlg,IDC_SLIDER5)*0.001f;
          p_cnf->light[p_cnf->lightakt].k2 += (float)pozice*0.0001f;
          doe_printf(hDlg,IDC_STATIC2,"Linearni %f",p_cnf->light[p_cnf->lightakt].k2);        
          if(p_cnf->cnf.svetla_dynamicke)
            ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
        } else if((HWND)lParam == GetDlgItem(hDlg,IDC_SLIDER6)) {
          p_cnf->light[p_cnf->lightakt].k3 = (float)pozice*0.00001f;
          p_cnf->light[p_cnf->lightakt].k3 += (float)doe_slider_getpos(hDlg,IDC_SLIDER30)*0.000001f;
          doe_printf(hDlg,IDC_STATIC3,"Kvadraticky: %f",p_cnf->light[p_cnf->lightakt].k3);
          if(p_cnf->cnf.svetla_dynamicke)
            ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
        } else if((HWND)lParam == GetDlgItem(hDlg,IDC_SLIDER30)) {
          p_cnf->light[p_cnf->lightakt].k3 = (float)doe_slider_getpos(hDlg,IDC_SLIDER6)*0.00001f;
          p_cnf->light[p_cnf->lightakt].k3 += (float)pozice*0.000001f;
          doe_printf(hDlg,IDC_STATIC3,"Kvadraticky: %f",p_cnf->light[p_cnf->lightakt].k3);
          if(p_cnf->cnf.svetla_dynamicke)
            ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
        }
        
        break;
      }
    case WM_INITDIALOG: {
      byte pom[100];
      int  shm;
      nastav_pozici_bl();    
      doe_vloz_stat_svetla(hDlg,p_cnf->lightakt,TRUE);
      doe_vloz_flare(hDlg);
      CheckDlgButton( hDlg, IDC_CHECK1, p_cnf->cnf.flare);
      
      // nove -> barva a zbytek
      SetDlgItemText(hDlg, IDC_EDIT2, itoa(p_cnf->barva_pozadi,pom,16));
      CheckDlgButton( hDlg, IDC_CHECK11, p_cnf->cnf.svetla);
      SetDlgItemText(hDlg, IDC_EDIT4, itoa(p_cnf->svetlost_sceny,pom,16));

      glGetIntegerv(GL_SHADE_MODEL,&shm);
      // gourand/flat            
      if(shm == GL_FLAT)
        CheckDlgButton(hDlg,IDC_RADIO1,TRUE);
      else
        CheckDlgButton(hDlg,IDC_RADIO2,TRUE);
      
      CheckDlgButton(hDlg,IDC_CHECK3,p_cnf->cnf.svetla_dynamicke);
      CheckDlgButton(hDlg,IDC_CHECK5,!p_cnf->cnf.svetla_bez_gourandu);
      CheckDlgButton(hDlg,IDC_CHECK2,p_cnf->ray.disk);

      sprintf(pom,"%.3f",p_cnf->ray.faktor);
      SetDlgItemText(hDlg,IDC_EDIT1, pom);      
      sprintf(pom,"%x",p_cnf->ray.ambient);
      SetDlgItemText(hDlg,IDC_EDIT5, pom);
      
      CheckDlgButton(hDlg,IDC_CHECK4,p_cnf->cnf.svetla_kreslit);
      CheckDlgButton(hDlg,IDC_CHECK6,p_cnf->cnf.light_maps);
      CheckDlgButton(hDlg,IDC_RADIO2, p_cnf->cnf.svetla_sceny);
      CheckDlgButton(hDlg,IDC_RADIO3,!p_cnf->cnf.svetla_sceny);            
      CheckDlgButton(hDlg,IDC_CHECK11,p_cnf->cnf.aktivni_pozadi);
      
      break;
    }
    case WM_DESTROY: {
      EndDialog( hDlg, FALSE );
      }
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

BOOL CALLBACK doe_calback_bocni_lista( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON1:
          break;
        case IDCANCEL:
  				EndDialog( hDlg, FALSE);
          break;
        case IDOK:
          EndDialog( hDlg, TRUE);
          break;
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {      
      TCITEM it;
      int i;
      bl.hwndTab = GetDlgItem(hDlg, IDC_TAB1);
      it.mask = TCIF_TEXT;
      for(i = 0; i < bl.polozek; i++) {
        it.pszText = bl.jmena[i]; 
        SendMessage(bl.hwndTab,TCM_INSERTITEM,i,(LPARAM) (const LPTCITEM)&it);
        bl.p_template[i] = load_dialog_resource(MAKEINTRESOURCE(bl.dlg[i]));
      }
      TabCtrl_AdjustRect(bl.hwndTab, FALSE, &bl.rc);
      bl.rc.top += 1;
      bl.rc.left += 1;
      TabCtrl_SetCurSel(bl.hwndTab,bl.aktivni);
      bl.hwndDlg = CreateDialogIndirect(hinst, bl.p_template[bl.aktivni], hDlg, bl.call[bl.aktivni]); 
      break;
    }
    case WM_NOTIFY: {
      NMHDR *p_nmhdr = (LPNMHDR) lParam;
      // zmen vyber - prepni na jinou kartu
      if(p_nmhdr->code == TCN_SELCHANGE) {
        int vybr = TabCtrl_GetCurSel(bl.hwndTab);
        if(bl.hwndTab != NULL) DestroyWindow(bl.hwndDlg); 
        bl.hwndDlg = CreateDialogIndirect(hinst, bl.p_template[vybr], hDlg, bl.call[vybr]); 
      } 
      break;
    }
    case WM_DESTROY: {
      EndDialog( hDlg, FALSE );
      break; 
    }
    default:
      return FALSE;
  }
  return DefWindowProc( hDlg, msg, wParam, lParam );
}

HWND doe_aktivace_bocni_listy_mesh_editor(HWND bocni_lista)
{
  memset(&bl,0,sizeof(bl));

  bl.p_sv = &cnf;
  bl.polozek = 13;

  strcpy(bl.jmena[0],"Objekty");
  strcpy(bl.jmena[1],"Material");
  strcpy(bl.jmena[2],"Svetla");
  strcpy(bl.jmena[3],"Efektory");
  strcpy(bl.jmena[4],"Obarvovac");
  strcpy(bl.jmena[5],"Mlha");
  strcpy(bl.jmena[6],"Kamery");
  strcpy(bl.jmena[7],"Animace");
  strcpy(bl.jmena[8],"Kosti");
  strcpy(bl.jmena[9],"TextAnim");
  strcpy(bl.jmena[10],"Chapadelnik");
  strcpy(bl.jmena[11],"Drtic");
  strcpy(bl.jmena[12],"K2");

  bl.dlg[0] = IDD_KONFIGURACE_OBJEKTY;
  bl.dlg[1] = IDD_KONFIGURACE_MATERIALY;
  bl.dlg[2] = IDD_KONFIGURACE_SVETLA;
  bl.dlg[3] = IDD_KONFIGURACE_SVETLA_DYN;
  bl.dlg[4] = IDD_KONFIGURACE_GOURAND;
  bl.dlg[5] = IDD_KONFIGURACE_MLHA;
  bl.dlg[6] = IDD_KONFIGURACE_KAMERY;
  bl.dlg[7] = IDD_KONFIGURACE_ANIMACE;
  bl.dlg[8] = IDD_KONFIGURACE_ANIMACE_JOINT;
  bl.dlg[9] = IDD_KONFIGURACE_ANIMACE_TEXTURY_EDITACE;
  bl.dlg[10] = IDD_KONFIGURACE_ANIMACE_TEXTURY;
  bl.dlg[11] = IDD_KONFIGURACE_ELIMINATOR;  
  bl.dlg[12] = IDD_KONFIGURACE_KAMERY_OMEZENI;
  
  
  bl.call[0] = doe_calback_konfigurace_objekty;
  bl.call[1] = doe_calback_konfigurace_materialy;
  bl.call[2] = doe_calback_konfigurace_svetla;
  bl.call[3] = doe_calback_konfigurace_svetla_dyn;
  bl.call[4] = doe_calback_konfigurace_gourand;
  bl.call[5] = doe_calback_konfigurace_mlha;
  bl.call[6] = doe_calback_konfigurace_kamery;
  bl.call[7] = doe_calback_konfigurace_animace;
  bl.call[8] = doe_calback_konfigurace_animace_joint;
  bl.call[9] = doe_calback_konfigurace_text_animace;
  bl.call[10] = doe_calback_konfigurace_text_animace_editace;
  bl.call[11] = doe_calback_konfigurace_eliminator;
  bl.call[12] = doe_calback_konfigurace_kamery_omezeni;
    
  bl.aktivni = 0;
  bl.t_dlg = IDC_TAB1;

  return(CreateDialog(hinst, MAKEINTRESOURCE(IDD_BOCNI_LISTA), bocni_lista, doe_calback_bocni_lista));
}


/*******************************************************************************
 Bocni lista - game editor
 *******************************************************************************
*/
void doe_bocni_lista_prvky_dopln_tridy(HWND hDlg)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  HWND hwnd = GetDlgItem( hDlg, IDC_LIST2);
  HWND hwnd1 = GetDlgItem( hDlg, IDC_LIST1);
  int  i,t,m;
    
  t = SendMessage(hwnd, LB_GETITEMDATA, SendMessage( hwnd, LB_GETCURSEL, 0,0),0);
  m = p_bnf->seznam_tridy_flagy[t];
  
  SendMessage(hwnd1, LB_RESETCONTENT, 0, 0);
  for(i = 0; i < m; i++) {
    SendMessage(hwnd1, LB_SETITEMDATA, SendMessage( hwnd1, LB_ADDSTRING, 0,(LPARAM)p_bnf->seznam_podtridy[t][i]),i);
  }
  p_cnf->bnf.kp.vybr_podtrida = 0;
  SendMessage(hwnd1, LB_SETCURSEL, p_cnf->bnf.kp.vybr_podtrida, 0);
}

void doe_bocni_lista_prvky_dopln_varianty(HWND hDlg)
{
  LISTA_PRVKU *p_lista = &p_cnf->lp;
  B_KONFIG *p_bnf = &p_cnf->bnf;
  HWND hwnd_class = GetDlgItem( hDlg, IDC_LIST2); // list podtrid
  HWND hwnd_subclass = GetDlgItem( hDlg, IDC_LIST1); // list variant
  HWND hwnd_var = GetDlgItem( hDlg, IDC_LIST8); // list podtrid
  int  i,trida,subtrida,ki,snum;
  byte pom[500];

  trida = SendMessage(hwnd_class, LB_GETITEMDATA, SendMessage( hwnd_class, LB_GETCURSEL, 0,0),0);
  subtrida = SendMessage(hwnd_subclass, LB_GETITEMDATA, SendMessage( hwnd_subclass, LB_GETCURSEL, 0,0),0);
  
  SendMessage(hwnd_var, LB_RESETCONTENT, 0, 0);

  // prepocitam seznam
  snum = 0;
  for(i = 0; i < MAX_PRVKU_DATABAZE; i++) {
    if(!p_bnf->p_prv_dat[i])
      continue;
    ki = p_bnf->p_prv_dat[i]->kof_index;
    if(p_bnf->p_kofola_prv[ki].Class == trida && 
       p_bnf->p_kofola_prv[ki].SubClass == subtrida) {    
       snum++;
    }
  }

  doe_lista_zrus();

  if(!snum)
    return;
  
  doe_lista_vyrob(snum);
 
  snum = 0;
  for(i = 0; i < MAX_PRVKU_DATABAZE; i++) {
    if(!p_bnf->p_prv_dat[i])
      continue;
    ki = p_bnf->p_prv_dat[i]->kof_index;
    if(p_bnf->p_kofola_prv[ki].Class == trida && 
       p_bnf->p_kofola_prv[ki].SubClass == subtrida) {      
       sprintf(pom,"%d %s",p_bnf->p_kofola_prv[ki].GUID,p_bnf->p_kofola_prv[ki].Name);
       SendMessage(hwnd_var, LB_SETITEMDATA, SendMessage( hwnd_var, LB_ADDSTRING, 0,(LPARAM)pom),i);
       p_lista->p_pos[snum] = snum;
       p_lista->p_prv[snum] = i;
       snum++;
    }
  }
  p_cnf->bnf.kp.vybr_varianta = 0;
  SendMessage(hwnd_var, LB_SETCURSEL, p_cnf->bnf.kp.vybr_varianta, 0);
  p_bnf->kp.akt_prvek = SendMessage(hwnd_var,LB_GETITEMDATA,SendMessage(hwnd_var,LB_GETCURSEL,0,0),0);
}

void doe_bocni_lista_prvky_vyber_variantu(HWND hDlg)
{  
  LISTA_PRVKU *p_lista = &p_cnf->lp;  
  B_KONFIG    *p_bnf = &p_cnf->bnf;
  HWND combo = GetDlgItem( hDlg, IDC_LIST8);
  int  vyber = SendMessage(combo,LB_GETCURSEL,0,0);
  
  p_cnf->bnf.kp.vybr_varianta = vyber;
  p_cnf->bnf.kp.akt_prvek = SendMessage(combo,LB_GETITEMDATA,vyber,0);

  oe_normalizuj_listu(p_lista,vyber);
}

BOOL CALLBACK doe_calback_bocni_lista_prvky(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_LIST2: // list trid
          if(HIWORD(wParam) == LBN_SELCHANGE || HIWORD(wParam) == LBN_DBLCLK) {
            p_cnf->bnf.kp.vybr_trida = SendMessage((HWND)lParam,LB_GETCURSEL,0,0);
            doe_bocni_lista_prvky_dopln_tridy(hDlg);
            doe_bocni_lista_prvky_dopln_varianty(hDlg);
          }
          break;
        case IDC_LIST1: // list podtrid
          if(HIWORD(wParam) == LBN_SELCHANGE || HIWORD(wParam) == LBN_DBLCLK) {
            p_cnf->bnf.kp.vybr_podtrida = SendMessage((HWND)lParam,LB_GETCURSEL,0,0);
            doe_bocni_lista_prvky_dopln_varianty(hDlg);
          }
          break;
        case IDC_LIST8: {
          if(HIWORD(wParam) == LBN_SELCHANGE || HIWORD(wParam) == LBN_DBLCLK) {
            doe_bocni_lista_prvky_vyber_variantu(hDlg);
          }
          break;
        }
        case IDC_EDIT1:
          {
            byte pom[200];
            GetDlgItemText(hDlg, IDC_EDIT1, pom, 200);
            be_prepni_na_patro(&p_cnf->bnf, atoi(pom));
            SendMessage(GetDlgItem( hDlg, IDC_SPIN1), UDM_SETPOS,0,
              (LPARAM)MAKELONG((short)p_cnf->bnf.cnf.vybrane_patro,0));
            break;
          }
        case IDC_CHECK5: // vyber patra
          p_cnf->bnf.cnf.vybrane_patro = IsDlgButtonChecked( hDlg, IDC_CHECK5);
          if(p_cnf->bnf.cnf.vybrane_patro)
            be_prikaz_selekce_patra_on(p_cnf);
          else
            be_prikaz_selekce_patra_off(p_cnf);
          break;
        case IDC_CHECK1: // modre krabice
          p_cnf->bnf.cnf.kreslit_stinove_objekty = IsDlgButtonChecked( hDlg, IDC_CHECK1);            
          break;
        case IDC_CHECK2: // dynamicke prvky
          p_cnf->bnf.cnf.kreslit_dynamicke_objekty = IsDlgButtonChecked( hDlg, IDC_CHECK2);
          be_prepni_kresleni_dynam_objektu(p_cnf, p_cnf->bnf.cnf.kreslit_dynamicke_objekty);
          break;
        case IDC_CHECK3: // staticke prvky
          p_cnf->bnf.cnf.kreslit_staticke_objekty = IsDlgButtonChecked( hDlg, IDC_CHECK3);
          be_prepni_kresleni_stat_objektu(p_cnf, p_cnf->bnf.cnf.kreslit_staticke_objekty);
          break;
        case IDC_CHECK4:
          p_cnf->bnf.cnf.kostka = IsDlgButtonChecked( hDlg, IDC_CHECK4);
          break;
        case IDC_RADIO1:
          p_cnf->bnf.cnf.akt_vrstva = IsDlgButtonChecked( hDlg, IDC_RADIO1) ? 0 : 1;
          break;
        case IDC_RADIO2:
          p_cnf->bnf.cnf.akt_vrstva = IsDlgButtonChecked( hDlg, IDC_RADIO1) ? 0 : 1;
          break;
        default:
          return FALSE;
      }
      break;
      
    case WM_VSCROLL:
      {  
        int param = LOWORD(wParam);
        if(param == SB_THUMBPOSITION)
          be_prepni_na_patro(&p_cnf->bnf,(int)SendMessage((HWND)lParam,UDM_GETPOS32,0,0));
        break;
      }

    case WM_INITDIALOG: {
      B_KONFIG *p_bnf = &p_cnf->bnf;
      HWND hwnd = GetDlgItem( hDlg, IDC_LIST2);
      HWND hwnd1 = GetDlgItem( hDlg, IDC_LIST1);
      HWND hwnd2 = GetDlgItem( hDlg, IDC_LIST8);
      byte pom[200];
      int  i;

      nastav_pozici_bl();
      
      SendMessage(hwnd, LB_RESETCONTENT, 0, 0);

      for(i = 0; i < p_bnf->seznam_trid; i++) {
        SendMessage(hwnd, LB_SETITEMDATA, SendMessage( hwnd, LB_ADDSTRING, 0,(LPARAM)p_bnf->seznam_tridy_prvku[i]),i);
      }
      SendMessage(hwnd, LB_SETCURSEL, p_cnf->bnf.kp.vybr_trida, 0); //zvolim vybranou tridu

      SendMessage(GetDlgItem(hDlg,IDC_SPIN1), UDM_SETRANGE32, 0, p_cnf->bnf.y_num);
      SendMessage(GetDlgItem(hDlg,IDC_SPIN1), UDM_SETPOS32, 0, p_cnf->bnf.cnf.vybrane_patro);

      doe_bocni_lista_prvky_dopln_tridy(hDlg);
      doe_bocni_lista_prvky_dopln_varianty(hDlg);

      p_cnf->bnf.kp.hwnd_list_trida = hwnd;
      p_cnf->bnf.kp.hwnd_list_pod_trida = hwnd1;
      p_cnf->bnf.kp.hwnd_list_varianta = hwnd2;
      p_cnf->bnf.kp.hwnd_dialog = hDlg;
      p_cnf->bnf.kp.lista_aktivni = TRUE;      
      p_cnf->bnf.kp.akt_prvek = FALSE;
            
      CheckDlgButton(hDlg,IDC_CHECK1,(p_cnf->bnf.cnf.kreslit_stinove_objekty) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg,IDC_CHECK2,(p_cnf->bnf.cnf.kreslit_dynamicke_objekty) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg,IDC_CHECK3,(p_cnf->bnf.cnf.kreslit_staticke_objekty) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg,IDC_CHECK4,(p_cnf->bnf.cnf.kostka) ? BST_CHECKED : BST_UNCHECKED);

      doe_check_stlac(hDlg,IDC_RADIO1,!p_cnf->bnf.cnf.akt_vrstva);
      doe_check_stlac(hDlg,IDC_RADIO2, p_cnf->bnf.cnf.akt_vrstva);

      SetDlgItemText(hDlg, IDC_EDIT1, itoa(p_bnf->grid.akt_patro,pom,10));

      p_cnf->lp.hDlg = GetDlgItem(hDlg,IDC_LIST8);
      p_cnf->lp.hDlg_box = hDlg;
      p_cnf->lp.control = IDC_LIST8;

      CheckDlgButton(hDlg, IDC_CHECK5, (p_cnf->bnf.cnf.vybrane_patro) ? BST_CHECKED : BST_UNCHECKED);
      break;
    }
    case WM_DESTROY:
      p_cnf->lp.hDlg = FALSE;
      p_cnf->lp.hDlg_box = FALSE;
      p_cnf->bnf.kp.lista_aktivni = FALSE;
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_lista_zrus(void)
{
  LISTA_PRVKU *p_lista = &p_cnf->lp;
  null_free(&p_lista->p_prv);
  null_free(&p_lista->p_pos);
  p_lista->prvnum = 0;
  p_lista->prvakt = 0;
}

void doe_lista_vyrob(int pocet)
{
  LISTA_PRVKU *p_lista = &p_cnf->lp;
  
  doe_lista_zrus();  
  p_lista->p_prv = malloc(sizeof(p_lista->p_prv[0])*pocet);
  mtest(p_lista->p_prv);
  memset(p_lista->p_prv,0,sizeof(p_lista->p_prv[0])*pocet);
  p_lista->p_pos = malloc(sizeof(p_lista->p_pos[0])*pocet);
  mtest(p_lista->p_pos);
  memset(p_lista->p_pos,0,sizeof(p_lista->p_pos[0])*pocet);
  p_lista->prvnum = pocet;
  p_lista->prvni = 0;
  p_lista->prvakt = 0;
}

void doe_vloz_sady(HWND hDlg)
{
  B_KONFIG   *p_bnf = &p_cnf->bnf;  
  HWND combo = GetDlgItem( hDlg, IDC_COMBO1);
  int  i;
  
  for(i = 0; i < MAX_SAD; i++) {
    if(p_bnf->sad.jmena[i][0]) {
    SendMessage( combo, CB_SETITEMDATA,
                 SendMessage( combo, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)p_bnf->sad.jmena[i]),
                 i);
    }
  }
  SendMessage( combo, CB_SETCURSEL, p_bnf->sad.sada_akt, 0 );
}

void doe_vloz_akt_sadu(HWND hDlg)
{
  LISTA_PRVKU *p_lista = &p_cnf->lp;
  B_KONFIG    *p_bnf = &p_cnf->bnf;
  SADA_PRVKU  *p_sada;
  HWND combo = GetDlgItem( hDlg, IDC_LIST1);
  int  snum,sprv;
  int  i,j;

  p_sada = p_bnf->sad.p_sada[p_bnf->sad.sada_akt];
  snum = p_bnf->sad.sadnum[p_bnf->sad.sada_akt];
  sprv = p_bnf->sad.sada_prvek;

  doe_lista_zrus();
  if(snum) {
    p_lista->p_prv = malloc(sizeof(p_lista->p_prv[0])*snum);
    p_lista->p_pos = malloc(sizeof(p_lista->p_pos[0])*snum);
    mtest(p_lista->p_prv);
    for(i = 0, j = 0; i < snum; i++) {
      SendMessage( combo, LB_SETITEMDATA, 
                   SendMessage( combo, LB_ADDSTRING, 0,(LPARAM)(LPCTSTR)p_sada[i].jmeno),
                   (int)(p_sada+i));      
      p_lista->p_prv[j] = p_sada[i].prvek;
      p_lista->p_pos[j] = i;
      j++;      
    }
    p_lista->prvnum = snum;
    p_lista->prvni = sprv;
    p_lista->prvakt = 0;
    SendMessage( combo, LB_SETCURSEL, sprv, 0 );
  }
}

int doe_najdi_prvek(int guid)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int i;
  for(i = 0; i < MAX_PRVKU_DATABAZE; i++) {
    if(p_bnf->p_prv_dat[i]) {
      if(p_bnf->p_kofola_prv[p_bnf->p_prv_dat[i]->kof_index].GUID == guid)
        return(i);
    }
  }
  return(K_CHYBA);
}

void oe_normalizuj_listu(LISTA_PRVKU *p_lista, int vyber)
{
  int oken = p_lista->oken;
  int stred = oken>>1;
  int pr,ak;

  // Experimentalni vyber -> ak,pr
  ak = stred;
  pr = vyber-ak;

  if(p_lista->prvnum <= oken) {
    p_lista->prvni = 0;
    p_lista->prvakt = vyber;
  } else {    
    if(pr < 0) { // leva korekce
      ak = vyber;
      pr = 0;
    } else if(pr+oken > p_lista->prvnum) { // prava korekce
      pr = p_lista->prvnum-oken;
      ak = vyber-pr;
    } 
    p_lista->prvni = pr;
    p_lista->prvakt = ak;
  }
}

void doe_vloz_akt_sadu_vyber(HWND hDlg)
{  
  B_KONFIG    *p_bnf = &p_cnf->bnf;
  LISTA_PRVKU *p_lista = &p_cnf->lp;
  SADA_PRVKU  *p_sada;
  HWND combo = GetDlgItem( hDlg, IDC_LIST1); 
  int  vyber = SendMessage(combo,LB_GETCURSEL,0,0);  
  
  p_sada = (SADA_PRVKU *)SendMessage(combo,LB_GETITEMDATA,vyber,0);
  
  doe_printf(hDlg,IDC_EDIT1,"%d",p_sada->trida);
  doe_printf(hDlg,IDC_EDIT8,"%d",p_sada->podtrida);
  doe_printf(hDlg,IDC_EDIT9,"%d",p_sada->guid);

  p_bnf->kp.akt_prvek = p_sada->prvek;
  p_bnf->sad.sada_prvek = vyber;

  oe_normalizuj_listu(p_lista,vyber);
}

int nahr_checks[]={IDC_CHECK1,
                   IDC_CHECK2,
                   IDC_CHECK3,
                   IDC_CHECK4,
                   IDC_CHECK5,
                   IDC_CHECK25,
                   IDC_CHECK6,
                   IDC_CHECK8,
                   IDC_CHECK9};

int nahr_edits[]={ IDC_EDIT2,
                   IDC_EDIT3,
                   IDC_EDIT7,
                   IDC_EDIT15,
                   IDC_EDIT16,
                   IDC_EDIT17,
                   IDC_EDIT18,
                   IDC_EDIT19,
                   IDC_EDIT24};

void doe_prikaz_nahrazeni_prvku(K_EDITOR *p_cnf, HWND hDlg)
{ 
  int i;
  for(i = 0; i < sizeof(nahr_checks)/sizeof(nahr_checks[0]); i++) {  
    if(IsDlgButtonChecked(hDlg,nahr_checks[i])) {
      be_prikaz_nahrad_prvek(p_cnf,doe_vyber_kolonku_int(hDlg,nahr_edits[i]));
    }
  }
}

void doe_prikaz_nahrazeni_prvku_disk(K_EDITOR *p_cnf, HWND hDlg)
{
  struct _finddata_t data;
  long handle;
  byte file[MAX_JMENO];
  byte level_dir[200];
  byte maska[200];

  
  GetDlgItemText(hDlg,IDC_EDIT4,maska,199);
  

  if(ddwquetot(hwnd_aplikace,"Odsud neni cesty zpet!!! Zalohuj vsechny levely!!! Blizi se posledni soud!!!","Diskove nahrazeni? Opravdu? Provede nahazeni ve vsech levelech na disku!")) {
    strcpy(level_dir,p_cnf->dir.edit_level_dir);
    chdir(level_dir);
    if((handle = _findfirst(maska,&data)) == -1) {
      kprintf(TRUE,"Nenalezen zadny level odpovidajici masce '%s'",maska);
      return;
    }

    p_cnf->cnf.render = FALSE;

    do {
      if(data.attrib&_A_SUBDIR) {        
        strcpy(file,data.name);
        if((file[0] == '.' && file[1] == 0) ||
           (file[0] == '.' && file[2] == 0))
           continue;

        chdir(level_dir);
        chdir(file);

        strcat(file,".lv6");
        kprintf(TRUE,"Nahrazuji v levelu %s",file);
        
        lo_vymaz_materialy(p_cnf->p_mat,MAX_EDIT_MATERIALU,p_cnf->p_text,MAX_EDIT_TEXTUR);
        if(be_nahraj_level(p_cnf,file,FALSE,FALSE,FALSE)) {
          doe_prikaz_nahrazeni_prvku(p_cnf,hDlg);
          be_prikaz_uloz_level(p_cnf);
        }
        be_prikaz_smaz_level(p_cnf, TRUE, TRUE, TRUE);
      }
    } while(!_findnext(handle,&data));
    _findclose(handle);
  
    p_cnf->cnf.render = TRUE;
  }  

}

void doe_prikaz_prepsani_guid(K_EDITOR *p_cnf, HWND hDlg)
{   
  be_prikaz_prepis_guid(p_cnf, 
                        doe_vyber_kolonku_int(hDlg,IDC_EDIT1),
                        doe_vyber_kolonku_int(hDlg,IDC_EDIT8),
                        doe_vyber_kolonku_int(hDlg,IDC_EDIT9));
}

void doe_prikaz_prepsani_guid_disk(K_EDITOR *p_cnf, HWND hDlg)
{
  struct _finddata_t data;
  long handle;
  byte file[MAX_JMENO];
  byte level_dir[200];
  byte maska[200];

  
  GetDlgItemText(hDlg,IDC_EDIT5,maska,199);

  if(ddwquetot(hwnd_aplikace,"Odsud neni cesty zpet!!! Zalohuj vsechny levely!!! Blizi se posledni soud!!!","Diskove nahrazeni? Opravdu? Provede prepsani guid ve vsech levelech na disku!")) {
    strcpy(level_dir,p_cnf->dir.edit_level_dir);
    chdir(level_dir);
    if((handle = _findfirst(maska,&data)) == -1) {
      kprintf(TRUE,"Nenalezen zadny level odpovidajici masce '%s'",maska);
      return;
    }
    p_cnf->cnf.render = FALSE;
    do {
      if(data.attrib&_A_SUBDIR) {
        strcpy(file,data.name);
        if((file[0] == '.' && file[1] == 0) ||
           (file[0] == '.' && file[2] == 0))
           continue;

        chdir(level_dir);
        chdir(file);

        strcat(file,".lv6");
        kprintf(TRUE,"Provadim upravu...",file);
        
        lo_vymaz_materialy(p_cnf->p_mat,MAX_EDIT_MATERIALU,p_cnf->p_text,MAX_EDIT_TEXTUR);
        if(be_nahraj_level(p_cnf,file,FALSE,FALSE,FALSE)) {
          doe_prikaz_prepsani_guid(p_cnf,hDlg);
          /*
          //int k = lo_najdi_kontejner(p_cnf->p_kont,KONT_NUM,"________zima_podlaha.b2m");
          //if(k != K_CHYBA)
            //p_cnf->p_kont[k]->kflag |= KONT_BRUTALNORMAL;
          int i;
          for(i = 0; i < MAX_EDIT_SVETEL; i++) {
            if(p_cnf->flare[i].akt) {
              p_cnf->flare[i].zflag |= FLR_NO_ZTEST;
            }
          }
          */
          be_prikaz_uloz_level(p_cnf);
        }
        be_prikaz_smaz_level(p_cnf, TRUE, TRUE, TRUE);
      }
    } while(!_findnext(handle,&data));
    _findclose(handle);
    p_cnf->cnf.render = TRUE;
  }
}

void doe_prikaz_roubovac(K_EDITOR *p_cnf, HWND hDlg)
{
  struct _finddata_t data;
  long handle;
  byte file[MAX_JMENO];
  byte level_dir[200];
  byte maska[200];

  
  GetDlgItemText(hDlg,IDC_EDIT25,maska,199);

  if(ddwquetot(hwnd_aplikace,"Odsud neni cesty zpet!!! Zalohuj vsechny levely!!! Blizi se posledni soud!!!","Diskove nahrazeni? Opravdu? Provede roubovani obruby ve vsech levelech na disku!")) {
    strcpy(level_dir,p_cnf->dir.edit_level_dir);
    chdir(level_dir);
    if((handle = _findfirst(maska,&data)) == -1) {
      kprintf(TRUE,"Nenalezen zadny level odpovidajici masce '%s'",maska);
      return;
    }
    p_cnf->cnf.render = FALSE;
    do {
      if(data.attrib&_A_SUBDIR) {
        strcpy(file,data.name);
        if((file[0] == '.' && file[1] == 0) ||
           (file[0] == '.' && file[2] == 0))
           continue;

        chdir(level_dir);
        chdir(file);

        strcat(file,".lv6");
        kprintf(TRUE,"Roubuji obrubu v levelu %s",file);
        
        lo_vymaz_materialy(p_cnf->p_mat,MAX_EDIT_MATERIALU,p_cnf->p_text,MAX_EDIT_TEXTUR);
        if(be_nahraj_level(p_cnf,file,FALSE,FALSE,FALSE) && be_najdi_obrubu(p_cnf) != K_CHYBA) {
          be_prikaz_vyrob_obrubu_levelu(p_cnf,be_najdi_obruba_material(p_cnf),TRUE);
          be_prikaz_uloz_level(p_cnf);
        }
        be_prikaz_smaz_level(p_cnf, TRUE, TRUE, TRUE);
      }
    } while(!_findnext(handle,&data));
    _findclose(handle);
    p_cnf->cnf.render = TRUE;
  }
}

void doe_prikaz_rezac(K_EDITOR *p_cnf, HWND hDlg)
{
  struct _finddata_t data;
  long handle;
  byte file[MAX_JMENO];
  byte level_dir[200];
  byte maska[200];

  
  GetDlgItemText(hDlg,IDC_EDIT26,maska,199);

  if(ddwquetot(hwnd_aplikace,"Odsud neni cesty zpet!!! Zalohuj vsechny levely!!! Blizi se posledni soud!!!","Diskove nahrazeni? Opravdu? Provede vyrezani ve vsech levelech na disku!")) {
    strcpy(level_dir,p_cnf->dir.edit_level_dir);
    chdir(level_dir);
    if((handle = _findfirst(maska,&data)) == -1) {
      kprintf(TRUE,"Nenalezen zadny level odpovidajici masce '%s'",maska);
      return;
    }
    p_cnf->cnf.render = FALSE;
    do {
      if(data.attrib&_A_SUBDIR) {
        strcpy(file,data.name);
        if((file[0] == '.' && file[1] == 0) ||
           (file[0] == '.' && file[2] == 0))
           continue;

        chdir(level_dir);
        chdir(file);

        strcat(file,".lv6");
        kprintf(TRUE,"Rezu v levelu %s",file);
        
        lo_vymaz_materialy(p_cnf->p_mat,MAX_EDIT_MATERIALU,p_cnf->p_text,MAX_EDIT_TEXTUR);
        if(be_nahraj_level(p_cnf,file,FALSE,FALSE,FALSE)) {
          kprintf(TRUE,"Rezu level...");
          be_prikaz_vyrezani_levelu(p_cnf);
          be_prikaz_uloz_level(p_cnf);
        }
        be_prikaz_smaz_level(p_cnf, TRUE, TRUE, TRUE);
      }
    } while(!_findnext(handle,&data));
    _findclose(handle);
    p_cnf->cnf.render = TRUE;
  }
}


void doe_prikaz_ambient_load(K_EDITOR *p_cnf, HWND hDlg)
{
  struct _finddata_t data;
  long handle;
  byte file[MAX_JMENO];
  byte level_dir[200];
  byte maska[200];
  FILE *f;

  
  GetDlgItemText(hDlg,IDC_EDIT44,maska,199);

  if(ddwquetot(hwnd_aplikace,"Odsud neni cesty zpet!!! Zalohuj vsechny levely!!! Blizi se posledni soud!!!","Diskove nahrazeni? Opravdu? Provede vyrezani ve vsech levelech na disku!")) {
    strcpy(level_dir,p_cnf->dir.edit_level_dir);
    chdir(level_dir);
    if((handle = _findfirst(maska,&data)) == -1) {
      kprintf(TRUE,"Nenalezen zadny level odpovidajici masce '%s'",maska);
      return;
    }

    f = fopen("c:\\ambient.txt","w");
    if(!f) {
      kprintf(TRUE,"chyba open c:\\ambient.txt");
      return;
    }

    p_cnf->cnf.render = FALSE;
    do {
      if(data.attrib&_A_SUBDIR) {
        strcpy(file,data.name);
        if((file[0] == '.' && file[1] == 0) ||
           (file[0] == '.' && file[2] == 0))
           continue;

        chdir(level_dir);
        chdir(file);

        strcat(file,".lv6");
        kprintf(TRUE,"ambientnik levelu %s",file);
        
        lo_vymaz_materialy(p_cnf->p_mat,MAX_EDIT_MATERIALU,p_cnf->p_text,MAX_EDIT_TEXTUR);
        if(be_nahraj_level(p_cnf,file,FALSE,FALSE,FALSE)) {
          fprintf(f,"%s\n%x\n",file,p_cnf->ray.ambient);
        }        
        be_prikaz_smaz_level(p_cnf, TRUE, TRUE, TRUE);
      }
    } while(!_findnext(handle,&data));
    _findclose(handle);
    fclose(f);
    p_cnf->cnf.render = TRUE;
  }
}

void doe_prikaz_ambient_save(K_EDITOR *p_cnf, HWND hDlg)
{
  dword ambient;
  byte  file[200];
  byte  level_dir[200];
  byte  maska[200];
  FILE *f,*fd;

  
  GetDlgItemText(hDlg,IDC_EDIT44,maska,199);

  if(ddwquetot(hwnd_aplikace,"Odsud neni cesty zpet!!! Zalohuj vsechny levely!!! Blizi se posledni soud!!!","Diskove nahrazeni? Opravdu?")) {
    strcpy(level_dir,p_cnf->dir.edit_level_dir);
    chdir(level_dir);

    kprintf(TRUE,"Seznam levelu se spatnym ambientem:");
    f = fopen("c:\\ambient.txt","r");
    if(!f) {
      kprintf(TRUE,"chyba open c:\\ambient.txt");
      return;
    }
    fd = fopen("c:\\ambient_diff.txt","a");
    if(!f) {
      kprintf(TRUE,"chyba open c:\\ambient_diff.txt");
      return;
    }

    p_cnf->cnf.render = FALSE;

    while(fgets(file,200,f)) {
      fgets_korekce(file);
      fscanf(f,"%x\n",&ambient);

      chdir(level_dir);
      zamen_koncovku(file,NULL);
      chdir(file);
      strcat(file,".lv6");

      lo_vymaz_materialy(p_cnf->p_mat,MAX_EDIT_MATERIALU,p_cnf->p_text,MAX_EDIT_TEXTUR);
      if(be_nahraj_level(p_cnf,file,FALSE,FALSE,FALSE)) {
        if(p_cnf->ray.ambient != ambient) {
          fprintf(fd,"%s\n",file);
          p_cnf->ray.ambient = ambient;
          be_prikaz_uloz_level(p_cnf);
        }        
      }
      be_prikaz_smaz_level(p_cnf, TRUE, TRUE, TRUE);
    }
    fclose(f);
    fclose(fd);
    p_cnf->cnf.render = TRUE;
  }
}

void doe_ini_nahrazeni_prvku(K_EDITOR *p_cnf, HWND hDlg)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int i;
  for(i = 0; i < sizeof(nahr_checks)/sizeof(nahr_checks[0]); i++) {  
    doe_check_stlac(hDlg, nahr_checks[i], p_bnf->nahr_akt[i]);
    doe_printf(hDlg,nahr_edits[i],"%d",p_bnf->nahr_guid[i]);
  }
  doe_printf(hDlg,IDC_EDIT4,"*.*");
}

void doe_ini_prepsani_guid(K_EDITOR *p_cnf, HWND hDlg)
{  
  doe_printf(hDlg,IDC_EDIT1,"0");
  doe_printf(hDlg,IDC_EDIT8,"0");
  doe_printf(hDlg,IDC_EDIT9,"0");
  doe_printf(hDlg,IDC_EDIT5,"*.*");
}

void doe_zrus_nahrazeni_prvku(K_EDITOR *p_cnf, HWND hDlg)
{  
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int i;
  for(i = 0; i < sizeof(nahr_checks)/sizeof(nahr_checks[0]); i++) {
    p_bnf->nahr_akt[i] = IsDlgButtonChecked(hDlg,nahr_checks[i]);
    p_bnf->nahr_guid[i] = doe_vyber_kolonku_int(hDlg,nahr_edits[i]);
  }
}

void doe_ini_roubovac(K_EDITOR *p_cnf, HWND hDlg)
{    
  doe_printf(hDlg,IDC_EDIT25,"*.*");
}

void doe_ini_rezac(K_EDITOR *p_cnf, HWND hDlg)
{    
  doe_printf(hDlg,IDC_EDIT26,"*.*");
}

void doe_ini_ambient(K_EDITOR *p_cnf, HWND hDlg)
{    
  doe_printf(hDlg,IDC_EDIT44,"*.*");
}

BOOL CALLBACK doe_calback_bocni_lista_levely(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON1:
          doe_prikaz_nahrazeni_prvku(p_cnf,hDlg);
          break;
        case IDC_BUTTON2:
          doe_prikaz_nahrazeni_prvku_disk(p_cnf,hDlg);
          break;
        case IDC_BUTTON4:
          doe_prikaz_prepsani_guid(p_cnf,hDlg);
          break;
        case IDC_BUTTON3:
          doe_prikaz_prepsani_guid_disk(p_cnf,hDlg);
          break;
        case IDC_BUTTON7:
          doe_prikaz_roubovac(p_cnf,hDlg);
          break;          
        case IDC_BUTTON8:
          doe_prikaz_rezac(p_cnf,hDlg);
          break;
        case IDC_BUTTON10:
          doe_prikaz_ambient_load(p_cnf,hDlg);
          break;
        case IDC_BUTTON11:
          doe_prikaz_ambient_save(p_cnf,hDlg);
          break;
        default:
          return FALSE;
      }
      break;
    case WM_INITDIALOG: 
      {        
        nastav_pozici_bl();
        doe_ini_nahrazeni_prvku(p_cnf,hDlg);
        doe_ini_prepsani_guid(p_cnf,hDlg);
        doe_ini_roubovac(p_cnf,hDlg);
        doe_ini_rezac(p_cnf,hDlg);
        doe_ini_ambient(p_cnf,hDlg);
        break;
      }
    case WM_DESTROY:
      {
        doe_zrus_nahrazeni_prvku(p_cnf,hDlg);
        break; 
      }
    default:
      return FALSE;
  }
  return TRUE;
}

// smetistator na prvky
BOOL CALLBACK doe_calback_bocni_lista_rezac(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON2:
          be_rozsir_level(&p_cnf->bnf, 0, 0, 0, 0, 1, 1);
          break;
        case IDC_BUTTON1:
          be_rozsir_level(&p_cnf->bnf, 0, 0, 0, 0, 1,-1);
          break;
        case IDC_BUTTON4:
          be_rozsir_level(&p_cnf->bnf, 0, 0, 0, 0,-1, 1);
          break;
        case IDC_BUTTON3:
          be_rozsir_level(&p_cnf->bnf, 0, 0, 0, 0,-1,-1);
          break;

        case IDC_BUTTON5:
          be_rozsir_level(&p_cnf->bnf, 1, 1, 0, 0, 0, 0);
          break;
        case IDC_BUTTON6:
          be_rozsir_level(&p_cnf->bnf, 1,-1, 0, 0, 0, 0);
          break;
        case IDC_BUTTON12:
          be_rozsir_level(&p_cnf->bnf,-1, 1, 0, 0, 0, 0);
          break;
        case IDC_BUTTON20:
          be_rozsir_level(&p_cnf->bnf,-1,-1, 0, 0, 0, 0);
          break;

        case IDC_BUTTON8:
          be_rozsir_level(&p_cnf->bnf, 0, 0, 1, 1, 0, 0);
          break;
        case IDC_BUTTON7:
          be_rozsir_level(&p_cnf->bnf, 0, 0, 1,-1, 0, 0);
          break;
        case IDC_BUTTON11:
          be_rozsir_level(&p_cnf->bnf, 0, 0,-1, 1, 0, 0);
          break;
        case IDC_BUTTON10:
          be_rozsir_level(&p_cnf->bnf, 0, 0,-1,-1, 0, 0);
          break;
        
        case IDC_EDIT10:
          p_cnf->bnf.x_start = doe_vyber_kolonku_float(hDlg,IDC_EDIT10);
          be_preumisti_level(&p_cnf->bnf);
          break;
        case IDC_EDIT11:
          p_cnf->bnf.y_start = doe_vyber_kolonku_float(hDlg,IDC_EDIT11);
          be_preumisti_level(&p_cnf->bnf);
          break;
        case IDC_EDIT12:
          p_cnf->bnf.z_start = doe_vyber_kolonku_float(hDlg,IDC_EDIT12);
          be_preumisti_level(&p_cnf->bnf);
          break;

        default:
          return FALSE;
      }
      break;
    case WM_INITDIALOG: 
      {
        B_KONFIG *p_bnf = &p_cnf->bnf;
        nastav_pozici_bl();
        doe_printf(hDlg,IDC_EDIT1,"%d",p_bnf->x_num);
        doe_printf(hDlg,IDC_EDIT8,"%d",p_bnf->y_num);
        doe_printf(hDlg,IDC_EDIT9,"%d",p_bnf->z_num);
        doe_printf(hDlg,IDC_EDIT10,"%.3f",p_bnf->x_start);
        doe_printf(hDlg,IDC_EDIT11,"%.3f",p_bnf->y_start);
        doe_printf(hDlg,IDC_EDIT12,"%.3f",p_bnf->z_start);
        break;
      }
    case WM_DESTROY:
      {
        B_KONFIG *p_bnf = &p_cnf->bnf;
        break; 
      }
    default:
      return FALSE;
  }
  return TRUE;
}

HWND doe_aktivace_bocni_listy_game_editor(HWND bocni_lista)
{
  memset(&bl,0,sizeof(bl));

  bl.p_sv = &cnf;
  bl.polozek = 3;

  strcpy(bl.jmena[0],"Vse");
  strcpy(bl.jmena[1],"Sady");
  strcpy(bl.jmena[2],"Rezacka");

  bl.dlg[0] = IDD_EDITORPRVKU_PRVKY;
  bl.dlg[1] = IDD_EDITORPRVKU_PRVKY_LOGIK;
  bl.dlg[2] = IDD_EDITORPRVKU_PRVKY_MODIFIKATOR;
  
  bl.call[0] = doe_calback_bocni_lista_prvky;
  bl.call[1] = doe_calback_bocni_lista_levely;
  bl.call[2] = doe_calback_bocni_lista_rezac;
  
  bl.aktivni = 0;
  bl.t_dlg = IDC_TAB1;

  return(CreateDialog(hinst, MAKEINTRESOURCE(IDD_BOCNI_LISTA), bocni_lista, doe_calback_bocni_lista));
}

void doe_prepni_do_mesh_editor(void)
{ 
  byte pom[100];

  if(bl.hwndTab != NULL) 
    DestroyWindow(bl.hwndDlg); 
  DestroyWindow(hwnd_bocni_lista_dialog);
  hwnd_bocni_lista_dialog = doe_aktivace_bocni_listy_mesh_editor(hwnd_bocni_lista);
  SetMenu(hwnd_aplikace,handle_menu[MENU_MESH_EDITOR]);
  sprintf(pom,"3D Mesh editor");
  SetWindowText(hwnd_aplikace,pom);
  SetFocus(hwnd_plocha);
  doe_zaskrkni_menu(p_cnf);
  akt_editor = 1;
}

void doe_prepni_do_game_editor(void)
{
  if(bl.hwndTab != NULL) 
    DestroyWindow(bl.hwndDlg); 
  DestroyWindow(hwnd_bocni_lista_dialog);
  hwnd_bocni_lista_dialog = doe_aktivace_bocni_listy_game_editor(hwnd_bocni_lista);  
  SetMenu(hwnd_aplikace,handle_menu[MENU_GAME_EDITOR]);
  be_level_to_window(&p_cnf->bnf); 
  SetFocus(hwnd_plocha);
  doe_zaskrkni_menu(p_cnf);
  akt_editor = 2;
}

/*******************************************************************************
 Editor Materialu
 *******************************************************************************
*/

static EDIT_MATERIAL tmm;

void vloz_mat_struct_text_flag(HWND hDlg, int cnt, int flag)
{
  HWND hCombo = GetDlgItem(hDlg,cnt);
  int  flags[] = {0,TEXT_GEN_DOT3,TEXT_GEN_DUDV},i;

  SendMessage(hCombo, CB_RESETCONTENT, 0, 0);
  SendMessage(hCombo, CB_SETITEMDATA, SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"COLOR"),0);
  SendMessage(hCombo, CB_SETITEMDATA, SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"DOT3"),TEXT_GEN_DOT3);
  SendMessage(hCombo, CB_SETITEMDATA, SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"DUDV"),TEXT_GEN_DUDV);
  
  for(i = 0; i < 3; i++) {
    if((TEXT_GEN&(flags[i])) == (TEXT_GEN&flag)) {
      SendMessage(hCombo, CB_SETCURSEL,(WPARAM)i,0);
      break;
    }
  }
}

int cti_text_flag(HWND hDlg, int cnt)
{
  HWND hCombo = GetDlgItem(hDlg,cnt);
  int  ret = SendMessage(hCombo, CB_GETITEMDATA, SendMessage(hCombo, CB_GETCURSEL,(WPARAM)0,0),0);  
  return(ret == CB_ERR ? 0 : ret);
}

// Vlozi do dialogu material
void vloz_mat_struct(HWND hDlg, EDIT_MATERIAL *p_tmm)
{
  HWND hCombo[5] = {GetDlgItem(hDlg,IDC_COMBO1),GetDlgItem(hDlg,IDC_COMBO4),
    GetDlgItem(hDlg,IDC_COMBO11),GetDlgItem(hDlg,IDC_COMBO6),
    GetDlgItem(hDlg,IDC_COMBO12)};
  HWND hCombo_t[4] = {GetDlgItem(hDlg,IDC_COMBO9),GetDlgItem(hDlg,IDC_COMBO14),
    GetDlgItem(hDlg,IDC_COMBO16),GetDlgItem(hDlg,IDC_COMBO13)};
  HWND hCombo_k[4] = {GetDlgItem(hDlg,IDC_COMBO18),GetDlgItem(hDlg,IDC_COMBO20),
    GetDlgItem(hDlg,IDC_COMBO22),GetDlgItem(hDlg,IDC_COMBO24)};
  int i,st;
    
  SetDlgItemText(hDlg, IDC_EDIT2, p_tmm->jmeno);
  
  SetDlgItemText(hDlg, IDC_EDIT1, p_tmm->textfile[0]);
  if(p_tmm->p_text[0]) {
    p_tmm->textflag[0] = p_tmm->p_text[0]->flag;
    vloz_mat_struct_text_flag(hDlg, IDC_COMBO3, p_tmm->textflag[0]);
  }
  
  SetDlgItemText(hDlg, IDC_EDIT8, p_tmm->textfile[1]);
  if(p_tmm->p_text[1]) {
    p_tmm->textflag[1] = p_tmm->p_text[1]->flag;
    vloz_mat_struct_text_flag(hDlg, IDC_COMBO26, p_tmm->textflag[1]);
  }
  
  SetDlgItemText(hDlg, IDC_EDIT9, p_tmm->textfile[2]);
  if(p_tmm->p_text[2]) {
    p_tmm->textflag[2] = p_tmm->p_text[2]->flag;
    vloz_mat_struct_text_flag(hDlg, IDC_COMBO28, p_tmm->textflag[2]);
  }
  
  SetDlgItemText(hDlg, IDC_EDIT10, p_tmm->textfile[3]);
  if(p_tmm->p_text[3]) {
    p_tmm->textflag[3] = p_tmm->p_text[3]->flag;
    vloz_mat_struct_text_flag(hDlg, IDC_COMBO30, p_tmm->textflag[3]);
  }
  
  SendMessage(hCombo[0],CB_SETCURSEL,(WPARAM)p_tmm->alfa_state,0);
  for(i = 0; i < 5; i++) {
    st = p_tmm->text_state[i].text_stage;
    SendMessage(hCombo[i+1],CB_SETCURSEL,(WPARAM)p_tmm->text_state[i].text_stage+1,0);
    SendMessage(hCombo_t[i],CB_SETCURSEL,(WPARAM)p_tmm->text_state[i].textury[0],0);
    SendMessage(hCombo_k[i],CB_SETCURSEL,(WPARAM)p_tmm->text_state[i].text_koord[0],0);
  }

  CheckDlgButton(hDlg,IDC_CHECK10,(p_tmm->flag&MAT_SYSTEM)   ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(hDlg,IDC_CHECK7, (p_tmm->flag&MAT_NO_ZMASK) ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(hDlg,IDC_CHECK9, (p_tmm->flag&MAT_NO_ZTEST) ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(hDlg,IDC_CHECK11,(p_tmm->flag&MAT_NO_CULL)  ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(hDlg,IDC_CHECK1, (p_tmm->flag&MAT_NO_BUMP)  ? BST_CHECKED : BST_UNCHECKED);
}

void text_korekce_materialu(EDIT_MATERIAL *p_tmm)
{
  if(p_tmm->flag2&MAT2_CALC_MAP1) p_tmm->flag2&=~MAT2_MAP1;
  if(p_tmm->flag2&MAT2_CALC_MAP2) p_tmm->flag2&=~MAT2_MAP2;
  if(p_tmm->flag2&MAT2_CALC_MAP3) p_tmm->flag2&=~MAT2_MAP3;
  if(p_tmm->flag2&MAT2_CALC_MAP4) p_tmm->flag2&=~MAT2_MAP4;
}

// vybere material z dialogu
void vyber_mat_struct(HWND hDlg, EDIT_MATERIAL *p_tmm)
{
  HWND hCombo[5] = {GetDlgItem(hDlg,IDC_COMBO1),GetDlgItem(hDlg,IDC_COMBO4),
    GetDlgItem(hDlg,IDC_COMBO11),GetDlgItem(hDlg,IDC_COMBO6),
    GetDlgItem(hDlg,IDC_COMBO12)};
  HWND hCombo_t[4] = {GetDlgItem(hDlg,IDC_COMBO9),GetDlgItem(hDlg,IDC_COMBO14),
    GetDlgItem(hDlg,IDC_COMBO16),GetDlgItem(hDlg,IDC_COMBO13)};
  HWND hCombo_k[4] = {GetDlgItem(hDlg,IDC_COMBO18),GetDlgItem(hDlg,IDC_COMBO20),
    GetDlgItem(hDlg,IDC_COMBO22),GetDlgItem(hDlg,IDC_COMBO24)};
  int i,st;
  
  GetDlgItemText(hDlg, IDC_EDIT2, p_tmm->jmeno, MAX_JMENO);
  GetDlgItemText(hDlg, IDC_EDIT1, p_tmm->textfile[0], MAX_JMENO);
  GetDlgItemText(hDlg, IDC_EDIT8, p_tmm->textfile[1], MAX_JMENO);
  GetDlgItemText(hDlg, IDC_EDIT9, p_tmm->textfile[2], MAX_JMENO);
  GetDlgItemText(hDlg, IDC_EDIT10,p_tmm->textfile[3], MAX_JMENO);

  p_tmm->textflag[0] = cti_text_flag(hDlg, IDC_COMBO3);
  if(p_tmm->p_text[0]) {
    p_tmm->p_text[0]->flag = p_tmm->textflag[0];
  }
  p_tmm->textflag[1] = cti_text_flag(hDlg, IDC_COMBO26);
  if(p_tmm->p_text[1]) {
    p_tmm->p_text[1]->flag = p_tmm->textflag[1];
  }
  p_tmm->textflag[2] = cti_text_flag(hDlg, IDC_COMBO28);
  if(p_tmm->p_text[2]) {
    p_tmm->p_text[2]->flag = p_tmm->textflag[2];
  }
  p_tmm->textflag[3] = cti_text_flag(hDlg, IDC_COMBO30);
  if(p_tmm->p_text[3]) {
    p_tmm->p_text[3]->flag = p_tmm->textflag[3];    
  }
  
  /* Nacti stage bloky
  */
  p_tmm->alfa_state  = SendMessage(hCombo[0],CB_GETCURSEL,(WPARAM)0,0);
  for(i = 0; i < MAT_TEXTUR; i++) {
    p_tmm->text_state[i].text_stage = st = SendMessage(hCombo[i+1],CB_GETITEMDATA,SendMessage(hCombo[i+1],CB_GETCURSEL,(WPARAM)0,0),0);
    if(text_stage_edit_blok[st].edit_funkce == K_CHYBA) {
      p_tmm->text_state[i].textury[0]    = SendMessage(hCombo_t[i],CB_GETCURSEL,(WPARAM)0,0);
      p_tmm->text_state[i].text_koord[0] = SendMessage(hCombo_k[i],CB_GETCURSEL,(WPARAM)0,0);
    }
  }    

  p_tmm->flag = IsDlgButtonChecked( hDlg, IDC_CHECK10)? (p_tmm->flag|MAT_SYSTEM)  :(p_tmm->flag&~MAT_SYSTEM);
  p_tmm->flag = IsDlgButtonChecked( hDlg, IDC_CHECK7) ? (p_tmm->flag|MAT_NO_ZMASK):(p_tmm->flag&~MAT_NO_ZMASK);
  p_tmm->flag = IsDlgButtonChecked( hDlg, IDC_CHECK9) ? (p_tmm->flag|MAT_NO_ZTEST):(p_tmm->flag&~MAT_NO_ZTEST);
  p_tmm->flag = IsDlgButtonChecked( hDlg, IDC_CHECK11)? (p_tmm->flag|MAT_NO_CULL) :(p_tmm->flag&~MAT_NO_CULL);
  p_tmm->flag = IsDlgButtonChecked( hDlg, IDC_CHECK1) ? (p_tmm->flag|MAT_NO_BUMP) :(p_tmm->flag&~MAT_NO_BUMP);
}

typedef void (* STAGE_MAT_FUNC)(EDIT_MATERIAL *p_mat, MATERIAL_TEXT *p_text);

STAGE_MAT_FUNC p_stage_func[] = {doe_editor_materialu_stage_dot3_color,
                                 doe_editor_materialu_stage_dot3_env,
                                 doe_editor_materialu_stage_dudv_env};

int vyber_volny_material(K_EDITOR *p_cnf)
{
  int i;

  for(i = 0; i < MAX_EDIT_MATERIALU; i++) {
    if(!p_cnf->p_mat[i])
      return(i);
  }
  return(K_CHYBA);
}

int pridej_texturu_mat(K_EDITOR *p_cnf, EDIT_MATERIAL *p_mat, int text, byte *p_text, int flag)
{  
  int t;

  if(!p_text[0])
    return(FALSE);

  if((t = lo_najdi_texturu(p_cnf->p_text,MAX_EDIT_TEXTUR,p_text,flag)) == K_CHYBA) {
    if((t = lo_najdi_volnou_texturu(p_cnf->p_text,MAX_EDIT_TEXTUR)) == K_CHYBA)
      chyba("Textury");
    strcpy(p_cnf->p_text[t].jmeno,p_text);
    p_cnf->p_text[t].flag = flag;
  }
    
  p_mat->textflag[text] = flag;
  p_mat->p_text[text] = p_cnf->p_text+t;
  strcpy(p_mat->textfile[text], p_text);
  
  return(TRUE);
}

/*
int pridej_texturu_mat_generovanou(K_EDITOR *p_cnf, EDIT_MATERIAL *p_mat, int stage)
{  
  MATERIAL_TEXT *p_stg = p_mat->text_state+stage; // texture bloky
  int t,i,tx,text;

  for(i = 0; i < MAT_TEXTUR; i++) {
    if((tx = p_stg->text_gen[i]) != K_CHYBA) {
      // tx - textura co se generuje

      if((t = lo_najdi_texturu(p_cnf->p_text,MAX_EDIT_TEXTUR,p_mat->textfile[tx],p_stg->text_gen_flagy[i])) == K_CHYBA) {
        if((t = lo_najdi_volnou_texturu(p_cnf->p_text,MAX_EDIT_TEXTUR)) == K_CHYBA)
          chyba("Textury");
        strcpy(p_cnf->p_text[t].jmeno,p_mat->textfile[tx]);
        p_cnf->p_text[t].flag |= p_stg->text_gen_flagy[i];
      }
      text = p_stg->textury[i];
      p_mat->p_text[text] = &p_cnf->p_text[t].text;
      p_mat->textnum[text] = t;
      strcpy(p_mat->textfile[text], p_mat->textfile[tx]);
    }
  }
  
  return(TRUE);
}
*/

int doe_nastav_pixelformat(HDC hDC)
{
  PIXELFORMATDESCRIPTOR pfd= {
    sizeof(PIXELFORMATDESCRIPTOR),		// Size Of This Pixel Format Descriptor
      1,								              // Version Number
      PFD_DRAW_TO_WINDOW |						// Format Must Support Window
      PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
      PFD_DOUBLEBUFFER,						// Must Support Double Buffering
      PFD_TYPE_RGBA,							// Request An RGBA Format
      0,							            // Select Our Color Depth
      0, 0, 0, 0, 0, 0,						// Color Bits Ignored
      0,								// No Alpha Buffer
      0,								// Shift Bit Ignored
      0,								// No Accumulation Buffer
      0, 0, 0, 0,				// Accumulation Bits Ignored
      16,								// 16Bit Z-Buffer (Depth Buffer)
      0,								// No Stencil Buffer
      0,								// No Auxiliary Buffer
      PFD_MAIN_PLANE,		// Main Drawing Layer
      0,								// Reserved
      0, 0, 0						// Layer Masks Ignored
  };
  GLuint	PixelFormat;						// Holds The Results After Searching For A Match
  
  pfd.cColorBits = 32; // vlozit pocet bitu per pixel
  if(!(PixelFormat=ChoosePixelFormat(hDC,&pfd))) {
    return(FALSE);
  }
  if(!SetPixelFormat(hDC,PixelFormat,&pfd)) {
    return(FALSE);
  }
  return(TRUE);
}

// zatim vytvori nove okno a do nej tp blitne z p_back
int zobraz_akt_material(HWND hDlg, EDIT_MATERIAL *p_tmm)
{
 HWND hwnd_mat;
 RECT src_rec;
 RECT dest_rec; 
 HDC hDC;
 int dx = 50, dy = 50;

 if(!p_tmm)
   return(FALSE);
 
 hwnd_mat = GetDlgItem(hDlg,IDC_STATIC_OKNO);
  
 if(!(hDC = GetDC(hwnd_mat)))
   chyba("GetHDC"); 

 doe_nastav_pixelformat(hDC);

 if(!wglMakeCurrent(hDC,hwconf.hRC))
   chyba("MCR");

 GetClientRect( hwnd_mat, &dest_rec);
 GetClientRect( hwnd_mat, &src_rec);
 ClientToScreen( hwnd_mat, (POINT*)&dest_rec.left );
 ClientToScreen( hwnd_mat, (POINT*)&dest_rec.right );
 
 dx = src_rec.right;
 dy = src_rec.bottom;

 set_matrix_view(0,0,dx,dy);
 set_matrix_2d(dx,dy);

 oe_nastav_material_single(NULL,p_tmm,NULL,NULL);

 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 if(p_cnf->cnf.barvy_z_materialu) {
   glColor4f(p_tmm->dxmat.diffuse_r,p_tmm->dxmat.diffuse_g,p_tmm->dxmat.diffuse_b,p_tmm->dxmat.diffuse_a);
   glSecondaryColor3fEXT(p_tmm->dxmat.specular_r,p_tmm->dxmat.specular_g,p_tmm->dxmat.specular_b);
 } else {
   glColor4f(1,1,1,1);
   glSecondaryColor3fEXT(1,1,1);
 }

 glBegin(GL_TRIANGLE_STRIP);      
   glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 1);
   glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 1);
   glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0, 1);      
   glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 1);
   glVertex3f(0.0f,0.0f,0.0f);      
   
   glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 0);
   glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 0);   
   glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0, 0);
   glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);      
   glVertex3f(0.0f,(float)dy,0.0f);     
   
   glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 1);
   glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1, 1);   
   glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 1, 1);
   glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 1, 1);      
   glVertex3f((float)dx,0.0f,0.0f);        
   
   glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 0);
   glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1, 0);
   glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 1, 0);
   glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 1, 0);
   glVertex3f((float)dx,(float)dy,0.0f);
 glEnd();  


 SwapBuffers(hDC); 

 oe_zrus_material_single(p_tmm);

 ret_matrix_2d();
 set_matrix_view(p_cnf->wx, p_cnf->wy, p_cnf->wxres, p_cnf->wyres);

 if(!wglMakeCurrent(hwconf.hDC, hwconf.hRC))
   chyba("WMC");
 if(!ReleaseDC(hwnd_mat,hDC))
   kprintf(1,"Chyba uvolneni hDC");

 return(0);
}

static int vybr_mat = 1;


static void doe_vloz_text_koord(HWND hDlg, int control, int stav)
{
  HWND hCombo = GetDlgItem(hDlg,control);
  byte pom[100];
  int  j;
  SendMessage(hCombo, CB_RESETCONTENT, 0, 0);  
  for(j = 0; j < MAT_TEXTUR; j++) {
    sprintf(pom,"Krd%d",j+1);
    SendMessage( hCombo, CB_ADDSTRING, 0, (LPARAM)pom);
  }
  SendMessage(hCombo,CB_SETCURSEL,(WPARAM)stav,0);
}

static void doe_init_stage_bloky(HWND hDlg)
{
 HWND hCombo[5] = {GetDlgItem(hDlg,IDC_COMBO1),GetDlgItem(hDlg,IDC_COMBO4),
                   GetDlgItem(hDlg,IDC_COMBO11),GetDlgItem(hDlg,IDC_COMBO6),
                   GetDlgItem(hDlg,IDC_COMBO12)};
 HWND hCombo_t[4] = {GetDlgItem(hDlg,IDC_COMBO9),GetDlgItem(hDlg,IDC_COMBO14),
                     GetDlgItem(hDlg,IDC_COMBO16),GetDlgItem(hDlg,IDC_COMBO13)};
 int  hCombo_k[4] = {IDC_COMBO18,IDC_COMBO20,IDC_COMBO22,IDC_COMBO24};
 byte pom[100];
 int  j,i;

 for(i = 0; i < 6; i++)
   SendMessage(hCombo[i], CB_RESETCONTENT, 0, 0);
 for(i = 0; i < 5; i++)
   SendMessage(hCombo_t[i], CB_RESETCONTENT, 0, 0);

 for(i = 0; i < STAGE_ALFA_FUNKCI; i++)
   SendMessage( hCombo[0], CB_ADDSTRING, 0,(LPARAM)alfa_stage_edit_blok[i].jmeno);
 
 for(j = 1; j < 6; j++)
   SendMessage(hCombo[j], CB_SETITEMDATA, SendMessage( hCombo[j], CB_ADDSTRING, 0,(LPARAM)"Nic"), K_CHYBA);
 for(i = 0; i < STAGE_TEXT_FUNKCI; i++) {
   for(j = 1; j < 6; j++)
     SendMessage(hCombo[j], CB_SETITEMDATA, SendMessage( hCombo[j], CB_ADDSTRING, 0,(LPARAM)text_stage_edit_blok[i].jmeno),i);
 }
 for(i = 0; i < 5; i++) {
   for(j = 0; j < MAT_TEXTUR; j++) {
     sprintf(pom,"Text%d",j+1);
     SendMessage( hCombo_t[i], CB_ADDSTRING, 0, (LPARAM)pom);
   }
 }
 for(i = 0; i < 5; i++) {
   doe_vloz_text_koord(hDlg, hCombo_k[i], i);
 }

 for(i = 0; i < 6; i++)
   SendMessage(hCombo[i],CB_SETCURSEL,(WPARAM)0,0);
 for(i = 0; i < 5; i++)
   SendMessage(hCombo_t[i],CB_SETCURSEL,(WPARAM)i,0);
}

void doe_get_stage_bloky_init_rada(HWND hDlg, int c_text, int c_koord, int button, int funkce, int stage)
{
  int stav = (funkce == -1);
  if(stage != K_CHYBA) {
    EnableDlgItem(hDlg, c_text, stav);
    EnableDlgItem(hDlg, c_koord,stav);
    EnableDlgItem(hDlg, button,!stav);
  } else {
    EnableDlgItem(hDlg, c_text, FALSE);
    EnableDlgItem(hDlg, c_koord,FALSE);
    EnableDlgItem(hDlg, button, FALSE);
  }
}

/* korekce pruhlednosti
*/
void doe_korekce_pruhlednosti(EDIT_MATERIAL *p_tmm)
{
 if(alfa_stage_edit_blok[p_tmm->alfa_state].alfa_pruhledny)
   p_tmm->flag|=MAT_PRUHLEDNY;
 else
   p_tmm->flag&=~MAT_PRUHLEDNY;
}

void doe_get_stage_bloky(HWND hDlg, EDIT_MATERIAL *p_tmm)
{
 HWND hCombo1 = GetDlgItem(hDlg,IDC_COMBO1);
 HWND hCombo2 = GetDlgItem(hDlg,IDC_COMBO4);
 HWND hCombo3 = GetDlgItem(hDlg,IDC_COMBO11);
 HWND hCombo4 = GetDlgItem(hDlg,IDC_COMBO6);
 HWND hCombo5 = GetDlgItem(hDlg,IDC_COMBO12);

 p_tmm->alfa_state  = SendMessage(hCombo1,CB_GETCURSEL,(WPARAM)0,0);
 doe_korekce_pruhlednosti(p_tmm);
 
 p_tmm->text_state[0].text_stage = SendMessage(hCombo2,CB_GETITEMDATA,
                                   SendMessage(hCombo2,CB_GETCURSEL,(WPARAM)0,0),0);
 doe_get_stage_bloky_init_rada(hDlg, IDC_COMBO9, IDC_COMBO18, IDC_BUTTON3,
                               text_stage_edit_blok[p_tmm->text_state[0].text_stage].edit_funkce,
                               p_tmm->text_state[0].text_stage);
 
 p_tmm->text_state[1].text_stage = SendMessage(hCombo3,CB_GETITEMDATA,
                                   SendMessage(hCombo3,CB_GETCURSEL,(WPARAM)0,0),0);
 doe_get_stage_bloky_init_rada(hDlg, IDC_COMBO14, IDC_COMBO20, IDC_BUTTON10,
                               text_stage_edit_blok[p_tmm->text_state[1].text_stage].edit_funkce, 
                               p_tmm->text_state[1].text_stage);
 
 p_tmm->text_state[2].text_stage = SendMessage(hCombo4,CB_GETITEMDATA,
                                   SendMessage(hCombo4,CB_GETCURSEL,(WPARAM)0,0),0);
 doe_get_stage_bloky_init_rada(hDlg, IDC_COMBO16, IDC_COMBO22, IDC_BUTTON4, 
                               text_stage_edit_blok[p_tmm->text_state[2].text_stage].edit_funkce,
                               p_tmm->text_state[2].text_stage);
 
 p_tmm->text_state[3].text_stage = SendMessage(hCombo5,CB_GETITEMDATA,
                                   SendMessage(hCombo5,CB_GETCURSEL,(WPARAM)0,0),0);
 doe_get_stage_bloky_init_rada(hDlg, IDC_COMBO13, IDC_COMBO24, IDC_BUTTON11,
                               text_stage_edit_blok[p_tmm->text_state[3].text_stage].edit_funkce,
                               p_tmm->text_state[3].text_stage);
}

BOOL CALLBACK doe_callback_editor_materialu( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        // flagy meshe
        case IDC_BUTTON5:
          doe_editor_materialu_flagy(&tmm);
          break;
        
        // zmena & init stage bloku
        case IDC_COMBO1:
        case IDC_COMBO4:
        case IDC_COMBO11:
        case IDC_COMBO6:
        case IDC_COMBO12:
          if(HIWORD(wParam) == LBN_DBLCLK || HIWORD(wParam) ==  CBN_SELCHANGE) {
            doe_get_stage_bloky(hDlg,&tmm);
            zobraz_akt_material(hDlg,&tmm);
          }
          break;
        // Vyber texture flagu
        case IDC_COMBO3:
          if(HIWORD(wParam) == LBN_DBLCLK || HIWORD(wParam) ==  CBN_SELCHANGE) {
            tmm.textflag[0] = cti_text_flag(hDlg, IDC_COMBO3);
            pridej_texturu_mat(p_cnf, &tmm, 0, tmm.textfile[0], tmm.textflag[0]);
          }
          break;
        case IDC_COMBO26:
          if(HIWORD(wParam) == LBN_DBLCLK || HIWORD(wParam) ==  CBN_SELCHANGE) {
            tmm.textflag[1] = cti_text_flag(hDlg, IDC_COMBO26);
            pridej_texturu_mat(p_cnf, &tmm, 1, tmm.textfile[1], tmm.textflag[1]);
          }
          break;
        case IDC_COMBO28:
          if(HIWORD(wParam) == LBN_DBLCLK || HIWORD(wParam) ==  CBN_SELCHANGE) {
            tmm.textflag[2] = cti_text_flag(hDlg, IDC_COMBO28);
            pridej_texturu_mat(p_cnf, &tmm, 2, tmm.textfile[0], tmm.textflag[2]);
          }
          break;
        case IDC_COMBO30:
          if(HIWORD(wParam) == LBN_DBLCLK || HIWORD(wParam) ==  CBN_SELCHANGE) {
            tmm.textflag[3] = cti_text_flag(hDlg, IDC_COMBO30);
            pridej_texturu_mat(p_cnf, &tmm, 3, tmm.textfile[3], tmm.textflag[3]);
          }
          break;
        case IDC_BUTTON3:
          {
            int fc = text_stage_edit_blok[tmm.text_state[0].text_stage].edit_funkce;
            if(fc != K_CHYBA) {
              p_stage_func[fc](&tmm,tmm.text_state);
              vloz_mat_struct(hDlg, &tmm);
            }
            break;
          }
        case IDC_BUTTON10:
          {
            int fc = text_stage_edit_blok[tmm.text_state[1].text_stage].edit_funkce;
            if(fc != K_CHYBA) {
              p_stage_func[fc](&tmm,tmm.text_state+1);
              vloz_mat_struct(hDlg, &tmm);
            }
            break;
          }
        case IDC_BUTTON4:
          {
            int fc = text_stage_edit_blok[tmm.text_state[2].text_stage].edit_funkce;
            if(fc != K_CHYBA) {
              p_stage_func[fc](&tmm,tmm.text_state+2);
              vloz_mat_struct(hDlg, &tmm);
            }
            break;
          }
        case IDC_BUTTON11:
          {
            int fc = text_stage_edit_blok[tmm.text_state[3].text_stage].edit_funkce;
            if(fc != K_CHYBA) {
              p_stage_func[fc](&tmm,tmm.text_state+3);
              vloz_mat_struct(hDlg, &tmm);
            }
            break;
          }
        case IDC_EDIT2: // jmeno materialu
          GetDlgItemText(hDlg, IDC_EDIT2, tmm.jmeno, MAX_JMENO);
          break;
        case IDC_EDIT1: //textura 1
          GetDlgItemText(hDlg, IDC_EDIT1, tmm.textfile[0], MAX_JMENO);
          pridej_texturu_mat(p_cnf, &tmm, 0, tmm.textfile[0], tmm.textflag[0]);
          break;
        case IDC_EDIT8: //textura 2
          GetDlgItemText(hDlg, IDC_EDIT8, tmm.textfile[1], MAX_JMENO);
          pridej_texturu_mat(p_cnf, &tmm, 1, tmm.textfile[1], tmm.textflag[1]);
          break;
        case IDC_EDIT9: //textura 3
          GetDlgItemText(hDlg, IDC_EDIT9, tmm.textfile[2], MAX_JMENO);
          pridej_texturu_mat(p_cnf, &tmm, 2, tmm.textfile[2], tmm.textflag[2]);
          break;
        case IDC_EDIT10: //textura 4
          GetDlgItemText(hDlg, IDC_EDIT10, tmm.textfile[3], MAX_JMENO);
          pridej_texturu_mat(p_cnf, &tmm, 3, tmm.textfile[3], tmm.textflag[3]);
          break;
        case IDC_BUTTON9: // potvrdit material
          vyber_mat_struct(hDlg, &tmm);
          // pridani textur -> kontrola, jestli jsou + prip pridani
          pridej_texturu_mat(p_cnf, &tmm, 0, tmm.textfile[0], tmm.textflag[0]);
          pridej_texturu_mat(p_cnf, &tmm, 1, tmm.textfile[1], tmm.textflag[1]);
          pridej_texturu_mat(p_cnf, &tmm, 2, tmm.textfile[2], tmm.textflag[2]);
          pridej_texturu_mat(p_cnf, &tmm, 3, tmm.textfile[3], tmm.textflag[3]);
          oe_reload_textur(p_cnf);
          zobraz_akt_material(hDlg,&tmm);
          break;
        case IDC_BUTTON1: { // vyber 1. bmp textury
          byte pom[100];
          if(doe_vyber_texturu_dialog(hDlg, p_cnf, pom)) {
            strcpy(tmm.textfile[0],pom);
            vloz_mat_struct(hDlg, &tmm);
            pridej_texturu_mat(p_cnf, &tmm, 0, tmm.textfile[0], tmm.textflag[0]);
            oe_reload_textur( p_cnf);
            zobraz_akt_material(hDlg, &tmm);
          }
          break;
        }          
        case IDC_BUTTON7: { // vyber 2. bmp textury
          byte pom[100];
          if(doe_vyber_texturu_dialog(hDlg, p_cnf, pom)) {
            strcpy(tmm.textfile[1],pom);
            vloz_mat_struct(hDlg, &tmm);
            pridej_texturu_mat(p_cnf, &tmm, 1, tmm.textfile[1], tmm.textflag[1]);
            oe_reload_textur( p_cnf);
            zobraz_akt_material(hDlg, &tmm);
          }
          break;
        }          
        case IDC_BUTTON2: { // vyber 3. bmp textury
          byte pom[100];
          if(doe_vyber_texturu_dialog(hDlg, p_cnf, pom)) {
            strcpy(tmm.textfile[2],pom);
            vloz_mat_struct(hDlg, &tmm);
            pridej_texturu_mat(p_cnf, &tmm, 2, tmm.textfile[2], tmm.textflag[2]);
            oe_reload_textur( p_cnf);
            zobraz_akt_material(hDlg, &tmm);
          }
          break;
        }          
        case IDC_BUTTON8: { // vyber 4. bmp textury
          byte pom[100];
          if(doe_vyber_texturu_dialog(hDlg, p_cnf, pom)) {
            strcpy(tmm.textfile[3],pom);
            vloz_mat_struct(hDlg, &tmm);
            pridej_texturu_mat(p_cnf, &tmm, 3, tmm.textfile[3], tmm.textflag[3]);
            oe_reload_textur( p_cnf);
            zobraz_akt_material(hDlg, &tmm);
          }
          break;
        }
        case IDOK:
  		    EndDialog( hDlg, 1 );
          break;
        case IDCANCEL:
  		    EndDialog( hDlg, 0 );
          break;

        default:
          return FALSE;
      }
      break;

    case WM_INITDIALOG:
      nastav_pozici();
      doe_init_stage_bloky(hDlg);
      vloz_mat_struct(hDlg,&tmm);
      doe_get_stage_bloky(hDlg, &tmm);
      zobraz_akt_material(hDlg, &tmm);
      break;
    case WM_DESTROY:
      vyber_mat_struct(hDlg, &tmm);
      pridej_texturu_mat(p_cnf, &tmm, 0, tmm.textfile[0], tmm.textflag[0]);
      pridej_texturu_mat(p_cnf, &tmm, 1, tmm.textfile[1], tmm.textflag[1]);
      pridej_texturu_mat(p_cnf, &tmm, 2, tmm.textfile[2], tmm.textflag[2]);
      pridej_texturu_mat(p_cnf, &tmm, 3, tmm.textfile[3], tmm.textflag[3]);
      EndDialog( hDlg, FALSE );
      break;
    default:
      return FALSE;
  }  
  return TRUE;
}

void oe_prikaz_nahraj_animaci(K_EDITOR *p_cnf, HWND hwnd, EDIT_MATERIAL *p_mat, byte *p_textdir)
{
  EDIT_TEXT *p_text = p_cnf->p_text;
  int max = MAX_EDIT_TEXTUR;
  byte pom[100];

  chdir(p_textdir);
  if(doe_dialog_load(hwnd,pom,100,"Load animace","*.ani","Animace")) {
    lo_vyrob_animaci_list(p_mat, pom, p_text, max, ANIM_LOAD_ALL,".");
    oe_reload_textur(p_cnf);
  }
}

void oe_prikaz_uloz_animaci(HWND hwnd, EDIT_MATERIAL *p_mat, byte *p_textdir)
{
  byte pom[100];

  chdir(p_textdir);
  if(doe_dialog_save(pom,100,"Save animace","*.ani","Animace")) {
    lo_uloz_animaci_list(p_mat, pom, ".");
    strcpy(p_mat->anim.jmeno,pom);
  }
}

void oe_prikaz_nahraj_frame(K_EDITOR *p_cnf, HWND hwnd, EDIT_MATERIAL *p_mat, byte *p_textdir)
{
  EDIT_TEXT *p_text = p_cnf->p_text;
  int        max  = MAX_EDIT_TEXTUR;
  byte       pom[100];

  chdir(p_textdir);
  if(doe_dialog_load(hwnd,pom,100,"Load frame animaci","*.ani","Frame animace")) {
    lo_vyrob_animaci_list(p_mat, pom, p_text, max, ANIM_LOAD_FRAME,".");
    oe_reload_textur(p_cnf);
  }
}

void oe_prikaz_nahraj_posun(HWND hwnd, EDIT_MATERIAL *p_mat, byte *p_textdir, EDIT_TEXT *p_text, int max)
{
  byte pom[100];

  chdir(p_textdir);
  if(doe_dialog_load(hwnd,pom,100,"Load posun animaci","*.ani","Frame animace")) {
    lo_vyrob_animaci_list(p_mat, pom, p_text, max, ANIM_LOAD_POSUN,".");    
  }
}
/*
  tmp.f[0] = (float)*p_time*0.001f;
  tmp.f[1] = (float)*p_event;
  *p_time = (dword)(tmp.f[0]*1000);
  *p_event = (byte)tmp.f[1];
*/
BOOL CALLBACK doe_calback_frame_prop(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {          
          byte pom[50];
          ANIM_FRAME *p_frame = tmp.p_pom1;
          p_frame->flag = IsDlgButtonChecked( hDlg, IDC_CHECK1) ? p_frame->flag|FRAME_EVENT : p_frame->flag&~FRAME_EVENT;
          p_frame->flag = IsDlgButtonChecked( hDlg, IDC_CHECK2) ? p_frame->flag|FRAME_TEXTURA : p_frame->flag&~FRAME_TEXTURA;
          p_frame->flag = IsDlgButtonChecked( hDlg, IDC_CHECK3) ? p_frame->flag|FRAME_ALFA_FAKTOR : p_frame->flag&~FRAME_ALFA_FAKTOR;
          p_frame->flag = IsDlgButtonChecked( hDlg, IDC_CHECK4) ? p_frame->flag|FRAME_POSUN : p_frame->flag&~FRAME_POSUN;

          GetWindowText(GetDlgItem(hDlg,IDC_EDIT9),pom,20);
          p_frame->u = (float)atof(pom);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT10),pom,20);
          p_frame->v = (float)atof(pom);

          GetWindowText(GetDlgItem(hDlg,IDC_EDIT1),pom,20);
          p_frame->time = (dword)(atof(pom)*1000.0f);
          GetWindowText(GetDlgItem(hDlg,IDC_EDIT8),pom,20);
          sscanf(pom,"%x",&p_frame->alfa);           
          EndDialog( hDlg, TRUE );
          break;
        }
        default:
          return FALSE;
      }
      break;
    case WM_INITDIALOG: {
      ANIM_FRAME *p_frame = tmp.p_pom1;
      byte pom[50];
      EnableWindow(GetDlgItem(hDlg,IDC_EDIT8),TRUE);
      EnableWindow(GetDlgItem(hDlg,IDC_CHECK2),TRUE);
      EnableWindow(GetDlgItem(hDlg,IDC_CHECK3),TRUE);
      CheckDlgButton(hDlg,IDC_CHECK1,p_frame->flag&FRAME_EVENT);
      CheckDlgButton(hDlg,IDC_CHECK2,p_frame->flag&FRAME_TEXTURA);
      CheckDlgButton(hDlg,IDC_CHECK3,p_frame->flag&FRAME_ALFA_FAKTOR);
      CheckDlgButton(hDlg,IDC_CHECK4,p_frame->flag&FRAME_POSUN);
      // u,v
      sprintf(pom,"%.3f",(float)p_frame->u);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT9),pom);
      sprintf(pom,"%.3f",(float)p_frame->v);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT10),pom);
      // cas
      sprintf(pom,"%.3f",(float)p_frame->time*0.001f);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT1),pom);
      itoa(p_frame->alfa,pom,16);
      SetWindowText(GetDlgItem(hDlg,IDC_EDIT8),pom);
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

/* dword *p_time, byte *p_event) */
int doe_vlastnosti_frame(HWND hwnd, ANIM_FRAME *p_frame)
{
  void *p_tmp = tmp.p_pom1;
  int   rc;
  tmp.p_pom1 = p_frame;
  rc = DialogBox( hinst, MAKEINTRESOURCE(IDD_FRAME_ANIMACE), hwnd, doe_calback_frame_prop);
  tmp.p_pom1 = p_tmp;
  return(rc);
}

static void doe_vypis_animaci(HWND hDlg, EDIT_MATERIAL *p_mat)
{
  HWND hanim = GetDlgItem(hDlg,IDC_LIST1);
  int i,f = SendMessage(hanim,LB_GETCURSEL,0,0);
  byte pom[100];
  byte event[20];
  byte alfa[20];
  byte posun[20];
  byte file[50];
  
  SendMessage(hanim, LB_RESETCONTENT, 0, 0);
  
  for(i = 0; i < p_mat->anim.framenum; i++) {
    
    // Pokud je to frame -> vloz jmeno souboru
    if(p_mat->anim.p_frame[i].flag&FRAME_TEXTURA) {
      strcpy(file,p_mat->anim.p_frame[i].file);
    } else {
      file[0] = '-'; file[1] = 0;
    }
    
    // Pokud je to event -> vloz E
    if(p_mat->anim.p_frame[i].flag&FRAME_EVENT) {
      event[0] = 'E'; event[1] = 0;
    }
    else {
      sprintf(event,"%.3f",(float)p_mat->anim.p_frame[i].time*0.001f);
    }
    
    // Vloz alfa hodnotu
    if(p_mat->anim.p_frame[i].flag&FRAME_ALFA_FAKTOR)
      sprintf(alfa,"A%x",p_mat->anim.p_frame[i].alfa);
    else
      alfa[0] = 0;    
    
    // Vloz posun textury
    if(p_mat->anim.p_frame[i].flag&FRAME_POSUN)
      sprintf(posun,"P %.3f %.3f",p_mat->anim.p_frame[i].u,p_mat->anim.p_frame[i].v);
    else
      posun[0] = 0;

    // Soupni to tam
    sprintf(pom,"%.3d %s %s %s %s",i,file,event,alfa,posun);
    SendMessage( hanim, LB_ADDSTRING, 0,(LPARAM)pom);
  } 
  SendMessage(hanim,LB_SETCURSEL,f,0);  
  
  CheckDlgButton(hDlg,IDC_CHECK1,p_mat->flag&MAT_ANIM_FRAME);
  SetDlgItemText(hDlg,IDC_EDIT1, p_mat->anim.jmeno);
}

BOOL CALLBACK doe_callback_editor_materialu_frame( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
      case IDC_LIST1: {   // list s animaci
          HWND hlist = GetDlgItem( hDlg, IDC_LIST1);
          if(HIWORD(wParam) == LBN_DBLCLK) {
            int frame = SendMessage(hlist,LB_GETCURSEL,0,0);            
            doe_vlastnosti_frame(hDlg, tmm.anim.p_frame+frame);
            doe_vypis_animaci(hDlg, &tmm);            
          }
          break;
        }
        case IDC_BUTTON1: {// load animace
          oe_prikaz_nahraj_frame(p_cnf,hDlg, &tmm, p_cnf->tdir.texture_dir[0]);
          doe_vypis_animaci(hDlg, &tmm);
          break;
        }
        case IDC_BUTTON2:
          oe_prikaz_uloz_animaci(hDlg, &tmm, p_cnf->tdir.texture_dir[0]);
          break;
        case IDC_BUTTON4: // smaze animaci
          lo_zrus_animaci(&tmm.anim);
          doe_vypis_animaci(hDlg, &tmm);
          break;
        case IDC_BUTTON5: { // pro vsechny framy
          int i;
          HWND hlist = GetDlgItem( hDlg, IDC_LIST1);
          int frame = SendMessage(hlist,LB_GETCURSEL,0,0);
          ANIM_FRAME frm;
          if(tmm.anim.p_frame) {
            frm = tmm.anim.p_frame[frame];
            if(doe_vlastnosti_frame(hDlg, &frm)) {
              for(i = 0; i < tmm.anim.framenum; i++) {
                tmm.anim.p_frame[i] = frm;
              }
            }
            doe_vypis_animaci(hDlg, &tmm);
          }
        }
        case IDC_CHECK1: // frame animace ON/OFF
          tmm.flag = IsDlgButtonChecked(hDlg,IDC_CHECK1) ? tmm.flag|MAT_ANIM_FRAME : tmm.flag&~MAT_ANIM_FRAME;
          break;
        case IDC_CHECK2:
          tmm.flag = IsDlgButtonChecked(hDlg,IDC_CHECK2) ? tmm.flag|MAT_POSUN2D : tmm.flag&~MAT_POSUN2D;
          break;
        case IDOK:
  		    EndDialog( hDlg, 1 );
          break;
        case IDCANCEL:
  		    EndDialog( hDlg, 0 );
          break;
        default:
          return FALSE;
      }
      break;
      
    case WM_INITDIALOG:
      nastav_pozici();
      doe_vypis_animaci(hDlg, &tmm);
      CheckDlgButton(hDlg,IDC_CHECK1,tmm.flag&MAT_ANIM_FRAME);
      CheckDlgButton(hDlg,IDC_CHECK2,tmm.flag&MAT_POSUN2D);
      break;
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }  
  return TRUE;
}

// Vlozi material do dialogu
void doe_material_vloz_barvy(HWND hDlg, EDIT_MATERIAL *p_tmm)
{   
  doe_printf(hDlg,IDC_EDIT3, "%x",float_rgb(&p_tmm->dxmat.diffuse_r));
  doe_printf(hDlg,IDC_EDIT18,"%d",float_a(p_tmm->dxmat.diffuse_a));
  
  doe_printf(hDlg,IDC_EDIT7, "%x",float_rgb(&p_tmm->dxmat.specular_r));

  doe_printf(hDlg,IDC_EDIT15,"%x",float_rgb(&p_tmm->dxmat.faktor_r));
  doe_printf(hDlg,IDC_EDIT19,"%d",float_a(p_tmm->dxmat.faktor_a));

  doe_printf(hDlg,IDC_EDIT1,"%d",ftoi(p_tmm->lesk*100.0f));
  
  CheckDlgButton(hDlg,IDC_CHECK3, p_tmm->flag2&MAT2_DIFFUSE ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(hDlg,IDC_CHECK4, p_tmm->flag2&MAT2_SPECULAR ? BST_CHECKED : BST_UNCHECKED);

  CheckDlgButton(hDlg,IDC_CHECK25, p_tmm->flag2&MAT2_DIFFUSE_RAY ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(hDlg,IDC_CHECK6, p_tmm->flag2&MAT2_SPECULAR_RAY ? BST_CHECKED : BST_UNCHECKED);

  CheckDlgButton(hDlg,IDC_CHECK1, p_tmm->flag2&MAT2_ENV_SPEC ? BST_CHECKED : BST_UNCHECKED);  
  CheckDlgButton(hDlg,IDC_CHECK2, p_tmm->flag2&MAT2_ENV_SPEC_OBE ? BST_CHECKED : BST_UNCHECKED);
  doe_printf(hDlg,IDC_EDIT2,"%f",p_tmm->env_add1);
  doe_printf(hDlg,IDC_EDIT4,"%f",p_tmm->env_scale);
  doe_printf(hDlg,IDC_EDIT5,"%f",p_tmm->env_add2);
  doe_printf(hDlg,IDC_EDIT8,"%x",float_rgb(&p_tmm->env_r));
}


void doe_material_vyber_barvy(HWND hDlg, EDIT_MATERIAL *p_tmm)
{
  byte pom[200];
  dword barva;

  GetDlgItemText(hDlg, IDC_EDIT3, pom, 30);
  sscanf(pom,"%x",&barva);
  rgb_float(barva,&p_tmm->dxmat.diffuse_r);
  
  GetDlgItemText(hDlg, IDC_EDIT18, pom, 30);
  sscanf(pom,"%d",&barva);
  p_tmm->dxmat.diffuse_a = a_float(barva);

  GetDlgItemText(hDlg, IDC_EDIT7, pom, 30);
  sscanf(pom,"%x",&barva);
  rgb_float(barva,&p_tmm->dxmat.specular_r);

  GetDlgItemText(hDlg, IDC_EDIT15, pom, 30);
  sscanf(pom,"%x",&barva);
  rgb_float(barva,&p_tmm->dxmat.faktor_r);
  
  GetDlgItemText(hDlg, IDC_EDIT19, pom, 30);
  sscanf(pom,"%d",&barva);
  p_tmm->dxmat.faktor_a = a_float(barva);
  
  GetDlgItemText(hDlg, IDC_EDIT1, pom, 30);
  p_tmm->lesk = ((float)atof(pom)/100.0f);

  p_tmm->flag2 = IsDlgButtonChecked( hDlg, IDC_CHECK3) ? p_tmm->flag2|MAT2_DIFFUSE  : p_tmm->flag2&~MAT2_DIFFUSE;
  p_tmm->flag2 = IsDlgButtonChecked( hDlg, IDC_CHECK4) ? p_tmm->flag2|MAT2_SPECULAR : p_tmm->flag2&~MAT2_SPECULAR;

  p_tmm->flag2 = IsDlgButtonChecked( hDlg, IDC_CHECK25) ? p_tmm->flag2|MAT2_DIFFUSE_RAY  : p_tmm->flag2&~MAT2_DIFFUSE_RAY;
  p_tmm->flag2 = IsDlgButtonChecked( hDlg, IDC_CHECK6) ? p_tmm->flag2|MAT2_SPECULAR_RAY : p_tmm->flag2&~MAT2_SPECULAR_RAY;

  p_tmm->flag2 = IsDlgButtonChecked( hDlg, IDC_CHECK1) ? p_tmm->flag2|MAT2_ENV_SPEC : p_tmm->flag2&~MAT2_ENV_SPEC;
  p_tmm->flag2 = IsDlgButtonChecked( hDlg, IDC_CHECK2) ? p_tmm->flag2|MAT2_ENV_SPEC_OBE : p_tmm->flag2&~MAT2_ENV_SPEC_OBE;
  GetDlgItemText(hDlg, IDC_EDIT8, pom, 30);
  sscanf(pom,"%x",&barva);
  rgb_float(barva,&p_tmm->env_r);
  
  p_tmm->env_add1 = doe_vyber_kolonku_float(hDlg, IDC_EDIT2);
  p_tmm->env_scale= doe_vyber_kolonku_float(hDlg, IDC_EDIT4);
  p_tmm->env_add2 = doe_vyber_kolonku_float(hDlg, IDC_EDIT5);
}


/* Pracuju s TMP materialem tmm
*/
BOOL CALLBACK doe_callback_editor_materialu_barvy( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_BUTTON3:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT3);
          break;
        case IDC_BUTTON10:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT7);
          break;
        case IDC_BUTTON11:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT15);
          break;
        case IDC_BUTTON5:
          doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT8);
          break;
        case IDC_BUTTON1:
          doe_material_vyber_barvy(hDlg,&tmm);
          oe_reset_barva_scena(p_cnf, &tmm, p_cnf->km.vybr_mat,0);
          break;
        case IDC_BUTTON2:
          doe_material_vyber_barvy(hDlg,&tmm);
          oe_reset_barva_scena(p_cnf, &tmm, p_cnf->km.vybr_mat,1);
          break;
        case IDC_BUTTON4:
          doe_material_vyber_barvy(hDlg,&tmm);
          oe_reset_barva_scena_alfa(p_cnf, &tmm, p_cnf->km.vybr_mat);
          break;
        case IDOK:
  		    EndDialog( hDlg, 1 );
          break;
        case IDCANCEL:
  		    EndDialog( hDlg, 0 );
          break;
        default:
          return FALSE;
      }
      break;      
    case WM_INITDIALOG: // vlozeni parametru
      nastav_pozici();
      doe_material_vloz_barvy(hDlg,&tmm);
      break;
    case WM_DESTROY:    // nacteni parametru
      doe_material_vyber_barvy(hDlg,&tmm);
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }  
  return TRUE;
}

// Vlozi material do dialogu
void doe_material_vloz_textmapping(HWND hDlg, EDIT_MATERIAL *p_tmm, int akt_text)
{ 
  float uhel;
  BOD   p,sc;
  int   s,sflag;

  s = p_tmm->flag2&glstav_posun_flagy2[akt_text];

  CheckDlgButton(hDlg,IDC_CHECK2, s ? BST_CHECKED : BST_UNCHECKED);

  matrix_to_pos_2d(p_tmm->ttext+akt_text,&p);
  matrix_to_scale_2d(p_tmm->ttext+akt_text,&sc);
  uhel = matrix_to_float(p_tmm->ttext+akt_text);

  doe_printf(hDlg,IDC_EDIT24,"%f",p.x);
  doe_printf(hDlg,IDC_EDIT25,"%f",p.y);
  doe_printf(hDlg,IDC_EDIT47,"%f",RAD2DEG(uhel));
  doe_printf(hDlg,IDC_EDIT51,"%f",0);
  doe_printf(hDlg,IDC_EDIT52,"%f",0);
  doe_printf(hDlg,IDC_EDIT49,"%f",sc.x);
  doe_printf(hDlg,IDC_EDIT50,"%f",sc.y);

  EnableDlgItem(hDlg,IDC_EDIT24,s);
  EnableDlgItem(hDlg,IDC_EDIT25,s);
  EnableDlgItem(hDlg,IDC_EDIT47,s);
  EnableDlgItem(hDlg,IDC_EDIT49,s);
  EnableDlgItem(hDlg,IDC_EDIT50,s);
  EnableDlgItem(hDlg,IDC_EDIT51,s);
  EnableDlgItem(hDlg,IDC_EDIT52,s);

  sflag = glstav_texgen_sphere_flagy[akt_text]|glstav_texgen_eye_lin_flagy[akt_text];
  doe_check_stlac(hDlg,IDC_RADIO1,!(p_tmm->flag&sflag));
  doe_check_stlac(hDlg,IDC_RADIO3,  p_tmm->flag&glstav_texgen_sphere_flagy[akt_text]);
  doe_check_stlac(hDlg,IDC_RADIO4,  p_tmm->flag&glstav_texgen_eye_lin_flagy[akt_text]);

  doe_printf(hDlg,IDC_EDIT1, "%f", p_tmm->texgen_koord_s[akt_text][0]);
  doe_printf(hDlg,IDC_EDIT8, "%f", p_tmm->texgen_koord_s[akt_text][1]);
  doe_printf(hDlg,IDC_EDIT9, "%f", p_tmm->texgen_koord_s[akt_text][2]);
  doe_printf(hDlg,IDC_EDIT10,"%f", p_tmm->texgen_koord_s[akt_text][3]);

  doe_printf(hDlg,IDC_EDIT11,"%f", p_tmm->texgen_koord_t[akt_text][0]);
  doe_printf(hDlg,IDC_EDIT12,"%f", p_tmm->texgen_koord_t[akt_text][1]);
  doe_printf(hDlg,IDC_EDIT13,"%f", p_tmm->texgen_koord_t[akt_text][2]);
  doe_printf(hDlg,IDC_EDIT20,"%f", p_tmm->texgen_koord_t[akt_text][3]);
}

void doe_material_vyber_textmapping(HWND hDlg, EDIT_MATERIAL *p_tmm, int akt_text)
{
  float rot;
  BOD   p,s,piv;

  p_tmm->flag2 = doe_get_check_flag(hDlg, IDC_CHECK2, p_tmm->flag2, glstav_posun_flagy2[akt_text]);
  if(p_tmm->flag2&glstav_posun_flagy2[akt_text]) {
    p.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT24);
    p.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT25);
    rot = DEG2RAD(doe_vyber_kolonku_float(hDlg,IDC_EDIT47));
    s.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT49);
    s.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT50);
    piv.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT51);
    piv.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT52);
    key_mat_to_text_matrix(p_tmm->ttext+akt_text, &p, rot, &s, &piv);
  }
  
  p_tmm->flag = doe_get_check_flag(hDlg, IDC_RADIO3, p_tmm->flag, glstav_texgen_sphere_flagy[akt_text]);
  p_tmm->flag = doe_get_check_flag(hDlg, IDC_RADIO4, p_tmm->flag, glstav_texgen_eye_lin_flagy[akt_text]);

  if(p_tmm->flag&(glstav_texgen_sphere_flagy[akt_text]|glstav_texgen_eye_lin_flagy[akt_text])) {
    p_tmm->flag2 |= glstav_texgen_on[akt_text];
  }

  p_tmm->texgen_koord_s[akt_text][0] = doe_vyber_kolonku_float(hDlg,IDC_EDIT1);
  p_tmm->texgen_koord_s[akt_text][1] = doe_vyber_kolonku_float(hDlg,IDC_EDIT8);
  p_tmm->texgen_koord_s[akt_text][2] = doe_vyber_kolonku_float(hDlg,IDC_EDIT9);
  p_tmm->texgen_koord_s[akt_text][3] = doe_vyber_kolonku_float(hDlg,IDC_EDIT10);

  p_tmm->texgen_koord_t[akt_text][0] = doe_vyber_kolonku_float(hDlg,IDC_EDIT11);
  p_tmm->texgen_koord_t[akt_text][1] = doe_vyber_kolonku_float(hDlg,IDC_EDIT12);
  p_tmm->texgen_koord_t[akt_text][2] = doe_vyber_kolonku_float(hDlg,IDC_EDIT13);
  p_tmm->texgen_koord_t[akt_text][3] = doe_vyber_kolonku_float(hDlg,IDC_EDIT20);
}

void doe_material_textmapping_scale(HWND hDlg, float sx, float sy)
{  
  doe_printf(hDlg,IDC_EDIT1, "%f",doe_vyber_kolonku_float(hDlg,IDC_EDIT1)*sx);
  doe_printf(hDlg,IDC_EDIT8, "%f",doe_vyber_kolonku_float(hDlg,IDC_EDIT8)*sx);
  doe_printf(hDlg,IDC_EDIT9, "%f",doe_vyber_kolonku_float(hDlg,IDC_EDIT9)*sx);
  doe_printf(hDlg,IDC_EDIT10,"%f",doe_vyber_kolonku_float(hDlg,IDC_EDIT10)*sx);
  
  doe_printf(hDlg,IDC_EDIT11,"%f",doe_vyber_kolonku_float(hDlg,IDC_EDIT11)*sy);
  doe_printf(hDlg,IDC_EDIT12,"%f",doe_vyber_kolonku_float(hDlg,IDC_EDIT12)*sy);
  doe_printf(hDlg,IDC_EDIT13,"%f",doe_vyber_kolonku_float(hDlg,IDC_EDIT13)*sy);
  doe_printf(hDlg,IDC_EDIT20,"%f",doe_vyber_kolonku_float(hDlg,IDC_EDIT20)*sy);
}

BOOL CALLBACK doe_callback_editor_materialu_textura( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_COMBO1:
          if(HIWORD(wParam) == CBN_SELCHANGE) {
            HWND hCombo1 = (HWND)lParam;
            doe_material_vyber_textmapping(hDlg,&tmm,tmp.i[0]);
            tmp.i[0] = SendMessage(hCombo1,CB_GETCURSEL,0,0);
            doe_material_vloz_textmapping(hDlg,&tmm,tmp.i[0]);
          }
          break;
        case IDC_CHECK2:
          {
            int s = IsDlgButtonChecked(hDlg,IDC_CHECK2);
            EnableDlgItem(hDlg,IDC_EDIT24,s);
            EnableDlgItem(hDlg,IDC_EDIT25,s);
            EnableDlgItem(hDlg,IDC_EDIT47,s);
            EnableDlgItem(hDlg,IDC_EDIT49,s);
            EnableDlgItem(hDlg,IDC_EDIT50,s);
            EnableDlgItem(hDlg,IDC_EDIT51,s);
            EnableDlgItem(hDlg,IDC_EDIT52,s);
            break;
          }
        case IDC_BUTTON1: // load z kamery
          {
            GLMATRIX *p_cam = p_cnf->p_camera;

            // X vektor
            doe_printf(hDlg,IDC_EDIT1, "%f", p_cam->_11);
            doe_printf(hDlg,IDC_EDIT8, "%f", p_cam->_12);
            doe_printf(hDlg,IDC_EDIT9, "%f", p_cam->_13);
            doe_printf(hDlg,IDC_EDIT10,"%f", p_cam->_14);
            
            // Y vektor
            doe_printf(hDlg,IDC_EDIT11,"%f", p_cam->_21);
            doe_printf(hDlg,IDC_EDIT12,"%f", p_cam->_22);
            doe_printf(hDlg,IDC_EDIT13,"%f", p_cam->_23);
            doe_printf(hDlg,IDC_EDIT20,"%f", p_cam->_24);

            break;
          }
        case IDC_BUTTON2: // scale vektoru
          {
            float sx = doe_vyber_kolonku_float(hDlg,IDC_EDIT21),
                  sy = doe_vyber_kolonku_float(hDlg,IDC_EDIT22);
            doe_material_textmapping_scale(hDlg,sx,sy);
            break;
          }
        case IDOK:
  		    EndDialog( hDlg, 1 );
          break;
        case IDCANCEL:
  		    EndDialog( hDlg, 0 );
          break;
        default:
          return FALSE;
      }
      break;      
    case WM_INITDIALOG: // vlozeni parametru
      {
        HWND hwnd1 = GetDlgItem( hDlg, IDC_COMBO1);
        nastav_pozici();
        tmp.i[0] = 0;
        SendMessage(hwnd1,CB_RESETCONTENT,0,0);
        SendMessage(hwnd1,CB_ADDSTRING,0,(LPARAM)(LPCTSTR)"Text-koordinaty 1");
        SendMessage(hwnd1,CB_ADDSTRING,0,(LPARAM)(LPCTSTR)"Text-koordinaty 2");
        SendMessage(hwnd1,CB_ADDSTRING,0,(LPARAM)(LPCTSTR)"Text-koordinaty 3");
        SendMessage(hwnd1,CB_ADDSTRING,0,(LPARAM)(LPCTSTR)"Text-koordinaty 4");
        SendMessage(hwnd1,CB_SETCURSEL, tmp.i[0], 0);
        doe_material_vloz_textmapping(hDlg,&tmm,tmp.i[0]);
        doe_printf(hDlg,IDC_EDIT21,"1");
        doe_printf(hDlg,IDC_EDIT22,"1");
        break;
      }
    case WM_DESTROY:    // nacteni parametru
      doe_material_vyber_textmapping(hDlg,&tmm,tmp.i[0]);
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }  
  return TRUE;
}

BOOL CALLBACK doe_calback_konfigurace_table_materialy( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE);
          break;
        case IDOK:
          EndDialog( hDlg, TRUE);
          break;
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {      
      TCITEM it;
      int i;
      tk.hwndTab = GetDlgItem(hDlg, IDC_TAB1);
      it.mask = TCIF_TEXT;
      for(i = 0; i < tk.polozek; i++) {
        it.pszText = tk.jmena[i]; 
        SendMessage(tk.hwndTab,TCM_INSERTITEM,i,(LPARAM) (const LPTCITEM)&it);
        tk.p_template[i] = load_dialog_resource(MAKEINTRESOURCE(tk.dlg[i]));
      }
      TabCtrl_AdjustRect(tk.hwndTab, FALSE, &tk.rc);
      tk.rc.top += 10;
      tk.rc.left += 5;
      TabCtrl_SetCurSel(tk.hwndTab,tk.aktivni);
      tk.hwndDlg = CreateDialogIndirect(hinst, tk.p_template[tk.aktivni], hDlg, tk.call[tk.aktivni]); 
      break;
    }
    case WM_NOTIFY: {
      NMHDR *p_nmhdr = (LPNMHDR) lParam;
      // zmen vyber - prepni na jinou kartu
      if(p_nmhdr->code == TCN_SELCHANGE) {
        int vybr = TabCtrl_GetCurSel(tk.hwndTab);
        if(tk.hwndTab != NULL) DestroyWindow(tk.hwndDlg); 
        tk.hwndDlg = CreateDialogIndirect(hinst, tk.p_template[vybr], hDlg, tk.call[vybr]); 
      } 
      break;
    }
    case WM_DESTROY: {
      EndDialog( hDlg, FALSE );
      break; 
    }
    default:
      return FALSE;
  }
  return TRUE;
}

void oe_vloz_scene_material(K_EDITOR *p_cnf, EDIT_MATERIAL **p_mat, int maz)
{
  if((*p_mat)->flag&MAT_SCENE) {
    p_cnf->p_smat = p_mat;
  } else {
    if(maz)
      p_cnf->p_smat = NULL;
  }
}

void doe_editor_materialu(K_EDITOR *p_cnf)
{
  K_EDITOR tmpconfig = *p_cnf;
  int      maz;

  memset(&tk,0,sizeof(tk));
  
  tk.polozek = 4;

  tk.p_sv = &tmpconfig;

  strcpy(tk.jmena[0],"Textura");
  strcpy(tk.jmena[1],"Frame");
  strcpy(tk.jmena[2],"Barvy");
  strcpy(tk.jmena[3],"Mapping");

  tk.dlg[0] = IDD_MATERIAL_EDITOR;
  tk.dlg[1] = IDD_MATERIAL_EDITOR_ANIMACE_FRAME;
  tk.dlg[2] = IDD_MATERIAL_EDITOR_ANIMACE_BARVY;
  tk.dlg[3] = IDD_MATERIAL_EDITOR_ANIMACE_TEXTURA;

  tk.call[0] = doe_callback_editor_materialu;
  tk.call[1] = doe_callback_editor_materialu_frame;
  tk.call[2] = doe_callback_editor_materialu_barvy;
  tk.call[3] = doe_callback_editor_materialu_textura;

  tk.aktivni = 0;
  tk.t_dlg = IDC_TAB1;
  
  if(!p_cnf->p_mat[p_cnf->km.vybr_mat])
    ddw("Neni vybran zadny material nebo je seznam materialu prazdny !");
  else {
    kopiruj_material_indir(p_cnf->p_mat[p_cnf->km.vybr_mat], &tmm);
    vybr_mat = p_cnf->km.vybr_mat;
    maz = tmm.flag&MAT_SCENE;
    if(DialogBox( hinst, MAKEINTRESOURCE(IDD_MATERIAL_EDITOR_TABLE), hwnd_aplikace, doe_calback_konfigurace_table_materialy)) {
      zrus_material(p_cnf->p_mat+p_cnf->km.vybr_mat);
      p_cnf->p_mat[p_cnf->km.vybr_mat] = kopiruj_material(&tmm);
      oe_updatuj_scenu_flag(p_cnf);
      if(p_cnf->cnf.barvy_z_materialu) {
        oe_reset_barva_scena(p_cnf, &tmm, p_cnf->km.vybr_mat,0);
        oe_reset_barva_scena(p_cnf, &tmm, p_cnf->km.vybr_mat,1);
      }
      oe_vloz_scene_material(p_cnf, p_cnf->p_mat+p_cnf->km.vybr_mat, maz);
      lo_chunk_stage_load_korekce(p_cnf->p_mat+p_cnf->km.vybr_mat, 1);
    }    
    oe_reload_textur(p_cnf);
  }
}

BOOL CALLBACK doe_callback_editor_materialu_flagy( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {
          EDIT_MATERIAL *p_tmm = tmp.p_pom;
          HWND cb1 = GetDlgItem( hDlg, IDC_COMBO1);
                              
          p_tmm->flag2 = IsDlgButtonChecked( hDlg, IDC_CHECK2) ? p_tmm->flag2|MAT2_MAP1 : p_tmm->flag2&~MAT2_MAP1;
          p_tmm->flag2 = IsDlgButtonChecked( hDlg, IDC_CHECK3) ? p_tmm->flag2|MAT2_CALC_MAP1 : p_tmm->flag2&~MAT2_CALC_MAP1;
          
          p_tmm->flag2 = IsDlgButtonChecked( hDlg, IDC_CHECK4) ? p_tmm->flag2|MAT2_MAP2 : p_tmm->flag2&~MAT2_MAP2;
          p_tmm->flag2 = IsDlgButtonChecked( hDlg, IDC_CHECK5) ? p_tmm->flag2|MAT2_CALC_MAP2 : p_tmm->flag2&~MAT2_CALC_MAP2;
          
          p_tmm->flag2 = IsDlgButtonChecked( hDlg, IDC_CHECK25) ? p_tmm->flag2|MAT2_MAP3 : p_tmm->flag2&~MAT2_MAP3;
          p_tmm->flag2 = IsDlgButtonChecked( hDlg, IDC_CHECK8)  ? p_tmm->flag2|MAT2_CALC_MAP3 : p_tmm->flag2&~MAT2_CALC_MAP3;
          
          p_tmm->flag2 = IsDlgButtonChecked( hDlg, IDC_CHECK28) ? p_tmm->flag2|MAT2_MAP4 : p_tmm->flag2&~MAT2_MAP4;
          p_tmm->flag2 = IsDlgButtonChecked( hDlg, IDC_CHECK26) ? p_tmm->flag2|MAT2_CALC_MAP4 : p_tmm->flag2&~MAT2_CALC_MAP4;

          p_tmm->flag = doe_get_check_flag(hDlg, IDC_CHECK1, p_tmm->flag, MAT_SCENE);
          p_tmm->flag = doe_get_check_flag(hDlg, IDC_CHECK7, p_tmm->flag, MAT_SCENE_NUTNY);

          p_tmm->flag = doe_get_check_flag(hDlg, IDC_CHECK9, p_tmm->flag, MAT_NO_SCMT);

          text_korekce_materialu(p_tmm);          
                    
          p_tmm->smaterial_pozice = SendMessage(cb1,CB_GETITEMDATA,SendMessage(cb1,CB_GETCURSEL,0,0),0);

          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      EDIT_MATERIAL *p_tmm = tmp.p_pom;
      HWND cb1 = GetDlgItem( hDlg, IDC_COMBO1);
      
      CheckDlgButton(hDlg,IDC_CHECK2,(p_tmm->flag2&MAT2_MAP1) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg,IDC_CHECK3,(p_tmm->flag2&MAT2_CALC_MAP1) ? BST_CHECKED : BST_UNCHECKED);
      
      CheckDlgButton(hDlg,IDC_CHECK4,(p_tmm->flag2&MAT2_MAP2) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg,IDC_CHECK5,(p_tmm->flag2&MAT2_CALC_MAP2) ? BST_CHECKED : BST_UNCHECKED);
      
      CheckDlgButton(hDlg,IDC_CHECK25,(p_tmm->flag2&MAT2_MAP3) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg,IDC_CHECK8,(p_tmm->flag2&MAT2_CALC_MAP3) ? BST_CHECKED : BST_UNCHECKED);
      
      CheckDlgButton(hDlg,IDC_CHECK28,(p_tmm->flag2&MAT2_MAP4) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg,IDC_CHECK26,(p_tmm->flag2&MAT2_CALC_MAP4) ? BST_CHECKED : BST_UNCHECKED);      

      CheckDlgButton(hDlg,IDC_CHECK1,(p_tmm->flag&MAT_SCENE) ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(hDlg,IDC_CHECK7,(p_tmm->flag&MAT_SCENE_NUTNY) ? BST_CHECKED : BST_UNCHECKED);

      CheckDlgButton(hDlg,IDC_CHECK9,(p_tmm->flag&MAT_NO_SCMT) ? BST_CHECKED : BST_UNCHECKED);
      
      SendMessage(cb1, CB_RESETCONTENT, 0, 0 );      
      SendMessage(cb1, CB_SETITEMDATA, SendMessage(cb1, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"Prvni"),0);
      SendMessage(cb1, CB_SETITEMDATA, SendMessage(cb1, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"1"),1);
      SendMessage(cb1, CB_SETITEMDATA, SendMessage(cb1, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"2"),2);
      SendMessage(cb1, CB_SETITEMDATA, SendMessage(cb1, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"3"),3);
      SendMessage(cb1, CB_SETITEMDATA, SendMessage(cb1, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"4"),4);
      SendMessage(cb1, CB_SETITEMDATA, SendMessage(cb1, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"5"),5);
      SendMessage(cb1, CB_SETITEMDATA, SendMessage(cb1, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"6"),6);
      SendMessage(cb1, CB_SETITEMDATA, SendMessage(cb1, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"7"),7);
      SendMessage(cb1, CB_SETITEMDATA, SendMessage(cb1, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"8"),8);
      SendMessage(cb1, CB_SETITEMDATA, SendMessage(cb1, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"9"),9);
      SendMessage(cb1, CB_SETITEMDATA, SendMessage(cb1, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"Posledni"),SMATERIAL_POSLEDNI);
      SendMessage(cb1, CB_SETCURSEL, p_tmm->smaterial_pozice, 0 );

      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_editor_materialu_flagy(EDIT_MATERIAL *p_mat)
{
  tmp.p_pom = p_mat;
  DialogBox(hinst, MAKEINTRESOURCE(IDD_MATERIAL_EDITOR_FLAGY), hwnd_aplikace, doe_callback_editor_materialu_flagy);
}

static void doe_vloz_basic_env_funkce(HWND hCombo, int vyber)
{
  SendMessage( hCombo, CB_RESETCONTENT, 0, 0);
  SendMessage( hCombo, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"MODULACE");
  SendMessage( hCombo, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"ADD");
  SendMessage( hCombo, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"DECAL");
  SendMessage( hCombo, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"BLEND");
  SendMessage( hCombo, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)"REPLACE");
  SendMessage( hCombo, CB_SETCURSEL, vyber, 0 );
}

/*
#define DOT3_ENV  2
#define DOT3_BUMP 0
#define DOT3_TEXT 1
*/

BOOL CALLBACK doe_callback_editor_materialu_stage_dot3_color( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {
          MATERIAL_TEXT *p_text = tmp.p_pom;
          EDIT_MATERIAL *p_mat = tmp.p_pom1;
          HWND hCombo1 = GetDlgItem( hDlg, IDC_COMBO1);
          HWND hCombo2 = GetDlgItem( hDlg, IDC_COMBO2);
          HWND hCombo3 = GetDlgItem( hDlg, IDC_COMBO4);
          HWND hCombo4 = GetDlgItem( hDlg, IDC_COMBO6);
          HWND hCombo5 = GetDlgItem( hDlg, IDC_COMBO7);
          HWND hCombo6 = GetDlgItem( hDlg, IDC_COMBO8);
          
          p_text->textury[DOT3_BUMP] = SendMessage( hCombo1, CB_GETCURSEL, 0, 0 ); // bump-mapa
          p_text->textury[DOT3_TEXT] = SendMessage( hCombo2, CB_GETCURSEL, 0, 0 ); // text-mapa
          
          p_text->text_funkce[DOT3_TEXT] = SendMessage( hCombo4, CB_GETCURSEL, 0, 0);

          p_text->text_koord[DOT3_BUMP] = SendMessage( hCombo5, CB_GETCURSEL, 0, 0);
          p_text->text_koord[DOT3_TEXT] = SendMessage( hCombo6, CB_GETCURSEL, 0, 0);

          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      MATERIAL_TEXT *p_text = tmp.p_pom;
      HWND hCombo1 = GetDlgItem( hDlg, IDC_COMBO1);
      HWND hCombo2 = GetDlgItem( hDlg, IDC_COMBO2);
      HWND hCombo3 = GetDlgItem( hDlg, IDC_COMBO4);
      HWND hCombo4 = GetDlgItem( hDlg, IDC_COMBO6);
      byte pom[100];
      int  i;

      SendMessage( hCombo1, CB_RESETCONTENT, 0, 0);
      SendMessage( hCombo2, CB_RESETCONTENT, 0, 0);
      SendMessage( hCombo3, CB_RESETCONTENT, 0, 0);
      SendMessage( hCombo4, CB_RESETCONTENT, 0, 0);

      for(i = 0; i < MAT_TEXTUR; i++) {
        sprintf(pom,"Text%d",i+1);
        SendMessage( hCombo1, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)pom);
        SendMessage( hCombo2, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)pom);
        SendMessage( hCombo3, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)pom);
      }
	    
      SendMessage( hCombo1, CB_SETCURSEL, p_text->textury[DOT3_BUMP], 0 ); // bump-mapa
	    SendMessage( hCombo2, CB_SETCURSEL, p_text->textury[DOT3_TEXT], 0 ); // text-mapa

      doe_vloz_basic_env_funkce(hCombo4, p_text->text_funkce[DOT3_TEXT]);

      doe_vloz_text_koord(hDlg, IDC_COMBO7, p_text->text_koord[DOT3_BUMP]);
      doe_vloz_text_koord(hDlg, IDC_COMBO8, p_text->text_koord[DOT3_TEXT]);

      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_editor_materialu_stage_dot3_color(EDIT_MATERIAL *p_mat, MATERIAL_TEXT *p_text)
{
  tmp.p_pom = p_text;
  tmp.p_pom1 = p_mat;
  DialogBox(hinst, MAKEINTRESOURCE(IDD_MATERIAL_EDITOR_FUNC_DOT3_COLOR), hwnd_aplikace, doe_callback_editor_materialu_stage_dot3_color);
}

BOOL CALLBACK doe_callback_editor_materialu_stage_dot3_env( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {
          MATERIAL_TEXT *p_text = tmp.p_pom;
          HWND hCombo_env = GetDlgItem( hDlg, IDC_COMBO7);
          HWND hCombo_bump = GetDlgItem( hDlg, IDC_COMBO1);
          HWND hCombo_text = GetDlgItem( hDlg, IDC_COMBO2);
          HWND hCombo_gen = GetDlgItem( hDlg, IDC_COMBO4);
          HWND hCombo_env_mod = GetDlgItem( hDlg, IDC_COMBO8);
          HWND hCombo_text_mod = GetDlgItem( hDlg, IDC_COMBO6);
          HWND hCombo_env_kord = GetDlgItem( hDlg, IDC_COMBO16);
          HWND hCombo_bump_kord = GetDlgItem( hDlg, IDC_COMBO9);
          HWND hCombo_text_kord = GetDlgItem( hDlg, IDC_COMBO10);
          
          p_text->textury[DOT3_BUMP] = SendMessage( hCombo_bump, CB_GETCURSEL, 0, 0 ); // bump-mapa
          p_text->textury[DOT3_TEXT] = SendMessage( hCombo_text, CB_GETCURSEL, 0, 0 ); // text-mapa
          p_text->textury[DOT3_ENV]  = SendMessage( hCombo_env, CB_GETCURSEL, 0, 0 ); // env-mapa

          p_text->text_funkce[DOT3_ENV]  = SendMessage( hCombo_env_mod, CB_GETCURSEL, 0, 0);
          p_text->text_funkce[DOT3_TEXT] = SendMessage( hCombo_text_mod, CB_GETCURSEL, 0, 0);
          
          p_text->text_koord[DOT3_ENV]  = SendMessage( hCombo_env_kord, CB_GETCURSEL, 0, 0);
          p_text->text_koord[DOT3_BUMP] = SendMessage( hCombo_bump_kord, CB_GETCURSEL, 0, 0);
          p_text->text_koord[DOT3_TEXT] = SendMessage( hCombo_text_kord, CB_GETCURSEL, 0, 0);

          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      MATERIAL_TEXT *p_text = tmp.p_pom;
      HWND hCombo_bump = GetDlgItem( hDlg, IDC_COMBO1);
      HWND hCombo_text = GetDlgItem( hDlg, IDC_COMBO2);
      HWND hCombo_env = GetDlgItem( hDlg, IDC_COMBO7);
      HWND hCombo_gener = GetDlgItem( hDlg, IDC_COMBO4);
      HWND hCombo_text_mod = GetDlgItem( hDlg, IDC_COMBO6);
      HWND hCombo_env_mod = GetDlgItem( hDlg, IDC_COMBO8);
      byte pom[100];
      int  i;

      SendMessage( hCombo_bump, CB_RESETCONTENT, 0, 0);
      SendMessage( hCombo_text, CB_RESETCONTENT, 0, 0);
      SendMessage( hCombo_env, CB_RESETCONTENT, 0, 0);
      SendMessage( hCombo_gener, CB_RESETCONTENT, 0, 0);
      SendMessage( hCombo_text_mod, CB_RESETCONTENT, 0, 0);
      SendMessage( hCombo_env_mod, CB_RESETCONTENT, 0, 0);

      for(i = 0; i < MAT_TEXTUR; i++) {
        sprintf(pom,"Text%d",i+1);
        SendMessage( hCombo_env, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)pom);
        SendMessage( hCombo_text, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)pom);
        SendMessage( hCombo_bump, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)pom);
        SendMessage( hCombo_gener, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)pom);
      }
	    
      SendMessage( hCombo_env, CB_SETCURSEL, p_text->textury[DOT3_ENV], 0 );
	    SendMessage( hCombo_bump, CB_SETCURSEL, p_text->textury[DOT3_BUMP], 0 );
	    SendMessage( hCombo_text, CB_SETCURSEL, p_text->textury[DOT3_TEXT], 0 );

      doe_vloz_basic_env_funkce(hCombo_env_mod, p_text->text_funkce[DOT3_ENV]);
      doe_vloz_basic_env_funkce(hCombo_text_mod, p_text->text_funkce[DOT3_TEXT]);

      doe_vloz_text_koord(hDlg, IDC_COMBO16, p_text->text_koord[DOT3_ENV]);  // env
      doe_vloz_text_koord(hDlg, IDC_COMBO9,  p_text->text_koord[DOT3_BUMP]); // bump
      doe_vloz_text_koord(hDlg, IDC_COMBO10, p_text->text_koord[DOT3_TEXT]); // text
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_editor_materialu_stage_dot3_env(EDIT_MATERIAL *p_mat, MATERIAL_TEXT *p_text)
{
  tmp.p_pom = p_text;
  tmp.p_pom1 = p_mat;
  DialogBox(hinst, MAKEINTRESOURCE(IDD_MATERIAL_EDITOR_FUNC_DOT3_ENV), 
            hwnd_aplikace, doe_callback_editor_materialu_stage_dot3_env);
}


BOOL CALLBACK doe_callback_editor_materialu_stage_dudv_env( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
/*
        case IDC_CHECK1:
          {
            MATERIAL_TEXT *p_text = tmp.p_pom;
            EnableDlgItem(hDlg, IDC_COMBO4, IsDlgButtonChecked(hDlg,IDC_CHECK1));
            if(!IsDlgButtonChecked(hDlg,IDC_CHECK1)) {
              p_text->text_gen[DUDV_BUMP] = K_CHYBA;
              p_text->text_gen_flagy[DUDV_BUMP] &= ~TEXT_GEN;
            }
            else {
              HWND hCombo3 = GetDlgItem( hDlg, IDC_COMBO4);
              EDIT_MATERIAL *p_mat = tmp.p_pom1;
              p_text->text_gen[DUDV_BUMP] = SendMessage( hCombo3, CB_GETCURSEL, 0, 0);
              p_text->text_gen_flagy[DUDV_BUMP] |= TEXT_GEN_DUDV;
              pridej_texturu_mat_generovanou(p_cnf, p_mat, DUDV_BUMP);
            }
            break;
          }
*/
        case IDCANCEL:
  				EndDialog( hDlg, 0);
          break;
        case IDOK: {
          MATERIAL_TEXT *p_text = tmp.p_pom;
          HWND hCombo_env = GetDlgItem( hDlg, IDC_COMBO7);
          HWND hCombo_bump = GetDlgItem( hDlg, IDC_COMBO1);
          HWND hCombo_text = GetDlgItem( hDlg, IDC_COMBO2);
          HWND hCombo_gen = GetDlgItem( hDlg, IDC_COMBO4);
          HWND hCombo_env_mod = GetDlgItem( hDlg, IDC_COMBO8);
          HWND hCombo_text_mod = GetDlgItem( hDlg, IDC_COMBO6);
          HWND hCombo_env_kord = GetDlgItem( hDlg, IDC_COMBO16);
          HWND hCombo_bump_kord = GetDlgItem( hDlg, IDC_COMBO9);
          HWND hCombo_text_kord = GetDlgItem( hDlg, IDC_COMBO10);
          
          p_text->textury[DUDV_BUMP] = SendMessage( hCombo_bump, CB_GETCURSEL, 0, 0 ); // bump-mapa
          p_text->textury[DUDV_TEXT] = SendMessage( hCombo_text, CB_GETCURSEL, 0, 0 ); // text-mapa
          p_text->textury[DUDV_ENV]  = SendMessage( hCombo_env, CB_GETCURSEL, 0, 0 ); // env-mapa
/*
          if(IsDlgButtonChecked(hDlg,IDC_CHECK1)) {
            p_text->text_gen[DUDV_BUMP] = SendMessage( hCombo_gen, CB_GETCURSEL, 0, 0);
          }
*/
          p_text->text_funkce[DUDV_ENV]  = SendMessage( hCombo_env_mod, CB_GETCURSEL, 0, 0);
          p_text->text_funkce[DUDV_TEXT] = SendMessage( hCombo_text_mod, CB_GETCURSEL, 0, 0);
          
          p_text->text_koord[DUDV_ENV]  = SendMessage( hCombo_env_kord, CB_GETCURSEL, 0, 0);
          p_text->text_koord[DUDV_BUMP] = SendMessage( hCombo_bump_kord, CB_GETCURSEL, 0, 0);
          p_text->text_koord[DUDV_TEXT] = SendMessage( hCombo_text_kord, CB_GETCURSEL, 0, 0);

          p_text->text_ati_bump_matrix[0] = doe_vyber_kolonku_float(hDlg, IDC_EDIT1);
          p_text->text_ati_bump_matrix[1] = doe_vyber_kolonku_float(hDlg, IDC_EDIT8);
          p_text->text_ati_bump_matrix[2] = doe_vyber_kolonku_float(hDlg, IDC_EDIT9);
          p_text->text_ati_bump_matrix[3] = doe_vyber_kolonku_float(hDlg, IDC_EDIT10);
/*          
          if(!IsDlgButtonChecked(hDlg,IDC_CHECK1)) {
            p_text->text_gen[DUDV_BUMP] = K_CHYBA;
            p_text->text_gen_flagy[DUDV_BUMP] &= ~TEXT_GEN;
          }
          else {
            HWND hCombo3 = GetDlgItem( hDlg, IDC_COMBO4);
            p_text->text_gen[DUDV_BUMP] = SendMessage( hCombo3, CB_GETCURSEL, 0, 0);
            p_text->text_gen_flagy[DUDV_BUMP] |= TEXT_GEN_DUDV;
          }
*/          
          EndDialog( hDlg, 1);
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      MATERIAL_TEXT *p_text = tmp.p_pom;
      HWND hCombo_bump = GetDlgItem( hDlg, IDC_COMBO1);
      HWND hCombo_text = GetDlgItem( hDlg, IDC_COMBO2);
      HWND hCombo_env = GetDlgItem( hDlg, IDC_COMBO7);
      HWND hCombo_gener = GetDlgItem( hDlg, IDC_COMBO4);
      HWND hCombo_text_mod = GetDlgItem( hDlg, IDC_COMBO6);
      HWND hCombo_env_mod = GetDlgItem( hDlg, IDC_COMBO8);
      byte pom[100];
      int  i;

      SendMessage( hCombo_bump, CB_RESETCONTENT, 0, 0);
      SendMessage( hCombo_text, CB_RESETCONTENT, 0, 0);
      SendMessage( hCombo_env, CB_RESETCONTENT, 0, 0);
      SendMessage( hCombo_gener, CB_RESETCONTENT, 0, 0);
      SendMessage( hCombo_text_mod, CB_RESETCONTENT, 0, 0);
      SendMessage( hCombo_env_mod, CB_RESETCONTENT, 0, 0);

      for(i = 0; i < MAT_TEXTUR; i++) {
        sprintf(pom,"Text%d",i+1);
        SendMessage( hCombo_env, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)pom);
        SendMessage( hCombo_text, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)pom);
        SendMessage( hCombo_bump, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)pom);
        SendMessage( hCombo_gener, CB_ADDSTRING, 0,(LPARAM)(LPCTSTR)pom);
      }
	    
      SendMessage( hCombo_env, CB_SETCURSEL, p_text->textury[DUDV_ENV], 0 );
	    SendMessage( hCombo_bump, CB_SETCURSEL, p_text->textury[DUDV_BUMP], 0 );
	    SendMessage( hCombo_text, CB_SETCURSEL, p_text->textury[DUDV_TEXT], 0 );
/*
      if(p_text->text_gen[DUDV_BUMP] != K_CHYBA) {
        SendMessage( hCombo_gener, CB_SETCURSEL, p_text->text_gen[DUDV_BUMP], 0 );
      } else
        SendMessage( hCombo_gener, CB_SETCURSEL, 0, 0 );

      doe_check_stlac(hDlg, IDC_CHECK1, p_text->text_gen[DUDV_BUMP] != K_CHYBA);
      EnableDlgItem(hDlg, IDC_COMBO4, p_text->text_gen[DUDV_BUMP] != K_CHYBA);
*/      
      doe_vloz_basic_env_funkce(hCombo_env_mod, p_text->text_funkce[DUDV_ENV]);
      doe_vloz_basic_env_funkce(hCombo_text_mod, p_text->text_funkce[DUDV_TEXT]);

      doe_vloz_text_koord(hDlg, IDC_COMBO16, p_text->text_koord[DUDV_ENV]);  // env
      doe_vloz_text_koord(hDlg, IDC_COMBO9,  p_text->text_koord[DUDV_BUMP]); // bump
      doe_vloz_text_koord(hDlg, IDC_COMBO10, p_text->text_koord[DUDV_TEXT]); // text

      doe_printf(hDlg, IDC_EDIT1, "%f",p_text->text_ati_bump_matrix[0]);
      doe_printf(hDlg, IDC_EDIT8, "%f",p_text->text_ati_bump_matrix[1]);
      doe_printf(hDlg, IDC_EDIT9, "%f",p_text->text_ati_bump_matrix[2]);
      doe_printf(hDlg, IDC_EDIT10,"%f",p_text->text_ati_bump_matrix[3]);
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

void doe_editor_materialu_stage_dudv_env(EDIT_MATERIAL *p_mat, MATERIAL_TEXT *p_text)
{
  tmp.p_pom = p_text;
  tmp.p_pom1 = p_mat;  
  DialogBox(hinst, MAKEINTRESOURCE(IDD_MATERIAL_EDITOR_FUNC_DUDV_ENV),
            hwnd_aplikace, doe_callback_editor_materialu_stage_dudv_env);
}

//
BOOL CALLBACK doe_callback_editor_materialu_flagovac( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDOK: {
          EDIT_MATERIAL  *p_mat;
          int flag;
          int flag1,flag2;
          int set,and,or;
          int i;

          flag = doe_vyber_kolonku_int_hexa(hDlg,IDC_EDIT1);
          flag1 = IsDlgButtonChecked(hDlg,IDC_CHECK1);
          flag2 = IsDlgButtonChecked(hDlg,IDC_CHECK2);

          set = IsDlgButtonChecked(hDlg,IDC_RADIO2);
          and = IsDlgButtonChecked(hDlg,IDC_RADIO3);
          or = IsDlgButtonChecked(hDlg,IDC_RADIO4);
          
          if(IsDlgButtonChecked(hDlg,IDC_CHECK3)) {            
            p_mat = p_cnf->p_mat[p_cnf->km.vybr_mat];
            if(p_mat) {
              if(flag1) {
                if(set)
                  p_mat->flag = flag;
                else if(and)
                  p_mat->flag &= flag;
                else
                  p_mat->flag |= flag;
              }
              if(flag2) {
                if(set)
                  p_mat->flag2 = flag;
                else if(and)
                  p_mat->flag2 &= flag;
                else
                  p_mat->flag2 |= flag;
              }
            }
          } else {
            for(i = 0; i < MAX_EDIT_MATERIALU; i++) {
              p_mat = p_cnf->p_mat[i];
              if(p_mat) {
                if(flag1) {
                  if(set)
                    p_mat->flag = flag;
                  else if(and)
                    p_mat->flag &= flag;
                  else
                    p_mat->flag |= flag;
                }
                if(flag2) {
                  if(set)
                    p_mat->flag2 = flag;
                  else if(and)
                    p_mat->flag2 &= flag;
                  else
                    p_mat->flag2 |= flag;
                }
              }
            }
          }

          EndDialog( hDlg, 1 );
          break;
        }
        case IDCANCEL:
  		    EndDialog( hDlg, 0 );
          break;
        default:
          return FALSE;
      }
      break;
      
    case WM_INITDIALOG:
      {
        CheckDlgButton(hDlg, IDC_RADIO2, TRUE);
        doe_printf(hDlg, IDC_EDIT1, "0");
        break;
      }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }  
  return TRUE;
}

void doe_editor_materialu_flagovac(K_EDITOR *p_cnf)
{
  DialogBox(hinst,MAKEINTRESOURCE(IDD_MATERIAL_EDITOR_FLAGOVAC),hwnd_aplikace,doe_callback_editor_materialu_flagovac);
}


/********************************************************************************
 Editor prvku
 ********************************************************************************
*/
void doe_prikaz_editor_prvku(K_EDITOR *p_cnf)
{
//  ko_editor_hernich_prvku(hinst, hwnd_aplikace);
//  be_reload_databaze(&p_cnf->bnf);
}


/*****
  Dalsi funkce
 *****
*/
/* nastavi progres--bar na rozsah min a max
*/
void doe_progresbar_nastav(HWND hwnd, int res, int min, int max)
{
  HWND progres = GetDlgItem(hwnd,res);
  SendMessage(progres,PBM_SETRANGE,0,MAKELPARAM(min,max));
  SendMessage(progres,PBM_SETPOS,0,0);
}

/* posune progress-bar o krok
*/
void doe_progresbar_posun(HWND hwnd, int res, int krok)
{
  HWND progres = GetDlgItem(hwnd,res);
  SendMessage(progres,PBM_DELTAPOS,(WPARAM)krok,0);
}

void doe_progresbar_setpos(HWND hwnd, int res, int pos)
{
  HWND progres = GetDlgItem(hwnd,res);
  SendMessage(progres,PBM_SETPOS,pos,0);
}

/* Funkce slideru
*/
void doe_slider_nastav(HWND hwnd, int res, int min, int max, int akt)
{
  HWND progres = GetDlgItem(hwnd,res);
  SendMessage(progres,TBM_SETRANGE,1,MAKELPARAM(min,max));
  SendMessage(progres,TBM_SETPOS,1,akt);
}

void doe_slider_setpos(HWND hwnd, int res, int pos)
{
  HWND progres = GetDlgItem(hwnd,res);
  SendMessage(progres,TBM_SETPOS,1,pos);
}

dword doe_slider_getpos(HWND hwnd, int res)
{
  HWND progres = GetDlgItem(hwnd,res);
  return(SendMessage(progres,TBM_GETPOS,0,0));
}

BOOL CALLBACK doe_calback_statistika( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDOK:
  		    EndDialog( hDlg, 1 );
          break;
        case IDCANCEL:
  		    EndDialog( hDlg, 0 );
          break;
        default:
          return FALSE;
      }
      break;
      
    case WM_INITDIALOG :{
      SCENE_STATISTIC stat;
      byte pom[50];

      oe_updatuj_statistiku_sceny(p_cnf, &stat);
           
      itoa(stat.textsize,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT1, pom);

      itoa(stat.texnum,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT8, pom);
      
      itoa(stat.lightsize,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT23, pom);

      itoa(stat.lightnum,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT41, pom);

      itoa(stat.kontsize,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT9, pom);
      
      itoa(stat.kontnum,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT10, pom);

      itoa(stat.objnum,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT11, pom);
/*
      itoa(stat.lev_kontsize,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT23, pom);
      
      itoa(stat.lev_kontnum,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT41, pom);

      itoa(stat.lev_objnum,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT28, pom);
*/
      itoa(stat.matsize,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT12, pom);

      itoa(stat.matnum,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT13, pom);

      itoa(stat.totmem/1000,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT20, pom);

      itoa(stat.freemem/1000,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT21, pom);

      itoa((stat.totmem-stat.freemem)/1000,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT22, pom);      
      
      itoa((stat.facu),pom,10);
      SetDlgItemText(hDlg, IDC_EDIT29, pom);
      
      itoa((stat.bodu),pom,10);
      SetDlgItemText(hDlg, IDC_EDIT30, pom);

      itoa((stat.polyface),pom,10);
      SetDlgItemText(hDlg, IDC_EDIT42, pom);
      
      itoa((stat.polynum),pom,10);
      SetDlgItemText(hDlg, IDC_EDIT43, pom);
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }  
  return TRUE;
}

void doe_statistika_sceny(K_EDITOR *p_cnf)
{
 DialogBox( hinst, MAKEINTRESOURCE(IDD_STATISTIKA), hwnd_aplikace, doe_calback_statistika);
}

BOOL CALLBACK doe_callback_barva_bodu( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
      case IDC_BUTTON1: {//barva
          dword barva;
          byte  pom[50];
          GetDlgItemText(hDlg,IDC_EDIT1,pom,49);
          sscanf(pom,"%x",&barva);
          if(doe_prikaz_vyber_barvu(p_cnf,hDlg,&barva)) 
            SetDlgItemText( hDlg, IDC_EDIT1, itoa(barva,pom,16));
          break;
        }
        case IDOK: {
          byte pom[50];
          GetDlgItemText(hDlg,IDC_EDIT1,pom,49);
          sscanf(pom,"%x",tmp.p_pom);
  		    if(IsDlgButtonChecked(hDlg,IDC_RADIO1))
            *(byte *)tmp.p_pom1 = COLOR_SET;
          else if(IsDlgButtonChecked(hDlg,IDC_RADIO2))
            *(byte *)tmp.p_pom1 = COLOR_ADD;
          else if(IsDlgButtonChecked(hDlg,IDC_RADIO3))
            *(byte *)tmp.p_pom1 = COLOR_SUB;
          EndDialog( hDlg, 1 );
          break;
        }
        case IDCANCEL:
  		    EndDialog( hDlg, 0 );
          break;
        default:
          return FALSE;
      }
      break;
      
    case WM_INITDIALOG :{
      byte pom[100];
      itoa(*(int *)tmp.p_pom,pom,16);
      SetDlgItemText(hDlg, IDC_EDIT1, pom);
      switch (*(byte *)tmp.p_pom1) {
      case COLOR_SET:
        CheckDlgButton(hDlg,IDC_RADIO1,TRUE);
        break;
      case COLOR_ADD:
        CheckDlgButton(hDlg,IDC_RADIO2,TRUE);
        break;
      case COLOR_SUB:
        CheckDlgButton(hDlg,IDC_RADIO3,TRUE);
        break;
      }
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }  
  return TRUE;
}

int doe_edit_barva_bodu(dword *p_barva, byte *p_op)
{
  tmp.p_pom = p_barva;
  tmp.p_pom1 = p_op;
  return(DialogBox( hinst, MAKEINTRESOURCE(IDD_BARVA_BODU), hwnd_aplikace, doe_callback_barva_bodu));
}


BOOL CALLBACK doe_callback_drobeni_facu( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDOK: {
          byte pom[50];
          GetDlgItemText(hDlg,IDC_EDIT1,pom,49);
          sscanf(pom,"%d",tmp.p_pom);
  		    if(IsDlgButtonChecked(hDlg,IDC_RADIO1))
            *(dword *)tmp.p_pom1 = FALSE;
          else if(IsDlgButtonChecked(hDlg,IDC_RADIO2))
            *(dword *)tmp.p_pom1 = TRUE;
          EndDialog( hDlg, 1 );
          break;
        }
        case IDCANCEL:
  		    EndDialog( hDlg, 0 );
          break;
        default:
          return FALSE;
      }
      break;
      
    case WM_INITDIALOG :{
      byte pom[100];
      itoa(*(dword *)tmp.p_pom,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT1, pom);
      CheckDlgButton(hDlg,IDC_RADIO1,!(*(dword *)tmp.p_pom1));
      CheckDlgButton(hDlg,IDC_RADIO2,(*(dword *)tmp.p_pom1));
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }  
  return TRUE;
}

int doe_drobeni_facu(dword *p_hloubka, dword *p_nahod)
{
  tmp.p_pom = p_hloubka;
  tmp.p_pom1 = p_nahod;
  return(DialogBox( hinst, MAKEINTRESOURCE(IDD_DROBIC_FACU), hwnd_aplikace, doe_callback_drobeni_facu));
}



BOOL CALLBACK doe_callback_uprava_okoli( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDOK: {
          byte pom[50];
          BOD *p_dist = tmp.p_pom;
          BOD *p_bod = tmp.p_pom1;

          GetDlgItemText(hDlg,IDC_EDIT1,pom,49);
          p_dist->x = (float)atoi(pom);
          GetDlgItemText(hDlg,IDC_EDIT8,pom,49);
          p_dist->y = (float)atoi(pom);
          GetDlgItemText(hDlg,IDC_EDIT9,pom,49);
          p_dist->z = (float)atoi(pom);

          GetDlgItemText(hDlg,IDC_EDIT10,pom,49);
          p_bod->x = (float)atoi(pom);
          GetDlgItemText(hDlg,IDC_EDIT11,pom,49);
          p_bod->y = (float)atoi(pom);
          GetDlgItemText(hDlg,IDC_EDIT12,pom,49);
          p_bod->z = (float)atoi(pom);

          EndDialog( hDlg, 1 );
          break;
        }
        case IDCANCEL:
  		    EndDialog( hDlg, 0 );
          break;
        default:
          return FALSE;
      }
      break;
      
    case WM_INITDIALOG :{
      byte pom[100];
      BOD *p_dist = tmp.p_pom;
      BOD *p_bod = tmp.p_pom1;
      
      itoa((int)p_dist->x,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT1, pom);
      itoa((int)p_dist->y,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT8, pom);
      itoa((int)p_dist->z,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT9, pom);

      itoa((int)p_bod->x,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT10, pom);
      itoa((int)p_bod->y,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT11, pom);
      itoa((int)p_bod->z,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT12, pom);
            
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }  
  return TRUE;
}

int doe_uprava_okoli(BOD *p_dist, BOD *p_bod)
{
  tmp.p_pom = p_dist;
  tmp.p_pom1 = p_bod;
  return(DialogBox( hinst, MAKEINTRESOURCE(IDD_OKOLI), hwnd_aplikace, doe_callback_uprava_okoli));
}

BOOL CALLBACK doe_callback_prikaz_raytracer( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDOK: {
          byte pom[50];
          RAY_TRACER *p_ray = tmp.p_pom;

          // stiny
          p_ray->stiny = IsDlgButtonChecked(hDlg,IDC_CHECK1);

          // faktor
          GetDlgItemText(hDlg,IDC_EDIT8,pom,49);
          p_ray->faktor = (float)atof(pom);

          // Stupen filtrace
          GetDlgItemText(hDlg,IDC_EDIT22,pom,49);
          p_ray->stupen_filtrace = atoi(pom);          

          // max rozmery textur
          GetDlgItemText(hDlg,IDC_EDIT12,pom,49);
          p_ray->max_x_surface = atoi(pom);
          GetDlgItemText(hDlg,IDC_EDIT13,pom,49);
          p_ray->max_y_surface = atoi(pom);

          // max rozmery textur
          GetDlgItemText(hDlg,IDC_EDIT20,pom,49);
          p_ray->max_x_lepic = atoi(pom);
          GetDlgItemText(hDlg,IDC_EDIT21,pom,49);
          p_ray->max_y_lepic = atoi(pom);
          
          p_ray->disk_mezi = IsDlgButtonChecked(hDlg,IDC_CHECK2);

          if(IsDlgButtonChecked(hDlg,IDC_RADIO3)) {
            p_ray->stinovani = SHAD_FLAT;
          }
          if(IsDlgButtonChecked(hDlg,IDC_RADIO5)) {
            p_ray->stinovani = SHAD_PHONG;
          }
          
          p_cnf->ray.supersampl = IsDlgButtonChecked(hDlg,IDC_CHECK3);
          p_cnf->ray.filtrace = IsDlgButtonChecked(hDlg,IDC_CHECK4);

          GetDlgItemText(hDlg,IDC_EDIT1,pom,49);
          p_ray->sup_hloubka = atoi(pom);
          GetDlgItemText(hDlg,IDC_EDIT9,pom,49);
          p_ray->sup_velikost_x = atoi(pom);
          GetDlgItemText(hDlg,IDC_EDIT10,pom,49);
          p_ray->sup_velikost_y = atoi(pom);

          GetDlgItemText(hDlg,IDC_EDIT41,pom,49);
          p_ray->okraj = atoi(pom);
          GetDlgItemText(hDlg,IDC_EDIT23,pom,49);
          p_ray->text_min_x = atoi(pom);
          GetDlgItemText(hDlg,IDC_EDIT28,pom,49);
          p_ray->text_min_y = atoi(pom);


          {
            HWND hwnd1 = GetDlgItem( hDlg, IDC_COMBO1);
            p_ray->p_akt_filtr_func = p_ray->filtr_func+SendMessage( hwnd1, CB_GETCURSEL, 0, 0 );
            hwnd1 = GetDlgItem( hDlg, IDC_COMBO4);
            p_ray->lightmap_filtr = SendMessage( hwnd1, CB_GETCURSEL, 0, 0 );
          }
         
          EndDialog( hDlg, 1 );
          break;
        }
        case IDCANCEL:
  		    EndDialog( hDlg, 0 );
          break;
        default:
          return FALSE;
      }
      break;
      
    case WM_INITDIALOG : {
      byte pom[100];
      RAY_TRACER *p_ray = tmp.p_pom;

      // stupen filtrace
      itoa((int)p_ray->stupen_filtrace,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT22, pom);
      
      // scale faktor
      sprintf(pom,"%.3f",p_ray->faktor);
      SetDlgItemText(hDlg, IDC_EDIT8, pom);

      // stiny
      CheckDlgButton(hDlg,IDC_CHECK1,p_ray->stiny);
      CheckDlgButton(hDlg,IDC_CHECK2,p_ray->disk_mezi);

      CheckDlgButton(hDlg,IDC_RADIO3, p_ray->stinovani == SHAD_FLAT);
      CheckDlgButton(hDlg,IDC_RADIO5, p_ray->stinovani == SHAD_PHONG);

      CheckDlgButton(hDlg,IDC_CHECK3, p_cnf->ray.supersampl);
      CheckDlgButton(hDlg,IDC_CHECK4, p_cnf->ray.filtrace);

      // max x textur ray-traceru
      itoa((int)p_ray->max_x_surface,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT12, pom);
      itoa((int)p_ray->max_y_surface,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT13, pom);

      // max x lepice      
      itoa((int)p_ray->max_x_lepic,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT20, pom);
      itoa((int)p_ray->max_y_lepic,pom,10);
      SetDlgItemText(hDlg, IDC_EDIT21, pom);
      
      doe_printf(hDlg,IDC_EDIT1,"%d",p_ray->sup_hloubka);
      doe_printf(hDlg,IDC_EDIT9,"%d",p_ray->sup_velikost_x);
      doe_printf(hDlg,IDC_EDIT10,"%d",p_ray->sup_velikost_y);
      
      doe_printf(hDlg,IDC_EDIT41,"%d",p_ray->okraj);
      doe_printf(hDlg,IDC_EDIT23,"%d",p_ray->text_min_x);
      doe_printf(hDlg,IDC_EDIT28,"%d",p_ray->text_min_y);

      // render funkce      
      {
        HWND hwnd1 = GetDlgItem( hDlg, IDC_COMBO1);
        int  i;        
        SendMessage( hwnd1, CB_RESETCONTENT, 0, 0 );
        for(i = 0; i < FILTR_FUNC_NUM; i++) {
          SendMessage( hwnd1, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) p_ray->filtr_func[i].jmeno);
          SendMessage( hwnd1, CB_SETITEMDATA, i, i);
        }
        SendMessage( hwnd1, CB_SETCURSEL, p_ray->p_akt_filtr_func-p_ray->filtr_func, 0 );

        hwnd1 = GetDlgItem( hDlg, IDC_COMBO4);        
        SendMessage( hwnd1, CB_RESETCONTENT, 0, 0 );        
        SendMessage( hwnd1, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "Linear");
        SendMessage( hwnd1, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) "Near");
        SendMessage( hwnd1, CB_SETCURSEL, p_ray->lightmap_filtr, 0 );
      }

      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }  
  return TRUE;
}

int doe_prikaz_raytracer(RAY_TRACER *p_ray)
{
  tmp.p_pom = p_ray;
  return(DialogBox( hinst, MAKEINTRESOURCE(IDD_RENDER), hwnd_aplikace, doe_callback_prikaz_raytracer));
}

void doe_printf(HWND hwnd, int cntrl, byte *p_text, ...)
{
 va_list argumenty;
 byte    text[500];

 va_start(argumenty,p_text);
 vsprintf(text,p_text,argumenty);
 va_end(argumenty);

 SetDlgItemText(hwnd, cntrl, text);  
}


// zatim vytvori nove okno a do nej tp blitne z p_back
int doe_zobraz_texturu(K_EDITOR *p_cnf, int text, byte druh, HWND hDlg)
{
 HWND hwnd_mat;
 HDC  hDC;
 RECT src_rec;
 RECT dest_rec;
 byte pom[200];
 EDIT_TEXT *p_text;
 int  dx=50,dy=50,n;
 GLint typ;
 
 // Prekreslit back-face do okna
 hwnd_mat = GetDlgItem(hDlg,IDC_STATIC_OKNO);

 if(!(hDC = GetDC(hwnd_mat)))
   chyba("GetHDC"); 
 doe_nastav_pixelformat(hDC);
 if(!wglMakeCurrent(hDC,hwconf.hRC))
   chyba("MCR");

 GetClientRect( hwnd_mat, &dest_rec);
 GetClientRect( hwnd_mat, &src_rec);
 ClientToScreen( hwnd_mat, (POINT*)&dest_rec.left );
 ClientToScreen( hwnd_mat, (POINT*)&dest_rec.right );
 
 dx = src_rec.right;
 dy = src_rec.bottom;

 set_matrix_2d(dx,dy);
 set_matrix_view(0,0,dx,dy);

 blend_off();
 
 text_set_num(1);
 text_off(GL_TEXTURE_1D);
 text_off(GL_TEXTURE_2D);

 p_text = (druh) ? p_cnf->p_text+text : p_cnf->p_lightmap+text;
 text_set_num(0);
 text_on(p_text->typ);
 text_set_blok(0,BLOK_MODULACE); 
 text_set(p_text->text,p_text->typ); 
 
 typ = p_cnf->p_text[text].typ;
  
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBegin(GL_TRIANGLE_STRIP);
     glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 1);
     glColor3f(1,1,1);
     glVertex3f(0.0f,0.0f,0.0f);
     
     glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 0);
     glColor3f(1,1,1);
     glVertex3f(0.0f,(float)dy,0.0f);     
     
     glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 1);
     glColor3f(1,1,1); 
     glVertex3f((float)dx,0.0f,0.0f);     
     
     glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1, 0);
     glColor3f(1,1,1); 
     glVertex3f((float)dx,(float)dy,0.0f);
   glEnd();  
 glFlush();

 SwapBuffers(hDC); 

 // Tiskni info o texture
 //glText cislo

 glGetTexLevelParameteriv(typ,0,GL_TEXTURE_WIDTH,&n);
 doe_printf(hDlg,IDC_EDIT1,"%d",n);
 glGetTexLevelParameteriv(typ,0,GL_TEXTURE_HEIGHT,&n);
 doe_printf(hDlg,IDC_EDIT8,"%d",n);
 glGetTexLevelParameteriv(typ,0,GL_TEXTURE_INTERNAL_FORMAT,&n);
 if(cti_texture_format(n, pom))
   doe_printf(hDlg,IDC_EDIT9,pom);
 else
   doe_printf(hDlg,IDC_EDIT9,"%d",n); 
 glGetTexLevelParameteriv(typ,0,GL_TEXTURE_BORDER,&n);
 doe_printf(hDlg,IDC_EDIT10,"%d",n);
 glGetTexLevelParameteriv(typ,0,GL_TEXTURE_RED_SIZE,&n);
 doe_printf(hDlg,IDC_EDIT11,"%d",n);
 glGetTexLevelParameteriv(typ,0,GL_TEXTURE_GREEN_SIZE,&n);
 doe_printf(hDlg,IDC_EDIT12,"%d",n);
 glGetTexLevelParameteriv(typ,0,GL_TEXTURE_BLUE_SIZE,&n);
 doe_printf(hDlg,IDC_EDIT13,"%d",n);
 glGetTexLevelParameteriv(typ,0,GL_TEXTURE_ALPHA_SIZE,&n);
 doe_printf(hDlg,IDC_EDIT20,"%d",n);
 glGetTexLevelParameteriv(typ,0,GL_TEXTURE_LUMINANCE_SIZE,&n);
 doe_printf(hDlg,IDC_EDIT21,"%d",n);
 glGetTexLevelParameteriv(typ,0,GL_TEXTURE_INTENSITY_SIZE,&n);
 doe_printf(hDlg,IDC_EDIT22,"%d",n);
 glGetTexLevelParameteriv(typ,0,GL_TEXTURE_COMPONENTS,&n);
 doe_printf(hDlg,IDC_EDIT23,"%d",n);

 doe_printf(hDlg,IDC_EDIT41,"%d",p_text->flag);
 

 text_set_num(0);
 text_off(GL_TEXTURE_1D);
 text_off(GL_TEXTURE_2D);
 
 ret_matrix_2d();
 set_matrix_view(p_cnf->wx, p_cnf->wy, p_cnf->wxres, p_cnf->wyres);

 if(!wglMakeCurrent(hwconf.hDC, hwconf.hRC))
   chyba("WMC");
 if(!ReleaseDC(hwnd_mat,hDC))
   kprintf(1,"Chyba uvolneni hDC");
  
 return(0);
}


BOOL CALLBACK doe_callback_texture_list( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDC_LIST1: {// texture list
          if(HIWORD(wParam) == LBN_DBLCLK || HIWORD(wParam) == LBN_SELCHANGE) {
            doe_zobraz_texturu(p_cnf, SendMessage((HWND)lParam, LB_GETITEMDATA,SendMessage((HWND)lParam,LB_GETCURSEL,0,0),0),(byte)tmp.i[0],hDlg);
          }
          break;
        }
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {          
          EndDialog( hDlg, TRUE );
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {      
      HWND htext = GetDlgItem(hDlg,IDC_LIST1);
      int  i;
      SendMessage(htext, LB_RESETCONTENT, 0, 0);
      if(tmp.i[0]) {
        for(i = 0; i < MAX_EDIT_TEXTUR; i++) {
          if(p_cnf->p_text[i].jmeno[0])        
            SendMessage(htext, LB_SETITEMDATA, SendMessage( htext, LB_ADDSTRING, 0,(LPARAM)p_cnf->p_text[i].jmeno),i);
        } 
      }
      else {
        for(i = 0; i < MAX_RAY_TEXTUR; i++) {
          if(p_cnf->p_lightmap[i].jmeno[0])
            SendMessage(htext, LB_SETITEMDATA, SendMessage( htext, LB_ADDSTRING, 0,(LPARAM)p_cnf->p_lightmap[i].jmeno),i);
        }
      }
      SendMessage(htext,LB_SETCURSEL,0,0);
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}


void doe_texture_list(K_EDITOR *p_cnf, int druh)
{
  tmp.i[0] = druh;
  DialogBox( hinst, MAKEINTRESOURCE(IDD_TEXTURE_SCANER), hwnd_aplikace, doe_callback_texture_list);
}


BOOL CALLBACK doe_callback_3ds_importer( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {
          tmp.i[0] = IsDlgButtonChecked( hDlg, IDC_CHECK1);
          tmp.i[1] = IsDlgButtonChecked( hDlg, IDC_RADIO1);
          tmp.i[2] = IsDlgButtonChecked( hDlg, IDC_CHECK2);
          tmp.i[3] = IsDlgButtonChecked( hDlg, IDC_CHECK3);
          EndDialog( hDlg, TRUE );
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {      
      CheckDlgButton(hDlg,IDC_CHECK1,tmp.i[0]);
      CheckDlgButton(hDlg,IDC_RADIO1,tmp.i[1]);
      CheckDlgButton(hDlg,IDC_RADIO2,!tmp.i[1]);
      CheckDlgButton(hDlg,IDC_CHECK2,tmp.i[2]);
      CheckDlgButton(hDlg,IDC_CHECK3,tmp.i[3]);
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

/* Druh 0 = static 
        1 = dynamic
*/
void doe_3ds_importer(byte *p_key, byte *p_druh, byte *p_rozloz, byte *p_kamera)
{
  tmp.i[0] = (int)*p_key;
  tmp.i[1] = (int)*p_druh;
  tmp.i[2] = (int)*p_rozloz;
  tmp.i[3] = (int)*p_kamera;
  if(DialogBox( hinst, MAKEINTRESOURCE(IDD_3DS_IMPORT), hwnd_aplikace, doe_callback_3ds_importer)) {
    *p_key = (byte)tmp.i[0];
    *p_druh = (byte)tmp.i[1];
    *p_rozloz = (byte)tmp.i[2];
    *p_kamera = (byte)tmp.i[3];
  }
}


typedef struct _DOLNI_SLIDER {

  int pouzit;      // je aktivni ?
  
  int min;         // rozsah key-framu
  int max;
  int akt;         // vybrana pozice
  
  byte popis[200]; // popis slideru (co je to zac - svetla, kamery, ...)
  
  HWND hDlg;       // dialog slideru
  HWND hDlg_dialog;

  int (*p_fce)(int);// pos-callback funkce

} DOLNI_SLIDER;

static DOLNI_SLIDER dsl;

long CALLBACK doe_callback_konzole(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{  
  word pozice;
  byte pom[200];
  int  min,sec,des;
  
  switch(message) {
    
    case WM_COMMAND: 
      {
        switch(LOWORD(wParam)) {
          case IDC_BUTTON4:
            // .. upravovaci funkci pro klicove animace
            ddw("Test");
            break;
        }
      }
      break;
    
    case WM_HSCROLL:
      if(LOWORD(wParam) == TB_THUMBPOSITION || LOWORD(wParam) == TB_THUMBTRACK) {
        pozice = HIWORD(wParam);
      } else  {
        pozice = (word)SendMessage((HWND)lParam,TBM_GETPOS,0,0);
      }
      if(dsl.p_fce)
        dsl.p_fce((int)pozice);
      
      dsl.akt = pozice;
      SetDlgItemText(dsl.hDlg_dialog, IDC_EDIT9, itoa((int)pozice,pom,10));      
      doe_sec_to_time((dsl.akt-dsl.min)/20.0f,&des, &sec, &min);
      sprintf(pom,"%.2d:%.2d:%.2d",min,sec,des);
      SetDlgItemText( hwnd, IDC_EDIT11, pom);
      
      break;
  }
  return DefWindowProc( hwnd, message, wParam, lParam );
}

void doe_hs_init(void)
{
  dsl.hDlg = GetDlgItem(hwnd_konzole_dialog,IDC_SLIDER1);
  dsl.hDlg_dialog = hwnd_konzole_dialog;
}

/* Nastavi rozsah dolniho slideru + text
*/
void doe_hs_nastav(int min, int max, int akt, byte *p_text,...)
{
  va_list argumenty;
  byte pom[500];

  va_start(argumenty,p_text);
  vsprintf(pom,p_text,argumenty);
  va_end(argumenty);
  
  SetDlgItemText(dsl.hDlg_dialog, IDC_STATIC1, pom);
  SetDlgItemText(dsl.hDlg_dialog, IDC_EDIT1, itoa(min,pom,10));
  SetDlgItemText(dsl.hDlg_dialog, IDC_EDIT9, itoa(akt,pom,10));
  SetDlgItemText(dsl.hDlg_dialog, IDC_EDIT10,itoa(max,pom,10));

  SendMessage(dsl.hDlg,TBM_SETRANGE,1,MAKELPARAM(min,max));
  SendMessage(dsl.hDlg,TBM_SETPOS,1,akt);
}

/* Zrusi slider
*/
void doe_hs_zrus(void)
{
  byte pom[200];
  SetDlgItemText(dsl.hDlg_dialog, IDC_STATIC1, "Nic");
  SetDlgItemText(dsl.hDlg_dialog, IDC_EDIT1, itoa(0,pom,10));
  SetDlgItemText(dsl.hDlg_dialog, IDC_EDIT9, itoa(0,pom,10));
  SetDlgItemText(dsl.hDlg_dialog, IDC_EDIT10,itoa(0,pom,10));
  dsl.p_fce = NULL;
}

/* Nastavi pozici dolniho slideru
*/
void doe_hs_setpos(int pos)
{ 
  byte pom[200];
  SendMessage(dsl.hDlg,TBM_SETPOS,1,pos);
  SetDlgItemText(dsl.hDlg_dialog, IDC_EDIT9, itoa(pos,pom,10));
}

/* Cte pozici slideru
*/
int doe_hs_getpos(void)
{
  return(SendMessage(dsl.hDlg,TBM_GETPOS,0,0));
}

/* Nahodi znacku slideru
*/
void doe_hs_setmark(int pozice)
{
  SendMessage(dsl.hDlg,TBM_SETTIC,0,pozice);
}

/* Zrusi znacku slideru
*/
void doe_hs_delmark(void)
{
  SendMessage(dsl.hDlg,TBM_CLEARTICS,1,0);
}

/* Zaregistruje funkci co se vola pri posunu dolniho slideru
*/
void doe_hs_posfce(int (*p_fce)(int))
{
  dsl.p_fce = p_fce;
}

void doe_okno_cti(int *p_min_x, int *p_min_y, int *p_max_x, int *p_max_y)
{
  RECT rect;
  GetWindowRect(hwnd_plocha,&rect);
  *p_min_x = 0;
  *p_min_y = 0;
  *p_max_x = rect.right - rect.left;
  *p_max_y = rect.bottom - rect.top;
}

void doe_prikaz_vypis_group(K_EDITOR *p_cnf)
{
  OBJ_VERTEX *p_vert;
  int i,k,o,v;

  if(p_cnf->groupnum) {
    for(i = 0; i < p_cnf->groupnum; i++) {
      k = p_cnf->p_group[i].k;
      o = p_cnf->p_group[i].o;
      v = p_cnf->p_group[i].v;
      
      p_vert = p_cnf->p_kont[k]->p_obj[o]->p_vertex+v;
      
      kprintf(1,"k[%d] o[%d] v[%d] [%s] - [%.2f,%.2f,%.2f]",k,o,v,p_cnf->p_kont[k]->p_obj[o]->jmeno,p_vert->x,p_vert->y,p_vert->z);
      kprintf(1,"   Difusni - RGBA [%.2f,%.2f,%.2f,%.2f]",p_vert->dr,p_vert->dg,p_vert->db,p_vert->da);
      kprintf(1,"   Spekular- RGBA [%.2f,%.2f,%.2f,%.2f]",p_vert->sr,p_vert->sg,p_vert->sb,1.0f);
      kprintf(1,"  MDifusni - RGBA [%.2f,%.2f,%.2f,%.2f]",p_vert->mdr,p_vert->mdg,p_vert->mdb,p_vert->mda);
      kprintf(1,"  MSpekular- RGBA [%.2f,%.2f,%.2f,%.2f]",p_vert->msr,p_vert->msg,p_vert->msb,1.0f);
      kprintf(1,"  T1 [%f,%f] T2 [%f,%f]",p_vert->tu1,p_vert->tv1,p_vert->tu2,p_vert->tv2);      
    }
  }
  doe_konzole_max(p_cnf);
}

BOOL CALLBACK doe_callback_podlozka_levelu( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {
          tmp.f[0] = doe_vyber_kolonku_float(hDlg,IDC_EDIT1);
          EndDialog( hDlg, TRUE );
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {
      doe_printf(hDlg,IDC_EDIT1,"%f",tmp.f[0]);
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_podlozka_levelu(K_EDITOR *p_cnf, float *p_vyska)
{
  tmp.f[0] = *p_vyska;
  if(DialogBox( hinst, MAKEINTRESOURCE(IDD_PODLOZKA),
                hwnd_aplikace, doe_callback_podlozka_levelu)) {
    *p_vyska = tmp.f[0];
    return(TRUE);
  }
  return(FALSE);
}



BOOL CALLBACK doe_callback_obruba_levelu( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {
          tmp.f[0] = doe_vyber_kolonku_float(hDlg,IDC_EDIT1);
          tmp.f[1] = doe_vyber_kolonku_float(hDlg,IDC_EDIT8);
          tmp.f[2] = doe_vyber_kolonku_float(hDlg,IDC_EDIT9);          
          tmp.i[0] = IsDlgButtonChecked(hDlg,IDC_CHECK1);          
          EndDialog( hDlg, TRUE );
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {      
      doe_printf(hDlg,IDC_EDIT1,"%f",tmp.f[0]);
      doe_printf(hDlg,IDC_EDIT8,"%f",tmp.f[1]);
      doe_printf(hDlg,IDC_EDIT9,"%f",tmp.f[2]);      
      CheckDlgButton(hDlg,IDC_CHECK1,tmp.i[0]);
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_obruba_levelu(K_EDITOR *p_cnf, float *p_vyska_obruby, float *p_vyska_zeme, float *p_sirka, int *p_text_x)
{
  tmp.f[0] = *p_vyska_obruby;
  tmp.f[1] = *p_vyska_zeme;
  tmp.f[2] = *p_sirka;
  tmp.i[0] = *p_text_x;
  if(DialogBox( hinst, MAKEINTRESOURCE(IDD_OBRUBA),hwnd_aplikace, doe_callback_obruba_levelu)) {
    *p_vyska_obruby = tmp.f[0];
    *p_vyska_zeme = tmp.f[1];
    *p_sirka = tmp.f[2];
    *p_text_x = tmp.i[0];    
    return(TRUE);
  }
  return(FALSE);
}



BOOL CALLBACK doe_callback_teselace_objektu( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {
          tmp.i[0] = doe_vyber_kolonku_int(hDlg,IDC_EDIT1);
          tmp.i[1] = doe_vyber_kolonku_int(hDlg,IDC_EDIT8);
          EndDialog( hDlg, TRUE );
          break;
        }
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: {      
      doe_printf(hDlg,IDC_EDIT1,"%d",tmp.i[0]);
      doe_printf(hDlg,IDC_EDIT8,"%d",tmp.i[1]);
      break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_teselace_objektu(int *p_x, int *p_y)
{
  tmp.i[0] = *p_x;
  tmp.i[1] = *p_y;
  if(DialogBox( hinst, MAKEINTRESOURCE(IDD_TESELACE), hwnd_aplikace, doe_callback_teselace_objektu)) {
    *p_x = tmp.i[0];
    *p_y = tmp.i[1];
    return(TRUE);
  } else
    return(FALSE);
}

/*
__inline void mix(__int16 *bs,  __int16 *db, __int32 x)
{
  __asm {
    mov ecx,x
    mov edi,ds
    mov esi,db
mix_loop:
    mov ax,[edi]
    mov dx,[esi]
    add ax,dx
    jnc neni

  };
for(i=0;i<x;i++)
			if(bs[i] + db[i] > 32767)
				bs[i] = 32767;
			else
				if(bs[i] + db[i] < -32768)
					bs[i] = -32768;
				else
					bs[i] += db[i];
}
*/

/*
typedef struct _FLARE_GEN {

  STATIC_LIGHT   *p_slight; // svetlo ktere to haze
  BOD             p;        // gen-bod
  
  float           start;
  dword           start_color;
  
  float           konec;
  dword           konec_color;

} FLARE_GEN;
*/
void doe_cti_flare_gen_vyber(HWND hDlg, FLARE_GEN *p_gen)
{
  p_gen->start = doe_vyber_kolonku_float(hDlg,IDC_EDIT9);
  p_gen->konec = doe_vyber_kolonku_float(hDlg,IDC_EDIT10);

  p_gen->start_color = doe_vyber_kolonku_int_hexa(hDlg,IDC_EDIT1);
  p_gen->konec_color = doe_vyber_kolonku_int_hexa(hDlg,IDC_EDIT8);

  if(IsDlgButtonChecked(hDlg,IDC_RADIO1)) {
    HWND hCombo2 = GetDlgItem( hDlg, IDC_COMBO1 );
    int i = SendMessage(hCombo2, CB_GETITEMDATA, SendMessage(hCombo2, CB_GETCURSEL, 0, 0 ), 0);    
    p_gen->p_slight = p_cnf->light+i;
    p_gen->p = p_gen->p_slight->p;
  } else {
    p_gen->p.x = doe_vyber_kolonku_float(hDlg,IDC_EDIT2);
    p_gen->p.y = doe_vyber_kolonku_float(hDlg,IDC_EDIT3);
    p_gen->p.z = doe_vyber_kolonku_float(hDlg,IDC_EDIT7);
  }
}

BOOL CALLBACK doe_callback_cti_flare_gen( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, FALSE );
          break;
        case IDOK: {
          doe_cti_flare_gen_vyber(hDlg, tmp.p_pom);
          EndDialog( hDlg, TRUE );
          break;
        }
        case IDC_BUTTON1:
          {
            doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT1);
          }
          break;
        case IDC_BUTTON2:
          {
            doe_vyber_barvu_do_kolonky(hDlg,IDC_EDIT8);
          }
          break;
        default:
          return FALSE;
      }
      break;    
    case WM_INITDIALOG: 
      {
        FLARE_GEN *p_gen = tmp.p_pom;
        doe_printf(hDlg,IDC_EDIT1, "%x",p_gen->start_color);
        doe_printf(hDlg,IDC_EDIT8, "%x",p_gen->konec_color);
        doe_printf(hDlg,IDC_EDIT9, "%f",p_gen->start);
        doe_printf(hDlg,IDC_EDIT10,"%f",p_gen->konec);
        
        doe_printf(hDlg,IDC_EDIT2,"%f",p_gen->p.x);
        doe_printf(hDlg,IDC_EDIT3,"%f",p_gen->p.y);
        doe_printf(hDlg,IDC_EDIT7,"%f",p_gen->p.z);
        
        if(p_gen->p_slight) {
          doe_check_stlac(hDlg,IDC_RADIO1, TRUE);
          doe_check_stlac(hDlg,IDC_RADIO2, FALSE);
        } else {
          doe_check_stlac(hDlg,IDC_RADIO2, TRUE);
          doe_check_stlac(hDlg,IDC_RADIO1, FALSE);
        }
        
        {
          HWND hCombo2;
          int  i,j,vyb = -1;
          
          hCombo2 = GetDlgItem( hDlg, IDC_COMBO1 );
          SendMessage( hCombo2, CB_RESETCONTENT, 0, 0 );
                    
          for(i = 0; i < MAX_EDIT_SVETEL; i++) {
            if(p_cnf->light[i].akt) {
              j = SendMessage( hCombo2, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) p_cnf->light[i].jmeno);
              SendMessage( hCombo2, CB_SETITEMDATA, j, i);              
              if(p_cnf->light+i == p_gen->p_slight)
                vyb = j;
            }
          }
          if(vyb != -1)
            SendMessage(hCombo2, CB_SETCURSEL, vyb, 0 );
        }
        break;
      }
    case WM_DESTROY:
      EndDialog( hDlg, FALSE );
      break; 
    default:
      return FALSE;
  }
  return TRUE;
}

int doe_cti_flare_gen(HWND hwnd, FLARE_GEN *p_gen)
{
  tmp.p_pom = p_gen;  
  return(DialogBox( hinst, MAKEINTRESOURCE(IDD_FLARE_GEN), hwnd, doe_callback_cti_flare_gen));
}

