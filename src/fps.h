#include <stdbool.h>
#include <stdint.h>

// must be power of two
#define FpsMaxSamples 32

typedef struct {
  int64_t samples_[FpsMaxSamples];
  int head_;
  int tail_;
  bool initialized_;
} Fps;

int64_t calculateWindow(Fps* fps, int64_t now);
