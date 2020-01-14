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
  Data files
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "age.h"
#include "data_file.h"

void newline_delete(char *p_line)
{
  int c = strlen(p_line);
  if(p_line[c-1] == '\n')
    p_line[c-1] = '\0';
}

LINE_TYPE line_type_get(char *p_line)
{  
  p_line = ini_skip_spaces(p_line);
  if(!p_line || !(*p_line))
    return(LINE_NONE);
  
  if(p_line[0] == SECTION_NAME_START) {
    return(LINE_SECTION_START);
    
  } else if(p_line[0] == SECTION_NAME_STOP) {
    return(LINE_SECTION_STOP);
  
  } else if(p_line[0] == SECTION_NAME_COMMENT2) {
    return(LINE_COMMENT);
    
  } else if(p_line[0] == SECTION_NAME_COMMENT[0]) {
    if(p_line[1] != '\0') {
      if(p_line[1] == SECTION_NAME_COMMENT[1])
        return(LINE_COMMENT);
      if(p_line[1] == SECTION_NAME_COMMENT_START[1])
        return(LINE_COMMENT_START);
    }
    
  } else if(p_line[0] == SECTION_NAME_COMMENT_STOP[0]) {
    if(p_line[1] != '\0' && p_line[1] == SECTION_NAME_COMMENT_STOP[1])
      return(LINE_COMMENT_STOP);
  } else if(p_line[0] == SECTION_NAME_INCLUDE[0] && 
            !strncasecmp(p_line, SECTION_NAME_INCLUDE, strlen(SECTION_NAME_INCLUDE))) {
    return(LINE_SECTION_INCLUDE);
  }
  
  return(LINE_SECTION_ITEM);  
}

bool tokenize_line_command(char *p_line, char *p_token, char *p_value)
{
  if(p_token) {
    ini_read_param(p_line, p_token, MAX_TOKEN_LEN);
  }
  
  p_line = ini_skip_separator(p_line,' ');
  if(!p_line) {
    return(FALSE);
  }
  
  if(p_value) {
    ini_read_param(p_line, p_value, MAX_TOKEN_LEN);
  }
  
  return((p_token == NULL || p_token[0]) && 
         (p_value == NULL || p_value[0]));
}

bool tokenize_line_param(char *p_line, char *p_token, char *p_value)
{
  if(p_token) {
    ini_read_param(p_line, p_token, MAX_TOKEN_LEN);
  }
  
  p_line = ini_skip_separator(p_line);
  if(!p_line) {
    return(FALSE);
  }
  
  if(p_value) {
    ini_read_param(p_line, p_value, MAX_TOKEN_LEN);
  }
  
  return((p_token == NULL || p_token[0]) && 
         (p_value == NULL || p_value[0]));
}

void data_file_section::line_print(void)
{
  pprintf("Line %d: Item: %s",name_get());
  if(value_string)
    pprintf("    String: %s",value_string);
  pprintf("    Int: %d",value_int);
  pprintf("    Float: %f",value_float);
}

void data_file_section::section_print(int indent)
{
  // Print this section
  pprintf("%*cSection: %s",indent*2,' ',name_get());
  
  // Print child
  DATA_FILE_SECTION *p_tmp = section_child_get();
  if(p_tmp)
    p_tmp->section_print(indent+1);
  
  // Print next section
  p_tmp = section_next();
  if(p_tmp)
    p_tmp->section_print(indent);
}

DATA_FILE_SECTION * data_file_section::section_new(int line, char *p_line, char *p_token, char *p_value)
{
  DATA_FILE_SECTION *p_new = new DATA_FILE_SECTION(p_file, line, p_line,p_token,p_value);
  object_list_insert_last(p_new);
  return(p_new);
}

bool data_file_section::section_new(int line, char *p_line)
{
  char token[MAX_TOKEN_LEN] = "";
  char value[MAX_TOKEN_LEN] = "";

  assert(p_line);  
  
  tokenize_line_param(p_line, token, value);
  
  if(token[0] && value[0]) {
    if(!strcasecmp(token,SECTION_NAME_NAME)) {
      name_set(value);
      source_line_set(line);
    }
    else if(!strcasecmp(token,SECTION_NAME_TYPE)) {
      assert(strlen(value) < MAX_NAME);
      strncpy(section_type,value,MAX_NAME);
      source_line_set(line);
    }
    else {
      section_new(line, p_line, token, value);
    }
    return(TRUE);
  }
  else {
    // A raw string
    section_new(line, p_line,
                token[0] ? token : NULL,
                value[0] ? value : NULL);
  }
  return(FALSE);
}

void data_file_section::load(void)
{
  bool in_comment = FALSE;
  
  char line[MAX_TOKEN_LEN];
  while(p_file->load_line(line)) {
    
    LINE_TYPE type = line_type_get(line);

    /*
     * Skip comments in input file
     */
    if(type == LINE_COMMENT_START) {
      in_comment = TRUE;
      continue;
    }
    else if(type == LINE_COMMENT_STOP) {
      in_comment = FALSE;
      continue;
    } else if(in_comment) {
      continue;
    }
    
    /*
     * Process lines
     */
    switch(type) {
      case LINE_NONE:
      case LINE_SECTION_INCLUDE:
      case LINE_COMMENT:
      case LINE_COMMENT_START:
      case LINE_COMMENT_STOP:
        break;
      
      // Create and load new sub-section
      case LINE_SECTION_START:
      {
        data_file_section *p_new = section_new(p_file->current_line_get(), NULL,NULL,NULL);
        p_new->load();
        break;
      }   
      // Load a line to current section
      case LINE_SECTION_ITEM:
      {
        section_new(p_file->current_line_get(), line);
        break;
      }
      // Close this section
      case LINE_SECTION_STOP:
      {
        return;
      }
    }
  }
}

void data_file_section::clear(void)
{
  // Remove raw line string
  if(raw_string) {
    ffree(raw_string);
    raw_string = NULL;
  }

  // Remove line string
  if(value_string) {
    ffree(value_string);
    value_string = NULL;
  }
    
  object_list_clear();
}

char * data_file_section::source_file_get(void)
{
  return(p_file->source_file_get());
}

void data_file_section::ppline(void)
{
  ppset(source_file_get(), line_number);
  pperror(FALSE, raw_string);
  ppclear();
}

data_file_section::data_file_section(DATA_FILE *p_file)
  : object_list_head(NULL),
    object_list(NULL),
    line_number(0),
    raw_string(NULL),
    value_string(NULL),
    value_int(0),
    value_float(0.0f)
{
  this->p_file = p_file;
  
  mark_clear(LINE_VALUE_MARK_LOADED);
  // Initialize values for section
  section_type[0] = '\0';
}

data_file_section::data_file_section(DATA_FILE *p_file, int line, char *p_raw_line, char *p_token, char *p_value)
  : object_list_head(NULL),
    object_list(NULL),
    line_number(line),
    raw_string(NULL),
    value_string(NULL),
    value_int(0),
    value_float(0.0f)
{
  this->p_file = p_file;
  
  mark_clear(LINE_VALUE_MARK_LOADED);  
  // Initialize values for section
  section_type[0] = '\0';  
  
  // Initialize values for line
  name_set(p_token);

  if(p_raw_line) {
    raw_string = sstrdup(p_raw_line);
  }
  
  if(p_value) {
    value_string = sstrdup(p_value);
    if(p_value[0] == '#' && isdigit(p_value[1])) {
      value_int = value_int_hexa = strtol(p_value+1,NULL,16);
      value_float = value_int;
    }
    else {
      value_int = atoi(p_value);
      value_int_hexa = strtol(p_value,NULL,16);
      value_float = atof(p_value);      
    }
  }  
}

data_file_section::~data_file_section(void)
{
  clear();
}

DATA_FILE_SECTION * data_file::section_new(void)
{
  DATA_FILE_SECTION *p_new = new DATA_FILE_SECTION(this);
  object_list_insert_last(p_new);
  return(p_new);
}

bool data_file::load_line(char *p_line)
{  
  char *ret = fgets(p_line, MAX_TOKEN_LEN, current_file);
  
  newline_delete(p_line);
  current_line_number++;
  
  return ret;
}
  
bool data_file::load(void)
{  
  DATA_FILE_SECTION *p_current = NULL;
  bool in_comment = FALSE;
  
  char line[MAX_TOKEN_LEN];
  while(load_line(line)) {
    LINE_TYPE type = line_type_get(line);
    
    /*
     * Skip comments in input file
     */
    if(type == LINE_COMMENT_START) {
      in_comment = TRUE;
      continue;
    }
    else if(type == LINE_COMMENT_STOP) {
      in_comment = FALSE;
      continue;
    } else if(in_comment) {
      continue;
    }
    
    switch(type) {
      case LINE_NONE:
      case LINE_COMMENT:
      case LINE_COMMENT_START:
      case LINE_COMMENT_STOP:
        break;
      // Create and load whole section
      case LINE_SECTION_START:
        p_current = section_new();
        p_current->load();
        break;
      case LINE_SECTION_ITEM:
        // Should not be here
        break;
      case LINE_SECTION_STOP:
        // Should not be here
        break;
      case LINE_SECTION_INCLUDE:
        {
          // Load sub-file
          char value[MAX_TOKEN_LEN];
          if(tokenize_line_command(line, NULL, value)) {
            DATA_FILE file;
            if(!file.load(value, FALSE)) {
              char local_file[MAX_FILENAME];
              return_path(current_dir, value, local_file, MAX_FILENAME);
              if(!file.load(local_file, FALSE)) {
                return(FALSE);
              }
            }
            
            import(&file);
          }
        }
        break;
    }
  }  
  return(TRUE);
}

bool data_file::load(const char *p_file, bool safe)
{
  FFILE f(NULL,p_file,"r",safe);
  if(f) {
    clear();
    
    source_file_set(p_file);
    current_file = f;
    return_dir(p_file, current_dir, MAX_FILENAME);
    
    load();
    
    f.close();
    return(TRUE);
  }
  return(FALSE);
}

void data_file::import(class data_file *p_data)
{
  DATA_FILE_SECTION *p_tmp = p_data->section_get_first();
  if(p_tmp) {
    object_list_insert_list(p_tmp);
  }
  
  // Clear original data file
  p_data->object_list_head_init();
}


void data_file::source_file_set(const char *p_file)
{
  assert(strlen(p_file) < MAX_FILENAME);
  strncpy(current_file_name, p_file, MAX_FILENAME);
}

void data_file::clear(void)
{  
  current_dir[0] = '\0';
  current_file_name[0] = '\0';
  current_line_number = 0;
  
  object_list_clear();
}

void data_file::print(void)
{
  DATA_FILE_SECTION *p_tmp = (DATA_FILE_SECTION *)object_list_get_first();
  if(p_tmp)
    p_tmp->section_print(0);  
}

data_file::data_file(void)
  : object_list_head(NULL)
{
  clear();
}

data_file::~data_file(void)
{
  clear();
}
