#include "GlfwWindowUserPointer.h"

#include "../common/CollisionManager.h"
#include "../common/DrawManager.h"
#include "../common/InputManager.h"
#include "../common/LoadingManager.h"
#include "../common/UpdateManager.h"

namespace faithful {
namespace details {

void GlfwWindowUserPointer::StateMenu() {
  collision_manager->StateMenu();
  draw_manager->StateMenu();
  input_manager->StateMenu();
  update_manager->StateMenu();
}
void GlfwWindowUserPointer::StateGame() {
  collision_manager->StateGame();
  draw_manager->StateGame();
  input_manager->StateGame();
  update_manager->StateGame();
}
void GlfwWindowUserPointer::StatePaused() {
  collision_manager->StatePaused();
  draw_manager->StatePaused();
  input_manager->StatePaused();
  update_manager->StatePaused();
}

void GlfwWindowUserPointer::StateCollisionPaused() {
  collision_manager->StatePaused();
}
void GlfwWindowUserPointer::StateDrawPaused() {
  draw_manager->StatePaused();
}
void GlfwWindowUserPointer::StateInputPaused() {
  input_manager->StatePaused();
}
void GlfwWindowUserPointer::StateUpdatePaused() {
  update_manager->StatePaused();
}

void GlfwWindowUserPointer::StateMenuLoadScreen() {
  collision_manager->StateMenuLoadScreen();
  draw_manager->StateMenuLoadScreen();
  input_manager->StateMenuLoadScreen();
}
void GlfwWindowUserPointer::StateMenuStartScreen() {
  collision_manager->StateMenuStartScreen();
  draw_manager->StateMenuStartScreen();
  input_manager->StateMenuStartScreen();
}
void GlfwWindowUserPointer::StateMenuConfigGeneral() {
  collision_manager->StateMenuConfigGeneral();
  draw_manager->StateMenuConfigGeneral();
  input_manager->StateMenuStartScreen();
}
void GlfwWindowUserPointer::StateMenuConfigLocalization() {
  collision_manager->StateMenuConfigLocalization();
  draw_manager->StateMenuConfigLocalization();
  input_manager->StateMenuStartScreen();
}
void GlfwWindowUserPointer::StateMenuConfigIO() {
  collision_manager->StateMenuConfigIO();
  draw_manager->StateMenuConfigIO();
  input_manager->StateMenuStartScreen();
}
void GlfwWindowUserPointer::StateMenuConfigSound() {
  collision_manager->StateMenuConfigSound();
  draw_manager->StateMenuConfigSound();
  input_manager->StateMenuStartScreen();
}
void GlfwWindowUserPointer::StateMenuConfigGraphic() {
  collision_manager->StateMenuConfigGraphic();
  draw_manager->StateMenuConfigGraphic();
  input_manager->StateMenuStartScreen();
}
void GlfwWindowUserPointer::StateMenuConfigKeys() {
  collision_manager->StateMenuConfigKeys();
  draw_manager->StateMenuConfigKeys();
  input_manager->StateMenuStartScreen();
}

void GlfwWindowUserPointer::StateGameLoadScreen() {
  collision_manager->StateGameLoadScreen();
  draw_manager->StateGameLoadScreen();
  input_manager->StateGameLoadScreen();
}
void GlfwWindowUserPointer::StateGameDefault() {
  collision_manager->StateGameDefault();
  draw_manager->StateGameDefault();
  input_manager->StateGameDefault();
}
void GlfwWindowUserPointer::StateGameInventory1() {
  collision_manager->StateGameInventory1();
  draw_manager->StateGameInventory1();
  input_manager->StateGameInventory();
}
void GlfwWindowUserPointer::StateGameInventory2() {
  collision_manager->StateGameInventory2();
  draw_manager->StateGameInventory2();
  input_manager->StateGameInventory();
}
void GlfwWindowUserPointer::StateGameInventory3() {
  collision_manager->StateGameInventory3();
  draw_manager->StateGameInventory3();
  input_manager->StateGameInventory();
}
void GlfwWindowUserPointer::StateGameInventory4() {
  collision_manager->StateGameInventory4();
  draw_manager->StateGameInventory4();
  input_manager->StateGameInventory();
}
void GlfwWindowUserPointer::StateGameGamePause() {
  collision_manager->StateGameGamePause();
  draw_manager->StateGameGamePause();
  input_manager->StateGameInventory();
}
void GlfwWindowUserPointer::StateGameBattle() {
  collision_manager->StateGameBattle();
  draw_manager->StateGameBattle();
  input_manager->StateGameBattle();
}

} // namespace details
} // namespace faithful
