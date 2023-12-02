#include "CollisionManager.h"

#include <forward_list>

#include "../entities/Object.h"

namespace faithful {


/// ___________DEPRECATED_____________
/// __________________________________
/// __________________________________
/// __________________________________

/*

void CollisionManager::StartTrack(const Object2D__not_complete* obj) {
  current_collision_list_->push_front(obj);
}
void CollisionManager::StopTrack(const Object3D__not_complete* obj) {
  current_collision_list_->remove(obj);
}

const std::forward_list<const SingleObjectImpl__not_complete*>*
  CollisionManager::NearbyObjects(const SingleObjectImpl__not_complete* obj) {
  return current_collision_list_;
}

void CollisionManager::BindCollisionList(
  std::forward_list<const SingleObjectImpl__not_complete*> *draw_list) {
  current_collision_list_ = draw_list;
}

std::forward_list<const SingleObjectImpl__not_complete*>*
  CollisionManager::default_collision_list_ =
  new std::forward_list<const SingleObjectImpl__not_complete*>;
std::forward_list<const SingleObjectImpl__not_complete*>*
  CollisionManager::current_collision_list_ = default_collision_list_;

float FastSqrt(float x) {
  float xhalf = 0.5f * x;
  int i = *(int*)&x;         // treat float as int
  i = 0x5f3759df - (i >> 1); // initial guess for Newton's method

  x = *(float*)&i;           // convert new bits into float
  x = x * (1.5f - xhalf * x * x); // Newton's method for more precision

  return 1.0f / x;
}

float fastSqrt(float x, int iterations) {
  float xhalf = 0.5f * x;
  int i = *(int*)&x;         // treat float as int
  i = 0x5f3759df - (i >> 1); // initial guess for Newton's method

  for (int iter = 0; iter < iterations; iter++) {
    x = *(float*)&i;          // convert new bits into float
    x = x * (1.5f - xhalf * x * x); // Newton's method for more precision
  }

  return 1.0f / x;
}


// TODO: specialization for circle
// vertex should be continious... idk
bool CollisionManager::CheckCollision(const Object2D__not_complete* obj1,
                                      const Object2D__not_complete* obj2) {
  if (obj1 == obj2) return false;

  for (int i = 0; i < obj2->vertex_num_; ++i) {
    std::cout << obj2->vertex_data_[i].point.x << " ";
  }
  std::cout << std::endl;

  for (int i = 0; i < obj2->vertex_num_ - 1; i++) {
    Object2D__not_complete::Point2D axis =
      {obj2->vertex_data_[i].point.y - obj2->vertex_data_[i + 1].point.y,
       obj2->vertex_data_[i + 1].point.x - obj2->vertex_data_[i].point.x};

    float length = FastSqrt(axis.x * axis.x + axis.y * axis.y);
    axis.x /= length;
    axis.y /= length;

    float min1 = std::numeric_limits<float>::max();
    float max1 = -min1;
    float min2 = min1;
    float max2 = -min1;

    for (int j = 0; j < obj1->vertex_num_; j++) {
      float projection1 = obj2->vertex_data_[j].point.x * axis.x
                          + obj2->vertex_data_[j].point.y * axis.y;
      min1 = std::min(min1, projection1);
      max1 = std::max(max1, projection1);

      float projection2 = obj1->vertex_data_[j].point.x * axis.x
                          + obj1->vertex_data_[j].point.y * axis.y;
      min2 = std::min(min2, projection2);
      max2 = std::max(max2, projection2);
    }

    if (max1 < min2 || max2 < min1) {
      return false;
    }

    std::cout << length << std::endl;
  }


  /// SECOND PASS

  for (int i = 0; i < obj1->vertex_num_ - 1; i++) {
    Object2D__not_complete::Point2D axis =
      {obj1->vertex_data_[i].point.y - obj1->vertex_data_[i + 1].point.y,
       obj1->vertex_data_[i + 1].point.x - obj1->vertex_data_[i].point.x};

    float length = FastSqrt(axis.x * axis.x + axis.y * axis.y);
    axis.x /= length;
    axis.y /= length;

    float min1 = std::numeric_limits<float>::max();
    float max1 = -min1;
    float min2 = min1;
    float max2 = -min1;

    for (int j = 0; j < obj2->vertex_num_; j++) {
      float projection1 = obj1->vertex_data_[j].point.x * axis.x
                          + obj1->vertex_data_[j].point.y * axis.y;
      min1 = std::min(min1, projection1);
      max1 = std::max(max1, projection1);

      float projection2 = obj2->vertex_data_[j].point.x * axis.x
                          + obj2->vertex_data_[j].point.y * axis.y;
      min2 = std::min(min2, projection2);
      max2 = std::max(max2, projection2);
    }

    if (max1 < min2 || max2 < min1) {
      return false;
    }
  }

  /// leftover 1

  Object2D__not_complete::Point2D axis =
    {obj2->vertex_data_[0].point.y
     - obj2->vertex_data_[obj2->vertex_num_ - 1].point.y,
     obj2->vertex_data_[obj2->vertex_num_ - 1].point.x
     - obj2->vertex_data_[0].point.x};

  float length = FastSqrt(axis.x * axis.x + axis.y * axis.y);
  axis.x /= length;
  axis.y /= length;

  float min1 = std::numeric_limits<float>::max();
  float max1 = -min1;
  float min2 = min1;
  float max2 = -min1;

  for (int j = 0; j < obj1->vertex_num_; j++) {
    float projection1 = obj2->vertex_data_[j].point.x * axis.x
                        + obj2->vertex_data_[j].point.y * axis.y;
    min1 = std::min(min1, projection1);
    max1 = std::max(max1, projection1);

    float projection2 = obj1->vertex_data_[j].point.x * axis.x
                        + obj1->vertex_data_[j].point.y * axis.y;
    min2 = std::min(min2, projection2);
    max2 = std::max(max2, projection2);
  }

  if (max1 < min2 || max2 < min1) {
    return false;
  }

  /// leftover 2

  axis = {obj1->vertex_data_[0].point.y
          - obj1->vertex_data_[obj1->vertex_num_ - 1].point.y,
          obj1->vertex_data_[obj1->vertex_num_ - 1].point.x
          - obj1->vertex_data_[0].point.x};

  length = FastSqrt(axis.x * axis.x + axis.y * axis.y);
  axis.x /= length;
  axis.y /= length;

  min1 = std::numeric_limits<float>::max();
  max1 = -min1;
  min2 = min1;
  max2 = -min1;

  for (int j = 0; j < obj2->vertex_num_; j++) {
    float projection1 = obj1->vertex_data_[j].point.x * axis.x
                        + obj1->vertex_data_[j].point.y * axis.y;
    min1 = std::min(min1, projection1);
    max1 = std::max(max1, projection1);

    float projection2 = obj2->vertex_data_[j].point.x * axis.x
                        + obj2->vertex_data_[j].point.y * axis.y;
    min2 = std::min(min2, projection2);
    max2 = std::max(max2, projection2);
  }

  if (max1 < min2 || max2 < min1) {
    return false;
  }

  //std::cout << "collision with " << obj2->get_id() << std::endl;
  return true;
}
*/


} // namespace faithful

