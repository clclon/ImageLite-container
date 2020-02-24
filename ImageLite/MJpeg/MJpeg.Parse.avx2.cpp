
#include "../ImageLite.h"
#include "NVJpegDecoder.Error.h"

enum maskid
{
    maskJB = 0,
    maskJE,
    maskJX
};

namespace ImageLite
{
#   include "MJpeg.Table.h"

    namespace MJpegAVX2
    {
#   if defined(__AVX2__)
#       if !defined(_BUILD_DLL)
        static bool isavx2init = false;;
#       endif
        static __m256i masks[3]{};
        //
        void __attribute__((noinline)) initavx2_internal()
        {
#           if !defined(_BUILD_DLL)
            if (isavx2init)
                return;
            isavx2init = true;
#           endif
            //
            masks[maskJB] = _mm256_set1_epi8(static_cast<char>(0xd8));
            masks[maskJE] = _mm256_set1_epi8(static_cast<char>(0xd9));
            masks[maskJX] = _mm256_set1_epi8(static_cast<char>(0xff));
            //
        }

        static inline ImageLite::MJpeg::StageType check_jpeg_tag_begin(const char *b, int32_t i, int32_t *pos)
        {
            __m256i  const a1 = _mm256_lddqu_si256((__m256i*)&b[i]);
            uint32_t const x1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(a1, masks[maskJB]));
            if (x1)
            {
                uint32_t x2 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(a1, masks[maskJX]));
                if (x2)
                {
                    uint32_t p0 = __builtin_ctz(x1),
                             p1 = __builtin_ctz(x2),
                             p2;
                             x2 = (x2 & (x2 - 1));
                             p2 = __builtin_ctz(x2);

                    if (((p0 - 1) == p1) &&
                        ((p0 + 1) == p2)) {
                        *pos = (i + p1);
                        return ImageLite::MJpeg::StageType::MST_BEGIN;
                    }
                }
            }
            return ImageLite::MJpeg::StageType::MST_UNKNOWN;
        }

        static inline ImageLite::MJpeg::StageType check_jpeg_tag_end(const char *b, int32_t i, int32_t *pos)
        {
            __m256i  const a1 = _mm256_lddqu_si256((__m256i*)&b[i]);
            uint32_t const x1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(a1, masks[maskJE]));
            if (x1)
            {
                uint32_t const x2 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(a1, masks[maskJX]));
                if (x2)
                {
                    uint32_t p0 = __builtin_ctz(x1),
                             p1 = __builtin_ctz(x2);

                    if ((p0 - 1) == p1) {
                        *pos = (i + p0);
                        return ImageLite::MJpeg::StageType::MST_END;
                    }
                }
            }
            return ImageLite::MJpeg::StageType::MST_UNKNOWN;
        }

        INTRIN_TARGET("avx2")
        int32_t __attribute__((noinline)) foreach_avx2(
                const char *b,
                int32_t sz,
                int32_t i,
                int32_t & spos,
                int32_t & epos,
                ImageLite::MJpeg::StageType & state)
        {
            for (; (i + 31) < sz; i += 32)
            {
                int32_t pos = 0;
                ImageLite::MJpeg::StageType m;

                switch(state)
                {
                    case ImageLite::MJpeg::StageType::MST_UNKNOWN:
                        {
                            if ((m = check_jpeg_tag_begin(b, i, &pos)) != ImageLite::MJpeg::StageType::MST_BEGIN)
                                continue;
                            break;
                        }
                    case ImageLite::MJpeg::StageType::MST_BEGIN:
                        {
                            if ((m = check_jpeg_tag_end(b, i, &pos)) != ImageLite::MJpeg::StageType::MST_END)
                                continue;
                            break;
                        }
                    case ImageLite::MJpeg::StageType::MST_END:
                        return i;
                }
                switch(m)
                {
                    case ImageLite::MJpeg::StageType::MST_BEGIN:
                        {
                            state = ImageLite::MJpeg::StageType::MST_BEGIN;
                            spos = pos;
                            i += (pos + sizeof(JFIFHEAD));
                            continue;
                        }
                    case ImageLite::MJpeg::StageType::MST_END:
                        {
                            state = ImageLite::MJpeg::StageType::MST_END;
                            epos = (pos + 1);
                            return i;
                        }
                    case ImageLite::MJpeg::StageType::MST_UNKNOWN:
                        break;
                }
            }
            return i;
        }

#   elif !defined(__ANDROID__)
        void __attribute__((noinline)) initavx2_internal() { throw std::runtime_error(_AVX2_NOT_SUPPORT_MSG); }
        int32_t __attribute__((noinline)) foreach_avx2(
                const char*,
                int32_t,
                int32_t,
                int32_t &,
                int32_t &,
                ImageLite::MJpeg::StageType &
            ) { throw std::runtime_error(_AVX2_NOT_SUPPORT_MSG); }
#   endif
    }
}
