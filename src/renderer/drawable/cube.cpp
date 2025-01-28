#include "stdafx.h"
#include "renderer/drawable/cube.h"

#include <vector>

cube::cube(renderer& rndr) {
  // Vertices
  const std::vector<vertex> vertices = {
      {-1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f},
      {-1.0f,  1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f},
      {-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f},
      {-1.0f,  1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f}};

  add_bind(std::make_unique<vertex_buffer>(rndr, vertices));

  // Vertex Shader
  auto vs = std::make_unique<vertex_shader>(
    rndr,
    std::filesystem::path(SHADER_SRC_DIR) / "vertex_shader.vs.hlsl"
  );
  auto vs_bytecode = vs->bytecode();
  add_bind(std::move(vs));

  // Pixel Shader
  add_bind(
    std::make_unique<pixel_shader>(
      rndr,
      std::filesystem::path(SHADER_SRC_DIR) / "pixel_shader.ps.hlsl"
    )
  );

  // Indices
  const std::vector<index> indices = {
    0, 2, 1,  2, 3, 1,
    1, 3, 5,  3, 7, 5,
    2, 6, 3,  3, 6, 7,
    4, 5, 7,  4, 7, 6,
    0, 4, 2,  2, 4, 6,
    0, 1, 4,  1, 5, 4
  };

  add_bind(std::make_unique<index_buffer>(rndr, indices));

  const color_buffer cb = {
    {
      {0.929f, 0.925f, 0.643f},
      {0.741f, 0.929f, 0.643f},
      {0.643f, 0.929f, 0.827f},
      {0.643f, 0.780f, 0.929f},
      {0.843f, 0.643f, 0.929f},
      {0.929f, 0.643f, 0.706f}
    }
  };

  add_bind(std::make_unique<ps_const_buffer<color_buffer>>(rndr, cb));

  // Input Layout
  add_bind(
    std::make_unique<input_layout>(
      rndr,
      std::vector<D3D11_INPUT_ELEMENT_DESC>{{
        "Position",
        0,
        DXGI_FORMAT_R32G32B32_FLOAT,
        0,
        0,
        D3D11_INPUT_PER_VERTEX_DATA,
        0
      }},
      vs_bytecode
    )
  );

}

auto cube::update(float dt_ms) noexcept -> void {
  // Update rotation angles
  yaw_ += dt_ms * 0.001f;
  pitch_ += dt_ms * 0.001f;
  roll_ += dt_ms * 0.001f;

  // Update rotation angles for world space
  chi_ += dt_ms * 0.001f;
  phi_ += dt_ms * 0.001f;
  theta_ += dt_ms * 0.001f;
}

auto cube::transform() const noexcept -> DirectX::XMMATRIX {
  return DirectX::XMMatrixRotationRollPitchYaw(roll_, pitch_, yaw_) *
         DirectX::XMMatrixTranslation(radius_, 0.0f, 0.0f) *
         DirectX::XMMatrixRotationRollPitchYaw(theta_, phi_, chi_) *
         // Put some distance between the camera and the cube
         DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}