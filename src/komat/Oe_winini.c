/*
  Klasika - editor objektu & sceny
*/
#define  WM_MOUSEWHEEL                   0x020A

#include "mss_on.h"
#include <windows.h>
#include <winuser.h>
#include <commctrl.h>
#include <stdio.h>
#include <math.h>
#include <direct.h>
#include <float.h>
#include <assert.h>
#include <time.h>
#include "objekt_editor_all.h"

//#pragma comment(lib,"mss.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dinput.lib")

#define  V_OKNE 1

dword      system_timer = 0;
dword      system_kurzor = 0;
byte       akt_editor = 1;
dword      karmin_aktivni = 1;
HWND       hwnd_plocha;
HWND       hwnd_aplikace;
HWND       hwnd_konzole;
HWND       hwnd_horni_lista;
HWND       hwnd_horni_lista_dialog;
HWND       hwnd_dolni_lista;
HWND       hwnd_dolni_lista_dialog;
HWND       hwnd_bocni_lista;
HWND       hwnd_bocni_lista_dialog;
HWND       hwnd_konzole;
HWND       hwnd_konzole_dialog;
HINSTANCE  hinst;

HWND       hwnd_bocni_lista2;
HMENU      handle_menu[10];

byte       work_dir[200];
byte       ini_file[300];

int        render_povolen;

void go_fraktaly_3d(void);
void go_object_editor(char *p_edit_kont, char *p_text_dir);
void go_scene_editor(void);
void doe_prekresli_hlavni_okno(void);
void doe_proved_vybr_prikaz(void);
void soe_prekresli_hlavni_okno(void);
void soe_proved_prikaz_menu(UINT prikaz, int rychle);
void soe_proved_vybr_prikaz(void);
dword doe_get_vybr_prikaz(void);
dword soe_get_vybr_prikaz(void);
DLGTEMPLATE * load_dialog_resource(byte *p_res);
int  efile(byte *p_file);;
void kprintf(byte log, byte *p_text,...);
void doe_hs_init(void);

//#define VYSKA_HORNI_LISTY 21
#define VYSKA_DOLNI_LISTY 21
#define VYSKA_KONZOLE     70
#define SIRKA_BOCNI_LISTY 220


__inline void nacti_polohu_mysi(WPARAM wParam, LPARAM lParam)
{                                     
  mi.dx = LOWORD(lParam)- mi.x;
  mi.dy = HIWORD(lParam)- mi.y;
  mi.x += mi.dx;                      
  mi.y += mi.dy;                      
}                                     

__inline void nacti_tlacitka_mysi(WPARAM wParam, LPARAM lParam)
{                                     
  mi.t1 = LOWORD(wParam) & MK_LBUTTON;
  mi.t2 = LOWORD(wParam) & MK_RBUTTON;
}                                     

long CALLBACK WindowProc_master(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{  
  switch(message) {
/*
#ifdef VYHRADNI_REZIM_MYSI
  case WM_ACTIVATE:
    if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) {
      system_kurzor = FALSE;
    }
    if(LOWORD(wParam) == WA_INACTIVE) {
      system_kurzor = TRUE;
    }
    break;    
#endif   
*/
  case WM_ACTIVATEAPP:
    if(wParam) {
      karmin_aktivni = TRUE;
      system_kurzor = FALSE;
    } else {
      karmin_aktivni = FALSE;
      system_kurzor = TRUE;
    }
    break;
  case WM_CLOSE:
    PostQuitMessage(0);
    return(FALSE);
  case WM_DESTROY:
    PostQuitMessage(0);
    return(FALSE);
  case WM_KEYDOWN:
    _a_klavesa = MapVirtualKey(wParam,2);
    key[MapVirtualKey(wParam,0)] = TRUE;
    break;
  case WM_KEYUP:    
    key[MapVirtualKey(wParam,0)] = FALSE;
    break;
  case WM_COMMAND:
    doe_proved_prikaz_menu(LOWORD(wParam),FALSE,TRUE);
    break;
  case WM_TIMER:
    system_timer++;
    break;
  }
 return DefWindowProc( hwnd, message, wParam, lParam );
}

HWND otevri_okno_master(HINSTANCE handle_aplikace)
{
  WNDCLASS    wc;
  HWND        hwnd;

  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc_master;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = handle_aplikace;
  wc.hIcon = LoadIcon(handle_aplikace, MAKEINTRESOURCE(IDI_ICON3));
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = GetStockObject(GRAY_BRUSH);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = "3D Editor";
  RegisterClass(&wc);

  // vytvoreni okna
  hwnd = CreateWindow(
    "3D Editor",
    "3D Editor",
    WS_OVERLAPPEDWINDOW|WS_MAXIMIZE|WS_VISIBLE,
    0,
    0,
    GetSystemMetrics(SM_CXSCREEN),
    GetSystemMetrics(SM_CYSCREEN),
    NULL,
    handle_menu[MENU_MESH_EDITOR],
    handle_aplikace,
    NULL);


  if (!hwnd) return NULL;

  ShowWindow(hwnd, SW_SHOWMAXIMIZED);
  UpdateWindow(hwnd);

  return(hwnd);
}

/*
  Obsluha plochy
*/
long CALLBACK WindowProc_plocha(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{  
  switch(message) {
    case WM_MOUSEMOVE: {      
      if(!hwconf.fullscreen)
        nacti_polohu_mysi(wParam, lParam);
      if(GetFocus() != hwnd)
        SetFocus(hwnd);
      break;
    }

    case WM_MOUSEWHEEL: {
      oe_prikaz_scena_zoom((signed short)HIWORD(wParam) > 0 ? 1 : -1);
      break;
    }

    case WM_LBUTTONUP:
      if(!hwconf.fullscreen)
        nacti_tlacitka_mysi(wParam, lParam);
      break;
    case WM_LBUTTONDOWN: {
      if(!hwconf.fullscreen)
        nacti_tlacitka_mysi(wParam, lParam);
      if(GetFocus() != hwnd)
        SetFocus(hwnd);
      break;
    }
    case WM_RBUTTONDOWN:
      if(!hwconf.fullscreen)
        nacti_tlacitka_mysi(wParam, lParam);
      if(GetFocus() != hwnd)
        SetFocus(hwnd);
      break;
    case WM_SETCURSOR:
      if(!system_kurzor) {
        SetCursor(NULL);
        return TRUE;
      }
      if(GetFocus() != hwnd)
        SetFocus(hwnd);
      break;
    case WM_CLOSE:
      SendMessage(hwnd_aplikace,WM_CLOSE,0,0);
      PostQuitMessage(0);
      return(FALSE);
    case WM_DESTROY:
      SendMessage(hwnd_aplikace,WM_DESTROY,0,0);
      PostQuitMessage(0);
      return(FALSE);
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,0);
      key[_a_klavesa] = TRUE;
      break;
    case WM_KEYUP:
      key[MapVirtualKey(wParam,0)] = FALSE;
      break;
    case WM_PAINT:
      if(render_povolen)
        doe_prekresli_hlavni_okno();
      break;      

    case WM_RBUTTONUP: {
      if(hwconf.fullscreen)
        break;
      if(wParam & MK_SHIFT)
        doe_proved_prikaz_menu(doe_get_vybr_prikaz(),TRUE,TRUE);
      else {
        system_kurzor = 1;
        nacti_tlacitka_mysi(wParam, lParam);
        if(akt_editor == 1)
          TrackPopupMenuEx(GetSubMenu(handle_menu[MENU_MESH_POPUP],0),TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,mi.x, mi.y, hwnd, NULL );
        else
          TrackPopupMenuEx(GetSubMenu(handle_menu[MENU_GAME_POPUP],0),TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,mi.x, mi.y, hwnd, NULL );
        system_kurzor = 0;
      }
      break; 
    }

    case WM_COMMAND:
      doe_proved_prikaz_menu(LOWORD(wParam),FALSE,TRUE);
      break;
  }
  return DefWindowProc( hwnd, message, wParam, lParam );
}


HWND otevri_okno_plocha(HINSTANCE handle_aplikace, HWND hwnd_otec)
{
  WNDCLASS    wc;
  HWND        hwnd;
  RECT        r;
  int x,y,dx,dy;

  GetClientRect(hwnd_otec,&r);
  x = 0; y = 0;
  dx = r.right-SIRKA_BOCNI_LISTY;
  dy = r.bottom-VYSKA_DOLNI_LISTY-VYSKA_KONZOLE;

  wc.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
  wc.lpfnWndProc = WindowProc_plocha;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = handle_aplikace;  
  wc.hIcon = LoadIcon(handle_aplikace, MAKEINTRESOURCE(IDI_ICON3));
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);  
  wc.hbrBackground = GetStockObject(GRAY_BRUSH);  
  wc.lpszMenuName = NULL;
  wc.lpszClassName = "Plocha";
  RegisterClass(&wc);

  hwnd = CreateWindowEx(
    FALSE,
    "Plocha",
    "Plocha",
    WS_CHILD|WS_VISIBLE,
    x,y,
    dx,
    dy,
    hwnd_otec,
    NULL,
    handle_aplikace,
    NULL);

  if (!hwnd) return NULL;

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);

  return(hwnd);
}



/*
  Bocni lista
*/
long CALLBACK WindowProc_bocni_lista(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{  
  switch(message) {
    case WM_CLOSE:
      PostQuitMessage(0);
      return(FALSE);
    case WM_DESTROY:
      PostQuitMessage(0);
      return(FALSE);
  }
  return DefWindowProc( hwnd, message, wParam, lParam );
}

long CALLBACK WindowProc_bocni_lista_dialog(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{  
  switch(message) {
    case WM_CLOSE:
      PostQuitMessage(0);
      return(FALSE);
    case WM_DESTROY:
      PostQuitMessage(0);
      return(FALSE);
  }
  return DefWindowProc( hwnd, message, wParam, lParam );
}

HWND otevri_bocni_listu(HINSTANCE handle_aplikace, HWND hwnd_otec)
{
  WNDCLASS    wc;
  HWND        hwnd;
  RECT        r;
  int x,y,dx,dy;

  GetClientRect(hwnd_otec,&r);
  x = r.right-SIRKA_BOCNI_LISTY-1;
  y = 0;
  dx = SIRKA_BOCNI_LISTY;
  dy = r.bottom-VYSKA_DOLNI_LISTY+1;
  
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc_bocni_lista;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = handle_aplikace;
  wc.hIcon = LoadIcon(handle_aplikace, MAKEINTRESOURCE(IDI_ICON3));
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = GetStockObject(COLOR_BACKGROUND);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = "Bocni lista";
  RegisterClass(&wc);

  hwnd = CreateWindowEx(
    FALSE,
    "Bocni lista",
    "Bocni lista",
    WS_CHILD,
    x,y,
    dx,
    dy,
    hwnd_otec,
    NULL,
    handle_aplikace,
    NULL);

  if (!hwnd) return NULL;

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);

  //hwnd to je to kam budu sipat ten dialog
  hwnd_bocni_lista_dialog = doe_aktivace_bocni_listy_mesh_editor(hwnd); 
  return(hwnd);
}

long CALLBACK doe_callback_konzole(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

/*
  Otevri konzoli
*/
HWND otevri_konzoli(HINSTANCE handle_aplikace, HWND hwnd_otec)
{
  WNDCLASS    wc;
  HWND        hwnd;
  RECT        r;
  int x,y,dx,dy;

  GetClientRect(hwnd_otec,&r);
  x = 0;
  y = r.bottom-VYSKA_DOLNI_LISTY-VYSKA_KONZOLE;
  dx = r.right-SIRKA_BOCNI_LISTY;
  dy = VYSKA_KONZOLE;
  
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc_bocni_lista;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = handle_aplikace;
  wc.hIcon = NULL;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = GetStockObject(COLOR_BACKGROUND);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = "Konzole";
  RegisterClass(&wc);

  hwnd = CreateWindowEx(
    FALSE,
    "Konzole",
    "Konzole",
    WS_CHILD,
    x,y,
    dx,
    dy,
    hwnd_otec,
    NULL,
    handle_aplikace,
    NULL);

  if (!hwnd) return NULL;

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);
  
  hwnd_konzole_dialog = CreateDialog(hinst, MAKEINTRESOURCE(IDD_KONZOLE), hwnd, doe_callback_konzole);
  return(hwnd);
}

/*
  Dolni lista
*/
long CALLBACK WindowProc_dolni_lista(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{  
  switch(message) {
    case WM_CLOSE:
      PostQuitMessage(0);
      return(FALSE);
    case WM_DESTROY:
      PostQuitMessage(0);
      return(FALSE);
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam,2);
      key[MapVirtualKey(wParam,0)] = TRUE;
      break;
  }
  return DefWindowProc( hwnd, message, wParam, lParam );
}

HWND otevri_dolni_listu(HINSTANCE handle_aplikace, HWND hwnd_otec)
{
  WNDCLASS wc;
  HWND     hwnd;
  RECT     r;
  int x,y,dx,dy;

  GetClientRect(hwnd_otec,&r);
  x = 0;
  y = r.bottom-VYSKA_DOLNI_LISTY;
  dx = r.right;
  dy = VYSKA_DOLNI_LISTY;

  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc_dolni_lista;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = handle_aplikace;
  wc.hIcon = LoadIcon(handle_aplikace, MAKEINTRESOURCE(IDI_ICON3));
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = GetStockObject(COLOR_BACKGROUND);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = "Dolni lista";
  RegisterClass(&wc);

  hwnd = CreateWindowEx(
    FALSE,
    "Dolni lista",
    "Dolni lista",
    WS_CHILD|WS_VISIBLE/*|WS_DLGFRAME*/,
    x,y,
    dx,
    dy,
    hwnd_otec,
    NULL,
    handle_aplikace,
    NULL);

  if (!hwnd) return NULL;

  ShowWindow(hwnd, SW_SHOWNORMAL);
  UpdateWindow(hwnd);

  //hwnd to je to kam budu sipat ten dialog
  hwnd_dolni_lista_dialog = CreateDialog(hinst, MAKEINTRESOURCE(IDD_HORNI_LISTA), hwnd, DefWindowProc);
  return(hwnd);
}

//hwnd_horni_lista = WInit_dolni_lista(hinst,hwnd_aplikace,SW_SHOWNORMAL,"Horni lista","Horni lista",0,0,r.right,VYSKA_HORNI_LISTY,WindowProc_master);

void get_work_dir(byte *p_work_dir, int max)
{
 _getcwd(p_work_dir,max);
}

#define TYP_NIC    0
#define TYP_LIST   1
#define TYP_RENDER 2

char * get_render_file(char *p_cmd, int *p_typ)
{
  char *p_pom;  
  char c;

  *p_typ = TYP_NIC;
  p_pom = strchr(p_cmd,'-');
  if(p_pom) {
    c = tolower(p_pom[1]);
    if(c == 'l') *p_typ = TYP_LIST;
    else if(c == 'r') *p_typ = TYP_RENDER;
    else return(NULL);
    if(p_pom = strchr(p_pom,' ')) {
      p_pom++;
      while(*p_pom) {
        if(*p_pom != ' ')
          return(p_pom);
        p_pom++;
      }
    }
  } 
  return(NULL);
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{ 
 byte pom[500],pom2[500];
 char *p_file;
 int   ren_typ;
  
 InitCommonControls();
  
 hinst = hi;
 get_work_dir(work_dir, 200);

 p_file = get_render_file(lpCmdLine,&ren_typ);

 if(!ren_typ) {
   atexit(konec_uvolni);
 
   handle_menu[MENU_GAME_EDITOR] = LoadMenu(hi,MAKEINTRESOURCE(IDR_MENU_GAME_EDIT));
   handle_menu[MENU_MESH_EDITOR] = LoadMenu(hi,MAKEINTRESOURCE(IDR_MENU_MESH_EDIT));
   handle_menu[MENU_GAME_POPUP] = LoadMenu(hi,MAKEINTRESOURCE(IDR_POPUP_GAME));
   handle_menu[MENU_MESH_POPUP] = LoadMenu(hi,MAKEINTRESOURCE(IDR_POPUP_MESH));
   
   hwnd_aplikace = otevri_okno_master(hi);
   hwnd_plocha = otevri_okno_plocha(hi, hwnd_aplikace);
   hwnd_bocni_lista = otevri_bocni_listu(hi, hwnd_aplikace);
   hwnd_dolni_lista = otevri_dolni_listu(hi, hwnd_aplikace);
   hwnd_konzole =  otevri_konzoli(hi, hwnd_aplikace);
   SetFocus(hwnd_plocha);
 }

 _getcwd(ini_file,500);
 strcat(ini_file,"\\berusky3d.ini");
 if(!efile(ini_file)) {
   kprintf(TRUE,"Pouzivam globalni konfiguraci v berusky3d.ini");
   strcpy(ini_file,"berusky3d.ini");
 } else {
   kprintf(TRUE,"Pouzivam lokalni konfiguraci \"%s\"",ini_file);
 }

 GetPrivateProfileString("editor","log_file","c:\\berusky2_editor.txt",pom,500,ini_file);
 if(!(cnf.bnf.debug_file = fopen(pom,"a")))
   ddw("Nepodarilo se otevrit log. soubor !");
 kprintf(TRUE, "-----------------------Start editor - datum %s cas %s-----------------",
          _strdate(pom),_strtime(pom2));
 
 if(!ren_typ) {
   doe_hs_init();

   nahraj_device_config(ini_file,INI_SEKCE,&hwconf);
   nahraj_universal_device_config(ini_file, "hra", &hwconf);
   if(!grf_start(hinst,hwnd_plocha,ini_file,INI_SEKCE,TRUE))
     chyba("Inicializace");
   go_object_editor(lpCmdLine,"");
   grf_stop(hwnd_plocha);
   
   DestroyWindow(hwnd_aplikace);
   DestroyMenu(handle_menu[MENU_GAME_EDITOR]);
   DestroyMenu(handle_menu[MENU_MESH_EDITOR]);
   DestroyMenu(handle_menu[MENU_GAME_POPUP]);
   DestroyMenu(handle_menu[MENU_MESH_POPUP]); 
 } else {

   render_povolen = FALSE;
   p_cnf->cnf.load_textur = FALSE;
   p_cnf->cnf.load_prvku = FALSE;
   
   oe_konfiguruj_editor(&cnf,INI_FILE,FALSE);

   if(ren_typ == TYP_LIST) {
     go_renderovac(p_file);
   } else {
     go_renderovac_renderuj(p_file,p_file);
   }
 }

 return(TRUE);
}

int spracuj_spravy_full_priorita(void)
{
  MSG  msg;
  while(PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )) {
    TranslateMessage(&msg); 
    DispatchMessage(&msg);
    if(msg.message == WM_QUIT) //return(0);
      oe_konec(&cnf);
  }   
  return(TRUE);
}

int spracuj_spravy_iddle(void)
{
 MSG  msg;
 if(karmin_aktivni) {
   while(PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )) {
     TranslateMessage(&msg); 
     DispatchMessage(&msg);
     if(msg.message == WM_QUIT) //return(0);
       oe_konec(&cnf);
   }   
 } else {
   while(!karmin_aktivni) {
     GetMessage(&msg,NULL,0,0);
     TranslateMessage(&msg); 
     DispatchMessage(&msg);
     if(msg.message == WM_QUIT) //return(0);
       oe_konec(&cnf);
   }
 }
 return(TRUE);
}

int spracuj_spravy(int priorita)
{ 
  if(p_cnf->cnf.editor_priorita)
    spracuj_spravy_full_priorita();
  else
    spracuj_spravy_iddle();
  return(1);
}

void konec_uvolni(void)
{
  grf_stop(hwnd_aplikace);
  DestroyWindow(hwnd_aplikace);
  DestroyMenu(handle_menu[MENU_GAME_EDITOR]);
  DestroyMenu(handle_menu[MENU_MESH_EDITOR]);
  DestroyMenu(handle_menu[MENU_GAME_POPUP]);
  DestroyMenu(handle_menu[MENU_MESH_POPUP]);     
}

void konec(void)
{
  system_kurzor = 1;
  if(MessageBox(hwnd_aplikace,"Opravdu, ale opravdu si pøeješ skonèit ? Vážnì jsi peèlive zvážil vsechny negativní dùsledky unáhleného rozhodnutí ?","Èubèí synu !",MB_ICONASTERISK|MB_YESNO|MB_SYSTEMMODAL) == IDYES) {
    exit(0);
  }
  system_kurzor = 0;
}
