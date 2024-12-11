#include "renderer.h"

#include <imgui_impl_dx11.h>

#include "imgui_impl_win32.h"
#include "stdafx.h"

auto renderer::clear_back_buffer(const float clear_color[4]) const noexcept -> void {
  p_device_context->OMSetRenderTargets(1, &main_render_target_view,
                                         nullptr);
  p_device_context->ClearRenderTargetView(main_render_target_view, clear_color);
}


auto renderer::create_device_d3d(const HWND h_wnd) -> bool {
  // Set up swap chain descriptor
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));  // Ensure that the memory is zeroed out
  sd.BufferCount = 2;           // Number of back buffers
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel channel layout
  sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  sd.BufferDesc.RefreshRate.Numerator = 0;         // Use existing refresh rate
  sd.BufferDesc.RefreshRate.Denominator = 0;       // Use existing refresh rate
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = h_wnd;
  sd.SampleDesc.Count = 1;      // Antialiasing i.e. nothing for now
  sd.SampleDesc.Quality = 0;    // Antialiasing i.e. nothing for now
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;  // Effect used for presentation

  constexpr UINT create_device_flags = 0;
  // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

  D3D_FEATURE_LEVEL feature_level;
  constexpr D3D_FEATURE_LEVEL feature_level_array[2] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_0,
  };

  HRESULT res = D3D11CreateDeviceAndSwapChain(
      nullptr,  // Pointer to the video adapter to use when creating a device.
      D3D_DRIVER_TYPE_HARDWARE,  // Driver Type
      nullptr, // Handle to software driver binary
      create_device_flags, //
      feature_level_array,       // Feature Levels
      2, // Feature Levels
      D3D11_SDK_VERSION, // SDK Version
      &sd,
      &p_swap_chain,
      &p_device,
      &feature_level,  // Returns the feature level of the device created
      &p_device_context);

  // Try high-performance WARP software driver if hardware is not available.
  if (res == DXGI_ERROR_UNSUPPORTED) {
    res = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags,
        feature_level_array, 2, D3D11_SDK_VERSION, &sd, &p_swap_chain,
        &p_device, &feature_level, &p_device_context);
  }

  if (res != S_OK) return false;

  create_render_target();

  return true;
}

auto renderer::init_backends(const HWND h_wnd) const -> void {
  ImGui_ImplWin32_Init(h_wnd);
  ImGui_ImplDX11_Init(p_device, p_device_context);
}

auto renderer::cleanup_device_d3d() -> void {
  cleanup_render_target();
  if (p_swap_chain) {
    p_swap_chain->Release();
    p_swap_chain = nullptr;
  }
  if (p_device_context) {
    p_device_context->Release();
    p_device_context = nullptr;
  }
  if (p_device) {
    p_device->Release();
    p_device = nullptr;
  }
}

auto renderer::create_render_target() -> void {
  // Gain access to texture subresource in swap chain (back buffer)
  ID3D11Resource* p_back_buffer;

  // TODO: handle unsuccessful render target creation
  p_swap_chain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&p_back_buffer));
  p_device->CreateRenderTargetView(
    p_back_buffer,
    nullptr,
    &main_render_target_view
  );

  p_back_buffer->Release();
}

auto renderer::cleanup_render_target() -> void {
  if (main_render_target_view) {
    main_render_target_view->Release();
    main_render_target_view = nullptr;
  }
}

auto renderer::shut_down() -> void {
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();

  cleanup_device_d3d();
}
