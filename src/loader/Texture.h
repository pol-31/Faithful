#ifndef FAITHFUL_SRC_LOADER_TEXTURE2D_H
#define FAITHFUL_SRC_LOADER_TEXTURE2D_H

#include "AssetBase.h"
#include "assets_data/TextureData.h"

namespace faithful {

class Texture
    : public details::assets::AssetBase<details::assets::TextureData> {
 public:
  using Base = details::assets::AssetBase<details::assets::TextureData>;
  using Base::Base;
  using Base::operator=;

  GLuint GetId() const {
    return data_->id;
  }

  bool Ready() const {
    return data_->ready;
  }

  void Bind();

 private:
  using Base::data_;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_TEXTURE2D_H
