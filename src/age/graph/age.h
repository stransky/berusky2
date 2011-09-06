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

/*
 * Main include file
 */

#ifndef __AGE_H__
#define __AGE_H__

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <limits.h>
#include <float.h>
#include <math.h>

// Defines for linux/windows
#include "compatibility.h"

// Constants and types
#include "defs.h"
#include "defs_config.h"
#include "typ.h"
#include "typ_graph.h"
#include "floats.h"
#include "ferguson.h"

// Service routines
#include "log.h"
#include "utils.h"
#include "mem_alloc.h"
#include "ini.h"

#include "flag.h"
#include "lock.h"
#include "change.h"
#include "array.h"
#include "barray.h"

// Extended types
#include "color_helpers.h"
#include "color.h"
#include "color_hsv.h"
#include "vector.h"
#include "polar.h"
#include "quat.h"
#include "matrix.h"
#include "matrix_notified.h"
#include "matrix_animated.h"
#include "intersect.h"

// OpenGL includes
#include "opengl.h"
#include "gext.h"
#include "vertex_arrays.h"
#include "glwrapper.h"
#include "texture_unit.h"

// Helper types
#include "mtree.h"
#include "mtree_named.h"
#include "btree.h"
#include "llist.h"
#include "llist_named.h"
#include "stack.h"

// Basic graphics
#include "camera_interface.h"
#include "world_interface.h"
#include "gpipe.h"

#include "surface.h"
#include "surface_sdl.h"
#include "surface_sw.h"
#include "surface_mask.h"
#include "surface_height.h"

#include "alpha.h"

// Bounding boxes
#include "box.h"
#include "aabb.h"
#include "obb.h"

// Normals
#include "normals.h"

// Lighting
#include "lighting.h"

#include "scene_reference.h"

// Basic scene classes
#include "object_base.h"
#include "object_name.h"
#include "object_mark.h"
#include "object_select.h"
#include "object_list.h"
#include "object_hierarchy.h"
#include "object_scene.h"

// Extended object classes
#include "object_world.h"
#include "object_box.h"
#include "object_transformation.h"
#include "object_geometry.h"

// Animations
#include "animation_track.h"
#include "animation_track_linear.h"
#include "animation_track_bezier.h"
#include "animation_time.h"
#include "animation.h"

// Main scene classes
#include "camera.h"
#include "material_texture.h"
#include "material.h"
#include "mesh.h"
#include "draw.h"

// Scene class
#include "selection.h"
#include "scene.h"
#include "scene_import_b2m.h"
#include "scene_import_3ds.h"
#include "scene_import_txt.h"

#include "print.h"

// Main engine class
#include "graph3d.h"

// Loop/events
#include "timer.h"
#include "input.h"
#include "age_main.h"

#endif //__AGE_H__
