
#include "ImageLite.h"
#include <HelperIntrin.h>

namespace ImageLite
{

    /// ICOVERT
    void CONVERT_BMP_native       (ImageLite::ImgBuffer const&, ImageLite::ImgBuffer&, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void*);
    void CONVERT_TO_NOPAD_native  (ImageLite::ImgBuffer const&, ImageLite::ImgBuffer&, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void*);
    void CONVERT_FROM_NOPAD_native(ImageLite::ImgBuffer const&, ImageLite::ImgBuffer&, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void*);
    void CONVERT_TO_SEPIA_native  (ImageLite::ImgBuffer const&, ImageLite::ImgBuffer&, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void*);
    void CONVERT_TO_GREY_native   (ImageLite::ImgBuffer const&, ImageLite::ImgBuffer&, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void*);
    void CONVERT_TO_BW_native     (ImageLite::ImgBuffer const&, ImageLite::ImgBuffer&, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void*);

    void STREAM_LUA_TABLE_native  (ImageLite::ImgBuffer const&, ImageLite::ImgBuffer&, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void*);

    /// ICOMPARE
    uint32_t ICOMPARE_native (ImgBuffer const&, ImgBuffer const&, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint8_t);
    uint32_t ICOMPARE_avx2   (ImgBuffer const&, ImgBuffer const&, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint8_t);
    uint32_t ICOMPARE_neon   (ImgBuffer const&, ImgBuffer const&, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint8_t);

    static inline uint8_t __attribute__((always_inline)) pix_icompare(uint8_t spix, uint8_t dpix, uint8_t pc)
    {
        return ((spix == dpix) ?
                1U : ((pc) ? ((std::max(spix, dpix) - std::min(spix, dpix)) <= pc) : 0U)
            );
    }
    static inline uint8_t __attribute__((always_inline)) rgb_icompare(uint8_t const *src, uint8_t const *dst, uint8_t pc)
    {
        _PIXSTRUCT const *spix = reinterpret_cast<_PIXSTRUCT*>(const_cast<uint8_t*>(src));
        _PIXSTRUCT const *dpix = reinterpret_cast<_PIXSTRUCT*>(const_cast<uint8_t*>(dst));
        uint8_t rpix = 0;
        {
            rpix += pix_icompare(spix->c1, dpix->c1, pc);
            rpix += pix_icompare(spix->c2, dpix->c2, pc);
            rpix += pix_icompare(spix->c3, dpix->c3, pc);
        }
        return rpix;
    }
}
