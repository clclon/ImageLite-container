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
#include <HelperIntrin.h>

#if (defined(_BUILD_AVX2) || defined(__AVX2__))
namespace ImageLite
{
    namespace MJpegAVX2
    {
        int32_t foreach_avx2(
            const char*,
            int32_t,
            int32_t,
            int32_t&,
            int32_t&,
            ImageLite::MJpeg::StageType&);
        void initavx2_internal();
    }
}
#endif

namespace ImageLite
{
#   include "MJpeg.Table.h"

    void foreach_native(
            const char*,
            int32_t,
            int32_t,
            int32_t&,
            int32_t&,
            MJpeg::StageType&
    );

    MJpeg::MJpeg()
    {
#       if (!defined(_BUILD_DLL) && (defined(_BUILD_AVX2) || defined(__AVX2__)))
        if (Helper::intrin.getcpuid() == Helper::Intrin::CpuId::CPU_AVX2)
            ImageLite::MJpegAVX2::initavx2_internal();
#       endif
    }

    void MJpeg::clear()
    {
        m_state = StageType::MST_UNKNOWN;
        m_prev = { 0, 0 };
        m_bimg.clear();
        m_dec.clear();
    }

    void MJpeg::stream(const char *b, uint32_t szp, mjpeg_cb fun)
    {
        try
        {
            ImageLite::ImgBuffer v;
            int32_t sz = static_cast<int32_t>(szp),
                    offset = 0;
            do
            {
                sz -= offset;
                if (sz <= 0)
                {
                    m_prev = { b[szp - 2], b[szp - 1] };
                    break;
                }
                if (parse(b + offset, sz, v, offset))
                    if (!fun(v, m_dec.imgparam.width, m_dec.imgparam.height))
                        break;
            }
            while (offset > 0);
        }
        catch (std::system_error const& ex_)
        {
            error = ((error == ex_.code()) ? error : ex_.code());
            throw;
        }
        catch (std::runtime_error const& ex_)
        {
            error = ImageLite::JpegGpu::make_error_code(ImageLite::JpegGpu::ErrorId::error_sys_execpt);
            throw;
        }
        catch (std::exception const& ex_)
        {
            error = ImageLite::JpegGpu::make_error_code(ImageLite::JpegGpu::ErrorId::error_sys_execpt);
            throw;
        }
        catch (...)
        {
            error = ImageLite::JpegGpu::make_error_code(ImageLite::JpegGpu::ErrorId::error_unknown);
            throw std::system_error(error);
        }
    }

    bool MJpeg::parse(const char *b, const int32_t sz, ImageLite::ImgBuffer & odata, int32_t & offset)
    {
        offset = sz;
        int32_t i = 0, spos = -1, epos = -1;
        {
            int32_t stage = 0;
            StageType m;

            do
            {
                if (m_state == StageType::MST_UNKNOWN)
                {
                    if ((sz >= 1) &&
                        ((m = check_jpeg_tag(std::get<0>(m_prev), std::get<1>(m_prev), b[0])) == StageType::MST_BEGIN))
                        break;

                    stage = 1;
                    if ((sz >= 2) &&
                        ((m = check_jpeg_tag(std::get<1>(m_prev), b[0], b[1])) == StageType::MST_BEGIN))
                        break;
                }
                else if (m_state == StageType::MST_BEGIN)
                {
                    stage = 1;
                    if ((sz >= 1) &&
                        ((m = check_jpeg_tag(std::get<1>(m_prev), b[0], 0)) == StageType::MST_END))
                        break;
                }
                m = StageType::MST_UNKNOWN;
            }
            while (0);

            switch(m)
            {
                case StageType::MST_BEGIN:
                    {
                        m_bimg.clear();
                        m_state = m;
                        if (stage)
                        {
                            i += (sizeof(JFIFHEAD) - 1);
                            m_bimg.push_back(std::get<1>(m_prev));
                        }
                        else
                        {
                            i += (sizeof(JFIFHEAD) - 2);
                            m_bimg.push_back(std::get<0>(m_prev));
                            m_bimg.push_back(std::get<1>(m_prev));
                        }
                        break;
                    }
                case StageType::MST_END:
                    {
                        if (!stage)
                            throw std::runtime_error("bad MST_END index!");

                        m_state = m;
                        epos = 1;
                        break;
                    }
                default:
                    break;
            }
        }
        //
        if (m_state != StageType::MST_END)
        {
#           if (defined(_BUILD_AVX2) || defined(__AVX2__))
            if (Helper::intrin.getcpuid() == Helper::Intrin::CpuId::CPU_AVX2)
            {
                i = ImageLite::MJpegAVX2::foreach_avx2(b, sz, i, spos, epos, m_state);
                if (m_state != StageType::MST_END)
                    ImageLite::foreach_native(b, sz, i, spos, epos, m_state);
            }
            else
#           endif
                ImageLite::foreach_native(b, sz, i, spos, epos, m_state);
        }
        //
        switch(m_state)
        {
            case StageType::MST_BEGIN:
                {
                    if (spos > -1)
                        m_bimg.assign(b + spos, b + sz);
                    else
                    {
                        m_bimg.reserve(m_bimg.size() + sz);
                        m_bimg.insert(m_bimg.end(), &b[0], b + sz);
                    }
                    break;
                }
            case StageType::MST_END:
                {
                    if (epos <= 0)
                    {
                        m_state = StageType::MST_UNKNOWN;
                        break;
                    }

                    odata.clear();

                    if (spos > -1)
                        m_bimg.assign(&b[spos], &b[epos]);
                    else
                    {
                        m_bimg.reserve(m_bimg.size() + (sz - (sz - epos)));
                        m_bimg.insert(m_bimg.end(), &b[0], &b[epos]);
                    }
                    offset = epos;
                    m_state = StageType::MST_UNKNOWN;

                    bool x = m_dec.imgstream(m_bimg, odata);
                    if ((!x) && (m_dec.error))
                    {
                        error = m_dec.error;
                        m_dec.error.clear();
                        throw std::system_error(error);
                    }
                    return x;
                }
            default:
                    break;
        }
        return false;
    }
}
