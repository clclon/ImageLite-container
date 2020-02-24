
#include "NVJpegDecoder.Internal.h"

namespace ImageLite
{
    namespace JpegGpu
    {
        static inline const char* l_app_error[] =
        {
            "",              // ErrorId::error_begin
            "bad device number",
            "device is prohibited mode",
            "device does not support CUDA",
            "unknown chroma subsampling",
            "bad image parameters",
            "bad image channel",
            "imgstream error",
            "decompress jpeg error",
            "CUDA API error",
            "not file access",
            "system object is null",
            "system error exeption",
            "this build not support AVX2",
            "unknown error", // ErrorId::error_unknown
            ""               // ErrorId::error_end
        };

        template <typename T>
        T geterror_internal(int32_t e)
        {
            if ((e > ErrorId::error_begin) && (e < ErrorId::error_end))
                return l_app_error[e];
            return l_app_error[ErrorId::error_unknown];
        }

        std::string ErrorCat::message(int32_t e) const
        {
            return geterror_internal<std::string>(e);
        }

        std::string geterror(int32_t e)
        {
            return geterror_internal<std::string>(e);
        }

        const char* geterrorc(int32_t e)
        {
            return geterror_internal<const char*>(e);
        }

        const char* ErrorCat::name() const noexcept
        {
            return "NVJpegDecoder Error";
        }

        ///

        template <class T>
        T make_error_type(ErrorId e)
        {
            return { static_cast<int32_t>(e), errCat };
        }

        std::error_code make_error_code(ErrorId e)
        {
            return make_error_type<std::error_code>(e);
        }

    }
}
