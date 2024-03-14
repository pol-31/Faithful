#ifndef FAITHFUL_SRC_MODULES_UPDATINGMODULE_H_
#define FAITHFUL_SRC_MODULES_UPDATINGMODULE_H_

#include "../loader/ModelPool.h"

#include "../entities/PhenomenonArea.h"
#include "../environment/Environment.h"
#include "../player/PlayerCharacter.h"

namespace faithful {
namespace details {

/** It optimizes the order of updating operations
 * to the OpenGL context to avoid consecutive Depth Test failures
 * */
class UpdatingModule {
 public:
  UpdatingModule() = delete;
  UpdatingModule(assets::ModelPool& model_pool,
                 Environment& environment,
                 PhenomenonAreaPool& phenomenon_area_pool,
                 PlayerCharacter& player_character);

  void ProcessTask();

  // updates all models actions logic / affects;
  // phenomenon area (e.g. gas intensity)
  void UpdateGameLogic();
  void UpdateAnimations();

 private:
  // There are Update() based either on GlobalTime or some __state__
  // Vegetation, Liquid, Terrain doesn't depend on GlobalTime or __state__, so we don't need them there
  // Weather, Sky, PhenomenonAreaPool depend on GlobalTime
  assets::ModelPool& model_pool_; /// __state__-based
  PhenomenonAreaPool& phenomenon_area_pool_; /// __time__-based
  PlayerCharacter& player_character_;/// __state__-based
  SkyHandler& sky_handler_; /// __time__-based
  WeatherHandler& weather_handler_; /// __time__-based
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_MODULES_UPDATINGMODULE_H_
