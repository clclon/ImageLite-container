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

#define color_shift(S,M) \
    _mm256_extracti128_si256(_mm256_shuffle_epi8(S,M), 0)

#define color_splice(R,G,B) \
    _mm_add_epi8(_mm_add_epi8(R,G),B)

enum maskid
{
    maskR1 = 0,
    maskR2,
    maskR3,
    maskG1,
    maskG2,
    maskG3,
    maskB1,
    maskB2,
    maskB3
};

namespace ImageLite
{
    namespace JpegGpuAVX2
    {
#   if defined(__AVX2__)
#       if !defined(_BUILD_DLL)
        static bool isavx2init = false;;
#       endif
        static __m256i masks[9]{};
        //
        void __attribute__((noinline)) initavx2_internal()
        {
#           if !defined(_BUILD_DLL)
            if (isavx2init)
                return;
            isavx2init = true;
#           endif
            //
            masks[maskR1] = _mm256_setr_epi8(0, -1, -1, 1, -1, -1, 2, -1, -1, 3, -1, -1, 4, -1, -1, 5, -1, -1, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
            masks[maskR2] = _mm256_setr_epi8(-1, -1, 6, -1, -1, 7, -1, -1, 8, -1, -1, 9, -1, -1, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
            masks[maskR3] = _mm256_setr_epi8(-1, 11, -1, -1, 12, -1, -1, 13, -1, -1, 14, -1, -1, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
            //
            masks[maskG1] = _mm256_setr_epi8(-1, 0, -1, -1, 1, -1, -1, 2, -1, -1, 3, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
            masks[maskG2] = _mm256_setr_epi8(5, -1, -1, 6, -1, -1, 7, -1, -1, 8, -1, -1, 9, -1, -1, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
            masks[maskG3] = _mm256_setr_epi8(-1, -1, 11, -1, -1, 12, -1, -1, 13, -1, -1, 14, -1, -1, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
            //
            masks[maskB1] = _mm256_setr_epi8(-1, -1, 0, -1, -1, 1, -1, -1, 2, -1, -1, 3, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
            masks[maskB2] = _mm256_setr_epi8(-1, 5, -1, -1, 6, -1, -1, 7, -1, -1, 8, -1, -1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
            masks[maskB3] = _mm256_setr_epi8(10, -1, -1, 11, -1, -1, 12, -1, -1, 13, -1, -1, 14, -1, -1, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
            //
        }

        INTRIN_TARGET("avx2")
        void __attribute__((noinline)) tobuffer_avx2(
                ImageLite::ImgBuffer const& Rc,
                ImageLite::ImgBuffer const& Gc,
                ImageLite::ImgBuffer const& Bc,
                [[maybe_unused]] int32_t sz,
                ImageLite::JpegGpu::NVJpegDecoder::gpuimage_t& param,
                ImageLite::ImgBuffer& odata,
                std::error_code& /* ec */
            )
        {
            int32_t y;
#           pragma omp parallel for private(y) schedule(static)
            for (y = 0; y < param.height; y++)
            {
                int32_t yo = (y * (param.pitch + param.pad));
                int32_t const yi = (y * param.width);

                int32_t x, n;
                for (x = 0, n = 0; ((x < param.width) && ((n + 47) < param.pitch)); x += 16, n += 48)
                {
                    assert(static_cast<int32_t>(odata.size()) >= (yo + n + 47));
                    assert(param.width >= (x + 15));
                    int32_t const pos = (yi + x);
                    assert(sz >= (pos + 15));

                    __m256i const r = _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(&Rc[pos]));
                    __m256i const g = _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(&Gc[pos]));
                    __m256i const b = _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(&Bc[pos]));
                    _mm_storeu_si128(reinterpret_cast<__m128i*>(&odata[yo + n + 0]),  color_splice(color_shift(r, masks[maskR1]), color_shift(g, masks[maskG1]), color_shift(b, masks[maskB1])));
                    _mm_storeu_si128(reinterpret_cast<__m128i*>(&odata[yo + n + 16]), color_splice(color_shift(r, masks[maskR2]), color_shift(g, masks[maskG2]), color_shift(b, masks[maskB2])));
                    _mm_storeu_si128(reinterpret_cast<__m128i*>(&odata[yo + n + 32]), color_splice(color_shift(r, masks[maskR3]), color_shift(g, masks[maskG3]), color_shift(b, masks[maskB3])));
                }
                if ((param.width - x) > 0)
                {
                    for (; x < param.width; x++, n += 3)
                    {
                        assert(static_cast<int32_t>(odata.size()) >= (yo + n + 2));
                        int32_t const pos = (yi + x);
                        assert(sz > pos);

                        odata[yo + n + 0] = static_cast<uint8_t>(Rc[pos]);
                        odata[yo + n + 1] = static_cast<uint8_t>(Gc[pos]);
                        odata[yo + n + 2] = static_cast<uint8_t>(Bc[pos]);
                    }
                }
                if (param.pad)
                {
                    yo += param.pitch;
                    for (int32_t i = 0; i < param.pad; i++)
                        odata[yo + i] = 0U;
                }
            }
        }
#   elif !defined(__ANDROID__)
#     if defined(_BUILD_AVX2)
#       error "broken assembly, support for AVX2 was not included at compile time, remove flag '_BUILD_AVX2'"
#     endif
#   endif
    }
}

