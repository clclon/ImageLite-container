
#include <string>
#include <system_error>
#include "ImageLite.Error.h"

namespace ImageLite
{
    static inline const char *l_imgl_error[] =
    {
        "",              // ErrorId::error_begin
#       define __ERRITEM(A,B) B,
#       include "ImageLite.Error.Items.h"
        "unknown error", // ErrorId::error_unknown
        ""               // ErrorId::error_end
    };
    //
    std::string_view geterror(int32_t e)
    {
        if ((e > ImageLite::ErrorId::error_imgl_begin) && (e < ImageLite::ErrorId::error_imgl_end))
            return l_imgl_error[e];
        return l_imgl_error[ImageLite::ErrorId::error_imgl_unknown];
    }

    const char* ErrorCat::name() const noexcept
    {
        return "Image Lite Error";
    }

    std::string_view ErrorCat::message(ImageLite::ErrorId e) const
    {
        return geterror(static_cast<int32_t>(e));
    }

    std::string ErrorCat::message(int32_t e) const
    {
        return std::string(geterror(e));
    }

    std::error_code make_error_code(ErrorId e)
    {
        return { static_cast<int32_t>(e), errCat };
    }
}
