#include "Biome.h"

#include "../executors/AudioThreadPool.h"

namespace faithful {

BiomeHandler::BiomeHandler(
    details::AudioThreadPool* audio_thread_pool,
    LiquidHandler* liquid_handler,
    SkyHandler* sky_handler,
    TerrainHandler* terrain_handler,
    VegetationHandler* vegetation_handler,
    WeatherHandler* weather_handler)
    : audio_thread_pool_(audio_thread_pool),
      liquid_handler_(liquid_handler),
      sky_handler_(sky_handler),
      terrain_handler_(terrain_handler),
      vegetation_handler_(vegetation_handler),
      weather_handler_(weather_handler) {
  Init();
}

void BiomeHandler::Init() {
  // TODO: what to set we should store externally in file biomes.txt with
  //   with all id/color/music characteristics and load them there
  biomes_;
}

void BiomeHandler::SetBiome(int new_id) {
  if (cur_biome_id_ == new_id) {
    return;
  }
  cur_biome_id_ = new_id;

  audio_thread_pool_->SetBackground(biome_[cur_biome_id_].music_id);

  liquid_handler_->SetLiquid(biomes_[cur_biome_id_].liquid);
  sky_handler_->SetSky(biomes_[cur_biome_id_].sky);
  terrain_handler_->SetTerrain(biomes_[cur_biome_id_].terrain);
  vegetation_handler_->SetVegetation(biomes_[cur_biome_id_].vegetation);
  weather_handler_->SetWeather(biomes_[cur_biome_id_].weather);
}

} // namespace faithful
