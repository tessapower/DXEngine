#include "stdafx.h"
#include "renderer.h"

#include <imgui_impl_dx11.h>

#include "imgui_impl_win32.h"

auto init_backends(const HWND h_wnd) -> void {
  ImGui_ImplWin32_Init(h_wnd);
  ImGui_ImplDX11_Init(g_p_d3d_device, g_p_d3d_device_context);
}

auto create_device_d3d(const HWND h_wnd) -> bool {
  // Setup swap chain
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 2;
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = h_wnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  constexpr UINT create_device_flags = 0;
  // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  D3D_FEATURE_LEVEL feature_level;
  constexpr D3D_FEATURE_LEVEL feature_level_array[2] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_0,
  };
  HRESULT res = D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags,
      feature_level_array, 2, D3D11_SDK_VERSION, &sd, &g_p_swap_chain,
      &g_p_d3d_device, &feature_level, &g_p_d3d_device_context);
  if (res == DXGI_ERROR_UNSUPPORTED)  // Try high-performance WARP software
                                      // driver if hardware is not available.
    res = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags,
        feature_level_array, 2, D3D11_SDK_VERSION, &sd, &g_p_swap_chain,
        &g_p_d3d_device, &feature_level, &g_p_d3d_device_context);
  if (res != S_OK) return false;

  create_render_target();
  return true;
}

auto cleanup_device_d3d() -> void {
  cleanup_render_target();
  if (g_p_swap_chain) {
    g_p_swap_chain->Release();
    g_p_swap_chain = nullptr;
  }
  if (g_p_d3d_device_context) {
    g_p_d3d_device_context->Release();
    g_p_d3d_device_context = nullptr;
  }
  if (g_p_d3d_device) {
    g_p_d3d_device->Release();
    g_p_d3d_device = nullptr;
  }
}

auto create_render_target() -> void {
  ID3D11Texture2D* p_back_buffer;
  g_p_swap_chain->GetBuffer(0, IID_PPV_ARGS(&p_back_buffer));
  g_p_d3d_device->CreateRenderTargetView(p_back_buffer, nullptr,
                                        &g_main_render_target_view);
  p_back_buffer->Release();
}

auto cleanup_render_target() -> void {
  if (g_main_render_target_view) {
    g_main_render_target_view->Release();
    g_main_render_target_view = nullptr;
  }
}

auto shut_down() -> void {
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
}
