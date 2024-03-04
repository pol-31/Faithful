#ifndef FAITHFUL_SRC_LOADER_MODEL_MODELBASE_H_
#define FAITHFUL_SRC_LOADER_MODEL_MODELBASE_H_

#include "../AssetBase.h"
#include "../assets_data/ModelData.h"
#include "../../collision/CollisionArea.h"
#include "../ShaderProgram.h"

namespace faithful {

class ModelBase
    : public details::assets::AssetBase<details::assets::ModelData> {
 public:
  using Base = details::assets::AssetBase<details::assets::ModelData>;
  using Base::Base;
  using Base::operator=;

  GLuint GetModelId() const {
    return data_->global_model_id;
  }

  const CollisionArea& GetCollisionArea() const {
    return data_->collision_area;
  }

  bool Ready() const {
    return data_->ready;
  }

  void PlaySound(int sound_id) const {
    data_->sounds[sound_id].Play();
  }

 protected:
  using Base::data_;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_MODEL_MODELBASE_H_
