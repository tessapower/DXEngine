#ifndef VERTEX_SHADER_H
#define VERTEX_SHADER_H

#include "stdafx.h"
#include "shader.h"

#include <filesystem>
#include <string>
#include <wrl.h>

class vertex_shader : public shader {
 public:
  vertex_shader(renderer& rndr, std::filesystem::path filename)
      : shader(filename) {
    entry_point_ = "main";
    target_ = "vs_5_0";

    HRESULT hr = compile();

    // TODO: Add macro that uses error_blob messages in case of failure
    if (FAILED(hr)) {
      auto error_msg = std::string{"Error compiling vertex shader file!\n"};

      if (error_blob_) {
        error_msg +=
            std::string{static_cast<char*>(error_blob_->GetBufferPointer())};
      } else {
        error_msg += "No error message available.";
      }

      throw exception(__FILE__, __LINE__, error_msg);
    }

    // Create the pixel shader
    hr = create(rndr);
    if (FAILED(hr)) {
      auto error_msg = std::string{"Error creating vertex shader!\n"};

      if (error_blob_) {
        error_msg +=
            std::string{static_cast<char*>(error_blob_->GetBufferPointer())};
      } else {
        error_msg += "No error message available.";
      }

      throw exception(__FILE__, __LINE__, error_msg);
    }
  }

  auto bind(renderer& rndr) noexcept -> void override {
    context(rndr)->VSSetShader(vertex_shader_.Get(), nullptr, 0u);
  }

  auto bytecode() const noexcept -> ID3DBlob* { return blob_.Get(); }

  protected:
  auto create(renderer& rndr) -> HRESULT {
    return device(rndr)->CreateVertexShader(
      blob_->GetBufferPointer(),
      blob_->GetBufferSize(),
      nullptr,
      &vertex_shader_);
  }

 protected:
  Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader_;
};

#endif // VERTEX_SHADER_H
