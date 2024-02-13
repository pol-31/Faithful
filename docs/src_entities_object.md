
/** What OBJECTS do we have:
* - Camera; POSITION (neither drawable nor collisionable)
* - Cursor; POSITION, DRAWABLE
* -
* - Models; POSITION, DRAWABLE, ANIMATION, MATERIAL; (enemy, character, obstacles) - external/internal;
* - Terrain; POSITION, DRAWABLE, ANIMATION, MATERIAL
* - Sky, sky-entities; POSITION, DRAWABLE, ANIMATION, MATERIAL
* - procedural grass/water, pacticle effects; POSITION, DRAWABLE, ANIMATION, MATERIAL
* - GUI;  POSITION, DRAWABLE, ANIMATION, MATERIAL; (text fields, button, slider, checkbox, etc...);
* // + sprite/image (maybe as part of a GUI);
*
*
* --- --- --- --- --- --- --- --- unified methods (don't need vtables)
* UNIFIED INTERFACE:
* POSITION (2D/3D): transform matrix                       --> mat3/mat4
* COLLISIONABLE: COLLIDE_TEST, COLLIDER:ON/OFF     --> __Collision*__
* --- --- --- --- --- --- --- --- virtual methods (+vtables)
* DIFFERENT INTERFACES:
* DRAWABLE: LOAD (from memory), DRAW, VISIBLE:ON/OFF
*   --> virtual because of convenient Load() calling
*    + (MATERIAL: is it just ids of opengl_textures (?)  --> std::array<Material, num> OR std::vector)
* ANIMATED: ANIMATION:RUN/STOP
*   --> virtual GetFrame(time) + unified Time/RunningAnimation interface
* --- --- --- --- --- --- --- ---
* */

DrawOrder: it's not declared by user and rather DrawManager know in which
order which kind of Model it should draw, as well as ProceduralGen, Sky, Terrain, etc