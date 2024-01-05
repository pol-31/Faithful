#ifndef FAITHFUL_RIGIDBODY_H
#define FAITHFUL_RIGIDBODY_H

#include "../entities/Object.h"

namespace simurgh {

class Rigidbody {
 public:
  Rigidbody() = default;
  Rigidbody(Object* object, float mass = 0,
            glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f))
      : object_(object),
        mass_(mass),
        velocity_(velocity){};

  void ApplyForce(float forceX, float forceY) {
    //
  }

  void Update(float deltaTime) {
    //
  }

  float get_mass() const {
    return mass_;
  }
  void set_mass(float mass) {
    mass_ = mass;
  }
  glm::vec3 get_velocity() const {
    return velocity_;
  }
  void setVelocity(float velocity_x, float velocity_y, float velocity_z = 0) {
    velocity_ = glm::vec3(velocity_x, velocity_y, velocity_z);
  }

 private:
  Object* object_ = nullptr;
  float mass_ = 0;
  glm::vec3 velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
};

}  // namespace simurgh

#endif  // FAITHFUL_RIGIDBODY_H
