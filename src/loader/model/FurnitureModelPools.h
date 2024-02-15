#ifndef FAITHFUL_SRC_LOADER_MODEL_FURNITUREMODELPOOLS_H_
#define FAITHFUL_SRC_LOADER_MODEL_FURNITUREMODELPOOLS_H_

#include <fstream>
#include <string>

#include "../IAssetPool.h"
#include "../../common/Object.h"

#include "Common.h"

#include "FurnitureModel.h"

#include "../../entities/Furniture.h"

namespace faithful {
namespace details {
namespace assets {

/// FurnitureModelTypePool and FurnitureModelInstancePool have
/// some kind of contract where:
/// FurnitureModelTypePool: loading, storing of general type info
/// FurnitureModelInstancePool: loading, storing of instance info and
///                             provides user-interface for interacting

/// T A C D (transform, animation, collision, draw):
/// unique_destructible_objects_
/// examples: table, chair, vase, window
/// class only for loading & storing (not for drawing or animation)
template <int max_instances>
class FurnitureModelTypePool : public IAssetPool<max_instances> {
 public:
  using Base = IAssetPool<max_instances>;

  FurnitureModelTypePool() {
    for (int i = 0; i < max_instances; ++i) {
      active_instances_[i].internal_id = i;
    }
  }
  ~FurnitureModelTypePool() = default;

  /// not copyable
  FurnitureModelTypePool(const FurnitureModelTypePool&) = delete;
  FurnitureModelTypePool& operator=(const FurnitureModelTypePool&) = delete;

  /// movable
  FurnitureModelTypePool(FurnitureModelTypePool&&) = default;
  FurnitureModelTypePool& operator=(FurnitureModelTypePool&&) = default;

  FurnitureModel* LoadFurniture(std::string path) {
    // Load();
    return nullptr;
  }

  // TODO: Is it blocking ? <<-- add thread-safety
  FurnitureModel* Load(std::string&& model_path) {
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
class FurnitureModelInstancePool : public IAssetPool<max_instances> {
 public:
  using Base = IAssetPool<max_instances>;

  FurnitureModelInstancePool() {
    for (int i = 0; i < max_instances; ++i) {
      active_instances_[i].internal_id = i;
    }
  }
  ~FurnitureModelInstancePool() = default;

  void Add(FurnitureModel* furniture) {
    // TODO: add to active_instances
  }

  void Draw() {
    // TODO: draw all
  }

  /// not copyable
  FurnitureModelInstancePool(const FurnitureModelInstancePool&) = delete;
  FurnitureModelInstancePool& operator=(const FurnitureModelInstancePool&) = delete;

  /// movable
  FurnitureModelInstancePool(FurnitureModelInstancePool&&) = default;
  FurnitureModelInstancePool& operator=(FurnitureModelInstancePool&&) = default;

 private:
  using Base::active_instances_;
  using Base::free_instances_;
  using Base::default_id_;
};

} // namespace assets
} // namespace details
}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_MODEL_FURNITUREMODELPOOLS_H_
