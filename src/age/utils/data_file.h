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
 * Data files
 */

#ifndef __DATA_FILES_H__
#define __DATA_FILES_H__

#define SECTION_NAME_START          '{'
#define SECTION_NAME_STOP           '}'
#define SECTION_NAME_NAME           "name"
#define SECTION_NAME_TYPE           "type"
#define SECTION_NAME_INCLUDE        "include"

#define SECTION_NAME_COMMENT        "//"
#define SECTION_NAME_COMMENT2       '#'
#define SECTION_NAME_COMMENT_START  "/*"
#define SECTION_NAME_COMMENT_STOP   "*/"

typedef enum {

  LINE_NONE = 0,
  
  LINE_SECTION_START,
  LINE_SECTION_STOP,
  LINE_SECTION_INCLUDE,
  LINE_SECTION_ITEM,
  
  LINE_COMMENT,
  LINE_COMMENT_START,
  LINE_COMMENT_STOP,
  
} LINE_TYPE;

class data_file;
typedef class data_file DATA_FILE;

/*
  Class for one data file section. This section can be one line 
  or 
*/

// Mark line as loaded
#define LINE_VALUE_MARK_LOADED     (1<<0)

typedef class data_file_section
  : public object_list_head,
    public object_list
{
  /*
   * Parent data file
   */
  DATA_FILE *p_file;
  
  /*
   * A line number of this file
   */  
  int     line_number;
  
  /*
   * raw line
   */
  char   *raw_string;
  
  /*
   * Values for this line
   */
  char   *value_string;
  
public:  
  
  int     value_int;
  int     value_int_hexa;
  float   value_float;
  
public:
  
  void source_line_set(int line)
  {
    line_number = line;
  }
  int source_line_get(void)
  {
    return(line_number);
  }

  char * source_file_get(void);
  
  char * line_name_get(void)
  {
    return(name_get());
  }
  
  char * line_raw_string_get(void)
  {
    return(raw_string);
  }

  char * line_value_string_get(void)
  {
    return(value_string);
  }
  
  int    line_value_int_get(void)
  {
    return(value_int);
  }
  
  unsigned int line_value_uint_get(void)
  {
    return((unsigned int)value_int);
  }
  
  unsigned int line_value_uint_hexa_get(void)
  {
    return((unsigned int)value_int_hexa);
  }
  
  float  line_value_float_get(void)
  {
    return(value_float);
  }
  
  // Normalize angle from degrees to radians
  float  line_value_angle_get(void)
  {
    return(DEG2RAD(value_float));
  }
  
public:

  bool line_value_is_loaded(void)
  {
    return(mark_get(LINE_VALUE_MARK_LOADED));
  }
  
  void line_mark_loaded(void)
  {
    mark_set(LINE_VALUE_MARK_LOADED);
  }
  
public:
  
  bool is_line(void)
  {
    return(value_string != NULL);
  }
  
  bool is_line_raw(void)
  {
    return(raw_string != NULL);
  }

public:

  void line_print(void);
  
  /*
   * Type of this section
   */
private:
  
  char section_type[MAX_NAME];
  
public:

  bool is_section(void)
  {
    return(section_type[0] != '\0');
  }

public:
  
  char * section_name_get(void)
  {
    return(name_get());
  }

  char * section_type_get(void)
  {
    return(section_type);
  }
      
public:
  
  data_file_section * section_new(int line, char *p_line, char *p_token, char *p_value);
  bool                section_new(int line, char *p_line);
  
public:
  
  void section_print(int indent);
  
public:
  
  /* 
   * Get first child of this section,
   * it can be line (value) or other section
   */
  data_file_section  * section_child_get(void)
  {
    return((data_file_section *)object_list_get_first());
  }
  
  /* 
   * Find specified child   
   */
  data_file_section  * section_child_find(const char *p_name)
  {
    return((data_file_section *)object_list_find(p_name));
  }
  
public:
  
  /* 
   * Get next section
   */
  data_file_section  * section_next(void)
  {
    return((data_file_section *)object_list_next());
  }  
  
public:
  
  void load(void); 
  void clear(void);
  void ppline(void);
  
public:
  data_file_section(DATA_FILE *p_file);
  data_file_section(DATA_FILE *p_file, int line,
                    char *p_raw_line, char *p_token, 
                    char *p_value);
  ~data_file_section(void);
  
} DATA_FILE_SECTION;

/*
 * List of all sections in the list
 */
typedef class data_file : 
  public object_list_head
{
  int     current_line_number;
  char    current_file_name[MAX_FILENAME];  
  char    current_dir[MAX_FILENAME];  
  FFILE   current_file;

private:
  
  DATA_FILE_SECTION * section_new(void);
  
public:

  DATA_FILE_SECTION * section_get_first(void)
  {
    return((DATA_FILE_SECTION *)object_list_get_first());
  }
  
  DATA_FILE_SECTION * section_find(const char *p_name)
  {
    return((DATA_FILE_SECTION *)object_list_find(p_name));
  }
  
public:
  
  void   import(DATA_FILE *p_data);

private:
  
  bool   load(void);
  void   source_file_set(const char *p_file);
  
public:
  
  int    current_line_get(void)
  {
    return(current_line_number);
  }
  
  char * source_file_get(void)
  {
    return(current_file_name);
  }  

  bool   load_line(char *p_line);
  bool   load(const char *p_file, bool safe = TRUE);

public:
  
  void   print(void);
  void   clear(void);

public:
  
  data_file(void);
  ~data_file(void);
  
} DATA_FILE;

#endif // __DATA_FILES_H__
