#ifndef __MMALLOC_H
#define __MMALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

void tiskni_chybu(int line, char *p_file, HWND hwnd,byte *p_text,...);

#define STC            __LINE__,__FILE__,NULL
#define chyba(text)    tiskni_chybu(__LINE__,__FILE__,NULL,text)
#define chybat         tiskni_chybu
#define chyba_be(text) tiskni_chybu_bez_exit(__LINE__,__FILE__,NULL,text)

#define mtest(p_mem) { if(!p_mem) { chyba((unsigned char *)"Pamet"); }}

__inline void * mmalloc(int velikost)
{
  //assert(velikost);
  if(velikost) {
    void *p_mem = malloc(velikost);
    mtest(((unsigned char *)p_mem));
    memset(p_mem,0,velikost);
    return(p_mem);
  } else {
    return(NULL);
  }
}

#ifdef __cplusplus
}
#endif

#endif