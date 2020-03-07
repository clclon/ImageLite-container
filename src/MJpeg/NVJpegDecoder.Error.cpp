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
#include <map>
#include <HelperMap.h>

namespace ImageLite
{
    namespace JpegGpu
    {
        Helper::CatalogMap l_mjpg_error = Helper::CatalogMapInit
            (error_begin, "")
            (error_nvjpeg_not_initialized, "nvjpeg status not initialized")
            (error_nvjpeg_invalid_parameter, "nvjpeg status invalid parameter")
            (error_nvjpeg_bad_jpeg, "nvjpeg status bad jpeg")
            (error_nvjpeg_jpeg_not_supported, "nvjpeg status jpeg not supported")
            (error_nvjpeg_allocator_failure, "nvjpeg status allocator failure")
            (error_nvjpeg_execution_failed, "nvjpeg status execution failed")
            (error_nvjpeg_arch_mismatch, "nvjpeg status arch mismatch")
            (error_nvjpeg_internal_error, "nvjpeg status internal error")
            (error_nvjpeg_implementation_not_supported, "nvjpeg status implementation not supported")
            (error_bad_device_number, "bad device number")
            (error_bad_device_mode, "device is prohibited mode")
            (error_device_not_support, "device does not support CUDA")
            (error_unknown_chroma, "unknown chroma subsampling")
            (error_bad_image, "bad image parameters")
            (error_bad_channel, "bad image channel")
            (error_bad_imgstream, "imgstream error")
            (error_decompress, "decompress jpeg error")
            (error_cuda_api, "CUDA API error")
            (error_file_access, "not file access")
            (error_sys_objectnull, "system object is null")
            (error_sys_execpt, "system error exeption")
            (error_not_support_avx, "this build not support AVX2")
            (error_unknown, "error unknown")
            (error_end, "");

        std::string ErrorCat::message(int32_t e) const
        {
            if ((e < ErrorId::error_nvjpeg_not_initialized) || (e > ErrorId::error_nvjpeg_implementation_not_supported))
                if ((e < ErrorId::error_bad_device_number) || (e > ErrorId::error_unknown))
                    e = ErrorId::error_unknown;

            auto r = l_mjpg_error.find(e);
            if (r == l_mjpg_error.end())
                return "";
            return r->second;
        }

        const char* ErrorCat::name() const noexcept
        {
            return "NVJpegDecoder Error";
        }

        ///

        std::error_code make_error_code(ErrorId e)
        {
            return { static_cast<int32_t>(e), errCat };
        }

    }
}
