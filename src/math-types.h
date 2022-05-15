#ifndef MATH_TYPES_H
#define MATH_TYPES_H

typedef struct {
  int x;
  int y;
} vec2_t;

typedef struct {
  int x;
  int y;
} point2_t;

typedef struct {
  point2_t pos;
  int width;
  int height;
} rect_t;

#endif // MATH_TYPES_H
