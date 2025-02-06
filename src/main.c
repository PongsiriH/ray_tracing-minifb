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
typedef struct IntersectionResult IntersectionResult;

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
  float radius;
  uint32_t color;
  float specular;
};

struct Light {
  enum Type { ambient, point, directional} type;
  float intensity;
  union {
    struct {

    } ambient;
    struct {
      Vec3 position;
    } point;
    struct {
      Vec3 direction;
    } directional;
  };
};

struct IntersectionResult {
    Sphere* sphere;
    float distance;
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

IntersectionResult closest_intersection(Vec3 O, Vec3 D, Sphere *spheres, uint32_t sphere_count, float t_min, float t_max) {   
    IntersectionResult result = {NULL, INFINITY};
    float closest_t = INFINITY;      // Track closest intersection distance
    Sphere *closest_sphere = NULL;  // Track closest intersected sphere
    
    float t_results[2];  // Store both intersection points for each sphere
    Sphere *sphere;

    for (int i = 0; i < sphere_count; i++) {
        sphere = &spheres[i];
        intersected_ray(O, D, sphere, t_results);

        for (int j = 0; j < 2; j++) {
            float t = t_results[j];
            if (t < closest_t && t_min < t && t < t_max) {
                closest_t = t;
                closest_sphere = sphere;
            }
        }
    }

    // Set our final result
    result.sphere = closest_sphere;
    result.distance = closest_t;
    
    return result;
}

float process_lights(Vec3 P, Vec3 N, Vec3 V, Sphere *spheres, uint32_t sphere_count, Light *lights, uint32_t light_count, float s) {
  Light *light;
  float intensity = 0;
  
  float n_dot_l;
  Vec3 L;
  float t_max;
  for (int n = 0; n < light_count; n++) {
    light = &lights[n];
    if (light->type == ambient) {
      intensity = light->intensity;
    } else {
      if (light->type == point) {
        L = vec3_subtract(light->point.position, P);
        t_max = 1;
      } else if (light->type == directional) {
        L = vec3_normalize(light->directional.direction);
        t_max = INFINITY;
      }

      // Shadow
      IntersectionResult closest_shadow_t = closest_intersection(P, L, spheres, sphere_count, 0.001, t_max);
      Sphere *closest_shadow = closest_shadow_t.sphere;
      if (closest_shadow != NULL) {
         continue;
      } 

      // Diffuse
      n_dot_l = vec3_dot(N, L);
      if (n_dot_l > 0) {
        intensity += light->intensity * n_dot_l / sqrtf(vec3_dot(N, N)) / sqrtf(vec3_dot(L, L));
      }
      
      // Specular
      if (s != -1) {
        Vec3 R = vec3_subtract(vec3_scalar_multiply(n_dot_l, vec3_scalar_multiply(2, N)), L);
        R = vec3_normalize(R);
        V = vec3_normalize(V);
        float r_dot_v = vec3_dot(R, V);
        if (r_dot_v > 0) {
          intensity += light->intensity * powf(r_dot_v / sqrtf(vec3_dot(R, R)) / sqrtf(vec3_dot(V,V)), s);
        } 
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
  IntersectionResult closest_sphere_t = closest_intersection(O, D, spheres, sphere_count, 1, INFINITY);
  Sphere *closest_sphere = closest_sphere_t.sphere;
  float closest_t = closest_sphere_t.distance;

  if (!closest_sphere) {
    return RGBA(255, 255, 255, 0);
  }

  uint32_t color = closest_sphere->color;
  Vec3 P = vec3_add(O, vec3_scalar_multiply(closest_t, D));
  Vec3 N = vec3_subtract(closest_sphere->position, P);
  N = vec3_normalize(N);
  Vec3 V = vec3_subtract(O, P);
  float light_intensity = process_lights(P, N, V, spheres, sphere_count, lights, light_count, closest_sphere->specular);
  color = scale_intensity(color, light_intensity);
  return color;
}

int main() {
  struct mfb_window *window = mfb_open_ex("main", WIDTH, HEIGHT, 0);
  uint32_t *buff = malloc(WIDTH * HEIGHT * sizeof(uint32_t));

  Vec3 O = (Vec3){0, 0, -5};

  uint32_t sphere_count = 4;
  Sphere *spheres = malloc(sphere_count * sizeof(Sphere));
  spheres[0] = (Sphere){{-1, 1, 5}, 1, RGBA(255, 0, 0, 255), 500};
  spheres[1] = (Sphere){{1, 0, 4}, 1, RGBA(120, 0, 120, 255), 1000};
  spheres[2] = (Sphere){{0.5, -502, 1}, 500, RGBA(100, 50, 0, 255), 5};
  spheres[3] = (Sphere){{0, -2, 3}, 1, RGBA(120, 120, 50, 255), 100};

  uint32_t light_count = 3;
  Light *lights = malloc(light_count * sizeof(Light));
  lights[0] = (Light){ambient, 0.5};             // Subtle ambient
  lights[1] = (Light){point, 5, .point = {{0,4,0}}};     // Moderate point light
  lights[2] = (Light){directional, 5, .directional={{1, 2, 1}}};

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
