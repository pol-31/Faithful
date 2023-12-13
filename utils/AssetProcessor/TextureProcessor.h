#ifndef ASSETPROCESSOR_IMAGEPROCESSOR_H
#define ASSETPROCESSOR_IMAGEPROCESSOR_H


#include <filesystem>

#include "astcencoder/astcencoder/Source/astcenc.h"
#define STB_IMAGE_IMPLEMENTATION
#include "astcencoder/astcencoder/Source/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "astcencoder/astcencoder/Source/stb_image_write.h"


// TODO: currently only LDR
void ProcessTexture(const std::filesystem::path& filename) {

  // For the purposes of this sample we hard-code the compressor settings
  static const unsigned int thread_count = 1;
  static const unsigned int block_x = 6;
  static const unsigned int block_y = 6;
  static const unsigned int block_z = 1;
  static const astcenc_profile profile = ASTCENC_PRF_LDR;
  static const float quality = ASTCENC_PRE_MEDIUM;
  static const astcenc_swizzle swizzle {
    ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A
  };

  // ------------------------------------------------------------------------
  // Load input image, forcing 4 components
  int image_x, image_y, image_c;
  uint8_t *image_data = (uint8_t*)stbi_load(filename.c_str(), &image_x, &image_y, &image_c, 4);
  if (!image_data)
  {
    printf("Failed to load image \"%s\"\n", filename.c_str());
    return;
  }

  // Compute the number of ASTC blocks in each dimension
  unsigned int block_count_x = (image_x + block_x - 1) / block_x;
  unsigned int block_count_y = (image_y + block_y - 1) / block_y;

  // ------------------------------------------------------------------------
  // Initialize the default configuration for the block size and quality
  astcenc_config config;
  config.block_x = block_x;
  config.block_y = block_y;
  config.profile = profile;

  astcenc_error status;
  status = astcenc_config_init(profile, block_x, block_y, block_z, quality, 0, &config);
  if (status != ASTCENC_SUCCESS)
  {
    printf("ERROR: Codec config init failed: %s\n", astcenc_get_error_string(status));
    return;
  }

  // ... power users can customize any config settings after calling
  // config_init() and before calling context alloc().

  // ------------------------------------------------------------------------
  // Create a context based on the configuration
  astcenc_context* context;
  status = astcenc_context_alloc(&config, thread_count, &context);
  if (status != ASTCENC_SUCCESS)
  {
    printf("ERROR: Codec context alloc failed: %s\n", astcenc_get_error_string(status));
    return;
  }

  // ------------------------------------------------------------------------
  // Compress the image
  astcenc_image image;
  image.dim_x = image_x;
  image.dim_y = image_y;
  image.dim_z = 1;
  image.data_type = ASTCENC_TYPE_U8;
  uint8_t* slices = image_data;
  image.data = reinterpret_cast<void**>(&slices);

  // Space needed for 16 bytes of output per compressed block
  size_t comp_len = block_count_x * block_count_y * 16;
  uint8_t* comp_data = new uint8_t[comp_len];

  status = astcenc_compress_image(context, &image, &swizzle, comp_data, comp_len, 0);
  if (status != ASTCENC_SUCCESS)
  {
    printf("ERROR: Codec compress failed: %s\n", astcenc_get_error_string(status));
    return ;
  }

  // ... the comp_data array contains the raw compressed data you would pass
  // to the graphics API, or pack into a wrapper format such as a KTX file.

  // If using multithreaded compression to sequentially compress multiple
  // images you should reuse the same context, calling the function
  // astcenc_compress_reset() between each image in the series.

  // ------------------------------------------------------------------------
  // Decompress the image
  // Note we just reuse the image structure to store the output here ...
  status = astcenc_decompress_image(context, comp_data, comp_len, &image, &swizzle, 0);
  if (status != ASTCENC_SUCCESS)
  {
    printf("ERROR: Codec decompress failed: %s\n", astcenc_get_error_string(status));
    return;
  }

  // If using multithreaded decompression to sequentially decompress multiple
  // images you should reuse the same context, calling the function
  // astcenc_decompress_reset() between each image in the series.

  // ------------------------------------------------------------------------
  // Store the result back to disk
  // TODO: path
  stbi_write_png("something.png", image_x, image_y, 4, image_data, 4 * image_x);

  // ------------------------------------------------------------------------
  // Cleanup library resources
  stbi_image_free(image_data);
  astcenc_context_free(context);
  delete[] comp_data;

}

/** The purpose of this header is to decompress image formats like
 * jpg, png, gif, tiff, bmp, webp, svg into .bin files that more
 * convenient to work with (loading to OpenGL buffers)
 * */

// Important notices: only auto-MipMap generation is used

/** There are 3 formats of loaded images: texture, sprite, cubemap part.
 * How textures will be used there:
 * 1) model loading (0 info about resolution and channels,
 *    but we can have benefit from its type: height/normal/diffuse/etc...);
 * 2) cubemap;
 * 3) ground as a normal map (+tesselation)
 * 4) 2d sprites for HUD, other buttons, items in inventory
 * We don't need cubemap as 6 textures in our game: the only way to see the sky - through
 * water reflection, where can simply can use 1 big texture (random (/noise-map) clouds,
 * time-based location of the sun and random time-based weather).
 * So how Cubemap_class represented:
 *   1 texture with some predefined configs/shaders (HDR with light illumination);
 *     (default texture and texture with noise map may also be good)
 *   low-res textures of sun, clouds, start, moon, etc... which affect light
 *     illumination from main texture;
 * */

/* Cubemap.h/.cpp
void Draw(glm::mat4 view, glm::mat4 projection)  {
  glDepthMask(GL_FALSE);
  glDepthFunc(GL_LEQUAL);
  shader_program_->UseProgram();
  shader_program_->AdjustMat4v("view", 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(view))));
  shader_program_->AdjustMat4v("projection", 1, GL_FALSE, glm::value_ptr(projection));
  glBindVertexArray(vao_);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);
}
void Config() {
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
 * */

// We have map(path: id) for finding AND info.txt with all this info
//    TODO: the issue is we don't want to bind objects to "magic" numbers,
//          so each time find id_by_path? OR we can create another program AssetUpdate
//          which going to walkthrough all files and update id-s by their path-s
//   SO  objData_+_objId  will be located in one file; objId_+_path:texId in another
//        SO we need another one file obj : obj_global_id (a.k. obj_type_id, like SkeletWarrior - type_id=5,
//                                                                                SkeletMag     - type_id=6)
//   SO each model class has its own __type_id__ which must be set ONLY BY DEV
//
/// Button has id, color, shader, .... AND texture id

class Model_ {
 protected:
  // Meshes
  // sounds
  // ...
  // global_id (manually)
};
class AnimatedModel : Model_ {
 protected:
  // animations
  // ...
};

// TODO: we don't want to create 1kk ubo-s,
//   because we don't want to see 1kk animated objects on the screen
//   So we NEED TO somehow utilize them

template <typename T>
int ModelGlobalId() {
  /* based on type; mb check info.txt */
}

/// Each such object has its global_id(for assets updating), total_size (for convenience in allocation)
class SkeletWarrior : public AnimatedModel {
  SkeletWarrior() {
    int global_id_ = ModelGlobalId<SkeletWarrior>();
  }
  void ChangeInstance(int instance_local_id) {}

  // allocate memory on Heap
  // for example we getting into location with new mobs
  void Load(void* bin_array /* OR file OR already shaped vector of Mesh, Animations, Sounds, etc... */) {
    // initialization of all params from Model/AnimatedModel classes,
    // including max amount of them (to allocate memory only once), SO
    // ... SO we don't need functions like CreateInstance(..), BUT
    // rather ChangeInstance(int id), which allow to run
    // animations, change transform matrix / properties
  }

  // deallocate memory to Heap
  // useful if some Model-like_class not exist on current location
  void Unload() {}
 private:
};

class ImageProcessor {

};


#endif //ASSETPROCESSOR_IMAGEPROCESSOR_H
