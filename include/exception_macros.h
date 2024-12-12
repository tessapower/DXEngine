#ifndef EXCEPTION_MACROS_H
#define EXCEPTION_MACROS_H

#include "app.h"

#define WIDE(x) L#x
#define W_FILE WIDE(__FILE__)

#define LAST_ERROR app::exception(W_FILE, __LINE__, GetLastError())

// HRESULT hr should exist in the local scope for these macros to work
#define HR_ERROR(hr) app::exception(W_FILE, __LINE__, hr)

#define RNDR_EXCEPT(hr) renderer::hr_exception(W_FILE, __LINE__, (hr))
#define RNDR_THROW(hr_call) if (FAILED(hr = (hr_call))) throw RNDR_EXCEPT(hr)
#define RNDR_DEVICE_REMOVED_EXCEPT(hr) \
  renderer::device_remove_exception(W_FILE, __LINE__, (hr))

#endif // EXCEPTION_MACROS_H
