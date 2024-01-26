#ifndef FAITHFUL_SRC_LOADER_SHADEROBJECT_H_
#define FAITHFUL_SRC_LOADER_SHADEROBJECT_H_

#include <string>

#include <glad/glad.h>

#include "ResourceManager.h"

namespace faithful {

class ShaderObject;

namespace details {
namespace shader {

template <int max_active_shader_objects>
class ShaderObjectManager : public faithful::details::IResourceManager<max_active_shader_objects> {
 public:
  using Base = faithful::details::IResourceManager<max_active_shader_objects>;
  using InstanceInfo = typename Base::InstanceInfo;

  ShaderObjectManager();
  ~ShaderObjectManager();

  /// not copyable
  ShaderObjectManager(const ShaderObjectManager&) = delete;
  ShaderObjectManager& operator=(const ShaderObjectManager&) = delete;

  /// movable
  ShaderObjectManager(ShaderObjectManager&&) = default;
  ShaderObjectManager& operator=(ShaderObjectManager&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  InstanceInfo Load(std::string&& texture_path);

 private:

  void LoadTextureData(int active_instance_id);

  int default_shader_id_ = 0; // adjust
};

} // namespace shader
} // namespace details


/* typical scenario:
   * { // one scope
   *  ShaderObject sho1(vertex_type, "some path 1");
   *  ShaderObject sho2(fragment_type, "some path 2");
   *
   *
   *  // create program_1
   *  program_1.AttachShader(sho1, sho2);
   *
   *
   *  // there it has not been deleted and internally it reused
   *  // but ++ref_counter
   *  ShaderObject sho3(vertex_type, "some path 1");
   *
   * program_2.AttachShader(sho3); <-- successfully reused
   *
   * }
   *
   * */

// TODO: inherit from details::IResource

class ShaderObject {
 public:
  ShaderObject(GLenum shader_type, std::string&& path) {
    // load
    // compile
    // keep id_
  }

  ~ShaderObject() {
    if (id_ != 0) {
      // glDeleteShader();
    }
  }

  int Id() const {
    return id_;
  }



  void Load(std::string&& path);

 private:
  details::shader::ShaderObjectManager* manager_;
  int id_ = 0;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_SHADEROBJECT_H_
