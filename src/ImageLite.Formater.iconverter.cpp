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
    void Formater::CONVERT_BMP(
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer & dst,
            uint32_t,
            uint32_t h,
            uint32_t x,
            uint32_t y,
            uint32_t,
            uint32_t wlen,
            void*) { Formater::ivtable.CONVERT_BMP_ptr(src, dst, 0U, h, x, y, 0U, wlen, nullptr); }

    void Formater::CONVERT_TO_NOPAD(
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer & dst,
            uint32_t w,
            uint32_t,
            uint32_t x,
            uint32_t y,
            uint32_t,
            uint32_t wlen,
            void*) { Formater::ivtable.CONVERT_TO_NOPAD_ptr(src, dst, w, 0U, x, y, 0U, wlen, nullptr); }

    void Formater::CONVERT_FROM_NOPAD(
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer & dst,
            uint32_t w,
            uint32_t,
            uint32_t x,
            uint32_t y,
            uint32_t,
            uint32_t wlen,
            void*) { Formater::ivtable.CONVERT_FROM_NOPAD_ptr(src, dst, w, 0U, x, y, 0U, wlen, nullptr); }

    void Formater::CONVERT_TO_SEPIA(
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer & dst,
            uint32_t,
            uint32_t,
            uint32_t x,
            uint32_t y,
            uint32_t,
            uint32_t wlen,
            void*) { Formater::ivtable.CONVERT_TO_SEPIA_ptr(src, dst, 0U, 0U, x, y, 0U, wlen, nullptr); }

    void Formater::CONVERT_TO_GREY(
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer & dst,
            uint32_t,
            uint32_t,
            uint32_t x,
            uint32_t y,
            uint32_t,
            uint32_t wlen,
            void*) { Formater::ivtable.CONVERT_TO_GREY_ptr(src, dst, 0U, 0U, x, y, 0U, wlen, nullptr); }

    void Formater::CONVERT_TO_BW(
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer & dst,
            uint32_t,
            uint32_t,
            uint32_t x,
            uint32_t y,
            uint32_t,
            uint32_t wlen,
            void*) { Formater::ivtable.CONVERT_TO_BW_ptr(src, dst, 0U, 0U, x, y, 0U, wlen, nullptr); }

    void Formater::STREAM_LUA_TABLE(
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer & dst, // not used
            uint32_t,
            uint32_t,
            uint32_t x,
            uint32_t y,
            uint32_t,
            uint32_t wlen,
            void    *userdata) { Formater::ivtable.STREAM_LUA_TABLE_ptr(src, dst, 0U, 0U, x, y, 0U, wlen, userdata); }
    ///

    bool Formater::iconvert(
            std::function<ImageLite::icovert_cb> fun,
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer & dst,
            uint32_t w,
            uint32_t h,
            uint32_t stride,
            uint32_t wpad,
            void *userdata)
    {
        if (!src.size())
            return false;

        uint32_t y,
                 wlen = (w * _PIXNUM) + wpad;

        assert((wlen % 4) == 0);
        dst.resize(h * wlen);

#       pragma omp parallel for private(y) schedule(static)
        for(y = 0; y < h; y++)
            for(uint32_t x = 0; x < w; x++)
            {
                fun (src, dst, w, h, x, y, stride, wlen, userdata);
            }
        return true;
    }

    bool Formater::igetrectangle(
            ImageLite::ImageData const & src,
            ImageLite::ImageData & dst,
            ImageLite::IRECT<int32_t> const & r)
    {
        if (!src.buffer.size())
            return false;
        if ((!r.w) || (!r.h))
            return false;

        ImageLite::IRECT<uint32_t>  drect;
        ImageLite::IPOINT<uint32_t> spoint;
        drect.set(r);
        spoint.set(src.point);

        uint32_t y,
                 swpad = (spoint.x % 4),
                 swlen = ((spoint.x * _PIXNUM) + swpad),
                 dwpad = (drect.w % 4),
                 dwlen = ((drect.w * _PIXNUM) + dwpad),
                 dyend = (drect.y + drect.h),
                 dxend = (drect.x + drect.w);

        assert((swlen % 4) == 0);
        assert((dwlen % 4) == 0);

        dst.buffer.resize(drect.h * dwlen);
        dst.point.set<uint32_t>(drect.w, drect.h);
        dst.stride = ((dwpad) ? static_cast<int32_t>(dwlen) : 0);

#       pragma omp parallel for private(y) schedule(static)
        for(y = 0; y < spoint.y; y++)
        {
            if ((y >= drect.y) && (y < dyend)) {} else continue;
            for(uint32_t x = 0; x < spoint.x; x++)
            {
                if ((x >= drect.x) && (x < dxend)) {} else continue;
                std::size_t spos = ((y * swlen) + (x * _PIXNUM)),
                            dpos = (((y - drect.y) * dwlen) + ((x - drect.x) * _PIXNUM));

                _PIXSTRUCT *pix = reinterpret_cast<_PIXSTRUCT*>(
                        const_cast<uint8_t*>(&src.buffer[spos])
                    );
                ::memcpy(&dst.buffer[dpos], pix, sizeof(*pix));
            }
        }
        return true;
    }

    bool Formater::iresize(
            ImageLite::ImageData const & src,
            ImageLite::ImageData & dst,
            ImageLite::IPOINT<int32_t> const & p)
    {
        if (!src.buffer.size())
            return false;
        if ((!p.x) || (!p.y))
            return false;

        (void)dst;

        // TODO (clanc#1#17.10.2019): write latter

        return true;
    }

    bool Formater::itostream(
            std::function<ImageLite::icovert_cb> fun,
            ImageLite::ImgBuffer const & src,
            uint32_t w,
            uint32_t h,
            uint32_t wpad,
            std::ofstream & s)
    {
        if (!src.size())
            return false;

        uint32_t y,
                 wlen = (w * _PIXNUM) + wpad;

        assert((wlen % 4) == 0);
        ImageLite::ImgBuffer btmp;

        for(y = 0; y < h; y++)
            for(uint32_t x = 0; x < w; x++)
            {
                fun (src, btmp, w, h, x, y, 0, wlen, static_cast<void*>(&s));
            }
        return true;
    }

}
