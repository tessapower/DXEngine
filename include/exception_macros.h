#ifndef EXCEPTION_MACROS_H
#define EXCEPTION_MACROS_H

#include "app.h"

#define WIDE(x) L#x
#define W_FILE WIDE(__FILE__)

#define LAST_ERROR app::exception(W_FILE, __LINE__, GetLastError())
#define HR_ERROR(hr) app::exception(W_FILE, __LINE__, hr)

#endif // EXCEPTION_MACROS_H
