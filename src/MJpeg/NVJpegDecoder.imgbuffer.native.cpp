
#include "NVJpegDecoder.Internal.h"

namespace ImageLite
{
    namespace JpegGpu
    {
        void tobuffer_native(
            ImageLite::ImgBuffer const& Rc,
            ImageLite::ImgBuffer const& Gc,
            ImageLite::ImgBuffer const& Bc,
            [[maybe_unused]] int32_t sz,
            NVJpegDecoder::gpuimage_t& param,
            ImageLite::ImgBuffer& odata,
            std::error_code& /* ec */
        )
        {
            int32_t y;
#           pragma omp parallel for private(y) schedule(static)
            for (y = 0; y < param.height; y++)
            {
                int32_t yo = (y * (param.pitch + param.pad));
                int32_t const yi = (y * param.width);

                for (int32_t x = 0, n = 0; ((x < param.width) && (n < param.pitch)); x++, n += 3)
                {
                    assert(static_cast<int32_t>(odata.size()) > (yo + n + 2));
                    int32_t const pos = (yi + x);
                    assert(sz > pos);

                    odata[yo + n + 0] = static_cast<uint8_t>(Rc[pos]);
                    odata[yo + n + 1] = static_cast<uint8_t>(Gc[pos]);
                    odata[yo + n + 2] = static_cast<uint8_t>(Bc[pos]);
                }
                if (param.pad)
                {
                    yo += param.pitch;
                    for (int32_t i = 0; i < param.pad; i++)
                        odata[yo + i] = 0U;
                }
            }
        }
    }
}

