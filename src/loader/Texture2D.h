#ifndef FAITHFUL_SRC_LOADER_TEXTURE2D_H
#define FAITHFUL_SRC_LOADER_TEXTURE2D_H

#include "IAsset.h"

namespace faithful {

class Texture2D : public details::assets::IAsset {
 public:
  using Base = details::assets::IAsset;
  using Base::Base;
  using Base::operator=;

  void Bind();

 private:
  using Base::internal_id_;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_TEXTURE2D_H
