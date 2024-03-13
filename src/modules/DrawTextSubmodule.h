#ifndef FAITHFUL_SRC_MODULES_DRAWTEXTSUBMODULE_H_
#define FAITHFUL_SRC_MODULES_DRAWTEXTSUBMODULE_H_

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../../utils/ConstexprVector.h"

#include "../loader/Texture.h"
#include "../loader/ShaderProgram.h"

namespace faithful {

class HudPreset;

namespace details {

namespace assets {

class ShaderObjectPool;

} // namespace assets

class DrawTextSubmodule {
 public:
  DrawTextSubmodule() = delete;
  DrawTextSubmodule(assets::ShaderObjectPool* shader_object_pool);
  ~DrawTextSubmodule();

  /// both for simple and textured text (just pass a different shader)
  void DrawText(ShaderProgram shader_program, GLuint vbo, Font font,
                glm::vec2 position, float scale, const std::string& text);

  void DrawMainMenuText(const HudPreset& cur_hud_preset);

 private:
  void InitFonts();

  void LoadFontBitmap(const Font& font);

  assets::ShaderObjectPool* shader_object_pool_;

  Texture menu_button_font_texture_;
  Texture menu_description_font_texture_;
  Texture menu_version_copyright_font_texture_;
  Texture game_font_texture_;
  Texture game_storytelling_font_texture_;

  ShaderProgram text_shader_program_;
  ShaderProgram textured_text_shader_program_;
  GLuint text_vao_;
  GLuint text_vbo_;
  GLuint textured_text_vao_;
  GLuint textured_text_vbo_;

  utils::ConstexprVector<GLuint, 5> font_bitmap_ids_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_MODULES_DRAWTEXTSUBMODULE_H_
