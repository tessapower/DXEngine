#ifndef CONST_BUFFER_H
#define CONST_BUFFER_H

#include "stdafx.h"
#include "buffer.h"
#include "exception_macros.h"
#include "renderer/renderer.h"

#include <wrl.h>

template <typename T>
class const_buffer : public buffer {
 public:
   const_buffer(renderer& rndr) {
    buffer_descriptor_.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_descriptor_.Usage = D3D11_USAGE_DYNAMIC;
    buffer_descriptor_.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    buffer_descriptor_.MiscFlags = 0u;
    buffer_descriptor_.ByteWidth = sizeof(T);
    buffer_descriptor_.StructureByteStride = 0u;

    // TODO: swap to info throw
    HRESULT hr;
    RENDER_THROW_ON_FAIL(
        device(rndr)->CreateBuffer(
          &buffer_descriptor_, nullptr, &buffer_
        )
    );
  }

  const_buffer(renderer& rndr, const T& constants) {
    buffer_descriptor_.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_descriptor_.Usage = D3D11_USAGE_DYNAMIC;
    buffer_descriptor_.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    buffer_descriptor_.MiscFlags = 0u;
    buffer_descriptor_.ByteWidth = sizeof(T);
    buffer_descriptor_.StructureByteStride = 0u;

    subresource_data_.pSysMem = &constants;

    // TODO: swap to info throw
    HRESULT hr;
    RENDER_THROW_ON_FAIL(
      device(rndr)->CreateBuffer(
        &buffer_descriptor_, &subresource_data_, &buffer_
    )
    );
  }

  auto update(renderer& rndr, const T& constants) {
    // TODO: swap to info throw

    D3D11_MAPPED_SUBRESOURCE msr;
    RENDER_THROW_ON_FAIL(
        context(rndr)->Map(get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr)
    );

    std::memcpy(msr.pData, &constants, sizeof(constants));
    context(rndr)->Unmap(get(), 0u);
  }
  
  virtual auto bind(renderer& rndr) noexcept -> void = 0;
};


template <typename T>
class vs_const_buffer : public const_buffer<T> {
 public:
  vs_const_buffer(renderer& rndr) : const_buffer(rndr) {}

  vs_const_buffer(renderer& rndr, T& consts) : const_buffer(rndr, consts) {}

  auto bind(renderer& rndr) noexcept -> void override {
    context(rndr)->VSSetConstantBuffers(0u, 1u, buffer_.GetAddressOf());
  }
};

template <typename T>
class ps_const_buffer : public const_buffer<T> {
 public:
  ps_const_buffer(renderer& rndr) : const_buffer(rndr) {}

  ps_const_buffer(renderer& rndr, T& consts) : const_buffer(rndr, consts) {}

  auto bind(renderer& rndr) noexcept -> void override {
    context(rndr)->PSSetConstantBuffers(0u, 1u, buffer_.GetAddressOf());
  }
};

#endif // CONSTANT_BUFFER_H
