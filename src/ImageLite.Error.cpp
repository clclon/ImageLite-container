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

#include <string>
#include <system_error>
#include "ImageLite.h"

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
