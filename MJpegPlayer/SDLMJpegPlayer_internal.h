#ifndef HEADER_6E4028AEC121830D
#define HEADER_6E4028AEC121830D

#pragma once

#if defined(__WIN32__)
#  if !defined(_WIN32_WINDOWS)
#    define _WIN32_WINDOWS 1
#  endif
#  if !defined(_WIN32_IE)
#    define _WIN32_IE 0x0600
#  endif
#  if !defined(_WIN32_WINNT)
#    define _WIN32_WINNT 0x0602
#  endif
#endif

#if defined __cplusplus
#  include <atomic>
#  include <algorithm>
#  include <memory>
#  include <vector>
#  include <map>
#  include <thread>
#  include <shared_mutex>
#  include <string>
#  include <string_view>
#  include <future>
#  include <functional>
#  include <chrono>
#  include <tuple>

#  include <iostream>
#  include <fstream>

#  include "HelperHttp.h"
#  include "HelperSDL2.h"
#  include <Commctrl.h>
#  include <ImageLite.h>
#  include <CustomDialog.h>

#else
#  include <windows.h>
#  include <commctrl.h>
#endif

#include "resource/resource.h"
#include "version.h"

#endif

