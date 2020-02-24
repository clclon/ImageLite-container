
#pragma once

/** @file
 @page lua_data Lua data table API
 @brief Lua virtual tables help.
 */

namespace ImageLite
{
    /// DATA:
    /// all image structures and enumerator
    ///
            enum BufferType
            {
                IBT_RGB = 0,
                IBT_BGR,
                IBT_RGB_PAD,
                IBT_BGR_PAD,
                IBT_BGRA,
                IBT_RGBA,
                IBT_PAD,
                IBT_NOPAD,
                IBT_NONE
            };
            //
            enum ImageType
            {
                IT_BMP,
                IT_PNG,
                IT_JPG,
                IT_RAW,
                IT_ILUA,
                IT_NONE
            };
            //
            enum SizeType
            {
                IST_BUFFER_BYTES,
                IST_CALC_BYTES,
                IST_IMAGE_WH_POINT,
                IST_NONE
            };
            //
            struct IPIX3
            {
                uint8_t c1, c2, c3;
            };
            struct IPIX4
            {
                uint8_t c1, c2, c3, c4;
            };
            //
            typedef struct __attribute__((__packed__))
            {
                uint16_t  bfType;
                uint32_t  bfSize;
                uint32_t  bfReserved; // 1,2
                uint32_t  bfOffBits;
            } IBITMAPFILEHEADER;
            //
            typedef struct __attribute__((__packed__))
            {
                uint32_t biSize;
                long     biWidth;
                long     biHeight;
                uint16_t biPlanes;
                uint16_t biBitCount;
                uint32_t biCompression;
                uint32_t biSizeImage;
                long     biXPelsPerMeter;
                long     biYPelsPerMeter;
                uint32_t biClrUsed;
                uint32_t biClrImportant;
            } IBITMAPINFOHEADER;
            //
            typedef struct
            {
                ImageLite::IBITMAPFILEHEADER fh;
                ImageLite::IBITMAPINFOHEADER ih;
            } IBMPHEADER;
            //
            using ImgBuffer = std::vector<uint8_t>;
            /*
                // Converter callback
                void (
                      buffer source,
                      buffer destination,
                      width,
                      height,
                      x [w iterate],
                      y [h iterate],
                      s [stride],
                      l [width bytes length]
                      v [user data void*] )
            */
            using icovert_cb = void (
                    ImageLite::ImgBuffer const&,
                    ImageLite::ImgBuffer&,
                    uint32_t,
                    uint32_t,
                    uint32_t,
                    uint32_t,
                    uint32_t,
                    uint32_t,
                    void*
                );
            using icompare_cb = uint32_t (
                    ImgBuffer const&,
                    ImgBuffer const&,
                    uint32_t,
                    uint32_t,
                    uint32_t,
                    uint32_t,
                    uint32_t,
                    uint8_t
                );

#           define icovert_BIND(F)                 \
                std::bind(&F,                      \
                          this,                    \
                          std::placeholders::_1,   \
                          std::placeholders::_2,   \
                          std::placeholders::_3,   \
                          std::placeholders::_4,   \
                          std::placeholders::_5,   \
                          std::placeholders::_6,   \
                          std::placeholders::_7,   \
                          std::placeholders::_8,   \
                          std::placeholders::_9)
            //
            struct ImageData
            {
                ImageLite::IBITMAPINFOHEADER      header;
                ImageLite::ImgBuffer              buffer;
                ImageLite::IPOINT<int32_t>        point;
                int32_t                           stride;
            };
            //
            struct ImageDataP
            {
                ImageLite::IBITMAPINFOHEADER      header;
                ImageLite::ImgBuffer const &      buffer;
                ImageLite::IPOINT<int32_t>        point;
                int32_t                           stride;
            };
            //
            /**
             @brief ImageTable, main image container to Lua API
             @luadata { width, height, itype, data{{ R,G,B },..}}
             @luacode
             @verbatim
                local imgTbl = {
                    width  = 1024,
                    height = 720,
                    itype  = 0,
                    data = {
                        255, 255, 255,
                        255, 255, 127,
                        255, 255, 80
                    }
                }
             @endverbatim
            */
            struct ImageTable
            {
                /**< image width */
                int32_t width  = 0;
                /**< image height */
                int32_t height = 0;
                /**< image type, always = 0 */
                int32_t itype  = 0;
                /**< image RGB linear buffer */
                ImageLite::ImgBuffer data;

                ImageTable() {}
                ImageTable(int32_t w_, int32_t h_)
                    : width(w_), height(h_) {}
                ImageTable(int32_t w_, int32_t h_, int32_t t_, ImageLite::ImgBuffer & b_)
                    : width(w_), height(h_), itype(t_), data(b_) {}
                ImageTable(ImageData & idt_)
                    : width(idt_.point.x), height(idt_.point.y)
                {
                    data = std::move(idt_.buffer);
                }
            };
            //
}
