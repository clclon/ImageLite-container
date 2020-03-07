#ifndef HEADER_2167D4DF370B5194
#define HEADER_2167D4DF370B5194


#pragma once

namespace ImageLite
{
    enum ErrorId
    {
        error_none = 0,
        error_begin = 1600,
#       define __ERRITEM(A,B) error_ ## A,
#       include "ImageLite.Error.Items.h"
        error_unknown,
        error_end
    };
    class DLL_EXPORT ErrorCat : public std::error_category
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
#endif // header guard

