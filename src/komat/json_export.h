/*
 *        .þÛÛþ þ    þ þÛÛþ.     þ    þ þÛÛÛþ.  þÛÛÛþ .þÛÛþ. þ    þ
 *       .þ   Û Ûþ.  Û Û   þ.    Û    Û Û    þ  Û.    Û.   Û Ûþ.  Û
 *       Û    Û Û Û  Û Û    Û    Û   þ. Û.   Û  Û     Û    Û Û Û  Û
 *     .þþÛÛÛÛþ Û  Û Û þÛÛÛÛþþ.  þþÛÛ.  þþÛÛþ.  þÛ    Û    Û Û  Û Û
 *    .Û      Û Û  .þÛ Û      Û. Û   Û  Û    Û  Û.    þ.   Û Û  .þÛ
 *    þ.      þ þ    þ þ      .þ þ   .þ þ    .þ þÛÛÛþ .þÛÛþ. þ    þ
 *
 * AGE (C) AnakreoN
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

#ifndef __JSON_EXPORT_H__
#define __JSON_EXPORT_H__

void json_export_start(char *p_file);
void json_export_end(void);

void json_export_kont(EDIT_KONTEJNER *p_kont, EDIT_MATERIAL ** p_mat, int max_mat);
void json_export_kont_single(EDIT_KONTEJNER *p_kont, EDIT_MATERIAL ** p_mat, int max_mat);
void json_export_materialy(EDIT_MATERIAL ** p_mat, int max_mat);
void json_export_light(STATIC_LIGHT * p_light, int lightnum);                       
void json_export_poly(EDIT_MESH_POLY *p_poly, int polynum, EDIT_MATERIAL ** p_mat, int max_mat, EDIT_TEXT *p_light_list);
void json_export_level(LEVELINFO * p_Level);
void json_export_lightmap(int num, bitmapa *p_bmp);

#endif // __JSON_EXPORT_H__
