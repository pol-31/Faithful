#ifndef FAITHFUL_SRC_LOADER_MODEL_H_
#define FAITHFUL_SRC_LOADER_MODEL_H_

#include "AssetBase.h"
#include "assets_data/ModelData.h"

namespace faithful {

class Model
    : public details::assets::AssetBase<details::assets::ModelData> {
 public:
  using Base = details::assets::AssetBase<details::assets::ModelData>;
  using Base::Base;
  using Base::operator=;

  GLuint GetId() const {
    return data_->id;
  }

  bool Ready() const {
    return data_->ready;
  }

  // TODO: all other model methods (e.g. draw / type?)

 private:
  using Base::data_;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_MODEL_H_
