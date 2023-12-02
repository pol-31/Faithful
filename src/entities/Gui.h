#ifndef FAITHFUL_GUI_H
#define FAITHFUL_GUI_H

#include <glm/glm.hpp>

#include "../loader/Sprite.h"

namespace simurgh {


class Gui {
 private:
  glm::vec2 position_;
  Sprite sprite_;
};

class Button {
 private:
  glm::vec2 position_;
  //scope_;
  Sprite sprite_;
};


} // namespace faithful

#endif //FAITHFUL_GUI_H
