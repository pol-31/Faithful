#ifndef FAITHFUL_SRC_MODULES_DRAWMODULE_H_
#define FAITHFUL_SRC_MODULES_DRAWMODULE_H_

#include <glm/glm.hpp>

#include "DrawButtonsSubmodule.h"
#include "DrawTextSubmodule.h"

#include "../loader/ModelPool.h"
#include "../loader/ShaderObjectPool.h"

#include "../entities/PhenomenonArea.h"
#include "../environment/Environment.h"
#include "../player/PlayerCharacter.h"

namespace faithful {
namespace details {

/** It optimizes the order of rendering operations
 * to the OpenGL context to avoid consecutive Depth Test failures.
 * Init most of shaders for drawing; provides interface for drawing
 * */
class DrawModule {
 public:
  DrawModule() = delete;
  DrawModule(assets::ShaderObjectPool& shader_object_pool,
             assets::ModelPool& model_pool,
             Environment& environment,
             PhenomenonAreaPool& phenomenon_area_pool,
             PlayerCharacter& player_character);

  void SetupMenu();
  void SetupGame();

  void Draw(HudPreset& cur_hud_preset);
  GLuint GetFrameBufferPixel(const glm::vec2& coord);
  void DrawPicking(HudPreset& cur_hud_preset);

 private:
  void Init();
  void InitPickingFrameBuffer();

  void DrawBackground();

  DrawTextSubmodule draw_text_submodule_;
  DrawButtonsSubmodule draw_buttons_submodule_;

  assets::ModelPool& model_pool_;
  PhenomenonAreaPool& phenomenon_area_pool_;
  PlayerCharacter& player_character_;
  LiquidHandler& liquid_handler_;
  SkyHandler& sky_handler_;
  TerrainHandler& terrain_handler_;
  VegetationHandler& vegetation_handler_;
  WeatherHandler& weather_handler_;

  GLuint picking_framebuffer_;
  GLuint picking_texture_;

  Texture button_texture_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_MODULES_DRAWMODULE_H_
