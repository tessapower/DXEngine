#ifndef BUFFER_H
#define BUFFER_H
#include "stdafx.h"

#include "renderer/bindable/bindable.h"
#include "renderer/renderer.h"
#include "exception_macros.h"
#include "dxgi_info_manager.h"

#include <d3d11.h>
#include <wrl.h>

class buffer : public bindable {
 public:
  buffer() = default;
  buffer(buffer&&) = default;
  buffer& operator=(buffer&&) = default;

  buffer(const buffer&) = delete;
  buffer& operator=(const buffer&) = delete;

  ~buffer() = default;

  auto get() const noexcept -> ID3D11Buffer* { return buffer_.Get(); }

 protected:
  Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
  D3D11_BUFFER_DESC buffer_descriptor_ = {};
  D3D11_SUBRESOURCE_DATA subresource_data_ = {};

  auto create(renderer& rndr) -> void {
    HRESULT hr;
    auto& dxgi_info_mgr_ = info_manager(rndr);
    HR_THROW_MSG(
      device(rndr)->CreateBuffer(
        &buffer_descriptor_,
        (subresource_data_.pSysMem == nullptr ? nullptr: &subresource_data_),
        &buffer_
      )
    );
  }
};

#endif // BUFFER_H
