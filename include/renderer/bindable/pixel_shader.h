#ifndef PIXEL_SHADER_H
#define PIXEL_SHADER_H
#include "stdafx.h"

#include "shader.h"
#include "renderer/renderer.h"

#include <d3dcompiler.h>
#include <filesystem>
#include <string>
#include <wrl.h>

class pixel_shader : public shader {
 public:
  pixel_shader(renderer& rndr, std::filesystem::path filename)
    : shader(filename) {
    entry_point_ = "main";
    target_ = "ps_5_0";

    HRESULT hr = compile();

    // TODO: Add macro that uses error_blob messages in case of failure
    if (FAILED(hr)) {
      auto error_msg = std::string{"Error compiling pixel shader file!\n"};

      if (error_blob_) {
        error_msg += std::string{static_cast<char*>(error_blob_->GetBufferPointer())};
      } else {
        error_msg += "No error message available.";
      }

      throw exception(__FILE__, __LINE__, error_msg);
    }

    // Create the pixel shader
    hr = create(rndr);
    if (FAILED(hr)) {
      auto error_msg = std::string{"Error creating pixel shader!\n"};

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
    context(rndr)->PSSetShader(pixel_shader_.Get(), nullptr, 0u);
  }

 protected:
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader_;
  
  auto create(renderer& rndr) -> HRESULT {
    return device(rndr)->CreatePixelShader(blob_->GetBufferPointer(),
                                           blob_->GetBufferSize(), nullptr,
                                           &pixel_shader_);
  }
};

#endif // PIXEL_SHADER_H
