#ifndef FAITHFUL_SRC_ENVIRONMENT_LIQUID_H_
#define FAITHFUL_SRC_ENVIRONMENT_LIQUID_H_

namespace faithful {

namespace details {
namespace environment {

/** How liquid represented: x:y of global_terrain_map as a quadriliteral,
 *     so main shape described by terrain pits.
 * Draw(), Collision(), no update
 *
 * How to draw: particle system + 1 texture (is tesselation necessary?)
 * How they differs: texture/color
 * Other: damage, force / viscosity
 *
 * All can be represented as single shader with simple textures,
 * so there's no need in LiquidHandler;
 * */

/// class represent current liquid
/// only one type of liquid can be drawn at the same time, so
/// different biomes has different liquids and mixtures not possible by now
class LiquidHandler {
 public:
  LiquidHandler() = default;
  // Draw, Collision

  // homoheneous
  void SetWater(); // color
  void SetAcid(); // color
  void SetBlood(); // color
  void SetHoney(); // color
  void SetQuicksilver(); // color
  void SetEctoplasm(); // color

  // heterogeneous
  void SetLava();
  void SetMud();
  void SetLiquidCrystal();

 private:
  int cur_tex_coord_x = 0;
  int cur_tex_coord_y = 0;
  float cur_damage_per_second = 0;
  float cur_viscosity = 3;

  // mask: we have only 2 textures and different masks affecting it

  // glossiness
  // temperature
  // opacity
  // sound
  // steam
  // boosting
};

} // namespace environment
} // namespace details

struct LiquidBase {
  static constexpr int tex_coord_x = 0;
  static constexpr int tex_coord_y = 0;
  static constexpr float damage_per_second = 0;
  static constexpr float viscosity = 3;
  // todo name
};

struct Water : public LiquidBase {
  static constexpr int tex_coord_x = 1;
  static constexpr int tex_coord_y = 1;
  static constexpr float damage_per_second = 0;
  static constexpr float viscosity = 1;
};

// TODO; there is no class Liquid, but rather types of liquid with traits:
//  - viscosity
//  - solidity (?)

} // namespace faithful

#endif  // FAITHFUL_SRC_ENVIRONMENT_LIQUID_H_
