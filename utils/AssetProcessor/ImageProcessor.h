#ifndef ASSETPROCESSOR_IMAGEPROCESSOR_H
#define ASSETPROCESSOR_IMAGEPROCESSOR_H

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
