#ifndef FAITHFUL_OBJECTS_2D_H
#define FAITHFUL_OBJECTS_2D_H

#include "Object.h"

namespace faithful {

/// ___________DEPRECATED_____________
/// __________________________________
/// __________________________________
/// __________________________________
/*

class DefaultRectangleObject {
 public:
  static void Init();
  static GLuint vao_;
  static GLuint vbo_;
  static GLuint texture_id_;
  static utility::ShaderProgram* shader_program_;

  static Object2D__not_complete::VertexData* vertex_data_;
  static std::size_t vertex_num_;

  static Object2D__not_complete::Point2D* start_vertex_position_;

 private:
  static bool initialized_;
};

class DefaultTriangleObject {
 public:
  static void Init();
  static GLuint vao_;
  static GLuint vbo_;
  static GLuint texture_id_;
  static utility::ShaderProgram* shader_program_;

  static Object2D__not_complete::VertexData* vertex_data_;
  static std::size_t vertex_num_;

  static Object2D__not_complete::Point2D* start_vertex_position_;

 private:
  static bool initialized_;
};

class DefaultCircleObject {
 public:
  static void Init();
  static GLuint vao_;
  static GLuint vbo_;
  static GLuint texture_id_;
  static utility::ShaderProgram* shader_program_;

  static Object2D__not_complete::VertexData* vertex_data_;
  static std::size_t vertex_num_;

  static Object2D__not_complete::Point2D* start_vertex_position_;

 private:
  static bool initialized_;
};


class Rectangle : public Object2D__not_complete {
 public:
  Rectangle() {
    DefaultInit();
  }
  Rectangle(bool drawable) : Object2D__not_complete(drawable) {
    DefaultInit();
  }
  Rectangle(glm::vec3 position) : Object2D__not_complete(position) {
    DefaultInit();
  }
  Rectangle(float x, float y, float z) : Object2D__not_complete(glm::vec3(x, y,
z)) { DefaultInit();
  }

 protected:
  void DefaultInit();
};

class Triangle : public Object2D__not_complete {
 public:
  Triangle() {
    DefaultInit();
  }
  Triangle(bool drawable) : Object2D__not_complete(drawable) {
    DefaultInit();
  }
  Triangle(glm::vec3 position) : Object2D__not_complete(position) {
    DefaultInit();
  }
  Triangle(float x, float y, float z) : Object2D__not_complete(glm::vec3(x, y,
z)) { DefaultInit();
  }

 protected:
  void DefaultInit();
};

class Circle : public Object2D__not_complete {
 public:
  Circle() {
    DefaultInit();
  }
  Circle(bool drawable) : Object2D__not_complete(drawable) {
    DefaultInit();
  }
  Circle(glm::vec3 position) : Object2D__not_complete(position) {
    DefaultInit();
  }
  Circle(float x, float y, float z) : Object2D__not_complete(glm::vec3(x, y, z))
{ DefaultInit();
  }

 protected:
  void DefaultInit();
};



class Hud : public Rectangle {
 public:
  Hud() = default;
  Hud(bool drawable) : Rectangle(drawable) {}
  Hud(glm::vec3 position) : Rectangle(position) {}
  Hud(float x, float y, float z) : Rectangle(glm::vec3(x, y, z)) {}
};


void InitObjects2D();

*/

}  // namespace faithful

#endif  // FAITHFUL_OBJECTS_2D_H
