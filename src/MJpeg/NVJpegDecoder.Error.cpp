/*
    MIT License

    Android Aremote Viewer, GUI ADB tools

    Android Viewer developed to view and control your android device from a PC.
    ADB exchange Android Viewer, support scale view, input tap from mouse,
    input swipe from keyboard, save/copy screenshot, etc..

    Copyright (c) 2016-2020 PS
    GitHub: https://github.com/ClClon/ImageLite-container

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sub license, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

#include "NVJpegDecoder.Internal.h"

namespace ImageLite
{
    namespace JpegGpu
    {
        static inline const char* l_app_error[] =
        {
            "",              // ErrorId::error_begin
            "nvjpeg status not initialized",
            "nvjpeg status invalid parameter",
            "nvjpeg status bad jpeg",
            "nvjpeg status jpeg not supported",
            "nvjpeg status allocator failure",
            "nvjpeg status execution failed",
            "nvjpeg status arch mismatch",
            "nvjpeg status internal error",
            "nvjpeg status implementation not supported",
            "bad device number",
            "device is prohibited mode",
            "device does not support CUDA",
            "unknown chroma subsampling",
            "bad image parameters",
            "bad image channel",
            "imgstream error",
            "decompress jpeg error",
            "CUDA API error",
            "not file access",
            "system object is null",
            "system error exeption",
            "this build not support AVX2",
            "unknown error", // ErrorId::error_unknown
            ""               // ErrorId::error_end
        };

        template <typename T>
        T geterror_internal(int32_t e)
        {
            if ((e > ErrorId::error_begin) && (e < ErrorId::error_end))
                return l_app_error[e];
            return l_app_error[ErrorId::error_unknown];
        }

        std::string ErrorCat::message(int32_t e) const
        {
            return geterror_internal<std::string>(e);
        }

        std::string geterror(int32_t e)
        {
            return geterror_internal<std::string>(e);
        }

        const char* geterrorc(int32_t e)
        {
            return geterror_internal<const char*>(e);
        }

        const char* ErrorCat::name() const noexcept
        {
            return "NVJpegDecoder Error";
        }

        ///

        template <class T>
        T make_error_type(ErrorId e)
        {
            return { static_cast<int32_t>(e), errCat };
        }

        std::error_code make_error_code(ErrorId e)
        {
            return make_error_type<std::error_code>(e);
        }

    }
}
