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

    /// ICOMPARE

    uint32_t __attribute__((noinline)) ICOMPARE_native(
        ImgBuffer const & src, ImgBuffer const & dst,
        uint32_t xs, uint32_t xd, uint32_t xe, uint32_t soff, uint32_t doff, uint8_t pc)
    {
        uint32_t x, cmp = 0;
        for(x = 0; x < xs; x++)
        {
            if (__builtin_expect(!!(x >= xe), 0)) return cmp;
            uint32_t spos = (soff + (x * _PIXNUM)),
                     dpos = (doff + ((x - xd) * _PIXNUM));

            __builtin_prefetch(&src[spos] + sizeof(_PIXSTRUCT), 0, 0);
            __builtin_prefetch(&dst[dpos] + sizeof(_PIXSTRUCT), 0, 0);
            cmp += rgb_icompare(&src[spos], &dst[dpos], pc);
        }
        return cmp;
    }

    template <typename T>
    static double ICOMPARE_(
            ImageLite::ImageData const & src,
            T const & dst,
            ImageLite::IRECT<int32_t> const & r,
            uint8_t pc)
    {
        if ((!src.buffer.size()) ||
            (!dst.buffer.size()))
            return 0.0;

        if ((!r.w)                      ||
            (!r.h)                      ||
            (!src.point.x)              ||
            (!src.point.y)              ||
            ((r.w + r.x) > src.point.x) ||
            ((r.h + r.y) > src.point.y))
            return 0.0;

        ImageLite::IPOINT<uint32_t> spoint;
        ImageLite::IRECT<uint32_t>  drect;
        drect.set(r);
        spoint.set(src.point);

        uint32_t y,
                 cmp   = 0,
                 swpad = (spoint.x % 4),
                 swlen = ((spoint.x * _PIXNUM) + swpad),
                 dwpad = (drect.w % 4),
                 dwlen = ((drect.w * _PIXNUM) + dwpad),
                 dyend = (drect.y + drect.h),
                 dxend = (drect.x + drect.w);

        assert((swlen % 4) == 0);
        assert((dwlen % 4) == 0);

        ImageLite::Formater::icompare_ptr fun;

        if (pc <= 1)
            fun = Formater::ivtable.ICOMPARE_ptr;
        else
            fun = &ICOMPARE_native;

#       pragma omp parallel for private(y) schedule(static) reduction(+:cmp)
        for(y = 0; y < spoint.y; y++)
        {
            if (__builtin_expect(!!((y >= drect.y) && (y < dyend)), 0)) {} else continue;
            uint32_t soff = (y * swlen),
                     doff = ((y - drect.y) * dwlen);

            if (__builtin_expect(!!(drect.x > 0), 0))
                for(uint32_t x = 0; x < spoint.x; x++)
                    if (__builtin_expect(!!(x < drect.x), 1)) continue;

            cmp += fun(src.buffer, dst.buffer, spoint.x, drect.x, dxend, soff, doff, pc);
        }
        if (__builtin_expect(!!(!cmp), 0))
            return 0.0;

        uint32_t dsz = ((drect.w * _PIXNUM) * drect.h);
        if (__builtin_expect(!!(cmp == dsz), 0))
            return 100.0;

        return (static_cast<double>(cmp) / (static_cast<double>(dsz) / 100.0));
    }

    double Formater::icompare(
            ImageLite::ImageData const & src,
            ImageLite::ImageData const & dst,
            ImageLite::IRECT<int32_t> const & r,
            uint8_t pc)
    {
        return ICOMPARE_<ImageLite::ImageData>(src, dst, r, pc);
    }

    double Formater::icompare(
            ImageLite::ImageData const & src,
            ImageLite::ImageDataP const & dst,
            ImageLite::IRECT<int32_t> const & r,
            uint8_t pc)
    {
        return ICOMPARE_<ImageLite::ImageDataP>(src, dst, r, pc);
    }
}
