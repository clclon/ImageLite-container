
#pragma once

#if defined(__ANDROID__)

// __ARM_ARCH_ISA_A64
// __ARM_ARCH_7S__
// __ARM_ARCH_7A__

#  if defined(__ARM_NEON)
#    if (defined(__i386__) || defined(__x86_64__))
#      if __has_include(<arm_neon_x86.h>)
#        include <arm_neon_x86.h>
#        define _BUILD_ARMNEON86_ENABLE 1
#      endif
#    else
#      if __has_include(<arm_neon.h>)
#        include <arm_neon.h>
#        define _BUILD_ARMNEON_ENABLE 1
#      endif
#      if (defined(__ALTIVEC__) && __has_include(<altivec.h>))
#        include <altivec.h>
#        define _BUILD_ALTIVEC_ENABLE 1
#      endif
#    endif
#  endif

#elif defined(_MSC_VER)

#  if __has_include(<intrin.h>) // SSE1
#    include <intrin.h>
#    define _BUILD_XMMINTRIN_ENABLE 1
#  endif

#else

#  if __has_include(<xmmintrin.h>) // SSE1
#    include <xmmintrin.h>
#    define _BUILD_XMMINTRIN_ENABLE 1
#  endif

#  if __has_include(<emmintrin.h>) // SSE2
#    include <emmintrin.h>
#    define _BUILD_EMMINTRIN_ENABLE 1
#  endif

#  if __has_include(<immintrin.h>) // AVX2
#    include <immintrin.h>
#    define _BUILD_IMMINTRIN_ENABLE 1
#  endif

#  if __has_include(<cpuid.h>)
#    include <cpuid.h>
#    define _BUILD_CPUID_ENABLE 1
#  endif

#endif

#if defined(__GNUC__)
#  define INTRIN_ALIGNED(n) __attribute__((aligned(n)))
#  if defined(__ANDROID__) && defined(__clang__)
#    define INTRIN_TARGET(x)  __attribute__((target(x)))
#  elif defined(__x86_64__) || defined(__clang__)
#    define INTRIN_TARGET(x)  __attribute__((target(x),stdcall))
#  elif defined(__i386__)
#    define INTRIN_TARGET(x)  __attribute__((target(x ",fpmath=sse"),stdcall))
#  else
#    define INTRIN_TARGET(x)  __attribute__((target(x)))
#  endif
#elif defined(_MSC_VER)
#  define INTRIN_ALIGNED(n) __declspec(align(n))
#  define INTRIN_TARGET(x)
#else
#  define INTRIN_ALIGNED(n) __declspec(align(n))
#  define INTRIN_TARGET(x)
#endif

#include <stdint.h>

namespace Helper
{
    class Intrin
    {
        public:
            //
            enum CpuId
            {
                CPU_NATIVE = 0,
                CPU_SSE,
                CPU_SSE2,
                CPU_SSE3,
                CPU_SSSE3,
                CPU_SSE41,
                CPU_SSE42,
                CPU_AVX,
                CPU_AVX2,
                CPU_POPCNT,
                CPU_ARM_V7,
                CPU_ARM_NEON,
                CPU_ARM_NEON_FMA,
                CPU_ARM64_SIMD,
                CPU_MIPS_R6,
                CPU_MIPS_MSA
            };

        private:
            //
            CpuId m_cpuid = CpuId::CPU_NATIVE;
            //
            void init();

        public:
            //
            Intrin();
            //
            CpuId getcpuid() const;
#           if (defined(__SSE2__) && !defined(__ANDROID__))
            template <typename T> static void print128i(__m128i);
#           endif

    };
    const Intrin intrin;

#   if (defined(__SSE2__) && !defined(__ANDROID__))
    extern template void Intrin::print128i<uint8_t>(__m128i);
    extern template void Intrin::print128i<uint16_t>(__m128i);
    extern template void Intrin::print128i<uint32_t>(__m128i);
    extern template void Intrin::print128i<uint64_t>(__m128i);
#   endif
}

