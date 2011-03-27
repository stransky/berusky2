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

#ifndef __SCENE_LOAD_H__
#define __SCENE_LOAD_H__


//v. 2
#define  CHUNK_KEYFRAME_ROT2  2006 // hlavicka key-framove animace
#define  CHUNK_KEYFRAME_NODE  2007 // vetev animace - pro jeden objekt
#define  CHUNK_KEYFRAME_POS   2008 // chunk s pozicema
#define  CHUNK_KEYFRAME_ROT   2009 // chunk s rotacema
#define  CHUNK_KEYFRAME_SCS   2010 // chunk s scalama

//v. 3 - sim animace
#define  CHUNK_SIM_ROOT        2020 // hlavicka key-framove animace
#define  CHUNK_SIM_NODE        2021 // vetev animace - pro jeden objekt
#define  CHUNK_SIM_NODE_EXT    2022 // vetev animace - pro jeden objekt
#define  CHUNK_SIM_NODE_EXT2   2024 // vetev animace - pro jeden objekt

/* 
 * Scene objects chunks
 */
#define  CHUNK_STATIC_LIGHT        5003 // chunk se statickym svetlem sceny
#define  CHUNK_STATIC_LIGHT_MESH   5004 // chunk se statickym svetlem (patri k naposledy nactenemu meshi)

#define  CHUNK_DYNAMIC_LIGHT       5100 // chunk se dynamickycm svetlem
#define  CHUNK_DYNAMIC_LIGHT_MESH  5101 // chunk se dynamickycm svetlem  (patri k naposledy nactenemu meshi)

#define  CHUNK_EXTRA_LIGHT         5200 // chunk se extra dynamickycm svetlem
#define  CHUNK_EXTRA_LIGHT_MESH    5201 // chunk se extra dynamickycm svetlem  (patri k naposledy nactenemu meshi)

#define  CHUNK_ZRCADLO             6000 // chunk se statickym svetlem
#define  CHUNK_ZRCADLO_2           6001 // chunk se statickym svetlem
#define  CHUNK_ZRCADLO_3           6002 // chunk se statickym svetlem
#define  CHUNK_ZRCADLO_3_POLOZKA   6003 // chunk se statickym svetlem

#define  CHUNK_FLARE               7000 // chunk se flare
#define  CHUNK_FLARE_SLOZENY       7001 // chunk se slozkama ke flare
#define  CHUNK_FLARE_MESH          7002 // chunk se flare_pripojeny k mesi
#define  CHUNK_FLARE_LIGHT         7003 // chunk se flare ke svetlu

#define  CHUNK_FLARE_EXT           7010 // chunk se flare
#define  CHUNK_FLARE_MESH_EXT      7011 // chunk se flare_pripojeny k mesi
#define  CHUNK_FLARE_LIGHT_EXT     7012 // chunk se flare ke svetlu

#define  CHUNK_AMBIENT             8000 // chunk s ambientnim svetlem
#define  CHUNK_LEVEL_KONFIG        8001 // chunk s kofiguraci levelu
#define  CHUNK_LEVEL_KONFIG_EXT    8002 // chunk s kofiguraci levelu

#define  CHUNK_MLHOKOSTKA          8010 // chunk se starou mlznou kostkou
#define  CHUNK_MLHOKOSTKA_2        8011 // chunk s mlznou kostkou

#define  CHUNK_ENVIRONMENT         8020 // chunk s environmentem

#define  CHUNK_KAMSET              8021 // chunk s environmentem
#define  CHUNK_KAMSET_2            8022 // chunk s environmentem

#define  ROOT_NODE_NAME            "$$$ROOT_NODE$$$"

#endif // __SCENE_LOAD_H__
