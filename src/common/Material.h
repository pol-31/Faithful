#ifndef FAITHFUL_SRC_COMMON_MATERIAL_H_
#define FAITHFUL_SRC_COMMON_MATERIAL_H_

#include "../loader/Texture.h"

namespace faithful {

/**
   * Because of astc encoding format we always have RGBA; so to store
   * rough_metal or normal map we do (official astc-enc recommendations):
   * """
   * To encode this we need to store only two input components
   * in the compressed data, and therefore use the rrrg coding swizzle
   * to align the data with the ASTC luminance+alpha endpoint.
   * """
   * So we decided to represent material as:
   * . albedo - rgba;
   * . metallic & roughness - as rrrg coding swizzle;
   * . normal - as rrrg coding swizzle;
   * - occlusion - rrrr
   * . emission - rgba
   * TODO: occlusion & emission can be stored in the same RGBA texture
 */

struct Material {
  Texture albedo;
  Texture emission;
  Texture metal_rough;
  Texture normal;
  Texture occlusion;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_COMMON_MATERIAL_H_
