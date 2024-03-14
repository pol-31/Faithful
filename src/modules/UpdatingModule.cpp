#include "UpdatingModule.h"

namespace faithful {
namespace details {

UpdatingModule::UpdatingModule(
    assets::ModelPool& model_pool,
    Environment& environment,
    PhenomenonAreaPool& phenomenon_area_pool,
    PlayerCharacter& player_character)
    : model_pool_(model_pool),
      phenomenon_area_pool_(phenomenon_area_pool),
      player_character_(player_character),
      sky_handler_(environment.sky_handler),
      weather_handler_(environment.weather_handler) {}

void UpdatingModule::ProcessTask() {
  // TODO: all from task_queue_
}

void UpdatingModule::UpdateAnimations() {
  // TODO: for each model call UpdateAnimation()
}
void UpdatingModule::UpdateGameLogic() {
  // TODO: for each model call Update()
}

} // namespace details
} // namespace faithful
