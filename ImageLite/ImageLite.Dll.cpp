
#include "ImageLite.h"
#include <HelperIntrin.h>

#if defined(_BUILD_DLL)
#   if (defined(_BUILD_AVX2) || defined(__AVX2__))
namespace ImageLite
{
    namespace MJpegAVX2
    {
        void initavx2_internal();
    }
    namespace JpegGpuAVX2
    {
        void initavx2_internal();
    }
}
#   endif

BOOL APIENTRY DllMain(HMODULE, DWORD id, LPVOID)
{
    switch (id)
    {
        case DLL_PROCESS_ATTACH:
        {
#           if (defined(_BUILD_AVX2) || defined(__AVX2__))
            if (Helper::intrin.getcpuid() == Helper::Intrin::CpuId::CPU_AVX2)
            {
                ImageLite::MJpegAVX2::initavx2_internal();
                ImageLite::JpegGpuAVX2::initavx2_internal();
            }
#           endif
            break;
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
#endif
