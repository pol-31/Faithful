#ifndef FAITHFUL_SRC_LOADER_MODEL_CREATUREMODELPOOLS_H_
#define FAITHFUL_SRC_LOADER_MODEL_CREATUREMODELPOOLS_H_

#include <fstream>
#include <string>

#include "../IAssetPool.h"
#include "../../common/Object.h"

#include "Common.h"

#include "CreatureModel.h"

#include "../../entities/Ambient.h"
#include "../../entities/Boss.h"
#include "../../entities/Enemy.h"
#include "../../entities/NPC.h"

namespace faithful {
namespace details {
namespace assets {

/// CreatureModelTypePool and CreatureModelInstancePool have
/// some kind of contract where:
/// CreatureModelTypePool: loading, storing of general type info
/// CreatureModelInstancePool: loading, storing of instance info and
///                             provides user-interface for interacting

/// T A C D S U (transform, animation, collision, draw, sound, update):
/// unique_bosses_, unique_enemies_, unique_npc_, unique_peaceful_creature_
/// class only for loading & storing (not for drawing or animation)
template <int max_instances>
class CreatureModelTypePool : public IAssetPool<max_instances> {
 public:
  using Base = IAssetPool<max_instances>;

  CreatureModelTypePool() {
    for (int i = 0; i < max_instances; ++i) {
      active_instances_[i].internal_id = i;
    }
  }
  ~CreatureModelTypePool() = default;

  /// not copyable
  CreatureModelTypePool(const CreatureModelTypePool&) = delete;
  CreatureModelTypePool& operator=(const CreatureModelTypePool&) = delete;

  /// movable
  CreatureModelTypePool(CreatureModelTypePool&&) = default;
  CreatureModelTypePool& operator=(CreatureModelTypePool&&) = default;

  CreatureModel* LoadAmbient(std::string path) {
    // Load();
    return nullptr;
  }
  CreatureModel* LoadBoss(std::string path) {
    // Load();
    return nullptr;
  }
  CreatureModel* LoadEnemy(std::string path) {
    // Load();
    return nullptr;
  }
  CreatureModel* LoadNPC(std::string path) {
    // Load();
    return nullptr;
  }


  // TODO: Is it blocking ? <<-- add thread-safety
  CreatureModel* Load(std::string&& model_path) {
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
class CreatureModelInstancePool : public IAssetPool<max_instances> {
 public:
  using Base = IAssetPool<max_instances>;

  CreatureModelInstancePool() {
    for (int i = 0; i < max_instances; ++i) {
      active_instances_[i].internal_id = i;
    }
  }
  ~CreatureModelInstancePool() = default;

  void Add(CreatureModel* creature) {
    // TODO: add to active_instances
  }

  void Draw() {
    // TODO: draw all
  }

  void Update() {
    // TODO: update all
  }

  /// not copyable
  CreatureModelInstancePool(const CreatureModelInstancePool&) = delete;
  CreatureModelInstancePool& operator=(const CreatureModelInstancePool&) = delete;

  /// movable
  CreatureModelInstancePool(CreatureModelInstancePool&&) = default;
  CreatureModelInstancePool& operator=(CreatureModelInstancePool&&) = default;

 private:
  using Base::active_instances_;
  using Base::free_instances_;
  using Base::default_id_;
};

} // namespace assets
} // namespace details
}  // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_MODEL_CREATUREMODELPOOLS_H_
