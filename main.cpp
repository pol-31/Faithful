#include <iostream>

//#include "Engine.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_USE_RAPIDJSON
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

bool loadModel(tinygltf::Model &model, const char *filename) {
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
  if (!warn.empty()) {
    std::cout << "WARN: " << warn << std::endl;
  }

  if (!err.empty()) {
    std::cout << "ERR: " << err << std::endl;
  }

  if (!res)
    std::cout << "Failed to load glTF: " << filename << std::endl;
  else
    std::cout << "Loaded glTF: " << filename << std::endl;

  return res;
}

int main() {
  std::cout << "Hello, World!" << std::endl;
  tinygltf::Model model;
  if (!loadModel(model, "/home/pavlo/Desktop/BoxTextured.gltf")) {
    std::cout << "cannot load the model" << std::endl;
  }
  std::cout << "Bye, World!" << std::endl;
  return 0;
}

// TODO: are cursor related to Camera obj

// TODO: CollisionManager -> 2 list: for 2d and 3d

// TODO: DefaultShader, DefaultSprite, Default, default.... hell
// TODO: Shaders madness
// TODO: Object.h - rotations

// TODO 3: collisions
// TODO 4: HUD __panel__ (pixel::hud_element)


// TODO 10: optimize ShaderProgram creating (DrawingProcessing), cause we create shader program for each object
// TODO 11: reuse GeometryObjects (we creating each copy from beginning by now)

// TODO 16: dirs TEST for further google testing

///////////////////////////
///////////////////////////
///////////////////////////
///////////////////////////


/* TODO 0: consider how our Game Engine should looks like <<prog_1>>
 *   what we need: floor(tesselation) adjustament, water, object positioning.
 *   Not even scene switching (implemented by code). For floor we just load/create/
 *   /rename height map and store them into binaries. Model changing, sounds, cubemaps,
 *   textures, materials - not concern of this program
 *
 * TODO 1: rewrite logging system (DONE)
 * TODO 2: delete _Runtime_Loading_, use only static updating <<prog_2>> ! <-- cur work
 * TODO 3: Models, Object, Object2D, Object3D --> delete.......
 * TODO 4: rewrite input handlers:
 *   we don't need them anymore, so just delete.
 *   Instead we need to add few _completed_pack_ of input keys
 *   with keys actions/control_btn-s, but taking
 *   into consideration priority and order of keys (most important to begin,
 *   less - to end; sometimes after action discard other checks)
 * TODO 5: rewrite Cursor.h and Camera.h:
 *   we need __exact__ number and types of camera/cursor with
 *   __exact__ textures/parameters (almost all)
 *
 * */
