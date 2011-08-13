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

#ifndef __INI_H__
#define __INI_H__

#define MAX_TOKEN_LEN       1000

#define TOKEN_TRUE1         "yes"
#define TOKEN_TRUE2         "on"

#define TOKEN_FALSE1        "no"
#define TOKEN_FALSE2        "off"

#ifndef TRUE
#define TRUE                (1==1)
#endif

#ifndef FALSE
#define FALSE               (1!=1)
#endif

#define SEPARATOR_INI       '='
#define SEPARATOR_DATA      '='

char *ini_remove_end_of_line(char *p_line);
char *ini_skip_spaces(char *p_line);
char *ini_skip_separator(char *p_line, char separator = SEPARATOR_INI);
char *ini_read_param(char *p_line, char *p_param, int max_len);

FFILE ini_open(const char *p_file, bool safe = TRUE);
void  ini_close(FFILE f);

char *ini_read_string(FFILE f, const char *p_template, char *p_out, int max_len, const char *p_default);
char *ini_read_string(const char *p_file, const char *p_template, char *p_out, int max_len, const char *p_default);

char *ini_read_string_section(FFILE f, const char *p_section, const char *p_template, char *p_out, int max_len, const char *p_default);
char *ini_read_string_section(const char *p_file, const char *p_section, const char *p_template, char *p_out, int max_len, const char *p_default);

int   ini_read_int(FFILE f, const char *p_template, int dflt);
int   ini_read_int(const char *p_file, const char *p_template, int dflt);

int   ini_read_int_section(FFILE f, const char *p_section, const char *p_template, int dflt);
int   ini_read_int_section(const char *p_file, const char *p_section, const char *p_template, int dflt);

float ini_read_float(FFILE f, const char *p_template, float dflt);
float ini_read_float(const char *p_file, const char *p_template, int dflt);

float ini_read_float_section(FFILE f, const char *p_section, const char *p_template, float dflt);
float ini_read_float_section(const char *p_file, const char *p_section, const char *p_template, int dflt);

int   ini_read_bool(FFILE f, const char *p_template, int dflt);
int   ini_read_bool(const char *p_file, const char *p_template, int dflt);

int   ini_read_bool_section(FFILE f, const char *p_section, const char *p_template, int dflt);
int   ini_read_bool_section(const char *p_file, const char *p_section, const char *p_template, int dflt);

int   is_token(const char *p_line, const char *p_token);
int   read_token(FFILE f_in, char *p_line, size_t max, char separator);

int   is_section(const char *p_line);
char *read_section(char *p_line, char *p_section, int max_len);

#endif  // __INI_H__
