#ifndef BINDABLE_H
#define BINDABLE_H
#include "stdafx.h"

#include "renderer/renderer.h"
#include "dxgi_info_manager.h"

#include <d3d11.h>

class bindable {
 public:
  virtual auto bind(renderer& rndr) noexcept -> void = 0;
  virtual ~bindable() = default;

 protected:
  // The following are helper methods to enable bindable child classes to
  // have access to private members of renderer without needing to make every
  // class a friend of renderer.

  /** @brief Returns a pointer to the given renderer's device context.
  */
  static auto context(renderer& rndr) noexcept -> ID3D11DeviceContext* {
    return rndr.p_device_context_.Get();
  }

  /** @brief Returns a reference to the given renderer's dxgi_info_manager.
   * Used to pull the dxgi_info_manager into scope for specific macros that
   * require its presence.
   */
  static dxgi_info_manager& info_manager(renderer& rndr) noexcept {
    return rndr.dxgi_info_mgr_;
  }

  /** @brief Returns a pointer to the given renderer's device.
   */
  static ID3D11Device* device(renderer& rndr) noexcept {
    return rndr.p_device_.Get();
  }
};

#endif  // BINDABLE_H
