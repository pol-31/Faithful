#ifndef FAITHFUL_SRC_LOADER_BIOME_H_
#define FAITHFUL_SRC_LOADER_BIOME_H_

/*
 * There should be listed all biomes with required textures / models / sounds / etc...
 * */



enum class Biome {
  Desert,
  Forest,
  Beach,
  Fields,
  Mountain,
  TotalBiomes
};

Biome GetBiomeId(int map_x_coord, int max_y_coord) {
  /* ohhh... radix search (z-curve / Morton-curve) */
}
// locator as in spaceship: locate with radius R clockwise each 15 degree
// then if location changed - Release & Acquire ids of IAssetManager
// (if we're moving in predictable direction - don't need to check all directions)





#endif  // FAITHFUL_SRC_LOADER_BIOME_H_
