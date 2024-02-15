#ifndef FAITHFUL_SRC_LOADER_ASSETINSTANCEINFO_H_
#define FAITHFUL_SRC_LOADER_ASSETINSTANCEINFO_H_

#include <string>

#include "../common/RefCounter.h"

namespace faithful {
namespace details {
namespace assets {

/// MusicManager, SoundManager, Texture1DManager, Texture2DManager,
/// ShaderManager, ModelManager using this as return type + some
/// specific info (if needed); BUT nothing prevents you from
/// returning data in other view, that's the matter of convenience
struct AssetInstanceInfo {
  AssetInstanceInfo() : ref_counter(new RefCounter) {};
  AssetInstanceInfo(std::string&& new_path, RefCounter* new_ref_counter,
                    int new_internal_id, int new_external_id)
      : path(new_path),
        ref_counter(new_ref_counter),
        internal_id(new_internal_id),
        external_id(new_external_id) {};
  std::string path{}; // TODO: does _hash() make sense?
  RefCounter* ref_counter;

  /// in aforementioned classes we return not pointer because
  /// sometimes we should directly said Load() is failed ->
  /// -> provide info like default_tex_id. That's where we
  /// can't just use nullptr or smt


  /// for OpenGL objects (shaders, textures) data is stored by OpenGl,
  /// so our id directly translated into internal OpenGL ids;
  /// for audio - data stored on the HEAP by (e.g.) SoundManager::data_ std::map
  int internal_id = -1;
  /// for OpenGL objects (shaders, textures) it's actually OpenGL id
  /// for Model / Sound / Music it coincides with its idx in active_assets_

  int external_id = -1;
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_ASSETINSTANCEINFO_H_
