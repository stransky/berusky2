/*
 *        .þÛÛþ þ    þ þÛÛþ.     þ    þ þÛÛÛþ.  þÛÛÛþ .þÛÛþ. þ    þ
 *       .þ   Û Ûþ.  Û Û   þ.    Û    Û Û    þ  Û.    Û.   Û Ûþ.  Û
 *       Û    Û Û Û  Û Û    Û    Û   þ. Û.   Û  Û     Û    Û Û Û  Û
 *     .þþÛÛÛÛþ Û  Û Û þÛÛÛÛþþ.  þþÛÛ.  þþÛÛþ.  þÛ    Û    Û Û  Û Û
 *    .Û      Û Û  .þÛ Û      Û. Û   Û  Û    Û  Û.    þ.   Û Û  .þÛ
 *    þ.      þ þ    þ þ      .þ þ   .þ þ    .þ þÛÛÛþ .þÛÛþ. þ    þ
 *
 * Berusky (C) AnakreoN
 * Martin Stransky <stransky@anakreon.cz> 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
  Utility
  
  TODO -> nacteni velikosti souboru z zlib??
*/

#ifndef __UTILS_H__
#define __UTILS_H__

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <zlib.h>

#define  MAX_FILENAME_LENGTH  200

#if PATH_MAX > MAX_FILENAME_LENGTH
#define  MAX_FILENAME         MAX_FILENAME_LENGTH
#else
#define  MAX_FILENAME         PATH_MAX
#endif

#ifndef  FALSE
#define  FALSE (1!=1)
#endif

#ifndef  TRUE
#define  TRUE  (1==1)
#endif

#ifndef  ERROR
#define  ERROR (-1)
#endif


// -------------------------------------------------------
// file interfaces
// -------------------------------------------------------

typedef class file_name {

  char name[MAX_FILENAME];
  
public:

  void set(const char *p_file)
  {
    strncpy(name,p_file,MAX_FILENAME);
  }

  const char * get(void)
  {
    return(name);
  }

public:
  
  file_name(void)
  {
    name[0] = '\0';
  }

} FILE_NAME;

#define NO_DIR (NULL)

typedef unsigned int t_length;


/* General file interface
*/

char * path_correction(char *p_file, int max_lenght);
char * return_path(const char *p_dir, const char *p_file, char *p_buffer, int max_lenght);
char * return_path_ext(const char *p_file, const char *p_cwd, char *p_buffer, int max_lenght);
char * return_dir(const char *p_file, char *p_buffer, int max_lenght);
char * return_file(const char *p_path, char *p_buffer, int max_lenght);

bool   dir_create(const char *p_dir);
char * dir_home_get(char *p_dir, int max);

bool   file_copy(char *p_src, char *p_src_dir, char *p_dest, char *p_dest_dir, bool safe = TRUE);
bool   file_exists(char *p_dir, char *p_file);

int    file_size_get(char * p_dir, char * p_file);

void   print_errno(bool new_line = FALSE);

/* File abstraction

   safe = TRUE  -> if it fails, call exit 
   safe = FALSE -> if it fails, return NULL file
*/
typedef class fhandle {
  
public:  
  
  virtual operator FILE*(void) = 0;
  virtual operator bool(void) = 0;
  
public:
  
  virtual bool      open(const char *p_dir, const char *p_file, const char *p_mode, bool safe = TRUE) = 0;
  virtual void      close(void) = 0;

  virtual t_length  read(void * p_mem, t_length lenght, bool safe = FALSE) = 0;
  virtual t_length  write(void * p_mem, t_length lenght, bool safe = FALSE) = 0;

  virtual int       seek(t_length offset, int location) = 0;
  virtual t_length  tell(void) = 0;   
  virtual void      rewind(void) = 0;

  virtual t_length  load(char * p_mem, t_length max_lenght, t_length start_address = 0) = 0;
  virtual t_length  load_text(char * p_mem, t_length max_lenght, t_length start_address = 0) = 0;
  
  virtual t_length  read_string(char * p_string, t_length max_lenght) = 0;
  virtual t_length  write_string(char * p_string) = 0;

  virtual t_length  load(const char * p_dir, const char * p_file, char * p_mem, t_length max_lenght, t_length start_address, bool safe) = 0;
  virtual t_length  load_text(const char * p_dir, const char * p_file, char * p_mem, t_length max_lenght, t_length start_address, bool safe) = 0;
  virtual void    * load(const char * p_dir, const char * p_file, t_length *p_lenght, t_length start_address, bool safe) = 0;
  virtual bool      save(const char * p_dir, const char * p_file, void *p_buffer, t_length lenght) = 0;

public:
  
  virtual ~fhandle(void) {};
  
} FHANDLE;

/* Standart file
*/
typedef class ffile : public fhandle {
  
	FILE     *f;
  
public:
    
  void operator=(void *f_)
  {
    f = (FILE *)f_;
  }

  void operator=(class ffile *f_)
  {
    f = f_->f;
  }

  bool operator==(void *f_)
  {
    return(f == f_);
  }

  operator FILE*(void)
  {
    return(f);
  }
  
  operator bool(void)
  {
    return(f != NULL);
  }
  
public:

  bool      open(const char *p_dir, const char *p_file, const char *p_mode, bool safe = TRUE);
  void      close(void);

  t_length  read(void * p_mem, t_length lenght, bool safe = FALSE);
  t_length  write(void * p_mem, t_length lenght, bool safe = FALSE);

  int       seek(t_length offset, int location);
  t_length  tell(void);   
  void      rewind(void);

  t_length  size_get(void);
  
  t_length  load(char * p_mem, t_length max_lenght, t_length start_address = 0);
  t_length  load_text(char * p_mem, t_length max_lenght, t_length start_address = 0);
  
  t_length  read_string(char * p_string, t_length max_lenght);
  t_length  write_string(char * p_string);

  t_length  load(const char * p_dir, const char * p_file, char * p_mem, t_length max_lenght, t_length start_address, bool safe);
  t_length  load_text(const char * p_dir, const char * p_file, char * p_mem, t_length max_lenght, t_length start_address, bool safe);
  void    * load(const char * p_dir, const char * p_file, t_length *p_lenght, t_length start_address, bool safe);
  bool      save(const char * p_dir, const char * p_file, void *p_buffer, t_length lenght);

public:

  ffile(void)
  {
    f = NULL;
  }

  ffile(const char *p_dir, const char *p_file, const char *p_mode, bool safe = TRUE)
  {
    f = NULL;
    open(p_dir, p_file, p_mode, safe);
  }
    
  ffile(FILE *f)
  {
    this->f = f;
  }

} FFILE;


/* Gzipped file
*/
typedef class gzhandle : public fhandle {
  
	gzFile  f;

public:
    
  void operator=(class gzhandle *f_)
  {
    f = f_->f;
  }

  bool operator==(void *f_)
  {
    return(f == f_);
  }
  
  operator bool(void)
  {
    return(f != NULL);
  }

  operator FILE*(void)
  {
    return(NULL);
  }
  
public:

  bool      open(const char *p_dir, const char *p_file, const char *p_mode, bool safe = TRUE);
  void      close(void);

  t_length  read(void * p_mem, t_length lenght, bool safe = FALSE);
  t_length  write(void * p_mem, t_length lenght, bool safe = FALSE);

  int       seek(t_length offset, int location);
  t_length  tell(void);
  void      rewind(void);
  
  t_length  load(char * p_mem, t_length max_lenght, t_length start_address = 0);
  t_length  load_text(char * p_mem, t_length max_lenght, t_length start_address = 0);
  
  t_length  read_string(char * p_string, t_length max_lenght);
  t_length  write_string(char * p_string);

  t_length  load(const char * p_dir, const char * p_file, char * p_mem, t_length max_lenght, t_length start_address, bool safe);
  t_length  load_text(const char * p_dir, const char * p_file, char * p_mem, t_length max_lenght, t_length start_address, bool safe);
  void    * load(const char * p_dir, const char * p_file, t_length *p_lenght, t_length start_address, bool safe);
  bool      save(const char * p_dir, const char * p_file, void *p_buffer, t_length lenght);

public:

  gzhandle(void)
  {
    f = NULL;
  }

  gzhandle(const char *p_dir, const char *p_file, const char *p_mode, bool safe = TRUE)
  {
    open(p_dir, p_file, p_mode, safe);
  }
  
} GZHANDLE;

/* Memory files
*/
typedef class mmhandle : public fhandle {

  char     *p_buffer;
  t_length  buf_len;
  t_length  buf_pos;
   
public:
  
  bool operator==(void *f_)
  {
    return(p_buffer == f_);
  }
  
  operator bool(void)
  {
    return(p_buffer != NULL);
  }
  
  operator FILE*(void)
  {
    return(NULL);
  }
  
public:

  bool      open(const char *p_dir, const char *p_file, const char *p_mode, bool safe = TRUE);
  void      close(void);

  t_length  read(void * p_mem, t_length lenght, bool safe = FALSE);
  t_length  write(void * p_mem, t_length lenght, bool safe = FALSE) { return(0); };

  int       seek(t_length offset, int location);
  t_length  tell(void);   
  void      rewind(void);

  t_length  size_get(void);
  
  t_length  load(char * p_mem, t_length max_lenght, t_length start_address = 0) { return(0); };
  t_length  load_text(char * p_mem, t_length max_lenght, t_length start_address = 0) { return(0); };
    
  t_length  read_string(char * p_string, t_length max_lenght);
  t_length  write_string(char * p_string) { return(0); };

  t_length  load(const char * p_dir, const char * p_file, char * p_mem, t_length max_lenght, t_length start_address, bool safe) { return(0); };
  t_length  load_text(const char * p_dir, const char * p_file, char * p_mem, t_length max_lenght, t_length start_address, bool safe) { return(0); };
  void    * load(const char * p_dir, const char * p_file, t_length *p_lenght, t_length start_address, bool safe) { return(NULL); };
  bool      save(const char * p_dir, const char * p_file, void *p_buffer, t_length lenght) { return(FALSE); };

public:

  mmhandle(void)
  {
    p_buffer = NULL;
    buf_len = 0;
    buf_pos = 0;
  }

  mmhandle(const char *p_dir, const char *p_file, const char *p_mode, bool safe = TRUE)
  {
    open(p_dir, p_file, p_mode, safe);
  }
    
} MMHANDLE;

// -------------------------------------------------------
// configuration management
// -------------------------------------------------------

typedef class dir_list {

  char levels[MAX_FILENAME];
  char levels_user[MAX_FILENAME];
  char gamedata[MAX_FILENAME];
  char graphics[MAX_FILENAME];
  char tmp[MAX_FILENAME];
  char game_binary[MAX_FILENAME];
  char cwd[MAX_FILENAME];

public:
  
  dir_list(void)
  {
    levels[0] = '\0';
    levels_user[0] = '\0';
    gamedata[0] = '\0';
    graphics[0] = '\0';
    tmp[0] = '\0';
    game_binary[0] = '\0';    
    cwd[0] = '\0';
  }

  ~dir_list(void) {};

private:
  
  void update_path(char *p_dir);

public:
  
  void load(char *p_ini);

  char * levels_get(void)
  {
    return(levels);
  }

  char * levels_user_get(void)
  {
    return(levels_user);
  }

  char * gamedata_get(void)
  {
    return(gamedata);
  }

  char * graphics_get(void)
  {
    return(graphics);
  }

  char * tmp_get(void)
  {
    return(tmp);
  }

  char * game_binary_get(void)
  {
    return(game_binary);
  }

  char * cwd_get(void)
  {
    return(cwd);
  }

} DIR_LIST;

bool get_fullscreen(char *p_ini_file);
int  get_colors(char *p_ini_file, int default_color_depth);

// -------------------------------------------------------
// tail operations
// -------------------------------------------------------

inline char * tail_get(const char * p_str, char c = '.')
{
  char *p_last = NULL;
  while((p_str = strchr(p_str, c))) {
    p_last = (char *)p_str;
    p_str++;
  }  
  return((char *)p_last);
}

inline char * tail_change(char * p_str, const char * p_end)
{
  char *p_tail = tail_get(p_str);
  if(p_tail)
    *p_tail = 0;
  if(p_end)
    strcat(p_tail, p_end);
  return(p_str);
}

inline char * tail_name_get(const char * p_str)
{
  return(tail_change((char *)p_str,NULL));
}


inline bool tail_compare(const char * p_str, const char * p_end)
{
  char *p_tail = tail_get((char *)p_str);  
  return(p_tail ? strcmp(p_tail, p_end) == 0 : FALSE);
}

inline int fgets_correction(char * p_kor)
{
  int delka = strlen(p_kor);
  if (p_kor[delka - 1] == '\n') {
    p_kor[delka - 1] = 0;
    return (delka - 1);
  }
  else {
    return (delka);
  }
}

// -------------------------------------------------------
// user-defined data
// -------------------------------------------------------

typedef struct class_user_data {

  void *p_data;

public:
  
  void user_data_set(void *p_data)
  {
    this->p_data = p_data;
  }
  
  void * user_data_get(void)
  {
    return(p_data);
  }
  
public:
  
  class_user_data(void)
  {
    p_data = NULL;
  }
  
} CLASS_USER_DATA;

// -------------------------------------------------------
// ugly hack
// -------------------------------------------------------

inline int void2int(void *p_value)
{
  int *p_tmp = (int *)(&p_value);
  return(*p_tmp);
}

inline void * int2void(int value)
{
  void *p_tmp = (void *)value;
  return(p_tmp);
}

bool file_copy(FFILE f_in, FFILE f_out, int len = 0);

#endif // __UTILS_H__
