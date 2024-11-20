#include "stdafx.h"
#include "Graphics.h"

renderer::renderer(const app& app) : h_wnd_(app.h_wnd()) {
  // First create a factory, this lets us create Direct2D resources
  HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory_);

  if (FAILED(hr)) {
    // throw some error
  }

  // Create Render Target
  // First we need the size of the drawing area
  GetClientRect(app.h_wnd(), &client_rect_);

  // Then we create a render target
  hr = d2d_factory_->CreateHwndRenderTarget(
    D2D1::RenderTargetProperties(), // Rendering options, px format, DPI info, etc
    D2D1::HwndRenderTargetProperties(
      app.h_wnd(),
      D2D1::SizeU(
        client_rect_.right - client_rect_.left,
        client_rect_.bottom - client_rect_.top
      )
    ),
    &render_target_
  );

  if (FAILED(hr)) {
    // throw some error
  }

  hr = render_target_->CreateSolidColorBrush(
    D2D1::ColorF(D2D1::ColorF::Red),
    &red_brush_
  );

  if (FAILED(hr)) {
    // throw some error
  }
}

renderer::~renderer() {
  safe_release(&red_brush_);
  safe_release(&render_target_);
  safe_release(&d2d_factory_);
}

void renderer::start_frame() const {
  render_target_->BeginDraw();
  render_target_->DrawRectangle(
    D2D1::RectF(
      static_cast<float>(client_rect_.left) + 100.0f,
      static_cast<float>(client_rect_.top) + 100.0f,
      static_cast<float>(client_rect_.right) - 100.0f,
      static_cast<float>(client_rect_.bottom) - 100.0f),
    red_brush_);
}

void renderer::end_frame() const {
  HRESULT hr = render_target_->EndDraw();
  
  if (FAILED(hr)) {
    // throw an error here or handle needing to recreate resources
    // D2DERR_RECREATE_TARGET
    // 
  }
}
