#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "stdafx.h"
#include "buffer.h"
#include "exception_macros.h"

#include <wrl.h>

class vertex_buffer : public buffer {
 public:
  template <typename T>
  vertex_buffer(renderer& rndr, std::vector<T> const& vertices)
    : stride_(sizeof(T)) {
    buffer_descriptor_.Usage = D3D11_USAGE_DEFAULT;
    buffer_descriptor_.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_descriptor_.CPUAccessFlags = 0u;
    buffer_descriptor_.MiscFlags = 0u;
    buffer_descriptor_.ByteWidth = static_cast<UINT>(sizeof(T) * vertices.size());
    buffer_descriptor_.StructureByteStride = sizeof(T);

    subresource_data_.pSysMem = vertices.data();

    HRESULT hr;
    RENDER_THROW_HR(
      device(rndr)->CreateBuffer(
        &buffer_descriptor_,
        &subresource_data_,
        &buffer_
      )
    );
  }

  virtual auto bind(renderer& rndr) noexcept -> void override {
    constexpr UINT offset = 0u;
    context(rndr)->IASetVertexBuffers(0u, 1u, buffer_.GetAddressOf(), &stride_, &offset);
  }

 protected:
  UINT stride_;
};

#endif // VERTEX_BUFFER_H
