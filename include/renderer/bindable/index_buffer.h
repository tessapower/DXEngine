#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H
#include "stdafx.h"

#include "buffer.h"
#include "renderer/renderer.h"

typedef unsigned short index;

class index_buffer : public buffer {
 public:
  index_buffer(renderer& rndr, std::vector<index> const& indices)
      : count_(static_cast<UINT>(indices.size())) {
    buffer_descriptor_.Usage = D3D11_USAGE_DEFAULT;
    buffer_descriptor_.BindFlags = D3D11_BIND_INDEX_BUFFER;
    buffer_descriptor_.CPUAccessFlags = 0u;
    buffer_descriptor_.MiscFlags = 0u;
    buffer_descriptor_.ByteWidth =
        static_cast<UINT>(count_ * sizeof(unsigned short));
    buffer_descriptor_.StructureByteStride = sizeof(unsigned short);

    subresource_data_.pSysMem = indices.data();

    create(rndr);
  }

  virtual auto bind(renderer& rndr) noexcept -> void override {
    context(rndr)->IASetIndexBuffer(buffer_.Get(), DXGI_FORMAT_R16_UINT, 0u);
  }

  auto count() const noexcept -> UINT { return count_; }

 protected:
  UINT count_;
};

#endif // INDEX_BUFFER_H
