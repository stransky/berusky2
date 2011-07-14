/* K-file - komati soubory
*/

#ifndef __KFILE_H__
#define __KFILE_H__

typedef struct _KFILE
{

  FILE *f;
  FFILE fz;
  APAK_HANDLE *pAHandle;

} KFILE;

inline int efile(char *p_file)
{
  return (file_exists(NULL, p_file));
}

// pokud ses preplej v "adresari" kde mas soboury.
// (pokud si otviral paky zpusobem, jakym sem ti rek, tak ano).
// Tak musis pouzit afindfirst, next, close .. jinak to nejde
inline int efileAPAK(APAK_HANDLE * pAHandle, char *p_file)
{
  struct _finddata_t fileinfo;
  long handle = afindfirst(pAHandle, (char *) p_file, &fileinfo);

  if (handle != -1) {
    afindclose(handle);
    return (TRUE);
  }
  else {
    return (FALSE);
  }
}

inline int kefile(APAK_HANDLE * pAHandle, char *p_file)
{
  if (pAHandle) {
    return (efileAPAK(pAHandle, p_file));
  }
  else {
    return (efile(p_file));
  }
}

inline int kread(void *buffer, size_t size, size_t count, KFILE * f)
{
  if (f->pAHandle) {
    return (aread(buffer, size, count, f->f));
  }
  else {
    return (ffread(buffer, size, count, f->fz));
  }
}

inline int kwrite(void *buffer, size_t size, size_t count, KFILE * f)
{
  if (f->pAHandle) {
    return (awrite(buffer, size, count, f->f));
  }
  else {
    return (ffwrite(buffer, size, count, f->fz));
  }
}

inline int kclose(KFILE * f)
{
  if (f->pAHandle) {
    aclose(f->f);
  }
  else {
    ffclose(f->fz);
  }
  free(f);
  return (TRUE);
}

inline KFILE *kopen(APAK_HANDLE * pHandle, char *p_file, char *p_mod)
{
  KFILE *p_fl = NULL;

  p_fl = (KFILE *) mmalloc(sizeof(p_fl[0]));
  if (!p_fl) {
    return (NULL);
  }
  memset(p_fl, 0, sizeof(p_fl[0]));
  p_fl->pAHandle = pHandle;

  if (pHandle) {
    p_fl->f = aopen(pHandle, p_file, p_mod);
    if (!p_fl->f) {
      free(p_fl);
      return (NULL);
    }
  }
  else {
    p_fl->fz = ffopen(p_file, p_mod);
    if (!p_fl->fz) {
      free(p_fl);
      return (NULL);
    }
  }
  return (p_fl);
}

inline char *kgets(char *string, int n, KFILE * f)
{
  if (f->pAHandle) {
    return (agets(string, n, f->f));
  }
  else {
    return (ffgets(string, n, f->fz));
  }
}

inline int ktell(KFILE * f)
{
  if (f->pAHandle) {
    return (atell(f->f));
  }
  else {
    return (fftell(f->fz));
  }
}

inline int kseek(KFILE * f, long offset, int origin)
{
  if (f->pAHandle) {
    return (aseek(f->f, offset, origin));
  }
  else {
    return (ffseek(f->fz, offset, origin));
  }
}

inline int keof(KFILE * f)
{
  if (f->pAHandle) {
    return (aeof(f->f));
  }
  else {
    return (ffeof(f->fz));
  }
}

#endif
