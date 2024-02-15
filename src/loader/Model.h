#ifndef FAITHFUL_SRC_LOADER_MODEL_H_
#define FAITHFUL_SRC_LOADER_MODEL_H_

#include "IAsset.h"

namespace faithful {

class Model : public details::assets::IAsset {
 public:
  using Base = details::assets::IAsset;
  using Base::Base;
  using Base::operator=;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_MODEL_H_
