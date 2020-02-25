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
        void tobuffer_native(
            ImageLite::ImgBuffer const& Rc,
            ImageLite::ImgBuffer const& Gc,
            ImageLite::ImgBuffer const& Bc,
            [[maybe_unused]] int32_t sz,
            NVJpegDecoder::gpuimage_t& param,
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

                for (int32_t x = 0, n = 0; ((x < param.width) && (n < param.pitch)); x++, n += 3)
                {
                    assert(static_cast<int32_t>(odata.size()) > (yo + n + 2));
                    int32_t const pos = (yi + x);
                    assert(sz > pos);

                    odata[yo + n + 0] = static_cast<uint8_t>(Rc[pos]);
                    odata[yo + n + 1] = static_cast<uint8_t>(Gc[pos]);
                    odata[yo + n + 2] = static_cast<uint8_t>(Bc[pos]);
                }
                if (param.pad)
                {
                    yo += param.pitch;
                    for (int32_t i = 0; i < param.pad; i++)
                        odata[yo + i] = 0U;
                }
            }
        }
    }
}

