#ifndef FAITHFUL_SRC_LOADER_REFCOUNTER_H_
#define FAITHFUL_SRC_LOADER_REFCOUNTER_H_

namespace faithful {
namespace details {

class RefCounter {
 public:
  RefCounter() : counter_(0) {}

  /// not copyable
  RefCounter(const RefCounter&) = delete;
  RefCounter& operator=(const RefCounter&) = delete;

  /// movable
  RefCounter(RefCounter&&) = default;
  RefCounter& operator=(RefCounter&&) = default;

  void Increment() {
    ++counter_;
  }

  void Decrement() {
    if (counter_ == 0) {
      // TODO: log <- warning
    } else {
      --counter_;
    }
  }

  bool Active() {
    return counter_ != 0;
  }

 private:
  int counter_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_REFCOUNTER_H_
