#ifndef FAITHFUL_SRC_ENVIRONMENT_LIQUID_H_
#define FAITHFUL_SRC_ENVIRONMENT_LIQUID_H_

namespace faithful {

// TODO: all liquids can be represented on the same textures
//      with different texture coordinates
//  NO:
//  - vegetation / weather don't have textures - just color
//  - terrain should be represented with big texture to avoid tiling
//  - sky represented as color and hdr texture

// TODO: standing in water ->
//  - lower camera/player y-pos
//  - effect / damage

struct Liquid {
  static constexpr int tex_coord_x = 0;
  static constexpr int tex_coord_y = 0;
  static constexpr float damage_per_second = 0;
  static constexpr float viscosity = 3;
  // todo name
};

// TODO: Water = Liquid(1, 1, 0, 1);

// TODO; there is no class Liquid, but rather types of liquid with traits:
//  - viscosity
//  - solidity (?)

// TODO: texture mask?

// glossiness
// temperature
// opacity
// sound
// steam
// boosting

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

  /* Water
   * Acid
   * Blood
   * Honey
   * Quicksilver
   * Ectoplasm
   * Lava
   * Mud
   * LiquidCrystal
   * */

  void SetLiquid(Liquid liquid) {}

 private:
  int cur_tex_coord_x = 0;
  int cur_tex_coord_y = 0;
  float cur_damage_per_second = 0;
  float cur_viscosity = 3;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_ENVIRONMENT_LIQUID_H_
