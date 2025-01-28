#ifndef EXCEPTION_MACROS_H
#define EXCEPTION_MACROS_H
#include "stdafx.h"

#include "renderer/renderer.h"

// Retrieves the last HRESULT error code from the OS and throws an exception
#define LAST_ERROR hr_exception(__FILE__, __LINE__, GetLastError())

// `HRESULT hr` should exist in the local scope for these macros to work.
// If using dxgi_info_manager to retrieve messages,
// `dxgi_info_manager dxgi_info_mgr_` should exist in the local scope before
// the call to the function that throws the exception.

// HR Exceptions
#define HR_EXCEPT(hr) hr_exception(__FILE__, __LINE__, (hr))

#define HR_EXCEPT_MSG(hr) \
  hr_exception(__FILE__, __LINE__, (hr), dxgi_info_mgr_.messages())

// HR throw calls

// Throws an exception if the HR call fails. `HRESULT hr` should exist in the local scope.
#define HR_THROW_ON_FAIL(hr_call) \
  if (FAILED(hr = (hr_call))) throw HR_EXCEPT(hr)

// Throws an exception if the HR call fails, and includes the messages from the
// `dxgi_info_manager dxgi_info_mgr_`, which it expects is in the local scope.
// `HRESULT hr` should also exist in the local scope.
#define HR_THROW_MSG(hr_call) \
  dxgi_info_mgr_.set();        \
  if (FAILED(hr = (hr_call))) throw HR_EXCEPT_MSG(hr)

// Rendering Exceptions
#define RENDER_EXCEPT(hr) \
  renderer::exception(__FILE__, __LINE__, (hr))

#define RENDER_EXCEPT_MSG(hr) \
  renderer::exception(__FILE__, __LINE__, (hr), dxgi_info_mgr_.messages())

#define RENDER_DEVICE_REMOVED_EXCEPT(hr) \
  renderer::device_removed_exception(__FILE__, __LINE__, (hr))

// Rendering throw calls

// Throws an exception if the HR call fails.
// Expects `HRESULT hr` to be in the local scope.
#define RENDER_THROW_HR(hr_call) \
  if (FAILED(hr = (hr_call))) throw RENDER_EXCEPT(hr)

// Throws an exception if the HR call fails.
// Expects `HRESULT hr` and `dxgi_info_manager dxgi_info_mgr_` to be in the local scope.
#define RENDER_THROW_HR_MSG(hr_call) \
  dxgi_info_mgr_.set();              \
  if (FAILED(hr = (hr_call))) throw RENDER_EXCEPT_MSG(hr)

// Info only throw calls

// Throws an exception for failed function calls that don't return an HRESULT value.
#define THROW_MSG(call)                            \
  dxgi_info_mgr_.set();                            \
  (call);                                          \
  {                                                \
    auto v = dxgi_info_mgr_.messages();            \
    if (!v.empty()) {                              \
      throw info_exception(__FILE__, __LINE__, v); \
    }                                              \
  }

#endif // EXCEPTION_MACROS_H
