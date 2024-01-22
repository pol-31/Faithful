# Assets loading
The Faithful/src/loader is designed for loading assets,
covering both dynamic and static loading

## Static loading
Static loading implies that it must be loaded mandatory,
and the loading of such assets is allowed to utilize all possible CPU and GPU
resources. This type is characteristic of assets related to the main menu,
settings window, etc. They have the highest priority because without them,
the game will not be playable. Of course, there are some exceptions,
where damaged or missing assets can be replaced with 'default' assets
(TODO: add hyperlink), but it is not the main focus of this file.
Applicable to all assets, including files related to game progress.

## Dynamic loading
Dynamic loading is the most common case since it pertains to the game itself.
The game map is too large to load all resources into memory at once, so
they are initially compressed into corresponding formats (TODO: add hyperlink)
and need to be dynamically fetched upon entering a specific location
(TODO: add a practical example), as well as removed from RAM when
they are no longer relevant. This is typical for textures, models,
and shaders (for audio - streaming, but more on that later).

As there are five types of assets: audio, 3D models, textures, shaders and
saves (user's game progress) - and they all have somewhat different
characteristics, they will be discussed separately.

## Implementation
### Asset preparation
OpenGL:
- generate N1 texture_id (1D / 2D)
- generate N2 VAO
- generate N3 VBO
- generate N4 UBO
- generate N5 shader programs (not shader object - we know all set-ups in advance)

OpenAL:
- generate N6 sources
- generate N7 listeners

Astc-enc:
- {1} astc context for {4} threads (1 & 4 ?)

MemoryManagement:
(we're still using mimalloc, but we can make it even more efficient)
- generate buffers for assets loading

### Textures
Cases:
* model (diffuse / normal / displacement / ao / etc...)
* background (we don't need cubemap, but just one high-quality hdr image)
* procedural generation (grass, leafs, snow - we need 1D images, which can be
simply represented as byte array. It's not too much memory, so there's
no compression)

So our image has:
- path to file
- global id
- OpenGL texture id
- is active
- properties (ldr/hdr/nmap deduce only once, reuse after)
### Audio
Cases:
* music (.ogg + Vorbis) - streaming
* sound (.wav) - read raw data
### Models
(only one representation)
### Shaders
(only one representation)
### Saves
(only one representation)