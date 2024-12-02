#include "stdafx.h"
#include <d3d11.h>
#include <tchar.h>

#include "app.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "renderer.h"

// Forward declarations of helper functions
auto WINAPI wnd_proc(HWND h_wnd, UINT msg, WPARAM w_param,
                     LPARAM l_param) -> LRESULT;

auto main(int, char**) -> int {
  // Create a window
  const app app(1280, 800, L"DX Engine");

  // Show the window
  app.show();

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplWin32_Init(app.h_wnd());
  ImGui_ImplDX11_Init(g_pd3d_device, g_pd3d_device_context);

  // Our state
  auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
  bool done = false;
  while (!done) {
    // Poll and handle messages (inputs, window resize, etc.)
    // See the WndProc() function below for our to dispatch events to the Win32
    // backend.
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      if (msg.message == WM_QUIT) done = true;
    }
    if (done) break;

    // Handle window being minimized or screen locked
    if (g_swap_chain_occluded &&
        g_p_swap_chain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
      Sleep(10);
      continue;
    }
    g_swap_chain_occluded = false;

    // Handle window resize (we don't resize directly in the WM_SIZE handler)
    if (g_resize_width != 0 && g_resize_height != 0) {
      cleanup_render_target();
      g_p_swap_chain->ResizeBuffers(0, g_resize_width, g_resize_height,
                                    DXGI_FORMAT_UNKNOWN, 0);
      g_resize_width = g_resize_height = 0;
      create_render_target();
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    {
      ImGui::Begin("Shaders");
      ImGui::Text(
          "This window will display a list of shaders that \nare dynamically "
          "loaded and applied to the scene.");

      ImGui::Spacing();

      ImGui::ColorEdit3(
          "clear color",
          reinterpret_cast<float*>(
              &clear_color));  // Edit 3 floats representing a color

      ImGui::Spacing();

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / io.Framerate, io.Framerate);
      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    const float clear_color_with_alpha[4] = {
        clear_color.x * clear_color.w, clear_color.y * clear_color.w,
        clear_color.z * clear_color.w, clear_color.w};
    g_pd3d_device_context->OMSetRenderTargets(1, &g_main_render_target_view,
                                              nullptr);
    g_pd3d_device_context->ClearRenderTargetView(g_main_render_target_view,
                                                 clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present
    const HRESULT hr = g_p_swap_chain->Present(1, 0);  // Present with vsync
    // HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
    g_swap_chain_occluded = (hr == DXGI_STATUS_OCCLUDED);
  }

  // Cleanup
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  return EXIT_SUCCESS;
}

// Helper functions

// Forward declare message handler from imgui_impl_win32.cpp
extern auto ImGui_ImplWin32_WndProcHandler(HWND h_wnd, UINT msg, WPARAM w_param,
                                           LPARAM l_param) -> LRESULT;

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if
// dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your
// main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to
// your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from
// your application based on those two flags.
auto WINAPI wnd_proc(const HWND h_wnd, const UINT msg, const WPARAM w_param,
                     const LPARAM l_param) -> LRESULT {
  if (ImGui_ImplWin32_WndProcHandler(h_wnd, msg, w_param, l_param)) return true;

  switch (msg) {
    case WM_SIZE:
      if (w_param == SIZE_MINIMIZED) return 0;
      g_resize_width = static_cast<UINT>(LOWORD(l_param));  // Queue resize
      g_resize_height = static_cast<UINT>(HIWORD(l_param));
      return 0;
    case WM_SYSCOMMAND:
      if ((w_param & 0xfff0) == SC_KEYMENU)  // Disable ALT application menu
        return 0;
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    default:
      break;
  }

  return DefWindowProcW(h_wnd, msg, w_param, l_param);
}
