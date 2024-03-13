#include "DrawButtonsSubmodule.h"

#include <glm/gtc/type_ptr.hpp>

#include "../loader/ShaderObjectPool.h"
#include "../../config/MainMenuLayout.h"
#include "../gui/HudPreset.h"

namespace faithful {

namespace details {

DrawButtonsSubmodule::DrawButtonsSubmodule(
    assets::ShaderObjectPool* shader_object_pool)
    : shader_object_pool_(shader_object_pool) {
  InitButtons();
  InitButtonsPicking();
}

DrawButtonsSubmodule::~DrawButtonsSubmodule() {
  glDeleteBuffers(1, &button_vbo_);
  glDeleteVertexArrays(1, &button_vao_);
}

void DrawButtonsSubmodule::DrawButtons(HudPreset& cur_hud_preset) {
  using namespace faithful::config::menu;
  button_shader_program_.Bind();
  button_shader_program_.SetUniform("tex_button", static_cast<int>(0));
  glActiveTexture(hud_texture);
  glBindVertexArray(button_vao_);

  glBindBuffer(GL_ARRAY_BUFFER, button_vbo_);
  /// offset the same as data size, because for each vertex we
  /// have 2 screen_coord and 2 tex_coord; no stride, one by one
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(button_1_text_coord),
                  sizeof(button_1_text_coord), button_1_text_coord.data());

  button_shader_program_.SetUniformVec4(
      "color", 1, glm::value_ptr(main_btn_color));
  button_shader_program_.SetUniform(
      "brightness", main_btn_brightness);
  cur_hud_preset.Draw();

  button_shader_program_.SetUniformVec4(
      "color", 1, glm::value_ptr(main_btn_selected_color));
  button_shader_program_.SetUniform(
      "brightness", main_btn_selected_brightness);
  cur_hud_preset.DrawSelected();

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DrawButtonsSubmodule::DrawButtonsPicking(HudPreset& cur_hud_preset) {
  button_picking_shader_program_.Bind();
  glBindVertexArray(button_vao_);
  cur_hud_preset.DrawPicking(button_picking_shader_program_);
}

void DrawButtonsSubmodule::InitButtons() {
  /// init shader
  ShaderObject button_vert_shader = shader_object_pool_->Load("Button.vert");
  ShaderObject button_frag_shader = shader_object_pool_->Load("Button.frag");
  button_shader_program_ = ShaderProgram(shader_object_pool_);
  button_shader_program_.AttachShader(button_vert_shader);
  button_shader_program_.AttachShader(button_frag_shader);
  button_shader_program_.Bake();

  /// pos_coord, tex_coord
  glGenVertexArrays(1, &button_vao_);
  glGenBuffers(1, &button_vbo_);

  glBindVertexArray(button_vao_);

  glBindBuffer(GL_ARRAY_BUFFER, button_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                        2 * sizeof(float), (void*)(12 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void DrawButtonsSubmodule::InitButtonsPicking() {
  /// init shader
  ShaderObject button_picking_vert_shader = shader_object_pool_->Load("ButtonPicking.vert");
  ShaderObject button_picking_frag_shader = shader_object_pool_->Load("ButtonPicking.frag");
  button_picking_shader_program_ = ShaderProgram(shader_object_pool_);
  button_picking_shader_program_.AttachShader(button_picking_vert_shader);
  button_picking_shader_program_.AttachShader(button_picking_frag_shader);
  button_picking_shader_program_.Bake();
}

} // namespace details
} // namespace faithful
