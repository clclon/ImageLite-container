#ifndef HEADER_38D00353AD0DEE2
#define HEADER_38D00353AD0DEE2


#pragma once

#include <memory>
#include <vector>
#include <cassert>
#include <string>
#include <sstream>
#include <fstream>
#include <system_error>
#include <nvjpeg.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#if defined(_MSC_VER)
#pragma warning( disable : 26446 )
#pragma warning( disable : 26451 )
#pragma warning( disable : 26471 )
#pragma warning( disable : 26472 )
#pragma warning( disable : 26481 )
#pragma warning( disable : 26482 )
#pragma warning( disable : 26485 )
#pragma warning( disable : 26490 )
#pragma warning( disable : 26812 )
#endif

#include "../ImageLite.h"
#include "NVJpegDecoder.Error.h"
#include "NVJpegDecoder.h"
#include <HelperIntrin.h>

namespace ImageLite
{
	namespace JpegGpu
	{
		struct gpuparams_t
		{
			nvjpegJpegState_t    state;
			nvjpegHandle_t       handle;
			nvjpegImage_t        imgout;
			nvjpegImage_t        imgsz;
			cudaStream_t         stream;

			int32_t              did,
				components,
				widths[NVJPEG_MAX_COMPONENT]{},
				heights[NVJPEG_MAX_COMPONENT]{};

			nvjpegChromaSubsampling_t chroma;
			nvjpegDevAllocator_t      gpuallocator;
			nvjpegOutputFormat_t      outfmt; // only NVJPEG_OUTPUT_RGB;
		};

		template <typename T>
		void iserror(T result, char const* const func, int const line)
		{
			if (result)
			{
				uint32_t const err = static_cast<uint32_t>(result);
				std::error_code const ec = ImageLite::JpegGpu::make_error_code(
                                                ((err > 9) ?
                                                    JpegGpu::ErrorId::error_cuda_api :
                                                    static_cast<JpegGpu::ErrorId>(err))
                                            );
				std::stringstream ss;
				ss << func << ":" << line << " code:" << err;
				ss << " (" << NVJpegDecoder::cudaGetError(err) << ")";
				throw std::system_error(ec, ss.str());
			}
		}
		template <typename T>
		void iserrorEc(std::error_code& ec, T result, char const* const func, int const line)
		{
			if (result)
			{
				uint32_t err = static_cast<uint32_t>(result);
				ec = JpegGpu::make_error_code(
                        ((err > 9) ?
                            JpegGpu::ErrorId::error_cuda_api :
                            static_cast<JpegGpu::ErrorId>(err))
                    );
				std::stringstream ss;
				ss << func << ":" << line << " code:" << err;
				ss << " (" << NVJpegDecoder::cudaGetError(err) << ")";
				throw std::system_error(ec, ss.str());
			}
		}
#       define CudaIsErrors(val) iserror((val), #val, __LINE__)
#       define CudaIsErrorsEc(val) iserrorEc(error, (val), #val, __LINE__)
	}
}

#endif // header guard 

