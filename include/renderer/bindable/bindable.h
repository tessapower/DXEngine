#ifndef BINDABLE_H
#define BINDABLE_H

#include "renderer/renderer.h"

class bindable {
 public:
  virtual auto bind(renderer& rndr) noexcept -> void = 0;
  virtual ~bindable() = default;

 protected:
  static auto context(renderer& rndr) noexcept -> ID3D11DeviceContext* {
    return rndr.p_device_context_.Get();
  }

  static dxgi_info_manager& info_manager(renderer& rndr) noexcept {
    return rndr.dxgi_info_mgr_;
  }

  static ID3D11Device* device(renderer& rndr) noexcept {
      return rndr.p_device_.Get();
  }
};

#endif  // BINDABLE_H
