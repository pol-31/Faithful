#include "Scene.h"

namespace faithful {

Scene::Scene(faithful::Camera camera, details::io::Cursor cursor)
    : camera_(camera), cursor_(cursor) {}

void Scene::MakeActive() {
  camera_.MakeActive();
  cursor_.MakeActive();
}




/* DEPRECATED:
 *     We draw in that order:
   *    hud (+ stencil buffer - depth buffer)
   *    default 3d/2d objects (+ stencil buffer)
   *      + off-screen rendering;
   *      + post-processing.
   * */

}  // namespace faithful
