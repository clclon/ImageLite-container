#ifndef HEADER_4AB01E24444A7446
#define HEADER_4AB01E24444A7446


#pragma once

#if !defined(_WIN32_IE)
#  define _WIN32_IE 0x0600
#endif

#if !defined(_WIN32_WINNT)
#  define _WIN32_WINNT 0x0602
#endif

#if !defined(DLL_EXPORT)
#  if (defined(_BUILD_DLL) || defined(_USRDLL))
#    define DLL_EXPORT __declspec(dllexport)
#  elif _BUILD_FROM_DLL
#    define DLL_EXPORT __declspec(dllimport)
#  else
#    define DLL_EXPORT
#  endif
#endif

#include <atomic>
#include <string>
#include <vector>
#include <functional>

#include <windows.h>
#include <commctrl.h>

#if !defined(GWL_USERDATA)
#  define GWL_USERDATA -21
#endif

//#include "ColorDialog.h"
#include "CustomDialog.h"
//#include "FileDialog.h"
//#include "NotifyDialog.h"

#endif

