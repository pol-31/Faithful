#ifndef FAITHFUL_SRC_LOADER_BIOME_H_
#define FAITHFUL_SRC_LOADER_BIOME_H_

#include <vector>

#include "Liquid.h"
#include "Sky.h"
#include "Terrain.h"
#include "Vegetation.h"
#include "Weather.h"

namespace faithful {

namespace details {

class AudioThreadPool;

} // namespace details

struct Biome {
  Liquid liquid;
  Sky sky;
  Terrain terrain;
  Vegetation vegetation;
  Weather weather;
  int music_id;
};

class BiomeHandler {
 public:
  BiomeHandler() = delete;
  BiomeHandler(details::AudioThreadPool* audio_thread_pool,
               LiquidHandler* liquid_handler,
               SkyHandler* sky_handler,
               TerrainHandler* terrain_handler,
               VegetationHandler* vegetation_handler,
               WeatherHandler* weather_handler);

  BiomeHandler(const BiomeHandler&) = default;
  BiomeHandler& operator=(const BiomeHandler&) = default;

  BiomeHandler(BiomeHandler&&) = default;
  BiomeHandler& operator=(BiomeHandler&&) = default;

  void SetBiome(int new_id);

 private:
  void Init();

  int cur_biome_id_{-1};

  details::AudioThreadPool* audio_thread_pool_;

  LiquidHandler* liquid_handler_;
  SkyHandler* sky_handler_;
  TerrainHandler* terrain_handler_;
  VegetationHandler* vegetation_handler_;
  WeatherHandler* weather_handler_;

  std::vector<Biome> biomes_;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_BIOME_H_
