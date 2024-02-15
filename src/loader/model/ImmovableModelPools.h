#ifndef FAITHFUL_SRC_LOADER_MODEL_IMMOVABLEMODELPOOLS_H_
#define FAITHFUL_SRC_LOADER_MODEL_IMMOVABLEMODELPOOLS_H_

#include <fstream>
#include <string>

#include "../IAssetPool.h"
#include "../../common/Object.h"

#include "Common.h"

#include "ImmovableModel.h"

#include "../../entities/Loot.h"
#include "../../entities/Obstacle.h"

namespace faithful {
namespace details {
namespace assets {

/// ImmovableModelTypePool and ImmovableModelInstancePool have
/// some kind of contract where:
/// ImmovableModelTypePool: loading, storing of general type info
/// ImmovableModelInstancePool: loading, storing of instance info and
///                             provides user-interface for interacting

/// T C D (transform, collision, draw):
/// unique_pickable_item_, unique_terrain_object_
/// examples: walls, big stone, loot
/// class only for loading & storing (not for drawing or animation)
template <int max_instances>
class ImmovableModelTypePool : public IAssetPool<max_instances> {
 public:
  using Base = IAssetPool<max_instances>;

  ImmovableModelTypePool() {
    for (int i = 0; i < max_instances; ++i) {
      active_instances_[i].internal_id = i;
    }
  }
  ~ImmovableModelTypePool() = default;

  /// not copyable
  ImmovableModelTypePool(const ImmovableModelTypePool&) = delete;
  ImmovableModelTypePool& operator=(const ImmovableModelTypePool&) = delete;

  /// movable
  ImmovableModelTypePool(ImmovableModelTypePool&&) = default;
  ImmovableModelTypePool& operator=(ImmovableModelTypePool&&) = default;

  ImmovableModel* LoadObstacle(std::string path) {
    // Load();
    return nullptr;
  }
  ImmovableModel* LoadLoot(std::string path) {
    // Load();
    return nullptr;
  }

  // TODO: Is it blocking ? <<-- add thread-safety
  ImmovableModel* Load(std::string&& model_path) {
    auto [model_id, ref_counter, is_new_id] = Base::AcquireId(model_path);
    if (model_id == -1) {
      return {"", nullptr, default_id_, default_id_};
    } else if (!is_new_id) {
      return {std::move(model_path), ref_counter, model_id, model_id};
    }
    if (!LoadData(model_id)) {
      return {"", nullptr, default_id_, default_id_};
    }
    return {std::move(model_path), ref_counter, model_id, model_id};
  }


 private:
  bool LoadData(int model_id) {
    auto& found_data = sound_heap_data_[model_id];

    auto model_path = active_instances_[model_id].path;
    std::ifstream model_file(model_path, std::ios::binary);
    if (!model_file.is_open()) {
      std::cerr << "Error: unable to open file: " << model_path << std::endl;
      return false;
    }


    // actual loading


    found_data.data.reset(new char[header.subchunk2_size]);
    model_file.read(found_data.data.get(), header.subchunk2_size);
    return true;
  }

  using Base::active_instances_;
  using Base::free_instances_;
  using Base::default_id_;
};

/// We've separated instances and types, because we need different
/// ref counters for them + __separate_segregation__, etc...
/// This class provides user interface for Drawing, Updating
template <int max_instances>
class ImmovableModelInstancePool : public IAssetPool<max_instances> {
 public:
  using Base = IAssetPool<max_instances>;

  ImmovableModelInstancePool() {
    for (int i = 0; i < max_instances; ++i) {
      active_instances_[i].internal_id = i;
    }
  }
  ~ImmovableModelInstancePool() = default;

  void Add(ImmovableModel* immovable) {
    // TODO: add to active_instances
  }

  void Draw() {
    // TODO: draw all
  }

  /// not copyable
  ImmovableModelInstancePool(const ImmovableModelInstancePool&) = delete;
  ImmovableModelInstancePool& operator=(const ImmovableModelInstancePool&) = delete;

  /// movable
  ImmovableModelInstancePool(ImmovableModelInstancePool&&) = default;
  ImmovableModelInstancePool& operator=(ImmovableModelInstancePool&&) = default;

 private:
  using Base::active_instances_;
  using Base::free_instances_;
  using Base::default_id_;
};

} // namespace assets
} // namespace details
}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_MODEL_IMMOVABLEMODELPOOLS_H_
