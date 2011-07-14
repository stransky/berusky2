#ifndef __MMALLOC_H
#define __MMALLOC_H

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

  void tiskni_chybu(int line, char *p_file, char *p_text, ...);

#define STC            __LINE__,__FILE__,NULL
#define chyba(text)    tiskni_chybu(__LINE__,__FILE__,text)
#define chybat         tiskni_chybu
#define chyba_be(text) tiskni_chybu_bez_exit(__LINE__,__FILE__,text)

  inline void *mmalloc(int velikost)
  {
    //assert(velikost);
    if (velikost) {
      void *p_mem = malloc(velikost);
        assert(p_mem);
        memset(p_mem, 0, velikost);
        return (p_mem);
    }
    else
    {
      return (NULL);
    }
  }

#ifdef __cplusplus
}
#endif

#endif
