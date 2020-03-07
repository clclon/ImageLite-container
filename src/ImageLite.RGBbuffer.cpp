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

#include "ImageLite.h"
#include <lodepng.h>
#include <toojpeg.h>

#define throw_sys_error(A) \
	throw std::system_error(ImageLite::make_error_code(ImageLite::ErrorId::A))
#define throw_sys_error_ex(A,B) \
	throw std::system_error(ImageLite::make_error_code(ImageLite::ErrorId::A), B)

namespace ImageLite
{
    static inline const char *l_ifmt[] =
    {
        "bmp",
        "png",
        "jpg",
        "raw",
        "lua",
        nullptr
    };

    ImageRGBbuffer::ImageRGBbuffer() {}

    ImageRGBbuffer::ImageRGBbuffer(ImageTable const & it, BufferType t)
    {
        if (!it.data.size())
            throw_sys_error(error_buffer_empty);
        if ((!it.width) || (!it.height))
            throw_sys_error(error_size_mismatch);
        if (it.itype != BufferType::IBT_RGB)
            throw_sys_error(error_not_support_type);

        ImageLite::ImageDataP idtmp
        {
            {},
            it.data,
            {
                it.width,
                it.height
            },
            0
        };

        _init(idtmp, m_idata, t);
        m_pad = 0;

        if (getsize(SizeType::IST_CALC_BYTES).x != static_cast<int32_t>(m_idata.buffer.size()))
            throw_sys_error(error_bad_size);
    }

    ImageRGBbuffer::ImageRGBbuffer(ImgBuffer const & b, uint32_t w, uint32_t h, uint32_t s, BufferType t)
    {
        if (!b.size())
            throw_sys_error(error_buffer_empty);
        if ((!w) || (!h))
            throw_sys_error(error_size_mismatch);

        ImageLite::ImageDataP idtmp
        {
            {},
            b,
            {
                static_cast<int32_t>(w),
                static_cast<int32_t>(h)
            },
            static_cast<int32_t>(s)
        };

        _init(idtmp, m_idata, t);

        if (getsize(SizeType::IST_CALC_BYTES).x != static_cast<int32_t>(m_idata.buffer.size()))
            throw_sys_error(error_bad_size);

        if (!s)
            m_pad = _set_stride();
        else
            m_pad = getpad();
    }

    ImageRGBbuffer::ImageRGBbuffer(uint8_t const *b, uint32_t sz, uint32_t w, uint32_t h, uint32_t s, BufferType t)
    {
        if (!sz)
            throw_sys_error(error_pointer_size);
        if (!b)
            throw_sys_error(error_pointer_null);
        if ((!w) || (!h))
            throw_sys_error(error_size_mismatch);

        ImageLite::ImageData idtmp
        {
            {},
            {},
            {
                static_cast<int32_t>(w),
                static_cast<int32_t>(h)
            },
            static_cast<int32_t>(s)
        };
        idtmp.buffer.resize(sz);
        idtmp.buffer.assign(&b[0], &b[sz]);

        _init(idtmp, m_idata, t);

        if (getsize(SizeType::IST_CALC_BYTES).x != static_cast<int32_t>(m_idata.buffer.size()))
            throw_sys_error(error_bad_size);

        if (!s)
            m_pad = _set_stride();
        else
            m_pad = getpad();
    }

    ImageRGBbuffer::ImageRGBbuffer(std::string_view const & s, ImageType t)
    {
        if (s.empty())
            throw_sys_error(error_file_name_empty);

        load(s, t);
    }

    ImageRGBbuffer::ImageRGBbuffer(std::string_view const & s, uint32_t w, uint32_t h, uint32_t s_, BufferType)
    {
        if (s.empty())
            throw_sys_error(error_file_name_empty);

        // TODO (clanc#1#18.10.2019): check BufferType
        load_raw(s, w, h, s_);
    }

    ImageRGBbuffer::ImageRGBbuffer(
            ImageLite::ImageData const &idata,
            ImageLite::IRECT<int32_t> const & ir,
            ImageLite::BufferType t)
    {
        if (!idata.buffer.size())
            throw_sys_error(error_buffer_empty);
        if ((!idata.point.x) || (!idata.point.y))
            throw_sys_error(error_size_mismatch);
        if ((idata.point.x < (ir.w + ir.x)) || (idata.point.y < (ir.h + ir.y)))
            throw_sys_error(error_size_is_large);

        ImageLite::ImageData idtmp{};
        _init(idata, idtmp, t);

        if ((idtmp.point.x == ir.w) &&
            (idtmp.point.y == ir.h))
        {
            m_idata = idtmp;
            m_pad = _set_stride();
        }
        else
        {
            if(!ImageLite::Formater::igetrectangle(
                    idtmp,
                    m_idata,
                    ir
                ))
                throw_sys_error(error_not_crop);
            m_pad = getpad();
        }
    }

    ImageRGBbuffer::ImageRGBbuffer(
            ImageLite::ImageData const &idata,
            ImageLite::IPOINT<int32_t> const & ip,
            ImageLite::BufferType t)
    {
        if (!idata.buffer.size())
            throw_sys_error(error_buffer_empty);
        if ((!idata.point.x) || (!idata.point.y))
            throw_sys_error(error_size_mismatch);
        if ((idata.point.x < ip.x) || (idata.point.y < ip.y))
            throw_sys_error(error_size_is_large);

        ImageLite::ImageData idtmp{};
        _init(idata, idtmp, t);

        if ((idtmp.point.x == ip.x) &&
            (idtmp.point.y == ip.y))
        {
            m_idata = idtmp;
            m_pad = _set_stride();
        }
        else
        {
            // TODO (clanc#1#18.10.2019): write resize
            // m_pad = getpad();
            throw_sys_error(error_not_crop);
        }
    }

    ImageRGBbuffer::ImageRGBbuffer(
            ImageLite::ImageData const &idata,
            ImageLite::BufferType t)
    {
        if (!idata.buffer.size())
            throw_sys_error(error_buffer_empty);
        if ((!idata.point.x) || (!idata.point.y))
            throw_sys_error(error_size_mismatch);

        ImageLite::ImageData idtmp{};
        _init(idata, idtmp, t);

        m_idata = idtmp;
        m_pad = _set_stride();
    }

    ImageRGBbuffer::~ImageRGBbuffer()
    {
    }

    ///

    template <typename T>
    void ImageRGBbuffer::_init(
            T const &src,
            ImageLite::ImageData & dst,
            ImageLite::BufferType t)
    {
        dst.point = src.point;
        BufferType tt = ispad(dst.point.x);
        // check pad
        switch (t)
        {
            case BufferType::IBT_BGR: { t = ((tt == BufferType::IBT_NOPAD) ? BufferType::IBT_BGR_PAD : t); break; }
            case BufferType::IBT_RGB: { t = ((tt == BufferType::IBT_NOPAD) ? BufferType::IBT_RGB_PAD : t); break; }
            default: break;
        }
        // convert is buffer not pad
        switch (t)
        {
            case BufferType::IBT_BGR:
            case BufferType::IBT_RGB:
                {
#                   if defined(__ANDROID__)
                    dst.buffer = src.buffer;
#                   else
                    if(!ImageLite::Formater::iconvert(
                            &ImageLite::Formater::CONVERT_FROM_NOPAD,
                            src.buffer,
                            dst.buffer,
                            src.point.x, src.point.y, src.stride, (src.point.x % 4),
                            nullptr
                        ))
                        throw_sys_error(error_buffer_empty);

                    if (!dst.buffer.size())
                        throw_sys_error(error_bad_size);
#                   endif
                    break;
                }
            case BufferType::IBT_BGR_PAD:
            case BufferType::IBT_RGB_PAD:
                {
#                   if defined(__ANDROID__)
                    if(!ImageLite::Formater::iconvert(
                            &ImageLite::Formater::CONVERT_TO_NOPAD,
                            src.buffer,
                            dst.buffer,
                            src.point.x, src.point.y, src.stride, (src.point.x % 4),
                            nullptr
                        ))
                        throw_sys_error(error_buffer_empty);

                    if (!dst.buffer.size())
                        throw_sys_error(error_bad_size);
#                   else
                    dst.buffer = src.buffer;
#                   endif
                    break;
                }
            default:
                throw_sys_error(error_not_support_type);
        }
        // convert buffer is bitmap reverse pixels
        switch (t)
        {
            case BufferType::IBT_RGB:
            case BufferType::IBT_RGB_PAD:
                {
                    break;
                }
            case BufferType::IBT_BGR:
            case BufferType::IBT_BGR_PAD:
                {
                    ImageLite::ImgBuffer ibtmp{};

                    if(!ImageLite::Formater::iconvert(
                                &ImageLite::Formater::CONVERT_BMP,
                                dst.buffer,
                                ibtmp,
                                dst.point.x, dst.point.y, dst.stride, (dst.point.x % 4),
                                nullptr
                            ))
                            throw_sys_error(error_buffer_empty);

                    if (!ibtmp.size())
                        throw_sys_error(error_bad_size);

                    dst.buffer = ibtmp;
                    break;
                }
            default:
                break;
        }
    }

    bool ImageRGBbuffer::empty()
    {
        return ((!m_idata.point.x) || (!m_idata.point.y) || (!m_idata.buffer.size()));
    }

    ImageLite::IPOINT<int32_t> ImageRGBbuffer::getsize(SizeType t)
    {
        ImageLite::IPOINT<int32_t> p{};

        switch (t)
        {
            case SizeType::IST_BUFFER_BYTES:
                {
                    p.set<std::size_t>(m_idata.buffer.size(), 0U);
                    break;
                }
            case SizeType::IST_CALC_BYTES:
                {
                    p.set((m_idata.point.y * ((m_idata.point.x * 3) + m_pad)), 0);
                    break;
                }
            case SizeType::IST_IMAGE_WH_POINT:
                {
                    p.set(m_idata.point.x, m_idata.point.y);
                    break;
                }
            default:
                break;
        }
        return p;
    }

    int32_t ImageRGBbuffer::getpos(ImageLite::IPOINT<int32_t> const & p)
    {
        return getpos(p.x, p.y);
    }

    int32_t ImageRGBbuffer::getpos(int32_t x, int32_t y)
    {
        return ((y * ((m_idata.point.x * 3) + m_pad)) + (x * 3));
    }

    int32_t ImageRGBbuffer::getpad()
    {
        return (m_idata.point.x % 4);
    }

    ImageLite::BufferType ImageRGBbuffer::ispad(int32_t w)
    {
        return (((
                ((w) ? w : m_idata.point.x) % 4) == 0) ?
                    ImageLite::BufferType::IBT_NOPAD :
                    ImageLite::BufferType::IBT_PAD
            );
    }

    ImageLite::IPIX3 ImageRGBbuffer::getpixel(ImageLite::IPOINT<int32_t> const & p)
    {
        return getpixel(p.x, p.y);
    }

    ImageLite::IPIX3 ImageRGBbuffer::getpixel(int32_t x, int32_t y)
    {
        ImageLite::IPIX3 pix{};
        uint32_t pos = getpos(x, y);
        _check_buffer_pos(pos);
        ::memcpy(&pix, &m_idata.buffer[pos], sizeof(pix));
        return pix;
    }

    ImageLite::IPIX3 ImageRGBbuffer::getpixel(int32_t pos_)
    {
        ImageLite::IPIX3 pix{};
        uint32_t pos = static_cast<uint32_t>(pos_);
        _check_buffer_pos(pos);
        ::memcpy(&pix, &m_idata.buffer[pos], sizeof(pix));
        return pix;
    }

    ImageLite::ImageData & ImageRGBbuffer::getbuffer()
    {
        return m_idata;
    }

    void ImageRGBbuffer::_check_buffer_pos(uint32_t pos)
    {
        if (pos >= m_idata.buffer.size())
            throw_sys_error(error_bad_index);
    }

    int32_t ImageRGBbuffer::_set_stride()
    {
        int32_t pad = getpad();
        m_idata.stride = ((pad) ? ((m_idata.point.x * 3) + pad) : 0);
        return pad;
    }

    ImageLite::ImageData ImageRGBbuffer::getbmp()
    {
        ImageLite::ImageData icnvbmp{};
        icnvbmp.point       = m_idata.point;
        icnvbmp.stride      = m_idata.stride;
        ImageLite::IPOINT<uint32_t> ip = m_idata.point.get<ImageLite::IPOINT<uint32_t>, uint32_t>();

        if(!ImageLite::Formater::iconvert(
                &ImageLite::Formater::CONVERT_BMP,
                m_idata.buffer,
                icnvbmp.buffer,
                ip.x, ip.y, m_idata.stride, m_pad,
                nullptr
                ))
                throw_sys_error(error_buffer_empty);

        icnvbmp.header = getbmpheader();
        return icnvbmp;
    }

    ImageLite::IBMPHEADER ImageRGBbuffer::getbmpfileheader(uint32_t sz)
    {
        ImageLite::IBMPHEADER bmph{};
        bmph.fh.bfType = 0x4D42; // "BM"
        bmph.fh.bfSize = ((sz) ? sz : m_idata.buffer.size()) + sizeof(bmph);
        bmph.fh.bfOffBits = sizeof(bmph);
        bmph.ih = getbmpheader();
        return bmph;
    }

    ImageLite::IBITMAPINFOHEADER ImageRGBbuffer::getbmpheader()
    {
        ImageLite::IBITMAPINFOHEADER bmi{};
        bmi.biSize   = sizeof(bmi);
        bmi.biWidth  = m_idata.point.x;
        bmi.biHeight = m_idata.point.y;
        bmi.biPlanes = 1U;
        bmi.biBitCount = 24; //(b * 8);
        bmi.biCompression = 0x0000; // BI_RGB
        return bmi;
    }

    double ImageRGBbuffer::icompare(ImageLite::ImageTable & dst, uint8_t pc)
    {
        ImageLite::ImageDataP data{ {}, dst.data, { dst.width, dst.height }, 0 };
        ImageLite::IRECT<int32_t> r = { 0, 0, dst.width, dst.height };
        return ImageLite::Formater::icompare(
                m_idata,
                data,
                r,
                pc);
    }

    double ImageRGBbuffer::icompare(ImageLite::ImageRGBbuffer & dst, uint8_t pc)
    {
        ImageLite::ImageData & data = dst.getbuffer();
        ImageLite::IRECT<int32_t> r = { 0, 0, data.point.x, data.point.y };
        return ImageLite::Formater::icompare(
                m_idata,
                data,
                r,
                pc);
    }

    double ImageRGBbuffer::icompare(ImageLite::ImageRGBbuffer & dst, ImageLite::IRECT<int32_t> const & r, uint8_t pc)
    {
        ImageLite::ImageData & data = dst.getbuffer();
        return ImageLite::Formater::icompare(
                m_idata,
                data,
                r,
                pc);
    }

    double ImageRGBbuffer::icompare(ImageLite::ImageData const & dst, uint8_t pc)
    {
        ImageLite::IRECT<int32_t> r = { 0, 0, dst.point.x, dst.point.y };
        return ImageLite::Formater::icompare(
                m_idata,
                dst,
                r,
                pc);
    }

    double ImageRGBbuffer::icompare(ImageLite::ImageData const & dst, ImageLite::IRECT<int32_t> const & r, uint8_t pc)
    {
        return ImageLite::Formater::icompare(
                m_idata,
                dst,
                r,
                pc);
    }

    void ImageRGBbuffer::sepia()
    {
        if(!ImageLite::Formater::iconvert(
                &ImageLite::Formater::CONVERT_TO_SEPIA,
                m_idata.buffer,
                m_idata.buffer,
                m_idata.point.x, m_idata.point.y, m_idata.stride, m_pad,
                nullptr
            ))
            throw_sys_error(error_buffer_empty);

        if (!m_idata.buffer.size())
            throw_sys_error(error_bad_size);
    }

    void ImageRGBbuffer::grey()
    {
        if(!ImageLite::Formater::iconvert(
                &ImageLite::Formater::CONVERT_TO_GREY,
                m_idata.buffer,
                m_idata.buffer,
                m_idata.point.x, m_idata.point.y, m_idata.stride, m_pad,
                nullptr
            ))
            throw_sys_error(error_buffer_empty);

        if (!m_idata.buffer.size())
            throw_sys_error(error_bad_size);
    }

    void ImageRGBbuffer::bw()
    {
        if(!ImageLite::Formater::iconvert(
                &ImageLite::Formater::CONVERT_TO_BW,
                m_idata.buffer,
                m_idata.buffer,
                m_idata.point.x, m_idata.point.y, m_idata.stride, m_pad,
                nullptr
            ))
            throw_sys_error(error_buffer_empty);

        if (!m_idata.buffer.size())
            throw_sys_error(error_bad_size);
    }

    ImageLite::ImageType ImageRGBbuffer::getimgtype(std::string_view const & s)
    {
        std::size_t pos = s.find_last_of(".");
        if (pos == std::string::npos)
            return ImageLite::ImageType::IT_NONE;

        if (s.compare((pos + 1), 3, l_ifmt[0]) == 0)
            return ImageLite::ImageType::IT_BMP;
        if (s.compare((pos + 1), 3, l_ifmt[1]) == 0)
            return ImageLite::ImageType::IT_PNG;
        if (s.compare((pos + 1), 3, l_ifmt[2]) == 0)
            return ImageLite::ImageType::IT_JPG;
        if (s.compare((pos + 1), 3, l_ifmt[3]) == 0)
            return ImageLite::ImageType::IT_RAW;
        if (s.compare((pos + 1), 3, l_ifmt[4]) == 0)
            return ImageLite::ImageType::IT_ILUA;
        else
            return ImageLite::ImageType::IT_NONE;
    }

    void ImageRGBbuffer::load(std::string_view const & s, ImageType t)
    {
        if (t == ImageType::IT_NONE)
            if ((t = getimgtype(s)) == ImageType::IT_NONE)
                throw_sys_error(error_format_not_support);

        try
        {
            switch (t)
            {
                case ImageType::IT_PNG:
                    {
                        ImageLite::ImgBuffer icnvpng;
                        uint32_t w,
                                 h,
                                 err = lodepng::decode(icnvpng, w, h, s.data(), LCT_RGB, 8);

                        if (err)
                            throw_sys_error_ex(error_png_lib_error, lodepng_error_text(err));

                        if (!icnvpng.size())
                            throw_sys_error(error_buffer_empty);

                        m_idata.point.set<uint32_t>(w, h);
                        m_pad = _set_stride();

                        switch (ispad(m_idata.point.x))
                        {
                            case BufferType::IBT_PAD:
                                {
                                    if(!ImageLite::Formater::iconvert(
                                            &ImageLite::Formater::CONVERT_FROM_NOPAD,
                                            icnvpng,
                                            m_idata.buffer,
                                            m_idata.point.x, m_idata.point.y, m_idata.stride, m_pad,
                                            nullptr
                                        ))
                                        throw_sys_error(error_buffer_empty);

                                    if (!m_idata.buffer.size())
                                        throw_sys_error(error_bad_size);
                                    break;
                                }
                            case BufferType::IBT_NOPAD:
                                {
                                    m_idata.buffer = icnvpng;
                                    m_pad = _set_stride();
                                    break;
                                }
                            default:
                                return;
                        }
                        break;
                    }
                case ImageType::IT_BMP:
                    {
                        std::ifstream f(s.data(), std::ios::binary);

                        if (!f.is_open())
                            throw_sys_error(error_file_not_access);

                        ImageLite::IBMPHEADER bmph{};
                        f.read(reinterpret_cast<char*>(&bmph), sizeof(bmph));

                        if (bmph.fh.bfType != 0x4D42)
                            throw_sys_error(error_bmp_not_format);

                        if ((!bmph.ih.biWidth) || (!bmph.ih.biHeight))
                            throw_sys_error(error_bmp_header_not_size);

                        if (bmph.ih.biBitCount != 24)
                            throw_sys_error(error_bmp_color_not_24);

                        if (bmph.ih.biCompression != 0x0000)
                            throw_sys_error(error_bmp_format_not_rgb);

                        if ((!bmph.fh.bfOffBits) || (bmph.fh.bfSize <= bmph.fh.bfOffBits))
                            throw_sys_error(error_bmp_size_mismatch);

                        m_idata.point.set<long>(bmph.ih.biWidth, bmph.ih.biHeight);
                        m_pad = _set_stride();

                        uint32_t sz = (bmph.fh.bfSize - bmph.fh.bfOffBits);
                        ImageLite::ImgBuffer icnvbmp(sz);
                        f.read(reinterpret_cast<char*>(&icnvbmp[0]), sz);

                        if (icnvbmp.size() != sz)
                            throw_sys_error(error_bmp_size_mismatch);

                        if(!ImageLite::Formater::iconvert(
                                &ImageLite::Formater::CONVERT_BMP,
                                icnvbmp,
                                m_idata.buffer,
                                m_idata.point.x, m_idata.point.y, m_idata.stride, m_pad,
                                nullptr
                            ))
                            throw_sys_error(error_bmp_not_convertible);
                        break;
                    }
                case ImageType::IT_JPG:
                    // required NVJPEGP.DLL
                    // https://github.com/clclon/nvjpeg-for-MINGW-CUDA-Toolkit-10.x
#                   if defined(_WIN64)
                    {
                        JpegGpu::NVJpegDecoder jpgd;
                        if (!jpgd.isenable())
                            throw std::system_error(jpgd.error);

                        jpgd.imgread(s.data(), m_idata.buffer);
                        m_idata.point.set(jpgd.imgparam.width, jpgd.imgparam.height);
                        m_pad = jpgd.imgparam.pad;
                        break;
                    }
#                   endif
                case ImageType::IT_RAW:
                    {
                        throw_sys_error(error_format_not_support);
                    }
                default:
                    throw_sys_error(error_format_not_support);
            }
        }
        catch (std::system_error const & ex_)
        {
            throw;
        }
    }

    /// RGB bytes file format (BMP buffer, not reverse bytes)
    void ImageRGBbuffer::load_raw(std::string_view const & s, uint32_t w, uint32_t h, uint32_t s_)
    {
        try
        {
            if ((!w) || (!h))
                throw_sys_error(error_size_mismatch);
            if (s.empty())
                throw_sys_error(error_buffer_empty);

            std::ifstream f(s.data(), std::ios::binary | std::ios::ate);
            if (!f.is_open())
                throw_sys_error(error_file_not_access);


            f.seekg (0, std::ios::end);
            std::size_t sz = f.tellg();
            if (!sz)
                throw_sys_error(error_file_empty);

            f.seekg(0, std::ios::beg);
            m_idata.buffer.resize(sz);
            f.read(reinterpret_cast<char*>(&m_idata.buffer[0]), sz);

            m_idata.point.set<uint32_t>(w, h);

            if (!s_)
                m_pad = _set_stride();
            else
            {
                m_idata.stride = static_cast<int32_t>(s_);
                m_pad = getpad();
            }
            return;
        }
        catch (std::system_error const & ex_)
        {
            throw;
        }
    }

    void ImageRGBbuffer::save(std::string_view const & s, ImageType t)
    {
        if (!m_idata.buffer.size())
            throw_sys_error(error_buffer_empty);

        if (t == ImageType::IT_NONE)
            t = getimgtype(s);

        // end file name parse
        try
        {
            switch (t)
            {
                case ImageType::IT_PNG:
                    {
                        ImageLite::ImgBuffer dst;
                        ImageLite::ImgBuffer icnvpng;
                        ImageLite::IPOINT<uint32_t> ip = m_idata.point.get<ImageLite::IPOINT<uint32_t>, uint32_t>();
                        BufferType tt = ispad(m_idata.point.x);

                        if (tt == BufferType::IBT_PAD)
                        {
                            if(!ImageLite::Formater::iconvert(
                                    &ImageLite::Formater::CONVERT_TO_NOPAD,
                                    m_idata.buffer,
                                    icnvpng,
                                    ip.x, ip.y, m_idata.stride, m_pad,
                                    nullptr
                                ))
                                throw_sys_error(error_buffer_empty);

                            if (!icnvpng.size())
                                throw_sys_error(error_bad_size);

                            icnvpng.resize(ip.y * (ip.x * 3));
                        }
                        uint32_t err = lodepng::encode(
                                        dst,
                                        ((tt == BufferType::IBT_PAD) ? icnvpng : m_idata.buffer),
                                        ip.x,
                                        ip.y,
                                        LCT_RGB,
                                        8
                                    );
                        if (err)
                            throw_sys_error_ex(error_png_lib_error, lodepng_error_text(err));

                        if (!dst.size())
                            throw_sys_error(error_buffer_empty);

                        lodepng::save_file(dst, s.data());
                        break;
                    }
                case ImageType::IT_JPG:
                    {
                        ImageLite::ImgBuffer dst;
                        ImageLite::ImgBuffer icnvpng;
                        ImageLite::IPOINT<uint32_t> ip = m_idata.point.get<ImageLite::IPOINT<uint32_t>, uint32_t>();
                        BufferType tt = ispad(m_idata.point.x);

                        if (tt == BufferType::IBT_PAD)
                        {
                            if(!ImageLite::Formater::iconvert(
                                    &ImageLite::Formater::CONVERT_TO_NOPAD,
                                    m_idata.buffer,
                                    icnvpng,
                                    ip.x, ip.y, m_idata.stride, m_pad,
                                    nullptr
                                ))
                                throw_sys_error(error_buffer_empty);

                            if (!icnvpng.size())
                                throw_sys_error(error_bad_size);

                            icnvpng.resize(ip.y * (ip.x * 3));
                        }
                        if (!TooJpeg::writeJpeg(
                                ((tt == BufferType::IBT_PAD) ? icnvpng : m_idata.buffer),
                                ip.x,
                                ip.y,
                                dst,
                                TooJpeg::inputType::IMGTYPE_RGB,
                                90
                            ))
                            throw_sys_error(error_jpg_lib_error);

                        if (!dst.size())
                            throw_sys_error(error_buffer_empty);

                        std::ofstream f(s.data(), std::ios::binary | std::ios::ate);
                        if (!f.is_open())
                            throw_sys_error(error_file_not_access);

                        f.write (reinterpret_cast<const char*>(&dst[0]), dst.size());
                        f.close();
                        break;
                    }
                case ImageType::IT_RAW:
                    {
                        std::ofstream f(s.data(), std::ios::binary | std::ios::ate);
                        if (!f.is_open())
                            throw_sys_error(error_file_not_access);
                        f.write (reinterpret_cast<const char*>(&m_idata.buffer[0]), m_idata.buffer.size());
                        f.close();
                        break;
                    }
                case ImageType::IT_BMP:
                    {
                        ImageLite::ImgBuffer icnvbmp;
                        ImageLite::IPOINT<uint32_t> ip = m_idata.point.get<ImageLite::IPOINT<uint32_t>, uint32_t>();

                        if(!ImageLite::Formater::iconvert(
                                &ImageLite::Formater::CONVERT_BMP,
                                m_idata.buffer,
                                icnvbmp,
                                ip.x, ip.y, m_idata.stride, m_pad,
                                nullptr
                            ))
                            throw_sys_error(error_bmp_not_convertible);

                        if (!icnvbmp.size())
                            throw_sys_error(error_buffer_empty);

                        std::ofstream f(s.data(), std::ios::binary | std::ios::ate);
                        if (!f.is_open())
                            throw_sys_error(error_file_not_access);

                        ImageLite::IBMPHEADER bmph = getbmpfileheader(icnvbmp.size());
                        f.write (reinterpret_cast<const char*>(&bmph), sizeof(bmph));
                        f.write (reinterpret_cast<const char*>(&icnvbmp[0]), icnvbmp.size());
                        f.close();
                        break;
                    }
                case ImageType::IT_ILUA:
                    {
                        std::string valname;
                        std::ofstream f(s.data(), std::ios::binary | std::ios::ate);
                        if (!f.is_open())
                            throw_sys_error(error_file_not_access);

                        ImageLite::IPOINT<uint32_t> ip = m_idata.point.get<ImageLite::IPOINT<uint32_t>, uint32_t>();
                        {
                            std::size_t pos;
                            if ((pos = s.find_last_of("/\\")) != std::string::npos)
                                valname = s.substr(pos + 1, s.length() - 1);
                            else
                                valname = s;

                            if ((pos = valname.find_last_of(".")) != std::string::npos)
                                valname = valname.substr(0, pos);
                        }

                        f << valname << "_img = {\n";
                        f << "\twidth  = " << ip.x << ",\n";
                        f << "\theight = " << ip.y << ",\n";
                        f << "\titype  = " << (int)BufferType::IBT_RGB << ", -- IBT_RGB\n";
                        f << "\tdata   = {\n";

                        if(!ImageLite::Formater::itostream(
                                &ImageLite::Formater::STREAM_LUA_TABLE,
                                m_idata.buffer,
                                ip.x, ip.y, m_pad,
                                f
                            ))
                            throw_sys_error(error_bmp_not_convertible);

                        f << "\t}\n};\nreturn " << valname << "_img\n\n";
                        f.close();
                        break;
                    }
                default:
                    throw_sys_error(error_format_not_support);
            }
        }
        catch (std::system_error const & ex_)
        {
            throw;
        }
    }
} // name space end

