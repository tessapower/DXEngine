#include "stdafx.h"
#include "renderer.h"

#include <imgui_impl_dx11.h>
#include <d3dcompiler.h>
#include <wrl.h>

#include "exception_macros.h"
#include "imgui_impl_win32.h"

//----------------------------------------------------------- hr_exception --//

renderer::hr_exception::hr_exception(
    const LPCWSTR file, const int line, const HRESULT hr) noexcept
    : exception(file, line), hr_(hr) {

  type_ = L"Renderer Exception";
}

auto renderer::hr_exception::what() const noexcept -> const char* {
  return "Renderer Exception";
}

auto renderer::hr_exception::msg() const noexcept -> LPCWSTR {
  std::wostringstream oss;
  oss << "[Error Code] 0x" << std::hex << std::uppercase << error_code()
      << std::dec << " (" << static_cast<unsigned long>(error_code()) << ")\n"
      << "[Description] " << error_string() << "\n"
      << source();

  what_buffer_ = oss.str();

  return what_buffer_.c_str();
}

auto renderer::hr_exception::error_code() const noexcept -> HRESULT {
  return hr_;
}

auto renderer::hr_exception::error_string() const noexcept -> std::wstring {
  return translate_error_code(hr_);
}

//--------------------------------------------------------------- renderer --//
auto renderer::clear_back_buffer(const float clear_color[4]) const noexcept
    -> void {
  p_device_context->OMSetRenderTargets(1, &main_render_target_view, nullptr);
  p_device_context->ClearRenderTargetView(main_render_target_view, clear_color);
}

auto renderer::create_device_d3d(const HWND h_wnd) -> HRESULT {
  // Set up swap chain descriptor
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));  // Ensure that the memory is zeroed out
  sd.BufferCount = 2;           // Number of back buffers
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

  constexpr UINT create_device_flags = 0;
  // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

  D3D_FEATURE_LEVEL feature_level;
  constexpr D3D_FEATURE_LEVEL feature_level_array[2] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_0,
  };

  HRESULT hr;

  RNDR_THROW(D3D11CreateDeviceAndSwapChain(
      nullptr,  // Pointer to the video adapter to use when creating a device.
      D3D_DRIVER_TYPE_HARDWARE,  // Driver Type
      nullptr,                   // Handle to software driver binary
      create_device_flags,
      feature_level_array,  // Feature Levels
      2,                    // Feature Levels
      D3D11_SDK_VERSION,    // SDK Version
      &sd, &p_swap_chain, &p_device,
      &feature_level,  // Returns the feature level of the device created
      &p_device_context));

  // Try high-performance WARP software driver if hardware is not available.
  if (hr == DXGI_ERROR_UNSUPPORTED) {
    RNDR_THROW(D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags,
        feature_level_array, 2, D3D11_SDK_VERSION, &sd, &p_swap_chain,
        &p_device, &feature_level, &p_device_context));
  }

  create_render_target();

  return hr;
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
  HRESULT hr;

  RNDR_THROW(p_swap_chain->GetBuffer(0, __uuidof(ID3D11Resource),
                                     reinterpret_cast<void**>(&p_back_buffer)));
  RNDR_THROW(p_device->CreateRenderTargetView(p_back_buffer, nullptr,
                                              &main_render_target_view));

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

auto renderer::test_draw() -> void {
  namespace wrl = Microsoft::WRL;

  struct vertex {
    float x;
    float y;
  };

  constexpr vertex vertices[] = {{0.0f, 0.5f}, {0.5f, -0.5f}, {-0.5f, -0.5f}};

  wrl::ComPtr<ID3D11Buffer> p_vertex_buffer;

  D3D11_BUFFER_DESC bd = {};
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  bd.CPUAccessFlags = 0u;
  bd.MiscFlags = 0u;
  bd.StructureByteStride = sizeof(vertex);

  HRESULT hr;
  D3D11_SUBRESOURCE_DATA sd = {};
  sd.pSysMem = vertices;
  RNDR_THROW(p_device->CreateBuffer(&bd, &sd, &p_vertex_buffer));

  // Bind vertex buffer to pipeline
  constexpr UINT stride = sizeof(vertex);
  constexpr UINT offset = 0u;
  p_device_context->IASetVertexBuffers(0u, 1, &p_vertex_buffer, &stride, &offset);

  // Create vertex shader
  wrl::ComPtr<ID3D11VertexShader> p_vertex_shader;
  wrl::ComPtr<ID3DBlob> p_blob;
  RNDR_THROW(D3DReadFileToBlob(L"${SHADER_DIR}/vertex_shader.cso", &p_blob));
  RNDR_THROW(p_device->CreateVertexShader(p_blob->GetBufferPointer(),
                               p_blob->GetBufferSize(), nullptr,
                               &p_vertex_shader));

  // Bind vertex shader to pipeline
  p_device_context->VSSetShader(p_vertex_shader.Get(), nullptr, 0u);

  p_device_context->Draw(std::size(vertices), 0u);
}
