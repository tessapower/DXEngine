#include "stdafx.h"

#include "renderer/renderer.h"
#include "exception_macros.h"
#include "renderer/bindable/index_buffer.h"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <filesystem>

//--------------------------------------------------------------- renderer --//
auto renderer::clear_back_buffer(const float clear_color[4]) const noexcept
    -> void {
  p_device_context_->ClearRenderTargetView(
    p_render_target_view_.Get(),
    clear_color
  );

  p_device_context_->ClearDepthStencilView(
    p_depth_stencil_view_.Get(),
    D3D11_CLEAR_DEPTH,
    1.0f,
    0u
  );
}

auto renderer::end_frame() -> void {
  HRESULT hr;

  // Handle window being minimized or screen locked
  if (swap_chain_occluded_ &&
      p_swap_chain_->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
    Sleep(10);

    return;
  }

  swap_chain_occluded_ = false;

  // TODO: use result of window resize
  // Handle window resize (we don't resize directly in the WM_SIZE handler)
  //if (resize_width_ != 0 && resize_height_ != 0) {
  //  cleanup_render_target();
  //  RENDER_THROW_ON_FAIL(p_swap_chain_->ResizeBuffers(
  //      0, resize_width_, resize_height_, DXGI_FORMAT_UNKNOWN, 0));
  //  resize_width_ = resize_height_ = 0;
  //  create_render_target();
  //}

  dxgi_info_mgr_.set();

  // Present the back buffer to the front buffer
  // Present with vsync
  if (FAILED(hr = p_swap_chain_->Present(1u, 0u))) {
    if (hr == DXGI_ERROR_DEVICE_REMOVED) {
      throw RENDER_DEVICE_REMOVED_EXCEPT(p_device_->GetDeviceRemovedReason());
    } else {
      throw RENDER_EXCEPT_MSG(hr);
    }
  }
  // HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
  swap_chain_occluded_ = (hr == DXGI_STATUS_OCCLUDED);
}

auto renderer::resize(const int width, const int height) -> void {
  resize_width_ = width;
  resize_height_ = height;
}

auto renderer::create_device_d3d(const HWND h_wnd) -> HRESULT {
  HRESULT hr;

  //----------------------------------------------------------- Swap Chain --//
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));  // Ensure that the memory is zeroed out
  sd.BufferCount = 1;           // Number of back buffers
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // Pixel channel layout
  sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  sd.BufferDesc.RefreshRate.Numerator = 0;    // Use existing refresh rate
  sd.BufferDesc.RefreshRate.Denominator = 0;  // Use existing refresh rate
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = h_wnd;
  sd.SampleDesc.Count = 1;    // Antialiasing i.e. nothing for now
  sd.SampleDesc.Quality = 0;  // Antialiasing i.e. nothing for now
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;  // Effect used for presentation

  const UINT create_device_flags = D3D11_CREATE_DEVICE_DEBUG;

  D3D_FEATURE_LEVEL feature_level;
  constexpr D3D_FEATURE_LEVEL feature_level_array[2] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_0,
  };

  RENDER_THROW_HR_MSG(D3D11CreateDeviceAndSwapChain(
      nullptr,  // Pointer to the video adapter to use when creating a device
      D3D_DRIVER_TYPE_HARDWARE,  // Driver Type
      nullptr,                   // Handle to software driver binary
      create_device_flags,       // Flags for device creation
      feature_level_array,       // Feature Levels
      2,
      D3D11_SDK_VERSION,  // SDK Version
      &sd,                // Pointer to the swap chain description
      &p_swap_chain_,     // Pointer to the swap chain created
      &p_device_,         // Pointer to the device created
      &feature_level,     // Returns the feature level of the device created
      &p_device_context_  // Pointer to the device context
      ));

  // Try high-performance WARP software driver if hardware is not available.
  if (hr == DXGI_ERROR_UNSUPPORTED) {
    RENDER_THROW_HR_MSG(D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags,
        feature_level_array, 2, D3D11_SDK_VERSION, &sd, &p_swap_chain_,
        &p_device_, &feature_level, &p_device_context_));
  }

  create_render_target();

  //--------------------------------------------------------- Depth Buffer --//
  D3D11_DEPTH_STENCIL_DESC dsd = {};
  dsd.DepthEnable = TRUE;
  dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  dsd.DepthFunc = D3D11_COMPARISON_LESS;

  // Create depth stencil state
  Microsoft::WRL::ComPtr<ID3D11DepthStencilState> p_depth_stencil_state;
  RENDER_THROW_HR_MSG(
      p_device_->CreateDepthStencilState(&dsd, &p_depth_stencil_state)
  );

  // Bind depth buffer to pipeline
  p_device_context_->OMSetDepthStencilState(p_depth_stencil_state.Get(), 1u);

  // Create depth stencil texture
  Microsoft::WRL::ComPtr<ID3D11Texture2D> p_depth_stencil_texture;
  D3D11_TEXTURE2D_DESC dstd = {};
  dstd.Width = 1280u;  // TODO: Get width and height from swap chain
  dstd.Height = 800u;  // TODO: Get width and height from swap chain
  dstd.MipLevels = 1u;
  dstd.ArraySize = 1u;
  dstd.Format = DXGI_FORMAT_D32_FLOAT;      // Format of each element in texture
  dstd.SampleDesc.Count = 1u;   // Anti-aliasing processing:
                               // number of multisamples per pixel
  dstd.SampleDesc.Quality = 0u; // Anti-aliasing processing: image quality level
  dstd.Usage = D3D11_USAGE_DEFAULT;
  dstd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  RENDER_THROW_HR_MSG(
      p_device_->CreateTexture2D(&dstd, nullptr, &p_depth_stencil_texture));

  // Create view of depth stencil texture
  D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
  dsvd.Format = DXGI_FORMAT_D32_FLOAT;
  dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  dsvd.Texture2D.MipSlice = 0u;
  RENDER_THROW_HR_MSG(p_device_->CreateDepthStencilView(
      p_depth_stencil_texture.Get(), &dsvd, &p_depth_stencil_view_));

  // Bind the render target and depth stencil buffer to the output-merger stage
  THROW_MSG(
    p_device_context_->OMSetRenderTargets(
      1u,
      p_render_target_view_.GetAddressOf(),
      p_depth_stencil_view_.Get()
    )
  );

  //------------------------------------------------------------- Viewport --//
  D3D11_VIEWPORT vp = {};
  vp.Width = 1280;
  vp.Height = 800;
  vp.MinDepth = 0;
  vp.MaxDepth = 1;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  THROW_MSG(p_device_context_->RSSetViewports(1u, &vp));

  return hr;
}

auto renderer::init_backends(const HWND h_wnd [[maybe_unused]]) const -> void {
  // ImGui_ImplWin32_Init(h_wnd);
  // ImGui_ImplDX11_Init(p_device_, p_device_context_);
}

auto renderer::create_render_target() -> void {
  // Gain access to texture subresource in swap chain (back buffer)
  Microsoft::WRL::ComPtr<ID3D11Resource> p_back_buffer;

  HRESULT hr;
  RENDER_THROW_HR_MSG(
    p_swap_chain_->GetBuffer(
      0,
      __uuidof(ID3D11Resource),
      &p_back_buffer
    )
  );

  RENDER_THROW_HR_MSG(
    p_device_->CreateRenderTargetView(
      p_back_buffer.Get(),
      nullptr,
      &p_render_target_view_
    )
  );
}

auto renderer::shut_down() -> void {
  // TODO: uncomment to reintroduce IMGUI
  // ImGui_ImplDX11_Shutdown();
  // ImGui_ImplWin32_Shutdown();
}

auto renderer::test_draw() -> void {
  //------------------------------------------------------------- Vertices --//
  struct vertex {
    float x;
    float y;
    float z;
  };

  constexpr vertex vertices[] = {
    {-1.0f, -1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f},
    {-1.0f,  1.0f, -1.0f},
    { 1.0f,  1.0f, -1.0f},
    {-1.0f, -1.0f,  1.0f},
    { 1.0f, -1.0f,  1.0f},
    {-1.0f,  1.0f,  1.0f},
    { 1.0f,  1.0f,  1.0f}
  };

  // Vertex Buffer
  Microsoft::WRL::ComPtr<ID3D11Buffer> p_vertex_buffer;

  //--------------------------------------------------------- Index Buffer --//

  const std::vector<index> indices = {
      0, 2, 1,  2, 3, 1,
      1, 3, 5,  3, 7, 5,
      2, 6, 3,  3, 6, 7,
      4, 5, 7,  4, 7, 6,
      0, 4, 2,  2, 4, 6,
      0, 1, 4,  1, 5, 4
  };

  //---------------------------------------- Constant Buffer for Transform --//

  struct ConstantBuffer {
    DirectX::XMMATRIX transform;
  };

  const ConstantBuffer cb = {
      DirectX::XMMatrixTranspose(
          DirectX::XMMatrixRotationZ(0.5f) * DirectX::XMMatrixRotationX(0.5f) *
          DirectX::XMMatrixTranslation(0.5f, 0.5f, 4.0f) *
          DirectX::XMMatrixPerspectiveLH(1.0f, 800.0f / 1280.0f, 0.5f, 10.0f))
  };

  //------------------------------------------- Constant Buffer for Colors --//

  struct ConstantColorBuffer {
    struct {
      float r, g, b, a;
    } face_colors[6];
  };

  const ConstantColorBuffer ccb = {
    {
      {0.929f, 0.925f, 0.643f},
      {0.741f, 0.929f, 0.643f},
      {0.643f, 0.929f, 0.827f},
      {0.643f, 0.780f, 0.929f},
      {0.843f, 0.643f, 0.929f},
      {0.929f, 0.643f, 0.706f}
    }
  };

  //--------------------------------------------------------- Input Layout --//
  // Input (vertex) layout (2D position only)
  Microsoft::WRL::ComPtr<ID3D11InputLayout> p_input_layout;
  constexpr D3D11_INPUT_ELEMENT_DESC ied[] = {
    {
      "Position",                   // Semantic name
      0,                            // Semantic index
      DXGI_FORMAT_R32G32B32_FLOAT,  // Format
      0,                            // Input slot
      0,                            // Aligned byte offset
      D3D11_INPUT_PER_VERTEX_DATA,  // Input slot class
      0                             // Instance data step rate
    }
  };

  // Set primitive topology to triangle list
  p_device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // Draw the thing
  p_device_context_->DrawIndexed(static_cast<UINT>(std::size(indices)), 0u, 0u);
}
