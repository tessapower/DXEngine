#ifndef EXCEPTION_MACROS_H
#define EXCEPTION_MACROS_H
#include "stdafx.h"

#include "app.h"
#include "renderer.h"

// Retrieves the last HRESULT error code from the OS and throws an exception
#define APP_LAST_ERROR app::exception(__FILE__, __LINE__, GetLastError())

// `HRESULT hr` should exist in the local scope for these macros to work
#define HR_EXCEPT(hr) hr_exception(__FILE__, __LINE__, (hr))
#define HR_THROW_FAILED(hr_call) \
  if (FAILED(hr = (hr_call))) throw HR_EXCEPT(hr)

// Rendering Exceptions
#define RENDER_EXCEPT(hr) renderer::exception(__FILE__, __LINE__, (hr))
#define RENDER_THROW_FAILED(hr_call) if (FAILED(hr = (hr_call))) throw RENDER_EXCEPT(hr)
#define RENDER_DEVICE_REMOVED_EXCEPT(hr) \
  renderer::device_removed_exception(__FILE__, __LINE__, (hr))

#endif // EXCEPTION_MACROS_H
