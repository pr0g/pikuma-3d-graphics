#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct tex2f_t {
  float u;
  float v;
} tex2f_t;

int redbrick_texture_width(void);
int redbrick_texture_height(void);
uint32_t* redbrick_texture_as_uint32(void);

#endif // TEXTURE_H
