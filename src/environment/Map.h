#ifndef FAITHFUL_SRC_ENVIRONMENT_MAP_H_
#define FAITHFUL_SRC_ENVIRONMENT_MAP_H_

#include <array>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "../loader/assets_data/ModelData.h"

// TODO 1: replace by glm::imat4x4 OR cache-friendly + sse/avx
// TODO 2: std::array can we represented more friendly
//    (shouldn't c++ compiler optimize it?)

namespace faithful {

namespace details {

class LoadingManager;

} // namespace details

struct Map {
  Map();
  struct ModelInfo {
    glm::vec2 pos;
    int global_model_id;
    int global_instance_id;
  };
  struct TileInfo {
    int tile_id;
    int tile_biome;
    std::vector<ModelInfo> instances;
  };

  /// ALL tiles data (load from file / link to source ?)
  std::array<std::array<TileInfo, 10>, 10> all_tiles_;

  int tile_width = 32;
  int tile_height = 32;
  int tile_total_width = tile_width * all_tiles_[0].size();
  int tile_total_height = tile_height * all_tiles_.size();
};

class MapHandler {
 public:
  struct TileData {
    int tile_id;
    int tile_biome;
    std::vector<std::shared_ptr<details::assets::ModelData>> instances;
  };

  enum class LoadingStatus {
    kCalm,
    kIntensive,
    kStress
  };

  MapHandler() = delete;
  MapHandler(details::LoadingManager* loading_manager);

  LoadingStatus UpdateTiles(glm::vec2 player_pos);

  static int PosToTileId(const glm::vec2 pos) {
    // TODO:
  }
  static glm::ivec2 PosToTileIds(const glm::vec2 pos) {
    // TODO:
  }
  static glm::vec2 TileIdToIds(int id) {
    // TODO:
  }
  static glm::vec2 TileIdToPos(int id) {
    // TODO:
  }

  int CurBiome() {
    auto tile_ids = TileIdToIds(last_tile_);
    return cur_map_.all_tiles_[tile_ids.y][tile_ids.x].tile_biome;
  }

 private:
  void Init();

  void LoadTileData(TileData& old_tile, int relative_shift);

  bool GetNotReadyNum();

  void ShiftUpLeft();
  void ShiftUp();
  void ShiftUpRight();
  void ShiftLeft();
  void ShiftRight();
  void ShiftDownLeft();
  void ShiftDown();
  void ShiftDownRight();

  details::LoadingManager* loading_manager_;

  int last_tile_{-1};

  Map cur_map_;

  /// tile id and "keep alive" shared pointer to ModelData.
  /// It doesn't represent any instance and serves just as a tip
  /// for ModelManager (src/loader/ModelManager.h) not to delete this model and
  /// not to move it to cache (which can be cleared after)
  std::array<std::array<TileData, 4>, 4> cur_tiles_;

  std::vector<int> dead_temporarily_instances_;
  std::vector<int> dead_permanently_instances_;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_ENVIRONMENT_MAP_H_
