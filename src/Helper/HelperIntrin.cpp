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

#include "HelperIntrin.h"
#if defined(__ANDROID__)
#  include "HelperIntrin/NDK/cpu-features.h"
#endif
#include <string>

namespace Helper
{
    Intrin::Intrin()
    {
        init();
    }

    Intrin::CpuId Intrin::getcpuid() const
    {
        return m_cpuid;
    }

    ///

    void Intrin::init()
    {
        try
        {
#   if defined(__GNUC__)
#       if defined(__ANDROID__)
        uint64_t fcpu = android_getCpuFeatures();
        switch (android_getCpuFamily())
        {
            case AndroidCpuFamily::ANDROID_CPU_FAMILY_ARM:
                {
                    if (fcpu & ANDROID_CPU_ARM_FEATURE_NEON_FMA)
                        m_cpuid = CpuId::CPU_ARM_NEON_FMA;
                    else if (fcpu & ANDROID_CPU_ARM_FEATURE_NEON)
                        m_cpuid = CpuId::CPU_ARM_NEON;
                    else if (fcpu & ANDROID_CPU_ARM_FEATURE_ARMv7)
                        m_cpuid = CpuId::CPU_ARM_V7;
                    break;
                }
            case AndroidCpuFamily::ANDROID_CPU_FAMILY_ARM64:
                {
                    if (fcpu & ANDROID_CPU_ARM64_FEATURE_ASIMD)
                        m_cpuid = CpuId::CPU_ARM64_SIMD;
                    break;
                }
            case AndroidCpuFamily::ANDROID_CPU_FAMILY_X86:
            case AndroidCpuFamily::ANDROID_CPU_FAMILY_X86_64:
                {
                    if (fcpu & ANDROID_CPU_X86_FEATURE_SSSE3)
                        m_cpuid = CpuId::CPU_SSE3;
                    else if (fcpu & ANDROID_CPU_X86_FEATURE_SSE4_1)
                        m_cpuid = CpuId::CPU_SSE41;
                    else if (fcpu & ANDROID_CPU_X86_FEATURE_SSE4_2)
                        m_cpuid = CpuId::CPU_SSE42;
                    else if (fcpu & ANDROID_CPU_X86_FEATURE_AVX)
                        m_cpuid = CpuId::CPU_AVX;
                    else if (fcpu & ANDROID_CPU_X86_FEATURE_AVX2)
                        m_cpuid = CpuId::CPU_AVX2;
                    else if (fcpu & ANDROID_CPU_X86_FEATURE_POPCNT)
                        m_cpuid = CpuId::CPU_POPCNT;
                    break;
                }
            case AndroidCpuFamily::ANDROID_CPU_FAMILY_MIPS64:
                {
                    if (fcpu & ANDROID_CPU_MIPS_FEATURE_R6)
                        m_cpuid = CpuId::CPU_MIPS_R6;
                    else if (fcpu & ANDROID_CPU_MIPS_FEATURE_MSA)
                        m_cpuid = CpuId::CPU_MIPS_MSA;
                    break;
                }
            default:
                break;
        }
#       else
        __builtin_cpu_init();
             if (__builtin_cpu_supports("avx2"))   { m_cpuid = CpuId::CPU_AVX2;  }
        else if (__builtin_cpu_supports("avx"))    { m_cpuid = CpuId::CPU_AVX;   }
        else if (__builtin_cpu_supports("sse4.2")) { m_cpuid = CpuId::CPU_SSE42; }
        else if (__builtin_cpu_supports("sse4.1")) { m_cpuid = CpuId::CPU_SSE41; }
        else if (__builtin_cpu_supports("ssse3"))  { m_cpuid = CpuId::CPU_SSSE3; }
        else if (__builtin_cpu_supports("sse3"))   { m_cpuid = CpuId::CPU_SSE3;  }
        else if (__builtin_cpu_supports("sse2"))   { m_cpuid = CpuId::CPU_SSE2;  }
        else if (__builtin_cpu_supports("sse"))    { m_cpuid = CpuId::CPU_SSE;   }
        else if (__builtin_cpu_supports("popcnt")) { m_cpuid = CpuId::CPU_POPCNT;}
#       endif

#   elif defined(_MSC_VER)
        do
        {
            int32_t cpuinfo[4]{};
            __cpuid(cpuinfo, 1);

            bool isavx2 = ((cpuinfo[2] & (1 << 28)) ? ((cpuinfo[2] & (1 << 27)) ? true : false) : false);
            if (isavx2)
            {
                uint64_t const x = _xgetbv(0);
                isavx2 = ((x & 0x6) == 0x6);
                if (isavx2)
                {
                    m_cpuid = CpuId::CPU_AVX2;
                    break;
                }
            }
            if (cpuinfo[2] & (1 << 20))
            {
                m_cpuid = CpuId::CPU_SSE42;
                break;
            }
            if (cpuinfo[2] & (1 << 19))
            {
                m_cpuid = CpuId::CPU_SSE41;
                break;
            }
            if (cpuinfo[2] & (1 << 9))
            {
                m_cpuid = CpuId::CPU_SSSE3;
                break;
            }
            if (cpuinfo[2] & (1 << 0))
            {
                m_cpuid = CpuId::CPU_SSE3;
                break;
            }
            if (cpuinfo[3] & (1 << 26))
            {
                m_cpuid = CpuId::CPU_SSE2;
                break;
            }
            if (cpuinfo[3] & (1 << 25))
            {
                m_cpuid = CpuId::CPU_SSE;
                break;
            }
        }
        while (0);
#   endif
        }
        catch (...) {}
    }

#   if (defined(__SSE2__) && !defined(__ANDROID__))
    template <typename T>
    __attribute__((target("sse2")))
    void Intrin::print128i(__m128i m)
    {
        if constexpr (std::is_same<T, uint8_t>::value)
        {
            alignas(16) uint8_t v[16];
            _mm_store_si128((__m128i*)v, m);
            printf("HEX v16_u8: %x %x %x %x | %x %x %x %x | %x %x %x %x | %x %x %x %x\n",
               v[0], v[1],  v[2],  v[3],  v[4],  v[5],  v[6],  v[7],
               v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15]);
        }
        else if constexpr (std::is_same<T, uint16_t>::value)
        {
            alignas(16) uint16_t v[8];
            _mm_store_si128((__m128i*)v, m);
            printf("HEX v8_u16: %x %x %x %x,  %x %x %x %x\n",
               v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
        }
        else if constexpr (std::is_same<T, uint32_t>::value)
        {
            alignas(16) uint32_t v[4];
            _mm_store_si128((__m128i*)v, m);
            printf("HEX v4_u32: %x %x %x %x\n",
               v[0], v[1], v[2], v[3]);
        }
        else if constexpr (std::is_same<T, uint64_t>::value)
        {
            alignas(16) uint64_t v[2];
            _mm_store_si128((__m128i*)v, m);
            printf("HEX v2_u64: %llx %llx\n", v[0], v[1]);
        }
    }
    template void Intrin::print128i<uint8_t>(__m128i);
    template void Intrin::print128i<uint16_t>(__m128i);
    template void Intrin::print128i<uint32_t>(__m128i);
    template void Intrin::print128i<uint64_t>(__m128i);
#   endif
}
