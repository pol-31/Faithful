## Assets
### General information
This directory contains all assets used in games. They already have specific
format, which are efficient for run-time loading and occupies acceptable
amount of memory. All assets doesn't have any specific considerations about
its hierarchy and can be arranged differently, if user wants, BUT then user
also should handle correctness of paths inside the game loader.
___
### Assets formats:
- Audio: Ogg/Vorbis (bitrate hasn't been chosen yet (*-TODO-*))
            OR .wav for short sounds
- Models: gltf with full extracted buffers (.bin) and textures data.
            Textures data for models are stored inside the models_textures/ dir
- Textures:
  * model textures: 8bit .astc (astc always has 4-channel images).
    normal map, metallic/roughness using swizzle rrrg (according to ARM ASTC Encoder
    recommendations for storing 2-channel images), while other like occlusion
    emission, albedo(base color) stored as full RGBA (Yeah, it can be optimized
    but don't implemented by now (*-TODO-*))
  * hdr: simply encoded as hdr. To distinguish hdr from ldr we're adding suffix
    "_hdr.astc"
  * other possible formats, like float16 or int16(short) haven't implemented yet
- Shaders:
    * Vertex Shader: .vert
    * Fragment Shader: .frag
    * Geometry Shader: .geom
    * Tessellation Control Shader (TCS): .tesc
    * Tessellation Evaluation Shader (TES): .tese
    * Compute Shader: .comp
  all stored in raw readable format (utf-8)
___
### Custom assets
To compress your own assets to such formats you can use Faithful/utils/AssetProcesser
both for compression and decompression back to readable format. It may be useful
because astc is not readable format.