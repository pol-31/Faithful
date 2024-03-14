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

#include "../../config/MainMenuLayout.h"

namespace faithful {
namespace details {

DrawModule::DrawModule(
    assets::ShaderObjectPool& shader_object_pool,
    assets::ModelPool& model_pool,
    Environment& environment,
    PhenomenonAreaPool& phenomenon_area_pool,
    PlayerCharacter& player_character)
    : draw_text_submodule_(shader_object_pool),
      draw_buttons_submodule_(shader_object_pool),
      model_pool_(model_pool),
      phenomenon_area_pool_(phenomenon_area_pool),
      player_character_(player_character),
      liquid_handler_(environment.liquid_handler),
      sky_handler_(environment.sky_handler),
      terrain_handler_(environment.terrain_handler),
      vegetation_handler_(environment.vegetation_handler),
      weather_handler_(environment.weather_handler) {}


void DrawModule::Init() {
  InitPickingFrameBuffer();

  // TODO: init hud texture
  button_texture_ = Texture(texture_pool_->Load("buttons.astc"));
  glActiveTexture(config::menu::hud_texture);
  button_texture_.Bind();

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

void DrawModule::InitPickingFrameBuffer() {
  glGenFramebuffers(1, &picking_framebuffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, picking_framebuffer_);

  glGenTextures(1, &picking_texture_);
  glBindTexture(GL_TEXTURE_2D, picking_texture_);
  // TODO: need to update texture::resolution when window resolution changes
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

void DrawModule::SetupMenu() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
}

void DrawModule::SetupGame() {
  //
}

void DrawModule::Draw(HudPreset& cur_hud_preset) {
  draw_buttons_submodule_.DrawButtons(cur_hud_preset);
  DrawBackground();
  draw_text_submodule_.DrawMainMenuText(cur_hud_preset);
}

GLuint DrawModule::GetFrameBufferPixel(const glm::vec2& coord) {

  glBindFramebuffer(GL_FRAMEBUFFER, picking_framebuffer_);
  GLuint object_id;
  glReadPixels(static_cast<GLint>(coord.x),
               static_cast<GLint>(coord.y),
               1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &object_id);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return object_id;
}

void DrawModule::DrawPicking(HudPreset& cur_hud_preset) {
  glClearTexImage(picking_texture_, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
  glBindFramebuffer(GL_FRAMEBUFFER, picking_framebuffer_);
  draw_buttons_submodule_.DrawButtonsPicking(cur_hud_preset);
  DrawModelPicking();
}

} // namespace details
} // namespace faithful
