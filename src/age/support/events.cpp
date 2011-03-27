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

#include "defs.h"
#include "utils.h"
#include "log.h"
#include "events.h"

int event_generator::last_event = FIRST_SYSTEM_GENERATED_EVENT;

const char * game_event::translation_table[] =
{
  "EV_NONE",
  "EV_TEST",
  "EV_CALLBACK",
  "UI_BLOCK_KEYS",
  "UI_UNBLOCK_ALL",
  "UI_KEY_DOWN",
  "UI_KEY_UP",
  "UI_MOUSE_MOVE",
  "UI_MOUSE_EVENT",
  "UI_APP_ACTIVATED",
  "UI_APP_DEACTIVATED",
  "UI_QUIT"  
};

const char * game_event::translate_event(int type)
{
  if(type >= 0 && type < (int)sizeof(game_event::translation_table)) {
    return(translation_table[type]);
  }
  else {
    return("USER_DEFINED");
  }
}

void event_stream::testcase(void)
{
  extend(EXTEND_STEP);
  clear();

  GAME_EVENT ev(EV_TEST);

  assert(empty());
  assert(!full());
  add(ev);
  assert(read_first == 0);
  assert(read_next == 0);
  assert(write_first == 0);
  assert(write_next == 1);
  commit();
  assert(!empty());
  assert(!full());
  assert(read_first == 0);
  assert(read_next == 1);
  assert(write_first == 1);
  assert(write_next == 1);
  ev = get();
  assert(empty());
  assert(!full());
  assert(read_first == 1);
  assert(read_next == 1);
  assert(write_first == 1);
  assert(write_next == 1);
}
