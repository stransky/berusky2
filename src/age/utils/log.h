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

#ifndef __LOG_H__
#define __LOG_H__

#define  LOG_ENABLED 1

// -------------------------------------------------------
// General macros
// -------------------------------------------------------

// Don't use standart I/O

/*
#define FILE

#define fopen(a,b)
#define fwrite(a,b,c,d)
#define fread(a,b,c,d)
#define fseek(a,b,c)
#define fclose(a)
*/

#define DEFAULT_DIR_MASK   0755
#define DEFAULT_FILE_MASK  0644

// -------------------------------------------------------
// log file management
// -------------------------------------------------------

//extern FFILE log_file;

void log_open(const char *p_file);
void log_open_ini(const char *p_ini_file);
void log_close(void);
void log_flush(void);

// -------------------------------------------------------
// logging - helper function
// -------------------------------------------------------

typedef enum {
  
  DEBUG_ALL = 0,
  DEBUG_LOAD,
  DEBUG_LOAD_CHUNK,
  DEBUG_LOAD_3DS,
  DEBUG_GENERATOR
  
} DEBUG_PART;

#ifdef  LOG_ENABLED

void ppset(const char *p_file, int line);
void ppclear(void);
void pperror(int quit, const char *p_text,...);
void ppwarning(const char *p_text,...);
void pprintf(const char *p_text,...);
void pprintfnl(const char *p_text,...);
void pdebug(int enabled, const char *p_text,...);

#define PERROR            ppset(__FILE__,__LINE__); pperror
#define PWARNING          ppset(__FILE__,__LINE__); ppwarning

#else

#define pperror(quit, p_text,...)
#define ppwarning(p_text,...)
#define ppprintf(p_text,...)
#define fgets_correction(p_kor)
#define pdebug(enabled,p_text,...)

#define PERROR            
#define PWARNING          

#endif // LOG_ENABLED

#endif // __LOG_H__
