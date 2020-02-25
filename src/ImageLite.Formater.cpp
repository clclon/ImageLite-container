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

#include "ImageLite.Formater.internal.h"

namespace ImageLite
{
    static Formater::imgVtable initvt()
    {
        ImageLite::Formater::icovert_ptr  CONVERT_BMP_ptr        = &CONVERT_BMP_native;
        ImageLite::Formater::icovert_ptr  CONVERT_TO_NOPAD_ptr   = &CONVERT_TO_NOPAD_native;
        ImageLite::Formater::icovert_ptr  CONVERT_FROM_NOPAD_ptr = &CONVERT_FROM_NOPAD_native;
        ImageLite::Formater::icovert_ptr  CONVERT_TO_SEPIA_ptr   = &CONVERT_TO_SEPIA_native;
        ImageLite::Formater::icovert_ptr  CONVERT_TO_GREY_ptr    = &CONVERT_TO_GREY_native;
        ImageLite::Formater::icovert_ptr  CONVERT_TO_BW_ptr      = &CONVERT_TO_BW_native;
        ImageLite::Formater::icovert_ptr  STREAM_LUA_TABLE_ptr   = &STREAM_LUA_TABLE_native;
        ImageLite::Formater::icompare_ptr ICOMPARE_ptr           = &ICOMPARE_native;

        switch(Helper::intrin.getcpuid())
        {
#           if (defined(_BUILD_AVX2) || defined(__AVX2__))
            case Helper::Intrin::CpuId::CPU_AVX2:
                {
                    ICOMPARE_ptr = &ICOMPARE_avx2;
                    break;
                }
#           endif
#           if defined(__ANDROID__) //__ARM_NEON
#             if (defined(_BUILD_ARMNEON86_ENABLE) || defined(_BUILD_ARMNEON_ENABLE))
            case Helper::Intrin::CpuId::CPU_ARM_NEON_FMA:
            case Helper::Intrin::CpuId::CPU_ARM_NEON:
                {
                    ICOMPARE_ptr = &ICOMPARE_neon;
                    break;
                }
#             endif
#           endif
            default:
                break;
       }
       return
       {
            CONVERT_BMP_ptr,
            CONVERT_TO_NOPAD_ptr,
            CONVERT_FROM_NOPAD_ptr,
            CONVERT_TO_SEPIA_ptr,
            CONVERT_TO_GREY_ptr,
            CONVERT_TO_BW_ptr,
            STREAM_LUA_TABLE_ptr,
            ICOMPARE_ptr
        };
    }

    /// Virtual table to CPU features dispatcher
    Formater::imgVtable Formater::ivtable = initvt();
    ///
}
