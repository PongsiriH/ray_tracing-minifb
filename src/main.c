#include "somehelper.h"
#include <MiniFB.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define RGBA(r, g, b, a) (uint32_t)(a << 24 | r << 16 | g << 8 | b)

#define WIDTH 800
#define HEIGHT 600

typedef struct Sphere Sphere;
typedef struct Light Light;

void put_pixel(uint32_t *buff, int x, int y, uint32_t color) {
  if (!(-WIDTH / 2 < x && x < WIDTH / 2) ||
      !(-HEIGHT / 2 < y && y < HEIGHT / 2)) {
    printf("put_pixel error %d, %d, %d, %d, %d\n", -WIDTH / 2, -HEIGHT / 2, x,
           y, x < WIDTH / 2);
    return;
  }
  x = WIDTH / 2 + x;
  y = HEIGHT / 2 - y;
  buff[y * WIDTH + x] = color;
}

struct Sphere {
  Vec3 position;
  uint32_t radius;
  uint32_t color;
};

struct Light {
  enum Type { ambient, point } type;
  float intensity;
  union {
    struct {

    } ambient;
    struct {
      Vec3 position;
    } point;
  };
};

Vec3 canvas_to_viewport(int x, int y, uint32_t Vw, uint32_t Vh, int d) {
  return (Vec3){(float)x * (Vw / (float)WIDTH), (float)y * (Vh / (float)HEIGHT),
                d};
}

void intersected_ray(Vec3 O, Vec3 D, Sphere *sphere, float *results) {
  float a, b, c;
  Vec3 CO = vec3_subtract(O, sphere->position);
  a = vec3_dot(D, D);
  b = 2 * vec3_dot(CO, D);
  c = vec3_dot(CO, CO) - (sphere->radius * sphere->radius);
  float discriminant = b * b - 4 * a * c;
  if (discriminant < 0) {
    results[0] = INFINITY;
    results[1] = INFINITY;
    return;
  }

  float sqrt_discriminant = sqrtf(discriminant);
  results[0] = (-b + sqrt_discriminant) / (2 * a);
  results[1] = (-b - sqrt_discriminant) / (2 * a);
}

float process_lights(Vec3 P, Vec3 N, Light *lights, uint32_t light_count) {
  Light *light;
  float intensity = 0;
  for (int n = 0; n < light_count; n++) {
    light = &lights[n];
    if (light->type == ambient) {
      intensity = light->intensity;
    } else {
      Vec3 L;
      if (light->type == point) {
        L = vec3_subtract(P, light->point.position);
      }
      float n_dot_l = vec3_dot(N, L);
      if (n_dot_l > 0) {
        intensity += light->intensity * n_dot_l / sqrtf(vec3_dot(N, N)) /
                     sqrtf(vec3_dot(L, L));
      }
    }
  }
  return intensity;
}

uint32_t scale_intensity(uint32_t color, float intensity) {
  if (!(0 <= intensity && intensity <= 1)) {
  }

  uint8_t r = color >> 16 & 0xFF;
  uint8_t g = color >> 8 & 0xFF;
  uint8_t b = color & 0xFF;
  uint8_t a = color >> 24 & 0xFF;

  r = fmin(r * intensity, 255);
  g = fmin(g * intensity, 255);
  b = fmin(b * intensity, 255);
  return RGBA((uint8_t)r, (uint8_t)g, (uint8_t)b, a);
}

uint32_t ray_tracing(Vec3 O, Vec3 D, Sphere *spheres, uint32_t sphere_count,
                     Light *lights, uint32_t light_count) {
  Sphere *closest_sphere = NULL;
  float closest_t = INFINITY;
  float results[2] = {0.0, 0.0};
  Sphere *sphere;
  for (int i = 0; i < sphere_count; i++) {
    sphere = &spheres[i];
    intersected_ray(O, D, sphere, results);

    for (int j = 0; j < 2; j++) {
      float t = results[j];
      if (t < closest_t && 1 < t && t < INFINITY) {
        closest_t = t;
        closest_sphere = sphere;
      }
    }
  }

  if (!closest_sphere) {
    return RGBA(255, 255, 255, 0);
  }

  uint32_t color = closest_sphere->color;
  Vec3 P = vec3_add(O, vec3_scalar_multiply(closest_t, D));
  Vec3 N = vec3_subtract(closest_sphere->position, P);
  N = vec3_normalize(N);
  float light_intensity = process_lights(P, N, lights, light_count);
  color = scale_intensity(color, light_intensity);
  return color;
}

int main() {
  struct mfb_window *window = mfb_open_ex("main", WIDTH, HEIGHT, 0);
  uint32_t *buff = malloc(WIDTH * HEIGHT * sizeof(uint32_t));

  Vec3 O = (Vec3){0, 0, -5};

  uint32_t sphere_count = 3;
  Sphere *spheres = malloc(sphere_count * sizeof(Sphere));
  spheres[0] = (Sphere){{0, 1, 1}, 1, RGBA(255, 0, 0, 255)};
  spheres[1] = (Sphere){{2, 0, 1}, 1, RGBA(120, 0, 120, 255)};
  spheres[2] = (Sphere){{2, -502, 1}, 500, RGBA(100, 50, 0, 255)};

  uint32_t light_count = 2;
  Light *lights = malloc(light_count * sizeof(Light));
  lights[0] = (Light){ambient, 0.2};
  lights[1] = (Light){point, 0.8, .point = {{-2, 5, -3}}};
  do {
    for (int x = -WIDTH / 2; x < WIDTH / 2; x++) {
      for (int y = -HEIGHT / 2; y < HEIGHT / 2; y++) {
        Vec3 D = canvas_to_viewport(x, y, 1, 1, 1);
        uint32_t color =
            ray_tracing(O, D, spheres, sphere_count, lights, light_count);
        put_pixel(buff, x, y, color);
      }
    }
    mfb_update(window, buff);
  } while (mfb_wait_sync(window));

  mfb_close(window);
  free(buff);
  free(spheres);
  free(lights);
  return 0;
}
