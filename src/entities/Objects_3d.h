#ifndef FAITHFUL_OBJECTS_3D_H
#define FAITHFUL_OBJECTS_3D_H

#include "Object.h"

namespace faithful {


/// ___________DEPRECATED_____________
/// __________________________________
/// __________________________________
/// __________________________________

class CubeCreator : public TrivialObject3DImpl {
 public:
  CubeCreator();
};

class Cube : Object3D {
 public:
  Cube();
};


class Model : public Object3D {
 public:
  Model(const char* path);
  void RunAnimation(unsigned int anim_id, bool repeat = false);
};


class Sprite3dCreator : public TrivialObject3DImpl {
 public:
  Sprite3dCreator();
};


} // namespace faithful

#endif // FAITHFUL_OBJECTS_3D_H
