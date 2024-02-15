#ifndef FAITHFUL_SRC_LOADER_TEXTURE1D_H_
#define FAITHFUL_SRC_LOADER_TEXTURE1D_H_

#include "IAsset.h"

namespace faithful {

class Texture1D : public details::assets::IAsset {
 public:
  using Base = details::assets::IAsset;
  using Base::Base;
  using Base::operator=;

  void Bind();

 private:
  using Base::internal_id_;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_TEXTURE1D_H_
