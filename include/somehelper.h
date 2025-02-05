// include/somehelper.h
#ifndef SOMEHELPER_H
#define SOMEHELPER_H

#include <stdint.h>
typedef struct Vec3 Vec3;

struct Vec3 {
  float x;
  float y;
  float z;
};

void draw_rectangle(uint32_t *buffer, int width, int height, int x, int y,
                    int w, int h, uint32_t color);
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_subtract(Vec3 a, Vec3 b);
Vec3 vec3_scalar_multiply(float k, Vec3 a);
Vec3 vec3_normalize(Vec3 a);
float vec3_dot(Vec3 a, Vec3 b);

#endif
