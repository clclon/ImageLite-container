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

    /// ICOMPARE NEON

#   if defined(__ANDROID__) // __ARM_NEON next
#     if (defined(_BUILD_ARMNEON86_ENABLE) || defined(_BUILD_ARMNEON_ENABLE))

    INTRIN_TARGET("mfpu=neon")
    uint32_t __attribute__((noinline)) ICOMPARE_neon(
        ImgBuffer const & src, ImgBuffer const & dst,
        uint32_t xs, uint32_t xd, uint32_t xe, uint32_t soff, uint32_t doff, uint8_t)
    {
        assert((xs % 8) == 0);

        auto summ_return = [](uint16x8_t a)
        {
            return static_cast<uint32_t>(
                vgetq_lane_u16(a, 0) +
                vgetq_lane_u16(a, 1) +
                vgetq_lane_u16(a, 2) +
                vgetq_lane_u16(a, 3) +
                vgetq_lane_u16(a, 4) +
                vgetq_lane_u16(a, 5) +
                vgetq_lane_u16(a, 6) +
                vgetq_lane_u16(a, 7)
            );
        };

        uint32_t i, spos, dpos;
        uint16x8_t  summ = vdupq_n_u16(0);

        for (i = 0U; i < xs; i += 8U)
        {
            if (__builtin_expect(!!(i >= xe), 0)) return summ_return(summ);
            spos = (soff + (i * _PIXNUM));
            dpos = (doff + ((i - xd) * _PIXNUM));
            //
            __builtin_prefetch(&src[spos] + 24, 0, 0);
            __builtin_prefetch(&dst[dpos] + 24, 0, 0);
            uint8x8x3_t vsrc = vld3_u8(&src[spos]);
            uint8x8x3_t vdst = vld3_u8(&dst[dpos]);
            //
            uint8x8_t rcmp = vrshr_n_u8(vceq_u8(vsrc.val[0], vdst.val[0]), 8);
            uint8x8_t gcmp = vrshr_n_u8(vceq_u8(vsrc.val[1], vdst.val[1]), 8);
            uint8x8_t bcmp = vrshr_n_u8(vceq_u8(vsrc.val[2], vdst.val[2]), 8);
            //
            summ = vaddw_u8 (summ, rcmp);
            summ = vaddw_u8 (summ, gcmp);
            summ = vaddw_u8 (summ, bcmp);
            //
        }
        return summ_return(summ);
    }
#     endif
#   endif

}
