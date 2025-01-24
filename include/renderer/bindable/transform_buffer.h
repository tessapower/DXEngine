#ifndef TRANSFORM_BUFFER_H
#define TRANSFORM_BUFFER_H

#include "stdafx.h"
#include "const_buffer.h"
#include "drawable.h"
#include "exception_macros.h"

#include <DirectXMath.h>

class transform_buffer : public bindable {
 public:
  transform_buffer(renderer& rndr, drawable const& parent)
      : vsc_buffer_(rndr), parent_(parent) {}

  auto bind(renderer& rndr) noexcept -> void override {
    vsc_buffer_.update(
      rndr,
      DirectX::XMMatrixTranspose(parent_.transform() * rndr.projection())
    );

    vsc_buffer_.bind(rndr);
  }

 protected:
  vs_const_buffer<DirectX::XMMATRIX> vsc_buffer_;
  const drawable& parent_;
};

#endif  // TRANSFORM_BUFFER_H
