#ifndef FAITHFUL_SRC_LOADER_SHADERPOOL_H_
#define FAITHFUL_SRC_LOADER_SHADERPOOL_H_

#include <string>
#include <vector>

#include "glad/glad.h"

#include "IAssetPool.h"
#include "AssetInstanceInfo.h"
#include "../config/Loader.h"

namespace faithful {
namespace details {

class RefCounter;

namespace assets {

/// should be only 1 instance for the entire program
class ShaderPool : public IAssetPool<
                       faithful::config::max_active_shader_program_num> {
 public:
  using Base = IAssetPool<faithful::config::max_active_shader_program_num>;

  ShaderPool();
  ~ShaderPool();

  /// not copyable
  ShaderPool(const ShaderPool&) = delete;
  ShaderPool& operator=(const ShaderPool&) = delete;

  /// movable
  ShaderPool(ShaderPool&&) = default;
  ShaderPool& operator=(ShaderPool&&) = default;

  /// returns this* as a second parameter
  std::pair<AssetInstanceInfo, ShaderPool*> CreateProgram();

  /// returns shader type as a second parameter
  std::pair<AssetInstanceInfo, GLenum> LoadShader(std::string&& shader_path);

  /// that's why we return this* -> after some ShaderProgram internal shaders
  /// becomes inactive (reattach / detach / Bake()) we want to ensure there
  /// is no ShaderObject id's with __handling_ref_counters__ TODO: -> docs
  void ScanRefCounters();

 private:
  struct ShaderObjectInfo {
    RefCounter* ref_counter;
    int opengl_id;
  };

  GLenum DeduceShaderType(const std::string& shader_path);
  bool IsValidShader(GLuint shader, GLenum shader_type,
                     std::string&& buffer) noexcept;
  bool ReadToBuffer(const std::string& path, std::string& buffer) noexcept;

  using Base::active_instances_;
  using Base::free_instances_;
  using Base::default_id_;

  /// for tracking of all id/ref_count and then deleted if ref_counter == 0
  std::vector<ShaderObjectInfo> active_shader_objects_;
};

} // namespace assets
} // namespace details
}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_SHADERPOOL_H_
