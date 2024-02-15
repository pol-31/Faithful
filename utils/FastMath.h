#ifndef FAITHFUL_UTILS_FASTMATH_H_
#define FAITHFUL_UTILS_FASTMATH_H_

namespace faithful {
namespace utils {

float FastSqrt(float x) {
  float xhalf = 0.5f * x;
  int i = *(int*)&x;         // treat float as int
  i = 0x5f3759df - (i >> 1); // initial guess for Newton's method

  x = *(float*)&i;           // convert new bits into float
  x = x * (1.5f - xhalf * x * x); // Newton's method for more precision

  return 1.0f / x;
}

float fastSqrt(float x, int iterations) {
  float xhalf = 0.5f * x;
  int i = *(int*)&x;         // treat float as int
  i = 0x5f3759df - (i >> 1); // initial guess for Newton's method

  for (int iter = 0; iter < iterations; iter++) {
    x = *(float*)&i;          // convert new bits into float
    x = x * (1.5f - xhalf * x * x); // Newton's method for more precision
  }

  return 1.0f / x;
}

} // namespace utils
} // namespace faithful

#endif  // FAITHFUL_UTILS_FASTMATH_H_
