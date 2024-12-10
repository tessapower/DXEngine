#ifndef RENDERER_H
#define RENDERER_H
#include <d3d11.h>

// Data
inline ID3D11Device* g_p_d3d_device = nullptr;
inline ID3D11DeviceContext* g_p_d3d_device_context = nullptr;
inline IDXGISwapChain* g_p_swap_chain = nullptr;
inline bool g_swap_chain_occluded = false;
inline UINT g_resize_width = 0, g_resize_height = 0;
inline ID3D11RenderTargetView* g_main_render_target_view = nullptr;

auto create_device_d3d(HWND h_wnd) -> bool;
auto cleanup_device_d3d() -> void;
auto create_render_target() -> void;
auto cleanup_render_target() -> void;
auto init_backends(HWND h_wnd) -> void;
auto shut_down() -> void;

#endif // RENDERER_H
