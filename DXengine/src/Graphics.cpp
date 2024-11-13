#include "Graphics.h"

Graphics::Graphics(app& app) : _hWnd(app.h_wnd()) {
  // First create a factory, this lets us create Direct2D resources
  HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_d2dFactory);

  if (FAILED(hr)) {
    // throw some error
  }

  // Create Render Target
  // First we need the size of the drawing area
  GetClientRect(app.h_wnd(), &_clientRect);

  // Then we create a render target
  hr = _d2dFactory->CreateHwndRenderTarget(
    D2D1::RenderTargetProperties(), // Rendering options, px format, DPI info, etc
    D2D1::HwndRenderTargetProperties(
      app.h_wnd(),
      D2D1::SizeU(
        _clientRect.right - _clientRect.left,
        _clientRect.bottom - _clientRect.top
      )
    ),
    &_renderTarget
  );

  if (FAILED(hr)) {
    // throw some error
  }

  _renderTarget->CreateSolidColorBrush(
    D2D1::ColorF(D2D1::ColorF::Red),
    &_redBrush
  );
}

Graphics::~Graphics() {
  SafeRelease(&_redBrush);
  SafeRelease(&_renderTarget);
  SafeRelease(&_d2dFactory);
}

void Graphics::startFrame() {
  _renderTarget->BeginDraw();
  _renderTarget->DrawRectangle(
    D2D1::RectF(
      _clientRect.left + 100.0f,
      _clientRect.top + 100.0f,
      _clientRect.right - 100.0f,
      _clientRect.bottom - 100.0f),
    _redBrush);
}

void Graphics::endFrame() {
  HRESULT hr = _renderTarget->EndDraw();
  
  if (FAILED(hr)) {
    // throw an error here or handle needing to recreate resources
    // D2DERR_RECREATE_TARGET
    // 
  }
}
