#ifndef TRANSFORM_BUFFER_H
#define TRANSFORM_BUFFER_H

#include "stdafx.h"
#include "const_buffer.h"
#include "renderer/drawable/drawable.h"
#include "exception_macros.h"

#include <DirectXMath.h>

class transform_buffer : public bindable {
 public:
  transform_buffer(renderer& rndr, drawable const& parent)
      : vsc_buffer_(rndr), parent_(parent) {}

    // TODO: ctor with optional initial value for vsc_buffer_

  auto bind(renderer& rndr) noexcept -> void override {
    vsc_buffer_.update(
      rndr,
      DirectX::XMMatrixTranspose(parent_.transform() * rndr.projection())
    );

    vsc_buffer_.bind(rndr);
  }

  // TODO: function to set translation
  // TODO: function to set rotation
  // TODO: function to set scale

 protected:
  // TODO: split into multiple matrices for translation, rotation, and scale
  vs_const_buffer<DirectX::XMMATRIX> vsc_buffer_;
  const drawable& parent_;
};

#endif  // TRANSFORM_BUFFER_H
