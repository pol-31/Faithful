#include "DrawModule.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../entities/PhenomenonArea.h"
#include "../environment/Liquid.h"
#include "../environment/Sky.h"
#include "../environment/Terrain.h"
#include "../environment/Vegetation.h"
#include "../environment/Weather.h"
#include "../loader/ModelPool.h"
#include "../player/PlayerCharacter.h"

namespace faithful {
namespace details {

DrawModule::DrawModule(assets::ShaderObjectPool* shader_object_pool,
                       assets::ModelPool* model_manager,
                       PhenomenonAreaPool* phenomenon_area_pool,
                       PlayerCharacter* player_character,
                       LiquidHandler* liquid_handler,
                       SkyHandler* sky_handler,
                       TerrainHandler* terrain_handler,
                       VegetationHandler* vegetation_handler,
                       WeatherHandler* weather_handler)
    : draw_text_submodule_(shader_object_pool),
      model_manager_(model_manager),
      phenomenon_area_pool_(phenomenon_area_pool),
      player_character_(player_character),
      liquid_handler_(liquid_handler),
      sky_handler_(sky_handler),
      terrain_handler_(terrain_handler),
      vegetation_handler_(vegetation_handler),
      weather_handler_(weather_handler) {}


void DrawModule::Init() {
  InitPickingFramebuffer();

  // TODO: init hud texture
  button_texture_ = Texture(texture_pool_->Load("buttons.astc"));
  glActiveTexture(hud_texture);
  button_texture_.Bind();
  glBindTexture(GL_TEXTURE_2D, &button_texture_);

  // TODO: add ALL shader_program, ubo creation to DrawManager::Init()
  unsigned int new_ubo;
  glGenBuffers(1, &new_ubo);
  auto new_shader_program = utility::DefaultShaderProgram::CreateBoneProgram();
  glBindBuffer(GL_UNIFORM_BUFFER, new_ubo);
  glBufferData(GL_UNIFORM_BUFFER, 200 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
  GLuint bindingPoint = ++last_ubo_bind_point_;
  glUniformBlockBinding(new_shader_program->get_program(),
                        glGetUniformBlockIndex(new_shader_program->get_program(), "BoneData"),
                        bindingPoint);
  glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, new_ubo);
  // --- ---


  glClearColor(0.2, 0.2, 0.2, 1.0);
}

void DrawModule::InitPickingFramebuffer() {
  glGenFramebuffers(1, &picking_framebuffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, picking_framebuffer_);

  glGenTextures(1, &picking_texture_);
  glBindTexture(GL_TEXTURE_2D, picking_texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI,
               window_.GetResolution().x, window_.GetResolution().y,
               0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, picking_texture_, 0);

  // GL version is 4.4 or greater.
  glClearTexImage(picking_texture_, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "Error: picking frame buffer is not completed" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DrawModule::DrawBackground() {
  // TODO: animated scene from game (wind, tesselation)
  /* tree, a lot of roots, mountain location, stones, sword stacked
   * at monument OR points to current button. Sea downwards. Env effects
   * depends on last save (color, dry, player appearance etc.)
   * */
}

void DrawModule::Update() {
  // TODO: process frame + all from task_queue_
}

} // namespace details
} // namespace faithful
