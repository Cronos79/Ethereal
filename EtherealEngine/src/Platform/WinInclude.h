#pragma once

// Avoid min/max macros that conflict with std::min/max
#ifndef NOMINMAX
#define NOMINMAX
#endif

// Lean and mean Windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Prevent inclusion of rarely-used stuff from Windows headers
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

// Optional: prevent GDI definitions (if you're using DirectX or a custom renderer)
#ifndef NOGDI
#define NOGDI
#endif

// Optional: prevent obsolete APIs
#ifndef NOKERNEL
#define NOKERNEL
#endif

// Now safe to include Windows
#include <windows.h>