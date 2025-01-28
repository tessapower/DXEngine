#ifndef CUBE_H
#define CUBE_H
#include "stdafx.h"

#include "renderer/renderer.h"
#include "renderer/drawable/drawable.h"

#include <DirectXMath.h>
#include <vector>

class cube : public drawable {
 public:
  cube(renderer& rndr);
  ~cube() = default;

  auto update(float dt_ms) noexcept -> void override;
  auto transform() const noexcept -> DirectX::XMMATRIX;

 private:
  // Positional data
  // Rotation around origin of box
  float yaw_ = 0.0f;   // x-axis
  float pitch_ = 0.0f; // y-axis
  float roll_ = 0.0f;  // z-axis
  // Distance from origin of world space
  float radius_ = 0.0f;
  // Rotation around origin of world space
  float chi_ = 0.0f;   // yaw/x-axis
  float phi_ = 0.0f;   // pitch/y-axis
  float theta_ = 0.0f; // roll/z-axis

  struct vertex {
    struct {
      float x, y, z;
    } position;
  };

  struct color_buffer {
    struct {
      float r, g, b, a;
    } face_colors[6];
  };
};

#endif // CUBE_H
