no relative paths!
"path/from/" -> deduce all formats
"path/to/" <- 4 folders models/music/sounds/textures + models.txt
naming for model&bin/music/sounds: id_actualName.extension
naming for textures: id_actualName_{hdr/ldr/rrrg/rrr1/rgb1}.astc

===
5 dynamic textures for model (GL_TEXTURE0-GL_TEXTURE4)
5 static textures for fonts (GL_TEXTURE11-GL_TEXTURE15) - but we can pack into one
1 texture for button
N texture for vfx objects
N textures for vfx effects

TODO: grass, foliage

vfx - grey_packing(including LUT (look-up textures) -
channel_packing - the same - use rgba)
+ another one (vector map) for effect like under fire - some
  vector displacemets
  TODO: do we need to separate LUT textures to another one?
  TODO: do we need to separate single-channeled textures to another one?
  TODO: do we need to separate vfx and other like buttons or logo?

TODO (map): color variance, water::flow/vorticity,
terrain cavity, topo, sometimes its enough only few or one bit
rather than full uint8 <- maybe we can do smt with this

===
Compression quality/speed:
From:
"https://chromium.googlesource.com/external/github.com/ARM-software/astc-encoder/+/HEAD/Docs/FormatOverview.md"
"""
ASTC at 8 bpt for LDR formats is comparable in quality to BC7 at 8 bpt.
ASTC at 8 bpt for HDR formats is comparable in quality to BC6H at 8 bpt.
"""
which is 4x4, so let's use 4x4 by default (maybe adjust more lately,
but currently there's no need in that) + ASTCENC_PRE_MEDIUM
Anyway we're using our own resolutions for models & textures, and
using uniform format for all allows us not to check it each time
before compression and always use 4x4 for fast decompression.
(- asm instruction for reading textures headers)

===

If decompression (game): hdr | ldr + build_DECOMPRESSION_ONLY
If decompression: hdr | ldr ASTCENC_FLG_DECOMPRESS_ONLY
If compression: hdr, ldr, ldr_normal, ldr_alpha_perceptual
where:
hdr:{ibl};
ldr_normal:{model_normal_map};
ASTCENC_FLG_MAP_NORMAL (+recover z-coord in shader)
ldr_alpha_perceptual{model_albedo, UI, VFX};
ASTCENC_FLG_USE_ALPHA_WEIGHT ASTCENC_FLG_USE_PERCEPTUAL
ldr:{other: maps, other model_textures, etc.}


Notes:
*For HDR we don't need rbga, because we need only light effect,
so swizzle is rgb1

===

Name/Type                           Res        Format         Context
___Extra
vfx                                  -       rgba uint8      ldr_alpha_perceptual
ui                                   -       rgba uint8      ldr_alpha_perceptual
leafs/foliage TODO<_---------------------------------------------------------

___IBL
ibl                                  -       rgb1 float32    hdr

___Model textures
albedo                               -       rgba uint8      ldr_alpha_perceptual
metal-roughness                      -       rrrg uint8      ldr
occlusion                            -       rrr1 uint8      ldr
emission                             -       rgb1 uint8      ldr
normal                               -       rrrg uint8      ldr_normal

___Map textures
map_height_terrain                 0.5 m     float16 rrr1    ldr
map_height_objects                 0.5 m     float16 rrr1    ldr
map_height_water                   0.5 m     float16 rrr1    ldr

map_variance_trees_bush
map_variance_undergrowth_stealth
map_variance_rock_color
map_variance_foliage_color
map_variance_lichen_density

map_erosion_wear                   0.5 m     uint8 rrr1      ldr
map_erosion_flow                   0.5 m     uint8 rrr1      ldr
map_erosion_deposition             0.5 m     uint8 rrr1      ldr

map_terrain_cavity
map_water_flow
map_water_vorticity

map_placement_trees                0.5 m     uint8 rrr1      ldr
map_placement_block_bush           0.5 m     uint8 rrr1      ldr
map_placement_undergrowth          0.5 m     uint8 rrr1      ldr
map_placement_stealth_plants       0.5 m     uint8 rrr1      ldr
map_placement_pickups              0.5 m     uint8 rrr1      ldr
map_placement_natural_resources    0.5 m     uint8 rrr1      ldr

map_region_destructibility         0.5 m     uint8 rrr1      ldr
map_region_activity_space          0.5 m     uint8 rrr1      ldr

map_topo_roads
map_topo_water
map_topo_objects

map_ecotope_effect
map_ecotope
