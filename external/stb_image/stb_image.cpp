#include "../../utils/Allocator.h"

//#define STBI_MALLOC(sz)          \
//   simurgh::details::stb_image::stb_image_allocate(sz)
//#define STBI_REALLOC(p,newsz)    \
//   simurgh::details::stb_image::stb_image_reallocate(p, newsz)
//#define STBI_FREE(p)             \
//   simurgh::details::stb_image::stb_image_free(p)

namespace faithful {

namespace details {
namespace stb_image {

void *stb_image_allocate(int sz) {
  utility::RawBufferAllocator<char> alloc;
  return alloc.Allocate(sz);
}

void *stb_image_reallocate(void *p, int sz) {
  utility::RawBufferAllocator<char> alloc;
  alloc.Deallocate(static_cast<char *>(p));
  return alloc.Allocate(sz);
}

void stb_image_free(void *p) {
  utility::RawBufferAllocator<char> alloc;
  alloc.Deallocate(static_cast<char *>(p));
}

} // namespace stb_image
} // namespace details

} // namespace simurgh

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"