#include "DrawTextSubmodule.h"

#include <fstream>

#include <glm/gtc/type_ptr.hpp>

#include "../loader/ShaderObjectPool.h"
#include "../../config/MainMenuLayout.h"
#include "../../config/FontInfo.h"
#include "../gui/HudPreset.h"

namespace faithful {
namespace details {

DrawTextSubmodule::DrawTextSubmodule(
    assets::ShaderObjectPool* shader_object_pool)
    : shader_object_pool_(shader_object_pool) {
  InitFonts();
}

DrawTextSubmodule::~DrawTextSubmodule() {
  for (std::size_t i = 0; i < font_bitmap_ids_.Size(); ++i) {
    glDeleteTextures(1, &font_bitmap_ids_[i]);
  }
  glDeleteBuffers(1, &text_vbo_);
  glDeleteBuffers(1, &textured_text_vbo_);
  glDeleteVertexArrays(1, &text_vao_);
  glDeleteVertexArrays(1, &textured_text_vao_);
}

/// both for simple and textured text (just pass a different shader)
void DrawTextSubmodule::DrawText(ShaderProgram shader_program, GLuint vbo, Font font,
                                 glm::vec2 position, float scale, const std::string& text) {
  using namespace faithful::config::menu;
  using namespace faithful::config::font;

  shader_program.SetUniform(
      "tex_bitmap", static_cast<int>(font.texture_slot_id - GL_TEXTURE0));

  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++) {
    if (*c == '\n') {
      position.y -= font.cell_height * scale;
      continue;
    }
    /// bitmap starts from ' ', that is 32 by ASCII
    int bitmap_id = *c - 32;
    int offset_x = bitmap_id % bitmap_columns_num;
    auto offset_y = static_cast<int>(bitmap_id / bitmap_columns_num);
    glm::vec2 left_upper = {font.cell_width * offset_x, font.tex_height - font.cell_height * offset_y};
    glm::vec2 left_lower = {left_upper.x, left_upper.y - font.cell_height};
    glm::vec2 right_upper = {left_upper.x + font.font_width, left_upper.y};
    glm::vec2 right_lower = {right_upper.x, left_lower.y};

    float w = font.font_width * scale;
    float h = font.font_height * scale;

    float vertices[6][4] = {
        {position.x, position.y + h, left_lower.x, left_lower.y},
        {position.x, position.y, left_upper.x, left_upper.y},
        {position.x + w, position.y, right_upper.x, right_upper.y},

        {position.x, position.y + h, left_lower.x, left_lower.y},
        {position.x + w, position.y, right_upper.x, right_upper.y},
        {position.x + w, position.y + h, right_lower.x, right_lower.y}
    };
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(font_text_coord),
                    sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    position.x += font.font_width * scale;
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

/// in mein menu only textured text
void DrawTextSubmodule::DrawMainMenuText(const HudPreset& cur_hud_preset) {
  using namespace faithful::config::menu;
  /// bind only once for all text
  textured_text_shader_program_.Bind();
  glBindVertexArray(textured_text_vao_);

  textured_text_shader_program_.SetUniform(
      "tex_background", static_cast<int>(hud_texture));

  /// set the same texture for all // TODO: can we do this before Main render loop
  glBindBuffer(GL_ARRAY_BUFFER, textured_text_vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(font_text_coord),
                  font_text_coord.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  /// draw all text for buttons
  textured_text_shader_program_.SetUniformVec4("tex_color", 1, glm::value_ptr(main_btn_text_color));
  textured_text_shader_program_.SetUniform("tex_brightness", main_btn_text_brightness);
  for (const auto& button : cur_hud_preset.GetButtons()) {
    glm::vec2 position = {button.second->GetPosition()[0],
                          button.second->GetPosition()[1]};
    DrawText(textured_text_shader_program_, textured_text_vbo_,
             main_btn_font, std::move(position),
             main_btn_scale, button.second->GetText());
  }

  const Button* current_button = cur_hud_preset.GetCurrent();
  if (current_button) {
    /// draw main text of a current button
    textured_text_shader_program_.SetUniformVec4("tex_color", 1, glm::value_ptr(main_btn_text_selected_color));
    textured_text_shader_program_.SetUniform("tex_brightness", main_btn_text_selected_brightness);
    glm::vec2 position = {current_button->GetPosition()[0],
                          current_button->GetPosition()[1]};
    DrawText(textured_text_shader_program_, textured_text_vbo_,
             main_btn_font, std::move(position),
             main_btn_scale, current_button->GetText());

    /// draw description of a current button
    textured_text_shader_program_.SetUniformVec4("tex_color", 1, glm::value_ptr(description_color));
    textured_text_shader_program_.SetUniform("tex_brightness", description_brightness);
    DrawText(textured_text_shader_program_, textured_text_vbo_,
             description_font, description_pos,
             description_scale, current_button->GetDescription());
  }

  /// draw game version & copyrights
  textured_text_shader_program_.SetUniformVec4("tex_color", 1, glm::value_ptr(version_copyright_color));
  textured_text_shader_program_.SetUniform("tex_brightness", version_copyright_brightness);
  DrawText(textured_text_shader_program_, textured_text_vbo_,
           version_copyright_font, version_pos,
           version_scale, version_text);
  DrawText(textured_text_shader_program_, textured_text_vbo_,
           version_copyright_font, copyright_pos,
           copyright_scale, copyright_text);
}

void DrawTextSubmodule::InitFonts() {
  using namespace faithful::config;

  font_bitmap_ids_.Fill(0);

  /// load fonts bitmaps
  LoadFontBitmap(font::MainMenuButton_YvGothicMedium);
  LoadFontBitmap(font::MainMenuDescription_Cambria);
  LoadFontBitmap(font::MainMenuVersion_MalgunGothic);
  LoadFontBitmap(font::GameItemMob_BahnschriftLightCondensed);
  LoadFontBitmap(font::GameStorytelling_Constantia);

  /// init shader
  ShaderObject button_vert_shader = shader_object_pool_->Load("Text.vert");
  ShaderObject button_frag_shader = shader_object_pool_->Load("Text.frag");
  text_shader_program_ = ShaderProgram(shader_object_pool_);
  text_shader_program_.AttachShader(button_vert_shader);
  text_shader_program_.AttachShader(button_frag_shader);
  text_shader_program_.Bake();

  ShaderObject textured_button_vert_shader = shader_object_pool_->Load("TexturedText.vert");
  ShaderObject textured_button_frag_shader = shader_object_pool_->Load("TexturedText.frag");
  textured_text_shader_program_ = ShaderProgram(shader_object_pool_);
  textured_text_shader_program_.AttachShader(textured_button_vert_shader);
  textured_text_shader_program_.AttachShader(textured_button_frag_shader);
  textured_text_shader_program_.Bake();

  glGenVertexArrays(1, &text_vao_);
  glGenBuffers(1, &text_vbo_);
  glBindVertexArray(text_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, text_vbo_);
  /// 2 for position, 2 for font bitmap coord * 6 vertices
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glBindVertexArray(0);

  glGenVertexArrays(1, &textured_text_vao_);
  glGenBuffers(1, &textured_text_vbo_);
  glBindVertexArray(textured_text_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, textured_text_vbo_);
  /// 2 for background text coord, 2 for position, 2 for font bitmap coord * 6 vertices
  /// we locate background tex_coord not with stride to other data, because
  /// we want to change them separately to each other
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(12 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(14 * sizeof(float)));
  glBindVertexArray(0);
}

void DrawTextSubmodule::LoadFontBitmap(const Font& font) {
  int width = 512, height = 512;
  std::ifstream file(font.path, std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Failed to open BMP file: " << font.path << std::endl;
    std::terminate();

  }

  /// read .bmp header
  file.seekg(18);
  file.read(reinterpret_cast<char*>(&width), sizeof(width));
  file.read(reinterpret_cast<char*>(&height), sizeof(height));

  int padding = (4 - (width / 8) % 4) % 4;

  size_t dataSize = width * height;
  auto imageData = std::make_unique<unsigned char[]>(dataSize);

  for (int y = height - 1; y >= 0; --y) {
    for (int x = 0; x < width / 8; ++x) {
      unsigned char byte;
      file.read(reinterpret_cast<char*>(&byte), sizeof(byte));
      for (int i = 0; i < 8; ++i) {
        int index = (height - y - 1) * width + x * 8 + i;
        imageData[index] = (byte >> (7 - i)) & 0x1;
      }
    }
    file.seekg(padding, std::ios::cur);
  }

  GLuint textureID;
  glGenTextures(1, &textureID);

  /// we bind them only once for the whole runtime, so
  /// there's no need to remember which id belongs to certain bitmap;
  /// but still need indices for deletion
  font_bitmap_ids_.PushBack(textureID);

  glActiveTexture(font.texture_slot_id);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0,
               GL_RED, GL_UNSIGNED_BYTE, imageData.get());
}

} // namespace details
} // namespace faithful
