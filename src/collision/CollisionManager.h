#ifndef FAITHFUL_COLLISIONMANAGER_H
#define FAITHFUL_COLLISIONMANAGER_H

#include <forward_list>
#include "../entities/Object.h"

namespace faithful {

// TODO: "Movable"-class detection also should be a huge optimization

/// ___________DEPRECATED_____________
/// __________________________________
/// __________________________________
/// __________________________________

// TODO: should be different for 2D and 3D
class CollisionManager {
 public:
//  using ValueType = const SingleObjectImpl__not_complete;
//  using ValuePointerType = const SingleObjectImpl__not_complete*;
  static void StartTrack(const Object2D *obj);
  static void StopTrack(const Object3D *obj);

  // TODO: implement optimization
  //    depends on obj position give him to check only
  //    located nearby with him objects

//  const std::forward_list<const SingleObjectImpl__not_complete*>*
//  NearbyObjects(const SingleObjectImpl__not_complete* obj);
//
//  static bool CheckCollision(const Object2D__not_complete* obj1,
//                             const Object2D__not_complete* obj2);
//
// protected:
//  friend class Scene;
//  friend class SingleObjectImpl__not_complete;
//
//  static void BindCollisionList(
//    std::forward_list<const SingleObjectImpl__not_complete*> *collision_list);
//
//  static std::forward_list<ValuePointerType>* default_collision_list_;
//  static std::forward_list<ValuePointerType>* current_collision_list_;
};

/* TODO: this:
bool CheckCollisionImpl(Rectangle, Rectangle);
bool CheckCollisionImpl(Rectangle, Circle);
bool CheckCollisionImpl(Rectangle, Ellipse);
bool CheckCollisionImpl(Rectangle, Square);

 OR
 SAT (separated axis theorem)

 */


} // namespace faithful

#endif // FAITHFUL_COLLISIONMANAGER_H
