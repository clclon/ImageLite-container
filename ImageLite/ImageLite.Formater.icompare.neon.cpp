
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
