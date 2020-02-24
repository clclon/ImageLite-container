
#pragma once

#if defined(__WIN32)
//
#  if !defined(_WIN32_IE)
#    define _WIN32_IE 0x0600
#  endif
#  if !defined(_WIN32_WINNT)
#    define _WIN32_WINNT 0x0502
#  endif
//
#  include <windows.h>
//
#  if __has_include(<commctrl.h>)
#    include <commctrl.h>
#    define _BUILD_GDIPLUS_ENABLE 1
#  endif
//
#  if __has_include(<gdiplus.h>)
#    include <gdiplus.h>
#    define _BUILD_GDIPLUS_ENABLE 1
#  endif
//
#endif

#if __has_include(<SDL2/SDL_rect.h>)
#  include <SDL2/SDL_rect.h>
#  define _BUILD_SDL2_ENABLE 1
#endif

#if !defined(_DEBUG)
#  define NDEBUG 1
#endif

#include <atomic>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <exception>
#include <stdexcept>
#include <functional>
#include <thread>
#include <cassert>

#define _PIXNUM 3
#define _PIXSTRUCT ImageLite::IPIX3
#define _AVX2_NOT_SUPPORT_MSG "broken assembly, support for AVX2 was not included at compile time"

#include "ImageLite.Error.h"
#include "ImageLite.Geometry.h"
#include "ImageLite.Data.h"
#include "ImageLite.Formater.h"
#include "ImageLite.RGBbuffer.h"

#if (defined(__WIN32) && defined(_BUILD_IMAGELITE_DRAW))
#  include "Draw/ImageLite.Draw.h"
#endif
#if defined(_WIN64)
#  include "MJpeg/NVJpegDecoder.h"
#  include "MJpeg.h"
#endif

