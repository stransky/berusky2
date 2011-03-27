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

/* State-bloky pro textury
*/

#ifndef   __TEXTURE_FUNC_H__
#define   __TEXTURE_FUNC_H__


/*
 *************************************************************************** 
    Popis editacni funkce
 ***************************************************************************
*/
/*
typedef struct _GAME_TEXT_BLOK {

  char   name[MAX_NAME];                 // jmeno bloku
  int    funkce;                         // michaci funkce
  int    edit_funkce;                    // editacni funkce (-1 pro default)
  int    textur;                         // pocet porebnych textur 1 (2,3,4 po trebuji extra dialog)
  int    delka;                          // delka bloku (pocet textur)
  int    textury_typ[MAX_GL_TEXT_UNITS]; // pozadovany typ textur pro dany efekt
  int    text_funkce[MAX_GL_TEXT_UNITS]; // pozadovany typ textur pro dany efekt
  
} GAME_TEXT_BLOK;
*/

//#define   MAX_STAGE_TEXT_FUNC   9
//#define   MAX_STAGE_TEXT_BLOK   9

//static GAME_TEXT_BLOK st_blok[MAX_STAGE_TEXT_BLOK]; // Tabulka funkci pro editor  

/*
 ***************************************************************************
    Textury + Parametry + Funkce v materialu
 ***************************************************************************
*/

/* Define pro stage_funkce
*/
/*
#define DOT3_BUMP             0
#define DOT3_TEXT             1
#define DOT3_ENV              2

#define DUDV_BUMP             0 // normalova mapa
#define DUDV_ENV              1 // Environment-mapa
#define DUDV_TEXT             2 // zakladni textura
*/
/* Automaticky nahrazovani 
*/
/*
#define AUTO_BUMP_DOT3        5 // dot3 color blok
#define AUTO_BUMP_DOT3ENV     6 // dot3 env blok
*/

#endif // __TEXTURE_FUNC_H__
