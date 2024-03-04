#ifndef FAITHFUL_SRC_LOADER_SHADEROBJECTPOOL_H_
#define FAITHFUL_SRC_LOADER_SHADEROBJECTPOOL_H_

#include <string>
#include <vector>

#include "glad/glad.h"

#include "IAssetPool.h"
#include "../../config/Loader.h"

#include "assets_data/ShaderObjectData.h"

#include "../common/OpenGlContextAwareBase.h"

namespace faithful {
namespace details {
namespace assets {

class ShaderObjectPool
    : public IAssetPool<ShaderObjectData, 0>,
      public OpenGlContextAwareBase {
 public:
  using Base = IAssetPool<ShaderObjectData, 0>;
  using DataType = typename Base::DataType;

  ShaderObjectPool() = default;

  /// not copyable
  ShaderObjectPool(const ShaderObjectPool&) = delete;
  ShaderObjectPool& operator=(const ShaderObjectPool&) = delete;

  /// movable
  ShaderObjectPool(ShaderObjectPool&&) = default;
  ShaderObjectPool& operator=(ShaderObjectPool&&) = default;

  static GLenum DeduceShaderType(const std::string& shader_path);
  static void CheckValidity(GLuint shader, GLenum shader_type, std::string buffer);
  static void ReadToBuffer(const std::string& path, std::string& buffer);

 private:
  // TODO: Is it blocking ? <<-- add thread-safety
  DataType LoadImpl(TrackedDataType& instance_info) override;
};

} // namespace assets
} // namespace details
}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_SHADEROBJECTPOOL_H_
