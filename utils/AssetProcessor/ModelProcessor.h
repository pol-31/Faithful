#ifndef ASSETPROCESSOR_MODELPROCESSOR_H
#define ASSETPROCESSOR_MODELPROCESSOR_H

#include <filesystem>

// TODO: optimization: EBO, Forthys
//       animation frames optimization (!)

void TestCgltf(const std::filesystem::path& filename);

void ProcessModel(const std::filesystem::path& filename);


#endif //ASSETPROCESSOR_MODELPROCESSOR_H
