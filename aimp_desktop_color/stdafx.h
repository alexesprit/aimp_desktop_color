// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <VersionHelpers.h>
#include <functional>
#include "dwmapi.h"

#include "comtypes.h"

typedef std::function<void()> EventCallback;
typedef std::function<bool(DWORD, int, void*)> CoreMessageCallback;
