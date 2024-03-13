#ifndef FAITHFUL_SRC_MODULES_DRAWMODULE_H_
#define FAITHFUL_SRC_MODULES_DRAWMODULE_H_

#include "DrawTextSubmodule.h"

namespace faithful {

class LiquidHandler;
class SkyHandler;
class TerrainHandler;
class VegetationHandler;
class WeatherHandler;

class PhenomenonAreaPool;

class PlayerCharacter;

namespace details {

namespace assets {

class ShaderObjectPool;
class ModelPool;

} // namespace assets

/** It optimizes the order of rendering operations
 * to the OpenGL context to avoid consecutive Depth Test failures.
 * Init most of shaders for drawing; provides interface for drawing
 * */
class DrawModule {
 public:
  DrawModule() = delete;
  DrawModule(assets::ShaderObjectPool* shader_object_pool,
             assets::ModelPool* model_manager,
             PhenomenonAreaPool* phenomenon_area_pool,
             PlayerCharacter* player_character,
             LiquidHandler* liquid_handler,
             SkyHandler* sky_handler,
             TerrainHandler* terrain_handler,
             VegetationHandler* vegetation_handler,
             WeatherHandler* weather_handler);

  void Update();

 private:
  void Init();
  void InitPickingFramebuffer();

  void DrawBackground();

  DrawTextSubmodule draw_text_submodule_;

  assets::ModelPool* model_manager_;
  PhenomenonAreaPool* phenomenon_area_pool_;
  PlayerCharacter* player_character_;
  LiquidHandler* liquid_handler_;
  SkyHandler* sky_handler_;
  TerrainHandler* terrain_handler_;
  VegetationHandler* vegetation_handler_;
  WeatherHandler* weather_handler_;

  GLuint picking_framebuffer_;
  GLuint picking_texture_;

  Texture button_texture_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_MODULES_DRAWMODULE_H_
