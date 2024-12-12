#ifndef RENDERER_H
#define RENDERER_H
#include "stdafx.h"
#include <d3d11.h>
#include <vector>
#include <string>

#include "engine_exception.h"

class renderer {
 public:
  class exception : public engine_exception {
    using engine_exception::engine_exception;
  };

  class hr_exception : public exception {
   public:
    hr_exception(LPCWSTR file, int line, HRESULT hr) noexcept;
    auto what() const noexcept -> const char* override;

    // TODO: add similar functions to app::exception

   private:
    HRESULT hr_;
  };

class device_removed_exception final : public hr_exception {
  using hr_exception::hr_exception;

  std::wstring reason_;
};

  ID3D11Device* p_device = nullptr;
  IDXGISwapChain* p_swap_chain = nullptr;
  ID3D11DeviceContext* p_device_context = nullptr;

  bool swap_chain_occluded = false;
  UINT resize_width = 0;
  UINT resize_height = 0;
  ID3D11RenderTargetView* main_render_target_view = nullptr;

  renderer() = default;
  ~renderer() = default;
  renderer(const renderer&) = delete;
  renderer& operator=(const renderer&) = delete;
  renderer(const renderer&&) = delete;
  renderer& operator=(const renderer&&) = delete;

  auto clear_back_buffer(const float clear_color[4]) const noexcept-> void;

  auto create_device_d3d(HWND h_wnd) -> bool;

  auto init_backends(HWND h_wnd) -> void;

  auto create_render_target() -> void;

  // TODO: use these in dtor?

  auto cleanup_device_d3d() -> void;

  auto cleanup_render_target() -> void;

  auto shut_down() -> void;
};

#endif // RENDERER_H
