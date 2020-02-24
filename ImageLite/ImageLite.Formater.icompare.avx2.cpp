
#include "ImageLite.Formater.internal.h"

namespace ImageLite
{

    /// ICOMPARE AVX2

#   if defined(__AVX2__)

    template <uint32_t N>
    static inline uint32_t __attribute__((always_inline)) count_mask(int32_t x)
    {
        static_assert(N == 8 || N == 16 || N == 32);

        if constexpr (N == 32)
        {
            uint32_t m;
            m  = (((x >>  0) & 0x0f) ? 4 : 0U);
            m += (((x >>  4) & 0x0f) ? 4 : 0U);
            m += (((x >>  8) & 0x0f) ? 4 : 0U);
            m += (((x >> 12) & 0x0f) ? 4 : 0U);
            m += (((x >> 16) & 0x0f) ? 4 : 0U);
            m += (((x >> 20) & 0x0f) ? 4 : 0U);
            m += (((x >> 24) & 0x0f) ? 4 : 0U);
            m += (((x >> 28) & 0x0f) ? 2 : 0U);
            return m;
        }
        if constexpr (N == 16)
        {
            auto calc_bit = [](int32_t b)
            {
                switch (b)
                {
                    case 1:
                    case 2:
                    case 4:
                    case 8:  return 1U;
                    case 3:
                    case 5:
                    case 6:
                    case 9:
                    case 10:
                    case 12: return 2U;
                    case 7:
                    case 11:
                    case 13:
                    case 14: return 3U;
                    case 15: return 4U;
                    default: return 0U;
                }
            };
            auto calc_bit_last = [](int32_t b)
            {
                switch (b)
                {
                    case 1:
                    case 2:
                    case 5:
                    case 6:
                    case 9:
                    case 10:
                    case 13:
                    case 14: return 1U;
                    case 3:
                    case 7:
                    case 11:
                    case 15: return 2U;
                    default: return 0U; /// 0,4,8,12
                }
            };
            uint32_t m;
            m  = calc_bit((x >> 0)  & 0x0f);
            m += calc_bit((x >> 4)  & 0x0f);
            m += calc_bit((x >> 8)  & 0x0f);
            m += calc_bit_last((x >> 12) & 0x0f);
            return m;
        }
        if constexpr (N == 8)
        {
            auto calc_bit = [](int32_t b)
            {
                switch (b)
                {
                    case 3:
                    case 5:
                    case 9:  return 1U;
                    case 7:
                    case 11: return 2U;
                    case 15: return 3U;
                    default: return 0U; /// 1, 0 = (v0, v1, v2, 0xFF)
                }
            };
            uint32_t m;
            m  = calc_bit((x >> 0)  & 0x0f);
            m += calc_bit((x >> 4)  & 0x0f);
            return m;
        }
    }

    INTRIN_TARGET("avx2")
    uint32_t __attribute__((noinline)) ICOMPARE_avx2(
        ImgBuffer const & src, ImgBuffer const & dst,
        uint32_t xs, uint32_t xd, uint32_t xe, uint32_t soff, uint32_t doff, uint8_t)
    {
        uint32_t i, spos, dpos, cmp = 0U;
        for (i = 0U; (i + 11U) < xs; i += 10U)
        {
            if (__builtin_expect(!!(i >= xe), 0)) return cmp;
            spos = (soff + (i * _PIXNUM));
            dpos = (doff + ((i - xd) * _PIXNUM));
            // + 10 pixels (10p x 3u), avail: (11p x 3u - 2u)
            __builtin_prefetch(&src[spos] + 32, 0, 0);
            __builtin_prefetch(&dst[dpos] + 32, 0, 0);
            __m256i a1 = _mm256_lddqu_si256((__m256i*)&src[spos]);
            __m256i a2 = _mm256_lddqu_si256((__m256i*)&dst[dpos]);
            cmp += count_mask<32>(_mm256_movemask_epi8(_mm256_cmpeq_epi32(a1, a2)));
        }
        for (; i < xs; i++)
        {
            if (__builtin_expect(!!(i >= xe), 0)) return cmp;
            spos = (soff + (i * _PIXNUM));
            dpos = (doff + ((i - xd) * _PIXNUM));
#           pragma GCC diagnostic push
#           pragma GCC diagnostic ignored "-Wpedantic"
            switch (xs - i)
            {
                case 6 ... 10:
                    {
                        __builtin_prefetch(&src[spos] + 16, 0, 0);
                        __builtin_prefetch(&dst[dpos] + 16, 0, 0);
                        __m128i a1 = _mm_lddqu_si128((__m128i*)&src[spos]);
                        __m128i a2 = _mm_lddqu_si128((__m128i*)&dst[dpos]);
                        cmp += count_mask<16>(_mm_movemask_epi8(_mm_cmpeq_epi8(a1, a2)));
                        i += 4U; // +4 = 5 pixels (5p x 3u), avail: (6p x 3u - 2u)
                        break;
                    }
                case 2 ... 5:
                    {
                        __builtin_prefetch(&src[spos] + 6, 0, 0);
                        __builtin_prefetch(&dst[dpos] + 6, 0, 0);
                        __m64 a1 = _mm_set_pi8(src[i], src[i + 1], src[i + 2], CHAR_MAX, src[i + 3], src[i + 4], src[i + 5], CHAR_MAX);
                        __m64 a2 = _mm_set_pi8(dst[i], dst[i + 1], dst[i + 2], CHAR_MAX, dst[i + 3], dst[i + 4], dst[i + 5], CHAR_MAX);
                        cmp += count_mask<8>(_mm_movemask_pi8(_mm_cmpeq_pi8(a1, a2)));
                        _mm_empty();
                        i += 1U; // +1 = 2 pixels (2p x 3u), avail: (2p x 3u)
                        break;
                    }

                case 1:
                    {
                        __builtin_prefetch(&src[spos] + sizeof(_PIXSTRUCT), 0, 0);
                        __builtin_prefetch(&dst[dpos] + sizeof(_PIXSTRUCT), 0, 0);
                        cmp += rgb_icompare(&src[spos], &dst[dpos], 0);
                        break;
                    }
                default:
                    {
                        assert(false && "buffer index overflow");
                    }
            }
#           pragma GCC diagnostic pop
        }
        return cmp;
    }
#   elif !defined(__ANDROID__)
    uint32_t __attribute__((noinline)) ICOMPARE_avx2(
            ImgBuffer const &,
            ImgBuffer const &,
            uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint8_t
        ) { throw std::runtime_error(_AVX2_NOT_SUPPORT_MSG); }
#   endif
}

