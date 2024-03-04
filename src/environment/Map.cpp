#include "Map.h"

#include <algorithm>

#include "../common/LoadingManager.h"

#include "../../config/Loader.h"

namespace faithful {

Map::Map() {
  for (auto& row : all_tiles_) {
    row.fill({-1, -1, {}});
  }
}

MapHandler::MapHandler(details::LoadingManager* loading_manager)
    : loading_manager_(loading_manager) {
  for (auto& row : cur_tiles_) {
    row.fill({-1, -1, {}});
  }
  Init();
}

void MapHandler::Init() {
  // TODO: load all tiles from file (id, biome, models/instances)
}

MapHandler::LoadingStatus MapHandler::UpdateTiles(glm::vec2 player_pos) {
  int current_tile_id = PosToTileId(player_pos);

  if (current_tile_id == last_tile_) {
    return LoadingStatus::kCalm;
  }
  last_tile_ = current_tile_id;
  if (current_tile_id == cur_tiles_[2][2].tile_id ||
      current_tile_id == cur_tiles_[2][3].tile_id ||
      current_tile_id == cur_tiles_[3][2].tile_id ||
      current_tile_id == cur_tiles_[3][3].tile_id) {
    return LoadingStatus::kCalm;
  }
  if (current_tile_id == cur_tiles_[0][0].tile_id) {
    ShiftUpLeft();
  } else if (current_tile_id == cur_tiles_[0][1].tile_id ||
             current_tile_id == cur_tiles_[0][2].tile_id) {
    ShiftUp();
  } else if (current_tile_id == cur_tiles_[0][3].tile_id) {
    ShiftUpRight();
  } else if (current_tile_id == cur_tiles_[1][0].tile_id ||
             current_tile_id == cur_tiles_[2][0].tile_id) {
    ShiftLeft();
  } else if (current_tile_id == cur_tiles_[1][3].tile_id ||
             current_tile_id == cur_tiles_[2][3].tile_id) {
    ShiftRight();
  } else if (current_tile_id == cur_tiles_[3][0].tile_id) {
    ShiftDownLeft();
  } else if (current_tile_id == cur_tiles_[3][1].tile_id ||
             current_tile_id == cur_tiles_[3][2].tile_id) {
    ShiftDown();
  } else if (current_tile_id == cur_tiles_[3][3].tile_id) {
    ShiftDownRight();
  }
  int not_ready_assets_num = GetNotReadyNum();
  if (not_ready_assets_num != 0) {
    if (not_ready_assets_num <
        faithful::config::kMapStressLoadingAssersNumThreshold) {
      return LoadingStatus::kIntensive;
    } else {
      return LoadingStatus::kStress;
    }
  }
}

void MapHandler::LoadTileData(TileData& old_tile, int relative_shift) {
  int new_tile_id = old_tile.tile_id + relative_shift;
  std::vector<int> models_to_add;
  glm::ivec2 new_models_ids = TileIdToIds(new_tile_id);
  std::vector<Map::ModelInfo> new_instances =
      cur_map_.all_tiles_[new_models_ids.y][new_models_ids.x].instances;

  for (const auto& dead_id : dead_permanently_instances_) {
    std::remove_if(new_instances.begin(), new_instances.end(),
                   [&](Map::ModelInfo& model_info) {
                     return model_info.global_instance_id == dead_id;
                   });
  }
  for (const auto& dead_id : dead_temporarily_instances_) {
    std::remove_if(new_instances.begin(), new_instances.end(),
                   [&](Map::ModelInfo& model_info) {
                     return model_info.global_instance_id == dead_id;
                   });
  }

  old_tile.tile_id = new_tile_id;
  for (const auto& model_id : new_instances) {
    old_tile.instances.push_back(
        loading_manager_->LoadModel(model_id.global_model_id));
  }
}

bool MapHandler::GetNotReadyNum() {
  int total_not_ready{0};
  /// check only central cells
  using InstanceType = const std::shared_ptr<details::assets::ModelData>*;
  std::vector<InstanceType> unique_models;
  for (std::size_t i = 1; i < cur_tiles_.size() - 1; ++i) {
    for (std::size_t j = 1; j < cur_tiles_[0].size() - 1; ++j) {
      for (const auto& instance : cur_tiles_[i][j].instances) {
        unique_models.push_back(&instance);
      }
    }
  }
  /// remove duplicates
  std::sort(
      unique_models.begin(), unique_models.end(),
      [](InstanceType inst1, InstanceType inst2) {
        return inst1->get()->global_model_id < inst2->get()->global_model_id;
      });
  auto new_end = std::unique(
      unique_models.begin(), unique_models.end(),
      [](InstanceType inst1, InstanceType inst2) {
        return inst1->get()->global_model_id == inst2->get()->global_model_id;
      });
  unique_models.erase(new_end, unique_models.end());

  for (const auto& instance : unique_models) {
    if (instance->get()->ready) {
      ++total_not_ready;
    }
    if (instance->get()->material.albedo.Ready()) {
      ++total_not_ready;
    }
    if (instance->get()->material.emission.Ready()) {
      ++total_not_ready;
    }
    if (instance->get()->material.metal_rough.Ready()) {
      ++total_not_ready;
    }
    if (instance->get()->material.normal.Ready()) {
      ++total_not_ready;
    }
    if (instance->get()->material.occlusion.Ready()) {
      ++total_not_ready;
    }
  }
  return total_not_ready;
}

void MapHandler::ShiftUpLeft() {
  cur_tiles_[3][1] = std::move(cur_tiles_[2][0]);
  cur_tiles_[3][2] = std::move(cur_tiles_[2][1]);
  cur_tiles_[3][3] = std::move(cur_tiles_[2][2]);
  cur_tiles_[2][1] = std::move(cur_tiles_[1][0]);
  cur_tiles_[2][2] = std::move(cur_tiles_[1][1]);
  cur_tiles_[2][3] = std::move(cur_tiles_[1][2]);
  cur_tiles_[1][1] = std::move(cur_tiles_[0][0]);
  cur_tiles_[1][2] = std::move(cur_tiles_[0][1]);
  cur_tiles_[1][3] = std::move(cur_tiles_[0][2]);
  LoadTileData(cur_tiles_[0][0], -cur_map_.tile_total_width - 1);
  LoadTileData(cur_tiles_[0][1], -cur_map_.tile_total_width - 1);
  LoadTileData(cur_tiles_[0][2], -cur_map_.tile_total_width - 1);
  LoadTileData(cur_tiles_[0][3], -cur_map_.tile_total_width - 1);
  LoadTileData(cur_tiles_[1][0], -cur_map_.tile_total_width - 1);
  LoadTileData(cur_tiles_[2][0], -cur_map_.tile_total_width - 1);
  LoadTileData(cur_tiles_[3][0], -cur_map_.tile_total_width - 1);
}
void MapHandler::ShiftUp() {
  cur_tiles_[1][0] = std::move(cur_tiles_[0][0]);
  cur_tiles_[1][1] = std::move(cur_tiles_[0][1]);
  cur_tiles_[1][2] = std::move(cur_tiles_[0][2]);
  cur_tiles_[1][3] = std::move(cur_tiles_[0][3]);
  cur_tiles_[2][0] = std::move(cur_tiles_[1][0]);
  cur_tiles_[2][1] = std::move(cur_tiles_[1][1]);
  cur_tiles_[2][2] = std::move(cur_tiles_[1][2]);
  cur_tiles_[2][3] = std::move(cur_tiles_[1][3]);
  cur_tiles_[3][0] = std::move(cur_tiles_[2][0]);
  cur_tiles_[3][1] = std::move(cur_tiles_[2][1]);
  cur_tiles_[3][2] = std::move(cur_tiles_[2][2]);
  cur_tiles_[3][3] = std::move(cur_tiles_[2][3]);
  LoadTileData(cur_tiles_[0][0], -cur_map_.tile_total_width);
  LoadTileData(cur_tiles_[0][1], -cur_map_.tile_total_width);
  LoadTileData(cur_tiles_[0][2], -cur_map_.tile_total_width);
  LoadTileData(cur_tiles_[0][3], -cur_map_.tile_total_width);
}
void MapHandler::ShiftUpRight() {
  cur_tiles_[3][0] = std::move(cur_tiles_[2][1]);
  cur_tiles_[3][1] = std::move(cur_tiles_[2][2]);
  cur_tiles_[3][2] = std::move(cur_tiles_[2][3]);
  cur_tiles_[2][0] = std::move(cur_tiles_[1][1]);
  cur_tiles_[2][1] = std::move(cur_tiles_[1][2]);
  cur_tiles_[2][2] = std::move(cur_tiles_[1][3]);
  cur_tiles_[1][0] = std::move(cur_tiles_[0][1]);
  cur_tiles_[1][1] = std::move(cur_tiles_[0][2]);
  cur_tiles_[1][2] = std::move(cur_tiles_[0][3]);
  LoadTileData(cur_tiles_[0][0], -cur_map_.tile_total_width + 1);
  LoadTileData(cur_tiles_[0][1], -cur_map_.tile_total_width + 1);
  LoadTileData(cur_tiles_[0][2], -cur_map_.tile_total_width + 1);
  LoadTileData(cur_tiles_[0][3], -cur_map_.tile_total_width + 1);
  LoadTileData(cur_tiles_[1][3], -cur_map_.tile_total_width + 1);
  LoadTileData(cur_tiles_[2][3], -cur_map_.tile_total_width + 1);
  LoadTileData(cur_tiles_[3][3], -cur_map_.tile_total_width + 1);
}
void MapHandler::ShiftLeft() {
  cur_tiles_[0][1] = std::move(cur_tiles_[0][0]);
  cur_tiles_[0][2] = std::move(cur_tiles_[0][1]);
  cur_tiles_[0][3] = std::move(cur_tiles_[0][2]);
  cur_tiles_[1][1] = std::move(cur_tiles_[1][0]);
  cur_tiles_[1][2] = std::move(cur_tiles_[1][1]);
  cur_tiles_[1][3] = std::move(cur_tiles_[1][2]);
  cur_tiles_[2][1] = std::move(cur_tiles_[2][0]);
  cur_tiles_[2][2] = std::move(cur_tiles_[2][1]);
  cur_tiles_[2][3] = std::move(cur_tiles_[2][2]);
  cur_tiles_[3][1] = std::move(cur_tiles_[3][0]);
  cur_tiles_[3][2] = std::move(cur_tiles_[3][1]);
  cur_tiles_[3][3] = std::move(cur_tiles_[3][2]);
  LoadTileData(cur_tiles_[0][0], -1);
  LoadTileData(cur_tiles_[1][0], -1);
  LoadTileData(cur_tiles_[2][0], -1);
  LoadTileData(cur_tiles_[3][0], -1);
}
void MapHandler::ShiftRight() {
  cur_tiles_[0][0] = std::move(cur_tiles_[0][1]);
  cur_tiles_[0][1] = std::move(cur_tiles_[0][2]);
  cur_tiles_[0][2] = std::move(cur_tiles_[0][3]);
  cur_tiles_[1][0] = std::move(cur_tiles_[1][1]);
  cur_tiles_[1][1] = std::move(cur_tiles_[1][2]);
  cur_tiles_[1][2] = std::move(cur_tiles_[1][3]);
  cur_tiles_[2][0] = std::move(cur_tiles_[2][1]);
  cur_tiles_[2][1] = std::move(cur_tiles_[2][2]);
  cur_tiles_[2][2] = std::move(cur_tiles_[2][3]);
  cur_tiles_[3][0] = std::move(cur_tiles_[3][1]);
  cur_tiles_[3][1] = std::move(cur_tiles_[3][2]);
  cur_tiles_[3][2] = std::move(cur_tiles_[3][3]);
  LoadTileData(cur_tiles_[0][3], 1);
  LoadTileData(cur_tiles_[1][3], 1);
  LoadTileData(cur_tiles_[2][3], 1);
  LoadTileData(cur_tiles_[3][3], 1);
}
void MapHandler::ShiftDownLeft() {
  cur_tiles_[0][1] = std::move(cur_tiles_[1][0]);
  cur_tiles_[0][2] = std::move(cur_tiles_[1][1]);
  cur_tiles_[0][3] = std::move(cur_tiles_[1][2]);
  cur_tiles_[1][1] = std::move(cur_tiles_[2][0]);
  cur_tiles_[1][2] = std::move(cur_tiles_[2][1]);
  cur_tiles_[1][3] = std::move(cur_tiles_[2][2]);
  cur_tiles_[2][1] = std::move(cur_tiles_[3][0]);
  cur_tiles_[2][2] = std::move(cur_tiles_[3][1]);
  cur_tiles_[2][3] = std::move(cur_tiles_[3][2]);
  LoadTileData(cur_tiles_[0][0], cur_map_.tile_total_width - 1);
  LoadTileData(cur_tiles_[1][0], cur_map_.tile_total_width - 1);
  LoadTileData(cur_tiles_[2][0], cur_map_.tile_total_width - 1);
  LoadTileData(cur_tiles_[3][0], cur_map_.tile_total_width - 1);
  LoadTileData(cur_tiles_[3][1], cur_map_.tile_total_width - 1);
  LoadTileData(cur_tiles_[3][2], cur_map_.tile_total_width - 1);
  LoadTileData(cur_tiles_[3][3], cur_map_.tile_total_width - 1);
}
void MapHandler::ShiftDown() {
  cur_tiles_[0][0] = std::move(cur_tiles_[1][0]);
  cur_tiles_[0][1] = std::move(cur_tiles_[1][1]);
  cur_tiles_[0][2] = std::move(cur_tiles_[1][2]);
  cur_tiles_[0][3] = std::move(cur_tiles_[1][3]);
  cur_tiles_[1][0] = std::move(cur_tiles_[2][0]);
  cur_tiles_[1][1] = std::move(cur_tiles_[2][1]);
  cur_tiles_[1][2] = std::move(cur_tiles_[2][2]);
  cur_tiles_[1][3] = std::move(cur_tiles_[2][3]);
  cur_tiles_[2][0] = std::move(cur_tiles_[3][0]);
  cur_tiles_[2][1] = std::move(cur_tiles_[3][1]);
  cur_tiles_[2][2] = std::move(cur_tiles_[3][2]);
  cur_tiles_[2][3] = std::move(cur_tiles_[3][3]);
  LoadTileData(cur_tiles_[3][0], cur_map_.tile_total_width);
  LoadTileData(cur_tiles_[3][1], cur_map_.tile_total_width);
  LoadTileData(cur_tiles_[3][2], cur_map_.tile_total_width);
  LoadTileData(cur_tiles_[3][3], cur_map_.tile_total_width);
}
void MapHandler::ShiftDownRight() {
  cur_tiles_[0][0] = std::move(cur_tiles_[1][1]);
  cur_tiles_[0][1] = std::move(cur_tiles_[1][2]);
  cur_tiles_[0][2] = std::move(cur_tiles_[1][3]);
  cur_tiles_[1][0] = std::move(cur_tiles_[2][1]);
  cur_tiles_[1][1] = std::move(cur_tiles_[2][2]);
  cur_tiles_[1][2] = std::move(cur_tiles_[2][3]);
  cur_tiles_[2][0] = std::move(cur_tiles_[3][1]);
  cur_tiles_[2][1] = std::move(cur_tiles_[3][2]);
  cur_tiles_[2][2] = std::move(cur_tiles_[3][3]);
  LoadTileData(cur_tiles_[0][3], cur_map_.tile_total_width + 1);
  LoadTileData(cur_tiles_[1][3], cur_map_.tile_total_width + 1);
  LoadTileData(cur_tiles_[2][3], cur_map_.tile_total_width + 1);
  LoadTileData(cur_tiles_[3][3], cur_map_.tile_total_width + 1);
  LoadTileData(cur_tiles_[3][2], cur_map_.tile_total_width + 1);
  LoadTileData(cur_tiles_[3][1], cur_map_.tile_total_width + 1);
  LoadTileData(cur_tiles_[3][0], cur_map_.tile_total_width + 1);
}

} // namespace faithful
