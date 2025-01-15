#ifndef TRANSFORM_BUFFER_H
#define TRANSFORM_BUFFER_H

#include "stdafx.h"
#include "const_buffer.h"
#include "exception_macros.h"

#include <DirectXMath.h>

class transform_buffer : public vs_const_buffer<DirectX::XMMATRIX> {
 public:
  transform_buffer(renderer& rndr, DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity()) 
    : transform_(DirectX::XMMatrixTranspose(transform)), vs_const_buffer(rndr, transform_) {

    HRESULT hr;
    auto& dxgi_info_mgr_ = info_manager(rndr);

    HR_THROW_INFO(device(rndr)->CreateBuffer(&buffer_descriptor_,
                                             &subresource_data_, &buffer_));
  }

 protected:
  DirectX::XMMATRIX transform_;
};

#endif // TRANSFORM_BUFFER_H
