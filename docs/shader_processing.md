
/** ShaderObject fully depends on ShaderPrograms and its counters.
* If shader_object_1 scope hasn't expired yet OR
* is used in at least one ShaderProgram (scope might have been expired),
* then we don't delete actual id
* */

/** This way we can protect our program from such cases:
* { // scope # 1
*  ShaderProgram shp;
*
* { // scope # 2
*   ShaderObject sho(vertex_type, "shader path")
*   shp.AttachShader(sho);
* } // -- ~sho();
*  // --- there happened a lot of things, including Shader object Clearing
*  shp.Bake(); < -- there shader object already invalid
* }
* */

/// TO REUSE ShaderObject we just need to bind it in the same scope
/// if user Load() the same file multiple times in the same scope - its his problems

/// comparing to other ...Manager classes we
/// don't need functionality from IAssetManager (and we
/// don't have id-pooling, so there's dynamic OpenGL calls for them)
/// should be only 1 instance for the entire program

// TODO: explain difference between ShaderProgram and ShaderObject (->docs)

