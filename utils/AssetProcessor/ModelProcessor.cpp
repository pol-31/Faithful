#include <filesystem>
#include <iostream>

#include "assimp/assimp/include/assimp/Importer.hpp"
#include "assimp/assimp/include/assimp/Exporter.hpp"
#include "assimp/assimp/include/assimp/scene.h"
#include "assimp/assimp/include/assimp/postprocess.h"

void ProcessModel(const std::filesystem::path& filename) {

  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene) {
    std::cerr << "Error loading FBX file: " << importer.GetErrorString() << std::endl;
    return;
  }

  // Export to GLB file
  Assimp::Exporter exporter;
  exporter.Export(scene, "glb2", "monkey.glb");

  std::cout << "Conversion successful: cube.fbx -> cube.glb" << std::endl;

  /* Assimp::Importer importer;
   const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

   if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
     // Assimp couldn't open the file
     std::cerr << "Assimp failed to open file: " << importer.GetErrorString() << std::endl;
     return false;
   }*/
}

