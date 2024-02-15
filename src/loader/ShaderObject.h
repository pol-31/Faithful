#ifndef FAITHFUL_SRC_LOADER_SHADEROBJECT_H_
#define FAITHFUL_SRC_LOADER_SHADEROBJECT_H_

#include "glad/glad.h"

#include "IAsset.h"
#include "../common/RefCounter.h"

namespace faithful {

class ShaderObject : public details::assets::IAsset {
 public:
  using Base = details::assets::IAsset;
  using Base::Base;
  using Base::operator=;

  ShaderObject(details::RefCounter* ref_counter,
               GLuint opengl_id, GLenum shader_type) {
    ref_counter_ = ref_counter;
    ref_counter_->Increment();
    internal_id_ = opengl_id;
    shader_type_ = shader_type;
  }

  GLenum GetShaderType() const {
    return shader_type_;
  }

 protected:
  friend class ShaderProgram;
  details::RefCounter* GetRefCounter() const {
    return ref_counter_;
  }

 private:
  GLenum shader_type_;
  using Base::internal_id_;
  using Base::ref_counter_;
};

}  // namespace faithful

#endif //FAITHFUL_SRC_LOADER_SHADEROBJECT_H_
