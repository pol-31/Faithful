#include "GlobalStateInfo.h"

#include "../common/CollisionManager.h"
#include "../common/DrawManager.h"
#include "../common/InputManager.h"
#include "../common/LoadingManager.h"
#include "../common/UpdateManager.h"

namespace faithful {
namespace details {

void GlobalStateInfo::StateMenu() {
  collision_manager->StateMenu();
  draw_manager->StateMenu();
  input_manager->StateMenu();
  update_manager->StateMenu();
}
void GlobalStateInfo::StateGame() {
  collision_manager->StateGame();
  draw_manager->StateGame();
  input_manager->StateGame();
  update_manager->StateGame();
}
void GlobalStateInfo::StatePaused() {
  collision_manager->StatePaused();
  draw_manager->StatePaused();
  input_manager->StatePaused();
  update_manager->StatePaused();
}

void GlobalStateInfo::StateCollisionPaused() {
  collision_manager->StatePaused();
}
void GlobalStateInfo::StateDrawPaused() {
  draw_manager->StatePaused();
}
void GlobalStateInfo::StateInputPaused() {
  input_manager->StatePaused();
}
void GlobalStateInfo::StateUpdatePaused() {
  update_manager->StatePaused();
}

void GlobalStateInfo::StateMenuLoadScreen() {
  collision_manager->StateMenuLoadScreen();
  draw_manager->StateMenuLoadScreen();
  input_manager->StateMenuLoadScreen();
}
void GlobalStateInfo::StateMenuStartScreen() {
  collision_manager->StateMenuStartScreen();
  draw_manager->StateMenuStartScreen();
  input_manager->StateMenuStartScreen();
}
void GlobalStateInfo::StateMenuConfigGeneral() {
  collision_manager->StateMenuConfigGeneral();
  draw_manager->StateMenuConfigGeneral();
  input_manager->StateMenuStartScreen();
}
void GlobalStateInfo::StateMenuConfigLocalization() {
  collision_manager->StateMenuConfigLocalization();
  draw_manager->StateMenuConfigLocalization();
  input_manager->StateMenuStartScreen();
}
void GlobalStateInfo::StateMenuConfigIO() {
  collision_manager->StateMenuConfigIO();
  draw_manager->StateMenuConfigIO();
  input_manager->StateMenuStartScreen();
}
void GlobalStateInfo::StateMenuConfigSound() {
  collision_manager->StateMenuConfigSound();
  draw_manager->StateMenuConfigSound();
  input_manager->StateMenuStartScreen();
}
void GlobalStateInfo::StateMenuConfigGraphic() {
  collision_manager->StateMenuConfigGraphic();
  draw_manager->StateMenuConfigGraphic();
  input_manager->StateMenuStartScreen();
}
void GlobalStateInfo::StateMenuConfigKeys() {
  collision_manager->StateMenuConfigKeys();
  draw_manager->StateMenuConfigKeys();
  input_manager->StateMenuStartScreen();
}

void GlobalStateInfo::StateGameLoadScreen() {
  collision_manager->StateGameLoadScreen();
  draw_manager->StateGameLoadScreen();
  input_manager->StateGameLoadScreen();
}
void GlobalStateInfo::StateGameDefault() {
  collision_manager->StateGameDefault();
  draw_manager->StateGameDefault();
  input_manager->StateGameDefault();
}
void GlobalStateInfo::StateGameInventory1() {
  collision_manager->StateGameInventory1();
  draw_manager->StateGameInventory1();
  input_manager->StateGameInventory();
}
void GlobalStateInfo::StateGameInventory2() {
  collision_manager->StateGameInventory2();
  draw_manager->StateGameInventory2();
  input_manager->StateGameInventory();
}
void GlobalStateInfo::StateGameInventory3() {
  collision_manager->StateGameInventory3();
  draw_manager->StateGameInventory3();
  input_manager->StateGameInventory();
}
void GlobalStateInfo::StateGameInventory4() {
  collision_manager->StateGameInventory4();
  draw_manager->StateGameInventory4();
  input_manager->StateGameInventory();
}
void GlobalStateInfo::StateGameGamePause() {
  collision_manager->StateGameGamePause();
  draw_manager->StateGameGamePause();
  input_manager->StateGameInventory();
}
void GlobalStateInfo::StateGameBattle() {
  collision_manager->StateGameBattle();
  draw_manager->StateGameBattle();
  input_manager->StateGameBattle();
}

} // namespace details
} // namespace faithful
