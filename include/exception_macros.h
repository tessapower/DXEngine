#ifndef EXCEPTION_MACROS_H
#define EXCEPTION_MACROS_H
#include "stdafx.h"

#include "app.h"
#include "renderer.h"

// Retrieves the last HRESULT error code from the OS and throws an exception
#define APP_LAST_ERROR app::exception(__FILE__, __LINE__, GetLastError())

// `HRESULT hr` should exist in the local scope for these macros to work.
// If using dxgi_info_manager to retrieve messages,
// `dxgi_info_manager dxgi_info_mgr_` should exist in the local scope before
// the call to the function that throws the exception.
#define HR_EXCEPT(hr) hr_exception(__FILE__, __LINE__, (hr))

#define HR_EXCEPT_INFO(hr) \
  hr_exception(__FILE__, __LINE__, (hr), dxgi_info_mgr_.messages())

#define HR_THROW_ON_FAIL(hr_call) \
  if (FAILED(hr = (hr_call))) throw HR_EXCEPT(hr)

#define HR_THROW_INFO(hr_call) \
  dxgi_info_mgr_.set();        \
  if (FAILED(hr = (hr_call))) throw HR_EXCEPT_INFO(hr)

// Rendering Exceptions Macros
#define RENDER_EXCEPT(hr) \
  renderer::exception(__FILE__, __LINE__, (hr))

#define RENDER_EXCEPT_INFO(hr) \
  renderer::exception(__FILE__, __LINE__, (hr), dxgi_info_mgr_.messages())

#define RENDER_DEVICE_REMOVED_EXCEPT(hr) \
  renderer::device_removed_exception(__FILE__, __LINE__, (hr))

// Rendering failure Macros
#define RENDER_THROW_ON_FAIL(hr_call) \
  if (FAILED(hr = (hr_call))) throw RENDER_EXCEPT(hr)

#define RENDER_THROW_INFO(hr_call) \
  dxgi_info_mgr_.set();               \
  if (FAILED(hr = (hr_call))) throw RENDER_EXCEPT_INFO(hr)

#endif // EXCEPTION_MACROS_H
