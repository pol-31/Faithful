#ifndef FAITHFUL_SRC_LOADER_TEXTURE2D_H
#define FAITHFUL_SRC_LOADER_TEXTURE2D_H

#include "AssetBase.h"

namespace faithful {

class Texture : public details::assets::AssetBase {
 public:
  using Base = details::assets::AssetBase;
  using Base::Base;
  using Base::operator=;

  void Bind();

 private:
  using Base::internal_id_;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_TEXTURE2D_H
