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

#include "ImageLite.h"
#include <HelperIntrin.h>

#if defined(_BUILD_DLL)
#   if (defined(_BUILD_AVX2) || defined(__AVX2__))
namespace ImageLite
{
    namespace MJpegAVX2
    {
        void initavx2_internal();
    }
    namespace JpegGpuAVX2
    {
        void initavx2_internal();
    }
}
#   endif

BOOL APIENTRY DllMain(HMODULE, DWORD id, LPVOID)
{
    switch (id)
    {
        case DLL_PROCESS_ATTACH:
        {
#           if (defined(_BUILD_AVX2) || defined(__AVX2__))
            if (Helper::intrin.getcpuid() == Helper::Intrin::CpuId::CPU_AVX2)
            {
                ImageLite::MJpegAVX2::initavx2_internal();
                ImageLite::JpegGpuAVX2::initavx2_internal();
            }
#           endif
            break;
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
#endif
