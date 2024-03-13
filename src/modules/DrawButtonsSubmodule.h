#ifndef FAITHFUL_SRC_MODULES_DRAWBUTTONSSUBMODULE_H_
#define FAITHFUL_SRC_MODULES_DRAWBUTTONSSUBMODULE_H_

#include <glad/glad.h>

#include "../loader/Texture.h"
#include "../loader/ShaderProgram.h"

namespace faithful {

class HudPreset;

namespace details {

namespace assets {

class ShaderObjectPool;

} // namespace assets

class DrawButtonsSubmodule {
 public:
  DrawButtonsSubmodule() = delete;
  DrawButtonsSubmodule(assets::ShaderObjectPool* shader_object_pool);
  ~DrawButtonsSubmodule();

  void DrawButtons(HudPreset& cur_hud_preset);
  void DrawButtonsPicking(HudPreset& cur_hud_preset);

 private:
  void InitButtons();
  void InitButtonsPicking();

  assets::ShaderObjectPool* shader_object_pool_;

  ShaderProgram button_shader_program_;
  GLuint button_vao_;
  GLuint button_vbo_;
  ShaderProgram button_picking_shader_program_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_MODULES_DRAWBUTTONSSUBMODULE_H_
