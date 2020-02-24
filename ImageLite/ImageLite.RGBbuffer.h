
#pragma once

namespace ImageLite
{
    /// BASE CLASS:
    /// ImageLite  = format buffer RGB + padding (BI_RGB)
    /// ImageLite = format buffer RGB, no padding (Android)
    /// all error convert to exception
    class ImageRGBbuffer
    {
        public:
            //
            using BufferType = ImageLite::BufferType;
            using ImageType  = ImageLite::ImageType;
            using SizeType   = ImageLite::SizeType;
            using ImageData  = ImageLite::ImageData;
            using ImgBuffer  = ImageLite::ImgBuffer;
            //
            ImageRGBbuffer();
            ImageRGBbuffer(ImageTable const&, BufferType = BufferType::IBT_RGB);
            ImageRGBbuffer(ImageData  const&, BufferType = BufferType::IBT_RGB_PAD);
            ImageRGBbuffer(ImageData  const&, ImageLite::IRECT<int32_t> const&,  BufferType = BufferType::IBT_RGB_PAD);  // get rectangle from source ImageData
            ImageRGBbuffer(ImageData  const&, ImageLite::IPOINT<int32_t> const&, BufferType = BufferType::IBT_RGB_PAD); // resize from source ImageData
            ImageRGBbuffer(ImgBuffer  const&, uint32_t, uint32_t, uint32_t = 0, BufferType = BufferType::IBT_RGB_PAD);
            ImageRGBbuffer(uint8_t const*, uint32_t, uint32_t, uint32_t, uint32_t = 0, BufferType = BufferType::IBT_RGB_PAD);
            ImageRGBbuffer(std::string_view const&, ImageType t = ImageType::IT_NONE);
            ImageRGBbuffer(std::string_view const&, uint32_t, uint32_t, uint32_t = 0, BufferType = BufferType::IBT_RGB_PAD);
            ~ImageRGBbuffer();
            //
            void                         load(std::string_view const&, ImageType t = ImageType::IT_NONE);
            void                         load_raw(std::string_view const&, uint32_t, uint32_t, uint32_t = 0);
            void                         save(std::string_view const&, ImageType t = ImageType::IT_NONE);
            bool                         empty();
            ImageLite::BufferType        ispad(int32_t = 0);
            int32_t                      getpad();
            ImageLite::IPOINT<int32_t>   getsize(SizeType);
            ImageLite::ImageData &       getbuffer();
            //
            int32_t                      getpos(ImageLite::IPOINT<int32_t> const&);
            int32_t                      getpos(int32_t, int32_t);
            //
            ImageLite::IPIX3             getpixel(int32_t);
            ImageLite::IPIX3             getpixel(ImageLite::IPOINT<int32_t> const&);
            ImageLite::IPIX3             getpixel(int32_t, int32_t);
            //
            ImageLite::ImageType         getimgtype(std::string_view const&);
            //
            ImageLite::ImageData         getbmp();
            ImageLite::IBITMAPINFOHEADER getbmpheader();
            ImageLite::IBMPHEADER        getbmpfileheader(uint32_t = 0);
            //
            double                       icompare(ImageLite::ImageTable&,      uint8_t = 1U);
            double                       icompare(ImageLite::ImageRGBbuffer&,  uint8_t = 1U);
            double                       icompare(ImageLite::ImageRGBbuffer&,  ImageLite::IRECT<int32_t> const&, uint8_t = 1U);
            double                       icompare(ImageLite::ImageData const&, uint8_t = 1U);
            double                       icompare(ImageLite::ImageData const&, ImageLite::IRECT<int32_t> const&, uint8_t = 1U);
            //
            void                         sepia();
            void                         grey();
            void                         bw();
            //
            template <class T>
            T get()
            {
                if constexpr (std::is_same<T, ImageLite::ImgBuffer>::value)
                {
                    return m_idata.buffer;
                }
                else if constexpr (std::is_same<T, ImageLite::ImageData>::value)
                {
                    return m_idata;
                }
                else if constexpr (std::is_same<T, ImageLite::ImageDataP>::value)
                {
                    return
                    {
                        m_idata.header,
                        m_idata.buffer,
                        m_idata.point,
                        m_idata.stride
                    };
                }
                else if constexpr (std::is_same<T, ImageLite::ImageTable>::value)
                {
                    return
                    {
                        m_idata.point.x,
                        m_idata.point.y,
                        0,
                        m_idata.buffer
                    };
                }
            }

        private:
            //
            int32_t                 m_pad = 0;
            ImageLite::ImageData    m_idata{};
            //
            template <typename T>
            void                    _init(T const&, ImageData&, BufferType);
            void                    _check_buffer_pos(uint32_t);
            int32_t                 _set_stride();
    };
}
