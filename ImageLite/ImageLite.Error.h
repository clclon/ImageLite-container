
#pragma once

namespace ImageLite
{
    enum ErrorId
    {
        error_imgl_begin = 0,
#       define __ERRITEM(A,B) error_imgl_ ## A,
#       include "ImageLite.Error.Items.h"
        error_imgl_unknown,
        error_imgl_end
    };
    class ErrorCat : public std::error_category
    {
        public:
            //
            const char* name() const noexcept override;
            std::string message(int32_t) const override;
            std::string_view message(ErrorId) const;
    };
    const ErrorCat errCat{};
    //
    std::error_code make_error_code(ErrorId);
    //
    std::string_view geterror(int32_t);
}

namespace std
{
    //
    template <>
    struct is_error_code_enum<ImageLite::ErrorId> : public true_type{};
}
