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

/* ini file handling
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "ini.h"

char *ini_remove_end_of_line(char *p_line)
{
  char *p_start = p_line;

  while (*p_line && *p_line != '\n')
    p_line++;

  if (*p_line == '\n')
    *p_line = '\0';

  return (p_start);
}

bool ini_is_space(char c)
{
  return(c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

bool ini_is_separator(char c)
{
  return(c == SEPARATOR_INI);
}

char *ini_skip_spaces(char *p_line)
{
  while (*p_line && ini_is_space(*p_line))
    p_line++;
  return (p_line);
}

char *ini_skip_separator(char *p_line, char separator)
{
  char *p_tmp = strchr(p_line, separator);
  if (p_tmp) {
    return (ini_skip_spaces(p_tmp + 1));
  }
  else {
    return (NULL);
  }
}

char *ini_read_param(char *p_line, char *p_param, int max_len)
{
  char *p_start = p_param;
  int i = 0;

  p_line = ini_skip_spaces(p_line);
  
  while (*p_line && i < max_len - 1 && !ini_is_space(*p_line) && !ini_is_separator(*p_line)) {
    *p_param++ = *p_line++;
    i++;
  }
  *p_param = '\0';

  return (p_start);
}

char *ini_read_string_section(FFILE f, const char *p_section,
                              const char *p_template, char *p_out,
                              int max_len, const char *p_default)
{
  char line[MAX_TOKEN_LEN];
  char section[MAX_TOKEN_LEN];
  bool section_found = FALSE;

  fseek(f, 0, SEEK_SET);
  while (fgets(line, MAX_TOKEN_LEN, f)) {
    if(!section_found) {
      section_found = (bool)read_section(line, section, MAX_TOKEN_LEN);
      if(section_found) {
        section_found = !strncasecmp(p_section, section, MAX_TOKEN_LEN);
      }
      if(section_found)
        continue;
    }
    
    if(section_found && is_section(line)) {
      // we hit next section - so it's not found
      break;
    }
  
    if(section_found) {
      int len = is_token(line, p_template);
      char *p_rest;
      if (len && (p_rest = ini_skip_separator(line + len))) {
        return (ini_read_param(p_rest, p_out, max_len));
      }
    }
  }

  return p_default ? (strcpy(p_out, p_default)) : NULL;
}

char *ini_read_string(FFILE f, const char *p_template, char *p_out,
                      int max_len, const char *p_default)
{
  char line[MAX_TOKEN_LEN];

  fseek(f, 0, SEEK_SET);
  while (fgets(line, MAX_TOKEN_LEN, f)) {
    int len = is_token(line, p_template);
    char *p_rest;
    if (len && (p_rest = ini_skip_separator(line + len))) {
      return (ini_read_param(p_rest, p_out, max_len));
    }
  }

  return p_default ? (strcpy(p_out, p_default)) : NULL;
}

bool ini_find_string_section(FFILE f, const char *p_section,
                             const char *p_template,
                             long *p_file_start, long *p_file_end)
{
  char line[MAX_TOKEN_LEN];
  char section[MAX_TOKEN_LEN];
  bool section_found = FALSE;
  long file_pos;
  long file_last_line_start = 0;
  long file_last_line_end = 0;

  file_pos = 0;
  fseek(f, 0, SEEK_SET);
  while (fgets(line, MAX_TOKEN_LEN, f)) {
    if(!section_found) {
      section_found = (bool)read_section(line, section, MAX_TOKEN_LEN);
      if(section_found) {
        section_found = !strncasecmp(p_section, section, MAX_TOKEN_LEN);
      }
      if(section_found)
        continue;
    }
    
    // Cache last non-section line from recent section
    if(section_found && !is_section(line) && !is_empty(line)) {
      file_last_line_start = file_pos;
      file_last_line_end = ftell(f);
    }
  
    if(section_found && is_section(line)) {
      // we hit next section - so it's not found,
      // create a new entry
      *p_file_start = file_last_line_start;
      *p_file_end = file_last_line_end;
      return(TRUE);
    }
  
    // Replace this line
    if(section_found && is_token(line, p_template)) {
      *p_file_start = file_pos;
      *p_file_end = ftell(f);
      return(TRUE);
    }
  
    file_pos = ftell(f);
  }

  // no section -> create a new one
  return(FALSE);
}

int ini_write_string_section(FFILE f_in, FFILE f_out, const char *p_section,
                             const char *p_template, const char *p_value)
{
  long file_start, 
       file_end;
  
  int found = ini_find_string_section(f_in, p_section, p_template, &file_start, &file_end);

  fseek(f_in, 0, SEEK_SET);
  fseek(f_out, 0, SEEK_SET);

  if(found) {
    file_copy(f_in, f_out, file_start);
    fprintf(f_out,"%s=%s\n",p_template, p_value);
    fseek(f_in, file_end, SEEK_SET);
    file_copy(f_in, f_out);
  }
  else {
    file_copy(f_in, f_out);
    fprintf(f_out,"\n[%s]\n",p_section);
    fprintf(f_out,"%s=%s\n",p_template, p_value);
  }

  return(TRUE);
}

bool ini_write_string_section(const char *p_file, const char *p_section, 
                              const char *p_template, const char *p_value)
{
  int ret;

  FFILE f_orig(NULL, p_file, "r", FALSE);
  if (!f_orig)
    return(FALSE);
    
  FFILE f_new(tmpfile());
  if (!f_new)
    return(FALSE);
  
  ret = file_copy(f_orig, f_new);
  if(!ret) {
    fclose(f_orig);
    fclose(f_new);
    return(FALSE);  
  }
    
  f_orig.close();
  
  if(f_orig.open(NULL, p_file, "w", FALSE)) {
    ret = ini_write_string_section(f_new, f_orig, p_section, p_template, p_value);
    fclose(f_orig);
  }
  
  fclose(f_new);
  
  return (ret);
}

int ini_read_int(FFILE f, const char *p_template, int dflt)
{
  char line[MAX_TOKEN_LEN];
  char *ret = ini_read_string(f, p_template, line, MAX_TOKEN_LEN, NULL);
  return ret ? atoi(ini_remove_end_of_line(ret)) : dflt;
}

int ini_read_int_section(FFILE f, const char *p_section, const char *p_template, int dflt)
{
  char line[MAX_TOKEN_LEN];
  char *ret = ini_read_string_section(f, p_section, p_template, line, MAX_TOKEN_LEN, NULL);
  return ret ? atoi(ini_remove_end_of_line(ret)) : dflt;
}

float ini_read_float(FFILE f, const char *p_template, float dflt)
{
  char line[MAX_TOKEN_LEN];
  char *ret = ini_read_string(f, p_template, line, MAX_TOKEN_LEN, NULL);
  return ret ? atof(ini_remove_end_of_line(ret)) : dflt;
}

float ini_read_float_section(FFILE f, const char *p_section, const char *p_template, float dflt)
{
  char line[MAX_TOKEN_LEN];
  char *ret = ini_read_string_section(f, p_section, p_template, line, MAX_TOKEN_LEN, NULL);
  return ret ? atof(ini_remove_end_of_line(ret)) : dflt;
}

char *ini_read_string(const char *p_file, const char *p_template, char *p_out,
                      int max_len, const char *p_default)
{
  FFILE f(NULL, p_file, "r", FALSE);

  if (!f)
    return p_default ? (strcpy(p_out, p_default)) : NULL;

  char *ret = ini_read_string(f, p_template, p_out, max_len, p_default);

  fclose(f);
  return (ret);
}

char *ini_read_string_section(const char *p_file, const char *p_section, 
                              const char *p_template, char *p_out,
                              int max_len, const char *p_default)
{
  FFILE f(NULL, p_file, "r", FALSE);

  if (!f)
    return p_default ? (strcpy(p_out, p_default)) : NULL;

  char *ret = ini_read_string_section(f, p_section, p_template,
                                      p_out, max_len, p_default);
  fclose(f);
  return (ret);
}

int ini_read_int(const char *p_file, const char *p_template, int dflt)
{
  FFILE f(NULL, p_file, "r", FALSE);
  if (!f)
    return (dflt);

  int ret = ini_read_int(f, p_template, dflt);
  
  fclose(f);
  return (ret);
}

int ini_read_int_section(const char *p_file, const char *p_section, 
                         const char *p_template, int dflt)
{
  FFILE f(NULL, p_file, "r", FALSE);
  if (!f)
    return (dflt);

  int ret = ini_read_int_section(f, p_section, p_template, dflt);
  
  fclose(f);
  return (ret);
}

float ini_read_float(const char *p_file, const char *p_template, float dflt)
{
  FFILE f(NULL, p_file, "r", FALSE);
  if (!f)
    return (dflt);

  int ret = ini_read_float(f, p_template, dflt);
  
  fclose(f);
  return (ret);
}

float ini_read_float_section(const char *p_file, const char *p_section, 
                             const char *p_template, float dflt)
{
  FFILE f(NULL, p_file, "r", FALSE);
  if (!f)
    return (dflt);

  int ret = ini_read_float_section(f, p_section, p_template, dflt);
  
  fclose(f);
  return (ret);
}

int ini_read_bool(FFILE f, const char *p_template, int dflt)
{
  char line[MAX_TOKEN_LEN];
  
  if (ini_read_string(f, p_template, line, MAX_TOKEN_LEN, NULL))
    return (dflt);
  else {
    if (is_token(line, TOKEN_FALSE1) || is_token(line, TOKEN_FALSE2))
      return (FALSE);
    else if (is_token(line, TOKEN_TRUE1) || is_token(line, TOKEN_TRUE2))
      return (TRUE);
    else
      return (dflt);
  }
}

int ini_read_bool_section(FFILE f, const char *p_section, const char *p_template, int dflt)
{
  char line[MAX_TOKEN_LEN];
  
  if (!ini_read_string_section(f, p_section, p_template, line, MAX_TOKEN_LEN, NULL))
    return (dflt);
  else {
    if (is_token(line, TOKEN_FALSE1) || is_token(line, TOKEN_FALSE2))
      return (FALSE);
    else if (is_token(line, TOKEN_TRUE1) || is_token(line, TOKEN_TRUE2))
      return (TRUE);
    else
      return (dflt);
  }
}

int ini_read_bool(const char *p_file, const char *p_template, int dflt)
{
  FFILE f(NULL, p_file, "r", FALSE);

  if (!f)
    return (dflt);

  int ret = ini_read_bool(f, p_template, dflt);

  fclose(f);
  return(ret);
}

int ini_read_bool_section(const char *p_file, const char *p_section, const char *p_template, int dflt)
{
  FFILE f(NULL, p_file, "r", FALSE);

  if (!f)
    return (dflt);

  int ret = ini_read_bool_section(f, p_section, p_template, dflt);

  fclose(f);
  return(ret);
}

/* Check if the given string is a token
*/
int is_token(const char *p_line, const char *p_token)
{
  const char *p_start = p_line;

  while (*p_line && *p_token && tolower(*p_line) == tolower(*p_token)) {
    p_line++;
    p_token++;
  }

  char line_end = *p_line;
  if(ini_is_space(line_end) || ini_is_separator(line_end)) {
    return (*p_token ? 0 : p_line - p_start);
  }
  else {
    return(0);
  }
}

/* Reading token (between %) from file
*/
int read_token(FFILE f_in, char *p_line, size_t max, char separator)
{
  size_t len;
  int c;

  for (len = 0;
       (c = fgetc(f_in)) != EOF && c != separator && len + 2 < max;
       len++, p_line++) {
    *p_line = c;
  }

  if (c == separator) {
    *p_line++ = c;
  }
  *p_line = 0;

  return (c == separator);
}

/* Check if the given string is a section (a string between [])
*/
int is_section(const char *p_line)
{
  char *p_start = ini_skip_spaces((char *)p_line);

  if(p_start[0] != '[')
    return(FALSE);
  
  return((int)strchr(p_start+1,']'));
}

/* Reading section (between []) from file
*/
char * read_section(char *p_line, char *p_section, int max_len)
{
  char *p_start = ini_skip_spaces(p_line);
  char *p_end;

  if(p_start[0] != '[')
    return(NULL);
  
  p_start++;
  
  if(!(p_end = strchr(p_start,']')))
    return(NULL);
  
  int len = p_end - p_start;
  if(len+1 >= max_len)
    return(NULL);
  
  strncpy(p_section, p_start, len);
  p_section[len] = '\0';
  
  return(p_section);
}

bool is_empty(const char *p_line)
{
  char *p_start = ini_skip_spaces((char *)p_line);
  return(!p_start[0]);
}

FFILE ini_open(const char *p_file, bool safe)
{
  FFILE f(NULL,p_file,"r",safe);
  return(f);
}

void ini_close(FFILE f)
{
  f.close();
}
