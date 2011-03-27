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
#include "log.h"

/*
  Logs & error messages
*/

FFILE log_file;

void log_close(void)
{
  if(log_file) {
    log_file.close();
    log_file = (void *)NULL;
  }
}

void log_open(const char *p_file)
{
  log_close();
 
  if(p_file) {
    log_file.open(NO_DIR,p_file,"a",FALSE);
    if(!log_file) {
      PERROR(TRUE, "Unable to open log file '%s', logging is disabled.",p_file);
    }
    pprintf("------------- start ----------------");
  }
}

void log_open_ini(const char *p_ini_file)
{
  char logfile[1000];
  #define INI_LOGFILE "logfile"
  #define INI_LOG     "log"

  if(ini_read_bool(p_ini_file, INI_LOG, FALSE)) {
    ini_read_string(p_ini_file, INI_LOGFILE, logfile, sizeof(logfile),"./b1logfile.txt");
    log_open(logfile);
  }
}

void log_flush(void)
{
  if(log_file)
    fflush(log_file);
}

/*
  Logs & error messages
*/
static const char *p_ppfile = NULL;
static int  ppline = 0;

void ppset(const char *p_file, int line)
{
  p_ppfile = p_file;
  ppline = line;
}

void ppclear(void)
{
  p_ppfile = NULL;
  ppline = 0;
}

void pperror(int quit, const char *p_text,...)
{
  char      text[2000];
  va_list   arguments;  

  va_start(arguments,p_text);
  vsnprintf(text,2000,p_text,arguments);
  va_end(arguments);

  if(p_ppfile)
    fprintf(stderr,"file %s: line %d: %s\n", p_ppfile, ppline, text);
  else 
    fprintf(stderr,"Error: %s\n",text);

  if(quit)
    abort();
}

void ppwarning(const char *p_text,...)
{
  char      text[2000];
  va_list   arguments;  

  va_start(arguments,p_text);
  vsnprintf(text,2000,p_text,arguments);
  va_end(arguments);

  if(ppline && p_ppfile)
    fprintf(stderr,"Warning at line %d file %s: %s\n", ppline, p_ppfile ? p_ppfile : "",text);  
  else
    fprintf(stderr,"Warning: %s\n",text);  
}
 
void pprintf(const char *p_text,...)
{
  char      text[2000];
  va_list   arguments;  

  va_start(arguments,p_text);
  vsnprintf(text,2000,p_text,arguments);
  va_end(arguments);

  fprintf(stderr,"%s\n",text);

  if(log_file) {
    fprintf(log_file,"%s\n",text);
  }
}

void pprintfnl(const char *p_text,...)
{
  char      text[2000];
  va_list   arguments;  

  va_start(arguments,p_text);
  vsnprintf(text,2000,p_text,arguments);
  va_end(arguments);

  fprintf(stderr,"%s",text);

  if(log_file) {
    fprintf(log_file,"%s",text);
  }
}

void pdebug(int enabled, const char *p_text,...)
{
  if(enabled) {
    char      text[2000];
    va_list   arguments;  
  
    va_start(arguments,p_text);
    vsnprintf(text,2000,p_text,arguments);
    va_end(arguments);
  
    fprintf(stderr,"%s\n",text);
  
    if(log_file) {
      fprintf(log_file,"%s\n",text);
    }
  }
}
