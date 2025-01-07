#ifndef RENDERER_H
#define RENDERER_H
#include "stdafx.h"

#include <d3d11.h>
#include <string>
#include <vector>

#include "engine_exception.h"
#include "dxgi_info_manager.h"

class renderer {
 public:
  class exception : public hr_exception {
   public:
    exception(const LPCSTR file, const int line, const HRESULT hr,
              std::vector<std::string> messages = {}) noexcept
        : hr_exception(file, line, hr, messages) {
     type_ = "Renderer Exception";
    }
  };

  class device_removed_exception final : public exception {
   public:
    device_removed_exception(const LPCSTR file, const int line,
                             const HRESULT hr,
                             std::vector<std::string> messages = {}) noexcept
        : exception(file, line, hr, messages) {
      type_ = "Renderer: Device Removed Exception";
    }

  };

private:
  dxgi_info_manager dxgi_info_mgr_;

  ID3D11Device* p_device_ = nullptr;
  IDXGISwapChain* p_swap_chain_ = nullptr;
  ID3D11DeviceContext* p_device_context_ = nullptr;

  bool swap_chain_occluded_ = false;
  UINT resize_width_ = 0;
  UINT resize_height_ = 0;
  ID3D11RenderTargetView* p_render_target_view_ = nullptr;

public:
  renderer() = default;
  ~renderer() = default;
  renderer(const renderer&) = delete;
  renderer& operator=(const renderer&) = delete;
  renderer(const renderer&&) = delete;
  renderer& operator=(const renderer&&) = delete;

  auto clear_back_buffer(const float clear_color[4]) const noexcept -> void;

  auto end_frame() -> void;

  auto resize(int width, int height) -> void;

  auto create_device_d3d(HWND h_wnd) -> HRESULT;

  // TODO: use these in dtor?
  auto cleanup_device_d3d() -> void;

  auto init_backends(HWND h_wnd) const -> void;

  auto create_render_target() -> void;

  auto cleanup_render_target() -> void;

  auto shut_down() -> void;

  auto test_draw() -> void;
};

#endif  // RENDERER_H
