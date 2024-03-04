#ifndef FAITHFUL_SRC_LOADER_SHADEROBJECT_H_
#define FAITHFUL_SRC_LOADER_SHADEROBJECT_H_

#include "AssetBase.h"
#include "assets_data/ShaderObjectData.h"

namespace faithful {

class ShaderObject
    : public details::assets::AssetBase<details::assets::ShaderObjectData> {
 public:
  using Base = details::assets::AssetBase<details::assets::ShaderObjectData>;
  using Base::Base;
  using Base::operator=;

  bool Ready() const {
    return data_->ready;
  }

  GLuint GetId() const {
    return data_->id;
  }

  GLenum GetType() const {
    return data_->type;
  }

 private:
  using Base::data_;
};

}  // namespace faithful

#endif //FAITHFUL_SRC_LOADER_SHADEROBJECT_H_
