#ifndef WINDOW_EXCEPTION_MACROS_H
#define WINDOW_EXCEPTION_MACROS_H

#include "Window.h"

#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)
#define WFILE WIDE1(__FILE__)

#define LAST_ERROR Window::Exception(WFILE, __LINE__, GetLastError())
#define HR_ERROR(hr) Window::Exception(WFILE, __LINE__, hr)

#endif // WINDOW_EXCEPTION_MACROS_H
