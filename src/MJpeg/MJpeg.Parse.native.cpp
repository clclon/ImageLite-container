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

#include "../ImageLite.h"
#include "NVJpegDecoder.Error.h"

namespace ImageLite
{
#   include "MJpeg.Table.h"

    void foreach_native(
                const char *b,
                int32_t sz,
                int32_t i,
                int32_t & spos,
                int32_t & epos,
                MJpeg::StageType & state)
    {
        for (; (i + 1) < sz; i++)
        {
            MJpeg::StageType m;

            switch(state)
            {
                case MJpeg::StageType::MST_UNKNOWN:
                    {
                        if ((m = check_jpeg_tag(b[i + 0], b[i + 1], b[i + 2])) != MJpeg::StageType::MST_BEGIN)
                            continue;
                        break;
                    }
                case MJpeg::StageType::MST_BEGIN:
                    {
                        if ((m = check_jpeg_tag(b[i + 0], b[i + 1], 0)) != MJpeg::StageType::MST_END)
                            continue;
                        break;
                    }
                case MJpeg::StageType::MST_END:
                    return;
            }
            switch(m)
            {
                case MJpeg::StageType::MST_BEGIN:
                    {
                        state = MJpeg::StageType::MST_BEGIN;
                        spos = i;
                        i += sizeof(JFIFHEAD);
                        continue;
                    }
                case MJpeg::StageType::MST_END:
                    {
                        state = MJpeg::StageType::MST_END;
                        epos = (i + 2);
                        return;
                    }
                case MJpeg::StageType::MST_UNKNOWN:
                    break;
            }
        }
    }
}
