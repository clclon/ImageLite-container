
#include "ImageLite.Formater.internal.h"

namespace ImageLite
{
    void __attribute__((noinline)) CONVERT_BMP_native(
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer & dst,
            uint32_t,
            uint32_t h,
            uint32_t x,
            uint32_t y,
            uint32_t,
            uint32_t wlen,
            void*)
    {
        std::size_t spos = ((y * wlen) + (x * _PIXNUM)),
                    dpos = (((h - y - 1) * wlen) + (x * _PIXNUM));

        assert(static_cast<int32_t>(spos) >= 0);
        assert(static_cast<int32_t>(dpos) >= 0);

        _PIXSTRUCT *pix = reinterpret_cast<_PIXSTRUCT*>(
                        const_cast<uint8_t*>(&src[spos])
                    );
        dst[dpos + 0] = pix->c3;
        dst[dpos + 1] = pix->c2;
        dst[dpos + 2] = pix->c1;
    }

    void __attribute__((noinline)) CONVERT_TO_NOPAD_native(
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer & dst,
            uint32_t w,
            uint32_t,
            uint32_t x,
            uint32_t y,
            uint32_t,
            uint32_t wlen,
            void*)
    {
        std::size_t spos = ((y * wlen) + (x * _PIXNUM)),
                    dpos = ((y * (w * _PIXNUM)) + (x * _PIXNUM));

        assert(static_cast<int32_t>(spos) >= 0);
        assert(static_cast<int32_t>(dpos) >= 0);

        _PIXSTRUCT *pix = reinterpret_cast<_PIXSTRUCT*>(
                        const_cast<uint8_t*>(&src[spos])
                    );
        dst[dpos + 0] = pix->c1;
        dst[dpos + 1] = pix->c2;
        dst[dpos + 2] = pix->c3;
    }

    void __attribute__((noinline)) CONVERT_FROM_NOPAD_native(
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer & dst,
            uint32_t w,
            uint32_t,
            uint32_t x,
            uint32_t y,
            uint32_t,
            uint32_t wlen,
            void*)
    {
        std::size_t spos = ((y * (w * _PIXNUM)) + (x * _PIXNUM)),
                    dpos = ((y * wlen) + (x * _PIXNUM));

        assert(static_cast<int32_t>(spos) >= 0);
        assert(static_cast<int32_t>(dpos) >= 0);

        _PIXSTRUCT *pix = reinterpret_cast<_PIXSTRUCT*>(
                        const_cast<uint8_t*>(&src[spos])
                    );
        dst[dpos + 0] = pix->c1;
        dst[dpos + 1] = pix->c2;
        dst[dpos + 2] = pix->c3;
    }

    void __attribute__((noinline)) CONVERT_TO_SEPIA_native(
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer & dst,
            uint32_t,
            uint32_t,
            uint32_t x,
            uint32_t y,
            uint32_t,
            uint32_t wlen,
            void*)
    {
        std::size_t pos = ((y * wlen) + (x * _PIXNUM));

        assert(static_cast<int32_t>(pos) >= 0);

        _PIXSTRUCT *spix = reinterpret_cast<_PIXSTRUCT*>(
                        const_cast<uint8_t*>(&src[pos])
                    );
        _PIXSTRUCT *dpix = reinterpret_cast<_PIXSTRUCT*>(
                        const_cast<uint8_t*>(&dst[pos])
                    );

        uint32_t rm = ((spix->c1 * 402U + spix->c2 * 787U + spix->c3 * 194U) >> 10);
        uint32_t gm = ((spix->c1 * 357U + spix->c2 * 702U + spix->c3 * 172U) >> 10);
        uint32_t bm = ((spix->c1 * 279U + spix->c2 * 547U + spix->c3 * 134U) >> 10);

        dpix->c1 = static_cast<uint8_t>((rm > 255U) ? 255U : rm);
        dpix->c2 = static_cast<uint8_t>((gm > 255U) ? 255U : gm);
        dpix->c3 = static_cast<uint8_t>((bm > 255U) ? 255U : bm);
    }

    ///
    void __attribute__((noinline)) CONVERT_TO_GREY_native(
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer & dst,
            uint32_t,
            uint32_t,
            uint32_t x,
            uint32_t y,
            uint32_t,
            uint32_t wlen,
            void*)
    {
        std::size_t pos = ((y * wlen) + (x * _PIXNUM));

        assert(static_cast<int32_t>(pos) >= 0);

        _PIXSTRUCT *spix = reinterpret_cast<_PIXSTRUCT*>(
                        const_cast<uint8_t*>(&src[pos])
                    );
        _PIXSTRUCT *dpix = reinterpret_cast<_PIXSTRUCT*>(
                        const_cast<uint8_t*>(&dst[pos])
                    );

        uint8_t grey = static_cast<uint8_t>(
                0.1 * spix->c1 + 0.6 * spix->c2 + 0.3 * spix->c3
            );
        dpix->c1 = dpix->c2 = dpix->c3 = grey;
    }

    void __attribute__((noinline)) CONVERT_TO_BW_native(
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer & dst,
            uint32_t,
            uint32_t,
            uint32_t x,
            uint32_t y,
            uint32_t,
            uint32_t wlen,
            void*)
    {
        std::size_t pos = ((y * wlen) + (x * _PIXNUM));

        assert(static_cast<int32_t>(pos) >= 0);

        _PIXSTRUCT *spix = reinterpret_cast<_PIXSTRUCT*>(
                        const_cast<uint8_t*>(&src[pos])
                    );
        _PIXSTRUCT *dpix = reinterpret_cast<_PIXSTRUCT*>(
                        const_cast<uint8_t*>(&dst[pos])
                    );

        uint8_t bw = static_cast<uint8_t>(
                ((spix->c1 + spix->c2 + spix->c3) > 382) ? 255U : 0U
            );
        dpix->c1 = dpix->c2 = dpix->c3 = bw;
    }

    ///

    void __attribute__((noinline)) STREAM_LUA_TABLE_native(
            ImageLite::ImgBuffer const & src,
            ImageLite::ImgBuffer&,
            uint32_t,
            uint32_t,
            uint32_t x,
            uint32_t y,
            uint32_t,
            uint32_t wlen,
            void    *userdata)
    {
        if (!userdata)
            return;

        std::ofstream *s = reinterpret_cast<std::ofstream*>(userdata);
        std::size_t spos = ((y * wlen) + (x * _PIXNUM));
        assert(static_cast<int32_t>(spos) >= 0);

        _PIXSTRUCT *pix = reinterpret_cast<_PIXSTRUCT*>(
                const_cast<uint8_t*>(&src[spos])
            );
        *s  << "\t\t{ "
            << static_cast<int16_t>(pix->c1) << ", "
            << static_cast<int16_t>(pix->c2) << ", "
            << static_cast<int16_t>(pix->c3) << " },\n";
    }
}
