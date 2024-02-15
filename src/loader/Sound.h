#ifndef FAITHFUL_SRC_LOADER_SOUND_H_
#define FAITHFUL_SRC_LOADER_SOUND_H_

#include "IAsset.h"

namespace faithful {

class Sound : public details::assets::IAsset {
 public:
  using Base = details::assets::IAsset;
  using Base::Base;
  using Base::operator=;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_SOUND_H_
