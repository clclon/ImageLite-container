
#include "ImageLite.Formater.internal.h"

namespace ImageLite
{
    static Formater::imgVtable initvt()
    {
        ImageLite::Formater::icovert_ptr  CONVERT_BMP_ptr        = &CONVERT_BMP_native;
        ImageLite::Formater::icovert_ptr  CONVERT_TO_NOPAD_ptr   = &CONVERT_TO_NOPAD_native;
        ImageLite::Formater::icovert_ptr  CONVERT_FROM_NOPAD_ptr = &CONVERT_FROM_NOPAD_native;
        ImageLite::Formater::icovert_ptr  CONVERT_TO_SEPIA_ptr   = &CONVERT_TO_SEPIA_native;
        ImageLite::Formater::icovert_ptr  CONVERT_TO_GREY_ptr    = &CONVERT_TO_GREY_native;
        ImageLite::Formater::icovert_ptr  CONVERT_TO_BW_ptr      = &CONVERT_TO_BW_native;
        ImageLite::Formater::icovert_ptr  STREAM_LUA_TABLE_ptr   = &STREAM_LUA_TABLE_native;
        ImageLite::Formater::icompare_ptr ICOMPARE_ptr           = &ICOMPARE_native;

        switch(Helper::intrin.getcpuid())
        {
#           if (defined(_BUILD_AVX2) || defined(__AVX2__))
            case Helper::Intrin::CpuId::CPU_AVX2:
                {
                    ICOMPARE_ptr = &ICOMPARE_avx2;
                    break;
                }
#           endif
#           if defined(__ANDROID__) //__ARM_NEON
#             if (defined(_BUILD_ARMNEON86_ENABLE) || defined(_BUILD_ARMNEON_ENABLE))
            case Helper::Intrin::CpuId::CPU_ARM_NEON_FMA:
            case Helper::Intrin::CpuId::CPU_ARM_NEON:
                {
                    ICOMPARE_ptr = &ICOMPARE_neon;
                    break;
                }
#             endif
#           endif
            default:
                break;
       }
       return
       {
            CONVERT_BMP_ptr,
            CONVERT_TO_NOPAD_ptr,
            CONVERT_FROM_NOPAD_ptr,
            CONVERT_TO_SEPIA_ptr,
            CONVERT_TO_GREY_ptr,
            CONVERT_TO_BW_ptr,
            STREAM_LUA_TABLE_ptr,
            ICOMPARE_ptr
        };
    }

    /// Virtual table to CPU features dispatcher
    Formater::imgVtable Formater::ivtable = initvt();
    ///
}
