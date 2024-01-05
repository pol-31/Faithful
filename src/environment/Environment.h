#ifndef FAITHFUL_ENVIRONMENT_H
#define FAITHFUL_ENVIRONMENT_H

#include "../entities/Objects_3d.h"
#include "../entities/Objects_2d.h"

namespace faithful {

class Floor2D {
 public:
  enum class DefaultType {
    Grass
  };

  Floor2D(DefaultType mode);
};

class Floor3D : public Cube {
 public:
  enum class DefaultType {
    Grass
  };

  Floor3D(DefaultType mode);
};

class Wallpaper {
 public:
  enum class DefaultType {
    Twilight,
    Map
  };

  Wallpaper(DefaultType mode);
};

}  // namespace faithful

#endif  // FAITHFUL_ENVIRONMENT_H
