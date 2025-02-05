// src/somehelper.c
#include "somehelper.h"
#include <math.h>

void draw_rectangle(uint32_t *buffer, int width, int height, int x, int y,
                    int w, int h, uint32_t color) {
  for (int i = y; i < y + h && i < height; i++) {
    for (int j = x; j < x + w && j < width; j++) {
      buffer[i * width + j] = color;
    }
  }
}

Vec3 vec3_add(Vec3 a, Vec3 b) {
  return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3 vec3_subtract(Vec3 a, Vec3 b) {
  return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

float vec3_dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

Vec3 vec3_scalar_multiply(float k, Vec3 a) {
  return (Vec3){k * a.x, k * a.y, k * a.z};
}

Vec3 vec3_normalize(Vec3 a) {
  float length = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
  return (Vec3){a.x / length, a.y / length, a.z / length};
}
