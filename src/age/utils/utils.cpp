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

#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>

#include "types.h"
#include "utils.h"
#include "ini.h"
#include "mem_alloc.h"

/*
 * Utility
 */
void dir_list::update_path(char *p_dir)
{
  char buffer[MAX_FILENAME];
  return_path(p_dir, "", buffer, MAX_FILENAME);
  /* Separate assertions so the line number in the assertion message
     will be more helpful. */
  if (chdir(cwd)) assert(0);
  if (chdir(buffer)) assert(0);
  if (getcwd(p_dir,MAX_FILENAME) == NULL) assert(0);
}

void dir_list::load(char *p_ini)
{
  #define INI_LEVEL       "level_data"
  #define INI_GAME        "game_data"
  #define INI_GRAPHICS    "graphics_data"
  #define INI_LEVEL_USER  "level_data_user"
  #define INI_TMP         "tmp_data"

  ini_read_string(p_ini, INI_LEVEL, levels, sizeof(levels), "./Lihen/Levels");
  ini_read_string(p_ini, INI_GAME, gamedata, sizeof(gamedata), "./Lihen/GameData");
  ini_read_string(p_ini, INI_GRAPHICS, graphics, sizeof(graphics), "./Lihen/Graphics");
  ini_read_string(p_ini, INI_LEVEL_USER, levels_user, sizeof(levels_user), "./Lihen/User");
  ini_read_string(p_ini, INI_TMP, tmp, sizeof(tmp), "/var/tmp");

  if (getcwd(cwd,MAX_FILENAME) == NULL)
    assert(0);
  
  update_path(levels);
  update_path(gamedata);
  update_path(graphics);  
  update_path(levels_user);
  update_path(tmp);
  if (chdir(cwd))
    assert(0);

  #define INI_BINARY      "game_binary"
  ini_read_string(p_ini, INI_BINARY, game_binary, sizeof(game_binary), "berusky");
  
  pprintf("level_data: %s",levels);
  pprintf("game_data: %s",gamedata);
  pprintf("graphics_data: %s",graphics);
  pprintf("level_data_user: %s",levels_user);
  pprintf("tmp_data: %s",tmp);
  pprintf("current working dir: %s",cwd);
}

bool get_fullscreen(char *p_ini_file)
{  
  #define INI_FULLSCREEN "fullscreen"
  return(ini_read_int(p_ini_file, INI_FULLSCREEN, FALSE));
}

int  get_colors(char *p_ini_file, int default_color_depth)
{ 
  #define INI_COLOR "color_depth"
  return(ini_read_int(p_ini_file, INI_COLOR, default_color_depth));
}

/*
  File interface
*/

#undef fopen
#undef fwrite
#undef fread
#undef fseek
#undef fclose

/* Create a path */
char * return_path(const char *p_dir, const char *p_file, char *p_buffer, int max_lenght)
{
  if(p_dir && p_dir[0] != '\0') {
    if(p_dir[0] == '~') {
      dir_home_get(p_buffer,max_lenght);
      strncat(p_buffer,p_dir+1,max_lenght-strlen(p_buffer)-1);
    } else {      
      strncpy(p_buffer,p_dir,max_lenght-1);
    }
    strcat(p_buffer,"/");
    strncat(p_buffer,p_file,max_lenght-strlen(p_buffer));
  } else {
    if(p_file[0] == '~') {
      dir_home_get(p_buffer,max_lenght);
      strncat(p_buffer,p_file+1,max_lenght-strlen(p_buffer)-1);
    } else {
      strncpy(p_buffer,p_file,max_lenght-1);
    }
  }
  return(p_buffer);
}

char * path_correction(char *p_file, int max_lenght)
{
  char tmp[MAX_FILENAME];
  return_path(NULL, p_file, tmp, MAX_FILENAME);
  assert(strlen(tmp) < (unsigned)max_lenght);
  strcpy(p_file, tmp);
  return(p_file);
}

/* Create a path */
char * return_path_ext(const char *p_file, const char *p_cwd, 
                       char *p_buffer, int max_lenght)
{
  if(p_file[0] == '.') {
    strncpy(p_buffer,p_cwd,max_lenght);
    strncat(p_buffer,p_file+1,max_lenght-strlen(p_buffer)-1);
  } else {
    strncpy(p_buffer,p_file,max_lenght-1);
  }
  return(p_buffer);
}

/* get a dir */
char * return_dir(const char *p_file, char *p_buffer, int max_lenght)
{
  strncpy(p_buffer,p_file,max_lenght-1);
  char *p_tmp = tail_get(p_buffer,'/');
  if(p_tmp) {
    *p_tmp = '\0';
  }
  return(p_buffer);
}

/* get a file */
char * return_file(const char *p_path, char *p_buffer, int max_lenght)
{
  const char *p_tmp = tail_get(p_path,'/');
  if(p_tmp) {
    p_tmp++;
  }
  else {
    p_tmp = p_path;
  }
  strncpy(p_buffer, p_tmp, max_lenght-1);
  return(p_buffer);
}

/*----------------------------------------------------------------------------
    ffile file interface - normal files
  ----------------------------------------------------------------------------
*/
/* Open a file */
bool ffile::open(const char * p_dir, const char * p_file, const char *p_mode, bool safe)
{  
  char filename[MAX_FILENAME];
  bool ret;

  assert(f == NULL);
  
  return_path(p_dir, p_file, filename, MAX_FILENAME);
  
  f = fopen(filename,p_mode);
  ret = (f != NULL);
  
  if(!ret && safe) {
    char cwd[MAX_FILENAME];
    PERROR(FALSE, "Unable to open %s!\nError: %s\nCurrent dir: %s",
           filename,strerror(errno),getcwd(cwd,MAX_FILENAME));
  }
  
  return(ret);
}

void ffile::close(void)
{
  if(f) {
    fclose(f);
    f = NULL;
  }
}

t_length ffile::read(void * p_mem, t_length lenght, bool safe)
{
  t_length ret;
  
  ret = fread(p_mem,1,lenght,f);
  
  if(safe && ret != lenght) {    
    PERROR(TRUE, "file_read: read error!");
  }
  
  return(ret);
}

t_length ffile::write(void * p_mem, t_length lenght, bool safe)
{
  t_length ret;
  
  ret = fwrite(p_mem,1,lenght,f);
  
  if(safe && ret != lenght) {    
    PERROR(TRUE, "file_write: write error!");
  }
  
  return(ret);
}

int ffile::seek(t_length offset, int location)
{
  return(fseek(f, offset, location));
}

t_length ffile::tell(void)
{
  return(ftell(f));
}

void ffile::rewind(void)
{
  fseek(f,0,SEEK_SET);
}

t_length ffile::size_get(void)
{
  t_length size;
  t_length zal;

  zal = ftell(f);
  fseek(f, 0, SEEK_END);
  size = ftell(f) + 1;
  fseek(f, zal, SEEK_SET);

  return(size);
}

/* load file into memory */
t_length ffile::load(char * p_mem, t_length max_lenght, t_length start_address)
{
  t_length loaded = 0;

  if(f) {
    if(start_address)
      fseek(f, start_address, SEEK_SET);
    loaded = fread(p_mem, 1, max_lenght, f);
    fclose(f);
  }

  return (loaded);
}

/* load file into memory */
t_length ffile::load_text(char * p_mem, t_length max_lenght, t_length start_address)
{
  t_length loaded = 0;

  if(f) {
    if(start_address)
      fseek(f, start_address, SEEK_SET);
    loaded = fread(p_mem, 1, max_lenght, f);
    p_mem[loaded] = '\0';
    fclose(f);
  }

  return (loaded);
}

t_length ffile::read_string(char * p_string, t_length max_lenght)
{
 t_length ret;
 t_length l;
   
 for(l = 0; l < (max_lenght-1); l++, p_string++) {
   ret = read(p_string,sizeof(char));
   if(ret != sizeof(sizeof(char)))
     break;
   if(*p_string == '\0')
     return(l);
 }
 *p_string = '\0';
 return(l);
}

t_length ffile::write_string(char * p_string)
{
  t_length to_write = strlen(p_string);
  return(write(p_string,to_write+1));    
}

t_length ffile::load(const char * p_dir, const char * p_file, char * p_mem, t_length max_lenght, t_length start_address, bool safe)
{
  if(open(p_dir, p_file, "rb", safe))
    return(load(p_mem, max_lenght, start_address));
  else
    return(0);
}

t_length ffile::load_text(const char * p_dir, const char * p_file, char * p_mem, t_length max_lenght, t_length start_address, bool safe)
{  
  if(open(p_dir, p_file, "rb", safe))
    return(load_text(p_mem, max_lenght, start_address));
  else
    return(0);
}

void * ffile::load(const char * p_dir, const char * p_file, t_length *p_lenght, t_length start_address, bool safe)
{  
  open(p_dir, p_file, "rb", safe);
  if(!f)
    return(NULL);
  
  t_length to_load = size_get();

  void *p_mem = mmalloc(to_load);
  *p_lenght = fread(p_mem, 1, to_load, f);
  fclose(f);
  
  return (p_mem);
}

bool ffile::save(const char * p_dir, const char * p_file, void *p_buffer, t_length lenght)
{  
  if(open(p_dir, p_file, "wb")) {
    t_length wrt = write(p_buffer, lenght);
    close();
    return(wrt == lenght);
  }
  else {
    return(FALSE);
  }
}

/*----------------------------------------------------------------------------
    gzhandle file interface - compressed files
  ----------------------------------------------------------------------------
*/
bool gzhandle::open(const char * p_dir, const char * p_file, const char *p_mode, bool safe)
{  
  char filename[MAX_FILENAME];
  bool ret;

  assert(f == NULL);
  
  return_path(p_dir, p_file, filename, MAX_FILENAME);
  
  f = gzopen(filename, p_mode);
  ret = (f != NULL);
  
  if(!ret && safe) {
    char cwd[MAX_FILENAME];
    PERROR(FALSE, "Unable to open %s!\nError: %s\nCurrent dir: %s",
            filename,strerror(errno),getcwd(cwd,MAX_FILENAME));
  }
  
  return(ret);
}

void gzhandle::close(void)
{
  if(f) {
    gzclose(f);
    f = NULL;
  }  
}

t_length gzhandle::read(void * p_mem, t_length lenght, bool safe)
{
  t_length ret;
  
  ret = gzread(f,p_mem,lenght);
  
  if(safe && ret != lenght) {    
    PERROR(TRUE, "file_read: read error!");
  }
  
  return(ret);
}

t_length gzhandle::write(void * p_mem, t_length lenght, bool safe)
{
  t_length ret;
  
  ret = gzwrite(f,p_mem,lenght);
  
  if(safe && ret != lenght) {    
    PERROR(TRUE, "file_write: write error!");
  }
  
  return(ret);
}

int gzhandle::seek(t_length offset, int location)
{
  return(gzseek(f, offset, location));
}

t_length gzhandle::tell(void)
{
  return(gztell(f));
}

void gzhandle::rewind(void)
{
  gzseek(f,0,SEEK_SET);
}

/* load file into memory */
t_length gzhandle::load(char * p_mem, t_length max_lenght, t_length start_address)
{
  t_length loaded = 0;

  if(f) {
    if(start_address)
      gzseek(f, start_address, SEEK_SET);
    loaded = gzread(f, p_mem, max_lenght);
    gzclose(f);
  }

  return (loaded);
}

/* load file into memory */
t_length gzhandle::load_text(char * p_mem, t_length max_lenght, t_length start_address)
{
  dword loaded = 0;

  if(f) {
    if(start_address)
      gzseek(f, start_address, SEEK_SET);
    loaded = gzread(f, p_mem, max_lenght);
    p_mem[loaded] = '\0';
    gzclose(f);
  }

  return (loaded);
}

t_length gzhandle::read_string(char * p_string, t_length max_lenght)
{
 t_length ret;
 t_length l;
   
 for(l = 0; l < (max_lenght-1); l++, p_string++) {
   ret = read(p_string,sizeof(char));
   if(ret != sizeof(sizeof(char)))
     break;
   if(*p_string == '\0')
     return(l);
 }
 *p_string = '\0';
 return(l);
}

t_length gzhandle::write_string(char * p_string)
{
  t_length to_write = strlen(p_string);
  return(write(p_string,to_write+1));
}

t_length gzhandle::load(const char * p_dir, const char * p_file, char * p_mem, t_length max_lenght, t_length start_address, bool safe)
{
  if(open(p_dir, p_file, "rb", safe))
    return(load(p_mem, max_lenght, start_address));
  else
    return(0);
}

t_length gzhandle::load_text(const char * p_dir, const char * p_file, char * p_mem, t_length max_lenght, t_length start_address, bool safe)
{  
  if(open(p_dir, p_file, "rb", safe))
    return(load_text(p_mem, max_lenght, start_address));
  else
    return(0);
}

void * gzhandle::load(const char * p_dir, const char * p_file, t_length *p_lenght, t_length start_address, bool safe)
{ 
  open(p_dir, p_file, "rb", safe);
  if(!f)
    return(NULL);
  
  if(start_address)
    seek(start_address, SEEK_SET);
  
  #define BUFFER_STEP  32768
  
  char *p_buffer = NULL;
  t_length pos = 0;
  t_length ret;

  do {    
    p_buffer = (char *)rrealloc(p_buffer, pos+BUFFER_STEP);
    ret = read(p_buffer+pos, BUFFER_STEP);
    pos += ret;
  } while(ret == BUFFER_STEP);
      
  *p_lenght = pos;
  
  return(p_buffer);
}

bool gzhandle::save(const char * p_dir, const char * p_file, void *p_buffer, t_length lenght)
{  
  if(open(p_dir, p_file, "wb")) {
    t_length wrt = write(p_buffer, lenght);
    close();
    return(wrt == lenght);
  }
  else {
    return(FALSE);
  }  
}

/*----------------------------------------------------------------------------
    memory file interface - files saved in memory
  ----------------------------------------------------------------------------
*/
// Only reading is supported now
bool mmhandle::open(const char * p_dir, const char * p_file, const char *p_mode, bool safe)
{  
  GZHANDLE f;
    
  p_buffer = (char *)f.load(p_dir, p_file, &buf_len, 0, safe);
  buf_pos = 0;
  
  return(p_buffer != NULL && buf_len > 0);
}

void mmhandle::close(void)
{
  if(p_buffer) {
    ffree(p_buffer);
  }
  
  p_buffer = NULL;
  buf_len = 0;
  buf_pos = 0;
}

t_length mmhandle::read(void * p_mem, t_length lenght, bool safe)
{
  t_length readed = 0;
  
  if(p_buffer) {
    readed = (buf_pos + lenght < buf_len) ? lenght : buf_len - buf_pos;
    memcpy(p_mem,p_buffer+buf_pos,readed);
    buf_pos += readed;
  }
  return(readed);
}

int mmhandle::seek(t_length offset, int location)
{
  switch(location) {
    case SEEK_SET:
      buf_pos = offset;
      break;
    case SEEK_CUR:
      buf_pos += offset;
      break;
    case SEEK_END:
      buf_pos = buf_len + offset;
      break;    
  }
  
  if(buf_pos > buf_len)
    buf_pos = buf_len;
  if(buf_pos < 0)
    buf_pos = 0;
  
  return(0);
}

t_length mmhandle::tell(void)
{
  return(buf_pos);
}

void mmhandle::rewind(void)
{
  buf_pos = 0;
}

t_length mmhandle::size_get(void)
{
  return(buf_len);
}

t_length mmhandle::read_string(char * p_string, t_length max_lenght)
{
 t_length ret;
 t_length l;
   
 for(l = 0; l < (max_lenght-1); l++, p_string++) {
   ret = read(p_string,sizeof(char));
   if(ret != sizeof(char))
     break;
   if(*p_string == '\0')
     return(l);
 }
 *p_string = '\0';
 return(l);
}

/*----------------------------------------------------------------------------
    General file interface
  ----------------------------------------------------------------------------
*/
#define FILE_BUF_SIZE 4096

bool file_copy(FFILE f_in, FFILE f_out, int len)
{  
  char buffer[FILE_BUF_SIZE];
  int  readed;
    
  if(len) {
    int to_read;    
    do {
      len -= FILE_BUF_SIZE;
      to_read = (len < 0) ? len+FILE_BUF_SIZE : FILE_BUF_SIZE;
          
      if((readed = fread(buffer,1,to_read,f_in))) {
        fwrite(buffer,1,readed,f_out);
      }
    
      if(readed != to_read) {
        pperror(1,"file_copy - missing data?");
      }
    } while(to_read == FILE_BUF_SIZE);
  }
  else {
    while((readed = fread(buffer,1,FILE_BUF_SIZE,f_in))) {
      fwrite(buffer,1,readed,f_out);
    }
  }

  return(TRUE);
}

bool file_copy(const char *p_src, const char *p_src_dir, 
               const char *p_dest, const char *p_dest_dir, bool safe)
{
  FFILE src(p_src_dir,  p_src,  "rb", safe);
  if(!src)
    return(FALSE);

  FFILE dst(p_dest_dir, p_dest, "wb", safe);
  if(!dst) {
    src.close();
    return(FALSE);
  }

  file_copy(src, dst);

  src.close();
  dst.close();

  return(TRUE);
}

bool file_exists(const char * p_dir, const char * p_file)
{
  FFILE f(p_dir, p_file, "rb", FALSE);
  if(!(f)) {
    return(FALSE);
  } 
  else {
    f.close();
    return(TRUE);
  }
}

t_length file_size_get(char * p_dir, char * p_file)
{
  FFILE f(p_dir, p_file, "rb", FALSE);
  if(!(f)) {
    return(FALSE);
  } 
  else {
    t_length size = f.size_get();
    f.close();
    return(size);
  }
}

t_length file_size_get(FILE *f)
{
  FFILE f_handle(f);
  if(!(f_handle)) {
    return(FALSE);
  } 
  else {
    return(f_handle.size_get());
  }
}

void print_errno(bool new_line)
{
  if(new_line) {
    pprintf("\n%s",strerror(errno));
  } else {
    pprintf(strerror(errno));
  }  
}

char * dir_home_get(char *p_dir, int max)
{
  assert(p_dir);

  char *p_tmp = getenv("HOME");
  if(p_tmp) {
    strncpy(p_dir,p_tmp,max);
  } else {
    // a homeless user?
    assert(max >= 1);
    p_dir[0] = '\0';
  }
  return(p_dir);
}

bool dir_create(const char *p_dir)
{
  assert(p_dir);

  char tmp_dir[MAX_FILENAME];
  return_path(p_dir, "", tmp_dir, MAX_FILENAME);

  struct stat st;

  // Check the dir
  pprintfnl("Checking %s...",tmp_dir);
  if(stat(tmp_dir,&st) == -1 && errno == ENOENT) {
    pprintfnl("missing, try to create it...");
#ifdef WINDOWS
    if(mkdir(tmp_dir) != -1) {
#else  
    if(mkdir(tmp_dir,DEFAULT_DIR_MASK) != -1) {
#endif
      pprintf("ok");
      return(TRUE);
    } else {
      print_errno(TRUE);
      pprintf("failed");
      return(FALSE);
    }
  } else {
    pprintf("ok");
    return(TRUE);
  }
}
