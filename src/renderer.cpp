#include "stdafx.h"
#include "renderer.h"

#include <imgui_impl_dx11.h>
#include <d3dcompiler.h>
#include <wrl.h>

#include "exception_macros.h"
#include "imgui_impl_win32.h"

#include <d3d11.h>
#include <iostream>
#include <filesystem>

//----------------------------------------------------------- hr_exception --//

renderer::hr_exception::hr_exception(
    const LPCSTR file, const int line, const HRESULT hr) noexcept
    : exception(file, line), hr_(hr) {

  type_ = "Renderer Exception";
}

auto renderer::hr_exception::what() const noexcept -> const char* {
  std::ostringstream oss;
  oss << "[Error Code] 0x" << std::hex << std::uppercase << error_code()
      << std::dec << " (" << static_cast<unsigned long>(error_code()) << ")\n"
      << source() << "\n"
      << "[Description] " << translate_error_code(hr_) << "\n";

  what_buffer_ = oss.str();

  return what_buffer_.c_str();
}

auto renderer::hr_exception::error_code() const noexcept -> HRESULT {
  return hr_;
}

//--------------------------------------------------------------- renderer --//
auto renderer::clear_back_buffer(const float clear_color[4]) const noexcept
    -> void {
  p_device_context->OMSetRenderTargets(1, &p_render_target_view, nullptr);
  p_device_context->ClearRenderTargetView(p_render_target_view, clear_color);
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
                                              &p_render_target_view));

  p_back_buffer->Release();
}

auto renderer::cleanup_render_target() -> void {
  if (p_render_target_view) {
    p_render_target_view->Release();
    p_render_target_view = nullptr;
  }
}

auto renderer::shut_down() -> void {
  // TODO: uncomment to reintroduce IMGUI
  //ImGui_ImplDX11_Shutdown();
  //ImGui_ImplWin32_Shutdown();

  cleanup_device_d3d();
}

auto renderer::test_draw() -> void {
  namespace wrl = Microsoft::WRL;

  //------------------------------------------------------------- Vertices --//
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
  bd.ByteWidth = sizeof(vertex);
  bd.StructureByteStride = 0;

  HRESULT hr;
  D3D11_SUBRESOURCE_DATA sd = {};
  sd.pSysMem = vertices;
  RNDR_THROW(p_device->CreateBuffer(&bd, &sd, &p_vertex_buffer));

  // Bind vertex buffer to pipeline
  constexpr UINT stride = sizeof(vertex);
  constexpr UINT offset = 0u;
  p_device_context->IASetVertexBuffers(0u, 1, p_vertex_buffer.GetAddressOf(), &stride, &offset);

  //--------------------------------------------------------- Pixel Shader --//
  // Create pixel shader
  wrl::ComPtr<ID3D11PixelShader> p_pixel_shader;
  wrl::ComPtr<ID3DBlob> p_blob;
  Microsoft::WRL::ComPtr<ID3DBlob> error_blob;

  auto path = std::filesystem::path(SHADER_SRC_DIR) / "pixel_shader.ps.hlsl";
  std::wstring wide_path = path.wstring();

  if (!exists(path)) {
    std::cerr << "Error: Pixel shader file not found.\n";

    return;
  }

  // Compile pixel shader
  hr = D3DCompileFromFile(
    wide_path.c_str(), // HLSL file
    nullptr,           // Macros
    nullptr,           // Include handler
    "main",            // Entry point
    "ps_5_0",          // Target shader model
    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Compile flags
    0,                                               // Effect flags
    p_blob.GetAddressOf(),    // Compiled shader output
    error_blob.GetAddressOf() // Error messages
  );


  // TODO: Add macro that uses error_blob messages in case of failure
  if (FAILED(hr)) {
    std::cerr << "Error: Failed to compile pixel shader.\n";

    if (error_blob) {
      std::cerr << "Shader Compilation Error: "
                << static_cast<char*>(error_blob->GetBufferPointer()) << "\n";
    }

    return;
  }

  // Create the pixel shader
  RNDR_THROW(
    p_device->CreatePixelShader(
      p_blob->GetBufferPointer(),
      p_blob->GetBufferSize(),
      nullptr,
      &p_pixel_shader
    )
  );

  // Bind pixel shader to pipeline
  p_device_context->PSSetShader(p_pixel_shader.Get(), nullptr, 0u);

  //-------------------------------------------------------- Vertex Shader --//
  wrl::ComPtr<ID3D11VertexShader> p_vertex_shader;

  path =
      std::filesystem::path(SHADER_SRC_DIR) / "vertex_shader.vs.hlsl";
  wide_path = path.wstring();

  if (!exists(path)) {
    std::cerr << "Error: Vertex shader file not found.\n";

    return;
  }

  // Compile vertex shader
  hr = D3DCompileFromFile(
      wide_path.c_str(),                                // HLSL file
      nullptr,                                          // Macros
      nullptr,                                          // Include handler
      "main",                                           // Entry point
      "vs_5_0",                                         // Target shader model
      D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,  // Compile flags
      0,                                                // Effect flags
      p_blob.GetAddressOf(),     // Compiled shader output
      error_blob.GetAddressOf()  // Error messages
  );

  // TODO: Add macro that uses error_blob messages in case of failure
  if (FAILED(hr)) {
    std::cerr << "Error: Failed to compile vertex shader.\n";

    if (error_blob) {
      std::cerr << "Shader Compilation Error: "
                << static_cast<char*>(error_blob->GetBufferPointer()) << "\n";
    }

    return;
  }

  // Create the vertex shader
  RNDR_THROW(
    p_device->CreateVertexShader(
      p_blob->GetBufferPointer(),
      p_blob->GetBufferSize(),
      nullptr,
      &p_vertex_shader
    )
  );

  // Bind vertex shader to pipeline
  p_device_context->VSSetShader(p_vertex_shader.Get(), nullptr, 0u);
  
  //--------------------------------------------------------- Input Layout --//
  // Input (vertex) layout (2D position only)
  wrl::ComPtr<ID3D11InputLayout> p_input_layout;
  constexpr D3D11_INPUT_ELEMENT_DESC ied[] = {
      {"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,
       D3D11_INPUT_PER_VERTEX_DATA, 0}
  };

  // Create input layout
  RNDR_THROW(p_device->CreateInputLayout(
      ied, std::size(ied), p_blob->GetBufferPointer(), p_blob->GetBufferSize(),
      &p_input_layout));

  // Bind input layout to pipeline
  p_device_context->IASetInputLayout(p_input_layout.Get());

  // Bind the render target
  p_device_context->OMSetRenderTargets(1u, &p_render_target_view, nullptr);

  // Set primitive topology to triangle list
  p_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // Configure view port
  D3D11_VIEWPORT vp;
  vp.Width = 1280;
  vp.Height = 800;
  vp.MinDepth = 0;
  vp.MaxDepth = 1;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  p_device_context->RSSetViewports(1u, &vp);

  // Draw the thing
  p_device_context->Draw(std::size(vertices), 0u);
}
