/* K-file - komati soubory
*/

#ifndef __KFILE_H__
#define __KFILE_H__

typedef struct _KFILE
{
  FFILE fz;
} KFILE;

inline int efile(char *p_file)
{
  return (file_exists(NULL, p_file));
}

inline int efiledir(char *p_dir, char *p_file)
{
  char filename[MAX_FILENAME];
  return efile(construct_path(filename, MAX_FILENAME, 2, p_dir, p_file));
}

inline int kefile(char *p_dir, char *p_file)
{
  return efiledir(p_dir, p_file);
}

inline int kread(void *buffer, size_t size, size_t count, KFILE * f)
{
  return (ffread(buffer, size, count, f->fz));
}

inline int kwrite(void *buffer, size_t size, size_t count, KFILE * f)
{
  return (ffwrite(buffer, size, count, f->fz));
}

inline int kclose(KFILE * f)
{
  ffclose(f->fz);
  free(f);
  return (TRUE);
}

inline KFILE *kopen(char *p_dir, char *p_file, char *p_mod)
{
  KFILE *p_fl = NULL;
  char filename[MAX_FILENAME];

  p_fl = (KFILE *) mmalloc(sizeof(p_fl[0]));
  if (!p_fl) {
    return (NULL);
  }
  memset(p_fl, 0, sizeof(p_fl[0]));

  construct_path(filename, MAX_FILENAME, 2, p_dir, p_file);
  p_fl->fz = ffopen(filename, p_mod);
  if (!p_fl->fz) {
    free(p_fl);
    return (NULL);
  }

  return (p_fl);
}

inline char *kgets(char *string, int n, KFILE * f)
{
  return (ffgets(string, n, f->fz));
}

inline int ktell(KFILE * f)
{
  return (fftell(f->fz));
}

inline int kseek(KFILE * f, long offset, int origin)
{
  return (ffseek(f->fz, offset, origin));
}

inline int keof(KFILE * f)
{
  return (ffeof(f->fz));
}

#endif
