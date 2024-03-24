5 dynamic textures for model (GL_TEXTURE0-GL_TEXTURE4)
1 static texture for fonts (GL_TEXTURE5) (we can pack them into one)
1 rgba texture for button
1 rgba texture for rgba vfx (default grey/channel packing)
N rrr1 textures for noises (throwing powder, phenomenon area, vfx_noise, other effects)

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

___________________________________________
Notes:
*For HDR we don't need rbga, because we need only light effect,
so swizzle is rgb1

*Terrain cavity - for ssao ("dynamic occlusion")
*Terrain occlusion - baked from higher resolution for "static occlusion"
*map_topo_ecotopes - we always should track closest ecotope(ecotope_middle_point)
*map_topo_water - vertex color - flow/vorticity types, so we removed them:
- map_water_flow (2 channels?)        1.0 m     uint8 rrrg      ldr
- map_water_vorticity                 1.0 m     uint8 rrr1      ldr
- type (type of liquid)
- width defined by terrain / terrain_/water_-height_map
  *map_topo_roads - vertex color - width, type

*grass/folliage - very simple objects (no veins, NO TEXTURES - only shader params,
so we don't need translucency there), drawn by 5-11 triangles (tesselation from lines/points)
folliage params:
- color - tex1d (vector)
- glossy - tex1d (vector)
- physics (weight, bending, Bezie's curve params, etc)
- ssao

what is Ecotope (need to somehow blend for each):
1) int ground_type_id {main, strand, etc}
2) int sky_type_id {day, night}
3) int background_music_id {day, night, battle}
4) std::map<float chance, int weather_id>
5) std::map<float chance, int tree_bush_id>
6) std::map<float chance, int undergrowth_stealth_id>
7) std::map<float chance, int rock_id>
8) std::map<float chance, int foliage_id>
9) std::map<float chance, int furniture_type_id>

___________________________________________

Threre is 2 types of maps:
__solid single map__:
- default open with all main events
- default open but "scary" in which you can reach only
  through special entrance objects. Layout absolutely the same,
  but map_topo_objects / map_topo_ecotope differs
  __separate 1km x 1km zone__:
- dreams - while sleeping
- special zones (e.g. cavens, boss location, bonus - hidden, etc.)

when dynamically load another one map tile, we should
look at object_height_priority, so then enemy (lowest priority) will
be placed after all buildings (highest prioriy), so
we indeed have map_height_object, BUT it computed dynamically,
as well as navmesh, which is also generated dynamically

DYNAMIC NAVMESH GENERATION: each 0.5 m sq we have height (which calculated
dynamically wrt objects_height) and (possibly) object with its own information
inside the vertex_color parameter about how to pass it
(in principle, our map is tiled by 0.5m...)

___________________________________________


TODO: object parkour for each object mesh
TODO: define all types of roads, liquid, weather, folliage, etc...
TODO: we need tool to "fly and draw" in real-time (read about flood-fill algorithm (a*?))

___________________________________________
OVERVIEW
___________________________________________
EXAMPLES:
vfx_main.astc
ui.astc
noise_vfx1.astc
hdr_ibl.astc
map_height_terrain.astc

NOTES:
hdr has only one mode and one swizzle (rgb1)
ldr has only uint8 type (for maps too, because WorldMachine con't upload in float16 and
only uint16, but astc don't have such mode. all {name}__{swizzle}.astc is ldr, so no need
to say it. nmap astc context is used only for tinygltf::model_processing (we don't store
nmap for terrain or vfx), which is handled internally, so we're not add suffix with this
__________________________________________________
COMPRESSION
__________________________________________________
MECHANIC:
-- for non-model textures:
if (prefix == map) { uint8 rrr1 ldr }
else if (prefix == noise) { uing8 rrr1 ldr }
else if (prefix == font) { uing8 rrr1 ldr }
else if (extension == hdr) { float32 rgb1 hdr } (prefix + "hdr_")
else { uint8 rgba ldr } (the same name, no suffixes, no prefixes)
-- for model textures (model_name + type):
albedo: uint8 rgba ldr_alpha_perceptual
metal_rough: uint8 rgba ldr_alpha_perceptual
normal: uint8 rgba ldr_alpha_perceptual
emission: uint8 rgba ldr_alpha_perceptual
ao: uint8 rgba ldr_alpha_perceptual

MODES: (total 4 swizzles, 4 contexts):
uint8 rgba      ldr_alpha_perceptual
uint8 rrrg      ldr
uint8 rrr1      ldr
uint8 rgb1      ldr
uint8 rrrg      ldr_normal
float32 rgb1    hdr
__________________________________________________
DECOMPRESSION
__________________________________________________
MECHANIC:
./maps/ -> uint8 raaa ldr
./noises/ -> uint8 raaa ldr
./models/ -> wrt type from {name}_{type}.astc, where
type is either {albedo, metal_rough, normal, emission, ao} -> uint8 raaa ldr
./ if prefix == font -> uint8 raaa ldr
./ else -> rgba

MODES (total 3 swizzles, 2 contexts):
rgba --> rgba ldr
rrr1 --> raaa ldr
rgb1 --> rgba ldr
rrrg --> ra11 ldr
hdr  --> rgba hdr
__________________________________________________
assets_info.txt
__________________________________________________
MECHANIC:
sound:name:id, music:name:id, model:..model_data..:id

IMPLEMENTATION:
after processing, we look at user_destination and:
if (!assets_info.txt) {
create assets_info.txt;
for each directory std::filesystem::dir_interator() + assign ++id;
}
else {
for each directory std::filesystem::dir_interator() + check if not exist ++id
}
__________________________________________________
TODO: models with equal names (even dirs different) can't be processed, replace request to user
__________________________________________________
__________________________________________________


// TODO: need real resolution (due to World Machine free version)
Name/Type                           Res        Format         Context
___Extra
vfx main                             -       uint8 rgba      ldr_alpha_perceptual
ui                                   -       uint8 rgba      ldr_alpha_perceptual
noise_vfx                            -       uint8 rrr1      ldr
noise_{other}                        -       uint8 rrr1      ldr
fonts                                -       uint8 raw        -

___IBL
ibl                                  -       float32 rgb1    hdr

___Model textures
albedo                               -       uint8 rgba      ldr_alpha_perceptual
metal-roughness                      -       uint8 rrrg      ldr
occlusion                            -       uint8 rrr1      ldr
emission                             -       uint8 rgb1      ldr
normal                               -       uint8 rrrg      ldr_normal

___Map textures
map_height_terrain                 0.5 m     uint8 rrr1       -
map_height_water                   0.5 m     uint8 rrr1       -

map_erosion_wear                   0.5 m     uint8 rrr1      ldr
map_erosion_flow                   0.5 m     uint8 rrr1      ldr
map_erosion_deposition             0.5 m     uint8 rrr1      ldr
map_terrain_cavity                 0.5 m     uint8 rrr1      ldr
map_terrain_occlusion              0.5 m     uint8 rrr1      ldr

map_placement_trees                1.0 m     uint8 rrr1      ldr
map_placement_block_bush           1.0 m     uint8 rrr1      ldr
map_placement_undergrowth          1.0 m     uint8 rrr1      ldr
map_placement_stealth_plants       1.0 m     uint8 rrr1      ldr
map_placement_pickups              2.0 m     uint8 rrr1      ldr
map_placement_natural_resources    2.0 m     uint8 rrr1      ldr
map_placement_furniture            2.0 m     uint8 rrr1      ldr

map_roads                          0.5 m     uint8 rrr1      ldr
map_topo_roads                       -       graph mesh       -
map_topo_water (+ flow + vorticity)  -       graph mesh       -

map_topo_objects                     -       point mesh       -
map_topo_ecotope                     -       point mesh       -
