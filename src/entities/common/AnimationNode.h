#ifndef FAITHFUL_ANIMATIONNODE_H
#define FAITHFUL_ANIMATIONNODE_H

#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "../../../utils/Span.h"

namespace faithful {


/// class Animation represent node of animation tree
/// class aligned by cache line and compacted in 32/64 bytes
///    (depends on bitness)
/// neither copy nor move semantic
class AnimationNode {
 public:
  struct AnimTransform {
    unsigned int idx_first;
    unsigned int idx_last;
    float factor;
  };
  template <typename T>
  class SkipIterator {
   public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    SkipIterator(T* ptr, size_t skipSize) : ptr(ptr), skipSize(skipSize) {}

    // Iterator operations
    SkipIterator& operator++() {
      ptr += skipSize;
      return *this;
    }

    SkipIterator operator++(int) {
      SkipIterator temp = *this;
      ptr += skipSize;
      return temp;
    }

    T& operator*() const {
      return *ptr;
    }

    T* operator->() const {
      return ptr;
    }

    friend bool operator==(const SkipIterator& lhs,
                           const SkipIterator& rhs) {
      return lhs.ptr == rhs.ptr;
    }

    friend bool operator!=(const SkipIterator& lhs,
                           const SkipIterator& rhs) {
      return !(lhs == rhs);
    }

    friend bool operator<(const SkipIterator& lhs,
                          const SkipIterator& rhs) {
      return *lhs < *rhs;
    }

    friend difference_type operator-(const SkipIterator& lhs,
                                     const SkipIterator& rhs) {
      return (lhs.ptr - rhs.ptr) / lhs.skipSize;  // Assuming skipSize is non-zero
    }

    SkipIterator& operator--() {
      ptr -= skipSize;
      return *this;
    }

    // Postfix decrement operator
    SkipIterator operator--(int) {
      SkipIterator temp = *this;
      ptr -= skipSize;
      return temp;
    }

    SkipIterator& operator+=(difference_type n) {
      ptr += n * skipSize;
      return *this;
    }

    // Compound assignment subtraction operator
    SkipIterator& operator-=(difference_type n) {
      ptr -= n * skipSize;
      return *this;
    }

   private:
    T* ptr;
    size_t skipSize;
  };

  AnimationNode() = default;

  AnimationNode(const AnimationNode&) = delete;
  AnimationNode(AnimationNode&&) = delete;
  AnimationNode& operator=(const AnimationNode&) = delete;
  AnimationNode& operator=(AnimationNode&&) = delete;

  ~AnimationNode();

  unsigned int get_position_offset() const {
    return offset_rotation_and_position_ &
           (~0U >> (sizeof(offset_rotation_and_position_) * 8 / 2));
  }
  unsigned int get_rotation_offset() const {
    return offset_rotation_and_position_
           >> (sizeof(offset_rotation_and_position_) * 8 / 2);
  }

  std::size_t get_size() const {
    std::size_t y = 1;
    for (int i = 0; i < children_.get_size(); ++i)
      y += children_[i]->get_size();
    return y;
  }

  /// simple linear interpolation
  glm::vec3 AnimPositionByTime(float time);
  glm::quat AnimRotationByTime(float time);
  glm::vec3 AnimScalingByTime(float time);

  void ProcessAnimationKey(
    float time,
    const glm::mat4& parent_transform,
    utility::Span<glm::mat4>& out_bone_transform,
    const glm::mat4& global_inverse_transform_);

  /// transform_matrix is the same as aiNodeAnim->mOffsetMatrix (or Identity Matrix)
  AnimationNode(const aiNodeAnim* node, glm::mat4 bone_transform,
                glm::mat4 node_transform, int bone_id,
                std::size_t children_num);
  void InsertNode(std::size_t child_num, AnimationNode* child_node);

 protected:
  AnimTransform AnimTransformByTime(unsigned int global_offset,
                                    unsigned int global_size,
                                    unsigned int local_size,
                                    float time);

  friend class ModelLoader;
  /// Function name is not exciting but it gives needed info and that's good
  void AssimpNodeAnimToSimurghAnimationNode(const aiNodeAnim* node);
  /// current animation order: xyztxyzt...wxyztwxyzt...xytxyt...
  /// possible to have 0 keys (bone with no animations)
  utility::Span<float> keys_;                      // 16 / 8 bytes
  /// key order: scaling, rotation, position (like SRT-rule)
  /// Position and Scaling - 4 float each (3 axis + key_time)
  /// Rotation (quaternion-based) - 5 float (quaternion4f + key_time)


  utility::Span<AnimationNode*> children_;        // 16 / 8 bytes
  glm::mat4* bone_transform_ = nullptr;           // 8  / 4 bytes
  glm::mat4* node_transform_ = nullptr;           // 8  / 4 bytes
  int bone_id_;                                   // 4 / 4 bytes

  /// arranged in that way: (for 4 byte - half is 2)
  ///   upper half bytes - rotation;
  ///   lower half bytes - position
  unsigned int offset_rotation_and_position_ = 0;  // 4 / 4 bytes

  /// for 64-bit system utility::Span occupy 16 bytes,
  /// meanwhile at 32-bit - 8 bytes (sizeof(pointers) == 8), so
  /// total we have 52 and 32 accordingly for 64 and 32-bit system
#if defined(__LP64__) || defined(__x86_64__)
  char padding_[8]; /// padding bytes for better cache coherency
#endif
} __attribute__((aligned(8 * sizeof(void*)), packed)); /// aligned by 64/32 cache line

// TODO: WARNING ignoring packing attribute because of unpacked non-POD field ... keys
// TODO: check on 32 bit sys
// std::cout << sizeof(AnimationNode) << " " << alignof(AnimationNode) << std::endl;


} // namespace faithful

#endif // FAITHFUL_ANIMATIONNODE_H
