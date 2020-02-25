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

#include "NVJpegDecoder.Internal.h"
#if defined(_DEBUG)
#  include <ctime>
#endif

#if (defined(_BUILD_AVX2) || defined(__AVX2__))
namespace ImageLite
{
    namespace JpegGpuAVX2
    {
        void tobuffer_avx2(
			ImageLite::ImgBuffer const&,
			ImageLite::ImgBuffer const&,
			ImageLite::ImgBuffer const&,
			int32_t,
			ImageLite::JpegGpu::NVJpegDecoder::gpuimage_t&,
			ImageLite::ImgBuffer&,
			std::error_code&);
        void initavx2_internal();
    }
}
#endif

namespace ImageLite
{
	namespace JpegGpu
	{
		void tobuffer_native(
			ImageLite::ImgBuffer const&,
			ImageLite::ImgBuffer const&,
			ImageLite::ImgBuffer const&,
			int32_t,
			NVJpegDecoder::gpuimage_t&,
			ImageLite::ImgBuffer&,
			std::error_code&
		);

		static int32_t gpu_malloc(void** p, size_t s) noexcept
		{
			return static_cast<int32_t>(cudaMalloc(p, s));
		}
		static int32_t gpu_free(void* p) noexcept
		{
			return static_cast<int32_t>(cudaFree(p));
		}

		NVJpegDecoder::NVJpegDecoder() noexcept
		{
			nvparam = std::make_unique<gpuparams_t>();
			init();
		}

		NVJpegDecoder::NVJpegDecoder(int32_t did) noexcept
		{
			nvparam = std::make_unique<gpuparams_t>();
			nvparam->did = did;
			init();
		}

		NVJpegDecoder::~NVJpegDecoder()
		{
			clean();
		}

		void NVJpegDecoder::clear()
		{
		    clean();
            for (int32_t i = 0; i < NVJPEG_MAX_COMPONENT; i++)
            {
                nvparam->imgout.channel[i] = nullptr;
                nvparam->imgout.pitch[i] = 0;
                nvparam->imgsz.pitch[i] = 0;
            }
            imgparam = {};
		}

		void NVJpegDecoder::clean()
		{
			try
			{
				CudaIsErrorsEc(cudaStreamDestroy(nvparam->stream));
				for (int32_t i = 0; i < NVJPEG_MAX_COMPONENT; i++)
					if (nvparam->imgout.channel[i])
						CudaIsErrorsEc(cudaFree(nvparam->imgout.channel[i]));

				CudaIsErrorsEc(nvjpegJpegStateDestroy(nvparam->state));
				CudaIsErrorsEc(nvjpegDestroy(nvparam->handle));
			}
			catch (...) {}
		}

		bool NVJpegDecoder::isenable()
		{
			return nvisenable;
		}

		const char* NVJpegDecoder::cudaGetError(uint32_t err)
		{
			return cudaGetErrorName(static_cast<cudaError_t>(err));
		}

		void NVJpegDecoder::init() noexcept
		{
		    try
		    {
		        nvparam->gpuallocator = { &gpu_malloc, &gpu_free };
		        nvparam->outfmt = NVJPEG_OUTPUT_RGB;

		        CudaIsErrorsEc(nvjpegCreateSimple(&nvparam->handle));
		        CudaIsErrorsEc(nvjpegJpegStateCreate(nvparam->handle, &nvparam->state));

		        int32_t devcnt = 0, cmode = -1, major = 0, minor = 0;
		        CudaIsErrorsEc(cudaGetDeviceCount(&devcnt));

		        if (nvparam->did < 0)
                    nvparam->did = 0;

                if (nvparam->did >= devcnt)
                {
                    error = JpegGpu::make_error_code(JpegGpu::ErrorId::error_bad_device_number);
                    return;
                }

                CudaIsErrorsEc(cudaDeviceGetAttribute(&cmode, cudaDevAttrComputeMode, nvparam->did));
                CudaIsErrorsEc(cudaDeviceGetAttribute(&major, cudaDevAttrComputeCapabilityMajor, nvparam->did));
                CudaIsErrorsEc(cudaDeviceGetAttribute(&minor, cudaDevAttrComputeCapabilityMinor, nvparam->did));
                if (cmode == cudaComputeModeProhibited)
                {
                    error = JpegGpu::make_error_code(JpegGpu::ErrorId::error_bad_device_mode);
                    return;
                }
                if (major < 1)
                {
                    error = JpegGpu::make_error_code(JpegGpu::ErrorId::error_device_not_support);
                    return;
                }
                CudaIsErrorsEc(cudaSetDevice(nvparam->did));

                for (int32_t i = 0; i < NVJPEG_MAX_COMPONENT; i++)
                {
                    nvparam->imgout.channel[i] = nullptr;
                    nvparam->imgout.pitch[i] = 0;
                    nvparam->imgsz.pitch[i] = 0;
                }
                CudaIsErrorsEc(cudaStreamCreateWithFlags(&nvparam->stream, cudaStreamNonBlocking));
#               if (!defined(_BUILD_DLL) && (defined(_BUILD_AVX2) || defined(__AVX2__)))
                if (Helper::intrin.getcpuid() == Helper::Intrin::CpuId::CPU_AVX2)
                    ImageLite::JpegGpuAVX2::initavx2_internal();
#               endif
                nvisenable = true;
		    }
		    catch(std::system_error const & ex_)
		    {
		        error = ex_.code();
		        return;
		    }
		    catch(...)
		    {
		        return;
		    }
		}

		void NVJpegDecoder::imgalloc()
		{
			if ((imgparam.width) && (imgparam.width == static_cast<int32_t>(nvparam->imgout.pitch[0])))
				return;

			uint32_t const sz = (imgparam.width * imgparam.height);
			for (int32_t i = 0; i < imgparam.channel; i++)
			{
				if (nvparam->imgout.channel[i])
					CudaIsErrorsEc(cudaFree(nvparam->imgout.channel[i]));
				CudaIsErrorsEc(cudaMalloc(&nvparam->imgout.channel[i], sz));
				nvparam->imgout.pitch[i] = imgparam.width;
				nvparam->imgsz.pitch[i] = sz;
			}
		}

		void NVJpegDecoder::imginfo(ImageLite::ImgBuffer const& data)
		{
			CudaIsErrorsEc(nvjpegGetImageInfo(nvparam->handle, &data[0], data.size(), &nvparam->components, &nvparam->chroma, nvparam->widths, nvparam->heights));
			CudaIsErrorsEc(cudaStreamSynchronize(nvparam->stream));
			imgparam.width = nvparam->widths[0];
			imgparam.height = nvparam->heights[0];
			imgparam.channel = nvparam->components;
			imgparam.pitch = (imgparam.channel * imgparam.width);
			imgparam.pad = (imgparam.width % 4);

			switch (nvparam->chroma)
			{
				case NVJPEG_CSS_444:
				case NVJPEG_CSS_440:
				case NVJPEG_CSS_422:
				case NVJPEG_CSS_420:
				case NVJPEG_CSS_411:
				case NVJPEG_CSS_410:
				case NVJPEG_CSS_GRAY:
					break;
				//case NVJPEG_CSS_UNKNOWN:
				default:
				{
					error = JpegGpu::make_error_code(JpegGpu::ErrorId::error_unknown_chroma);
					throw std::system_error(error);
				}
			}

			if (!imgparam.width || !imgparam.height || !imgparam.channel)
			{
				error = JpegGpu::make_error_code(JpegGpu::ErrorId::error_bad_image);
				throw std::system_error(error);
			}
			if (imgparam.channel >= NVJPEG_MAX_COMPONENT)
			{
				error = JpegGpu::make_error_code(JpegGpu::ErrorId::error_bad_channel);
				throw std::system_error(error);
			}

			imgalloc();
		}

		bool NVJpegDecoder::imgstream(ImageLite::ImgBuffer const& idata, ImageLite::ImgBuffer& odata)
		{
			if (!nvisenable)
				return false;

			try
			{
			    if (imgparam.empty())
                    imginfo(idata);

				odata.clear();
				imgdecompress(idata, odata);
			}
			catch (...)
			{
				error = JpegGpu::make_error_code(JpegGpu::ErrorId::error_decompress);
				throw;
			}
			return true;
		}

		bool NVJpegDecoder::imgread(std::string const& s, ImageLite::ImgBuffer& odata)
		{
			if (!nvisenable)
				return false;

			try
			{
				std::ifstream f(s, std::ios::in | std::ios::binary | std::ios::ate);
				if (!f.is_open())
				{
					error = JpegGpu::make_error_code(JpegGpu::ErrorId::error_file_access);
					return false;
				}

				uint32_t const sz = static_cast<uint32_t>(f.tellg());
				ImageLite::ImgBuffer idata(sz);

				f.seekg(0, std::ios::beg);
				f.read(reinterpret_cast<char*>(&idata[0]), sz);
				f.close();

				odata.clear();
				imginfo(idata);
				imgdecompress(idata, odata);
			}
			catch (...)
			{
				error = JpegGpu::make_error_code(JpegGpu::ErrorId::error_cuda_api);
				throw;
			}
			return true;
		}

		void NVJpegDecoder::imgdecompress(ImageLite::ImgBuffer const& idata, ImageLite::ImgBuffer& odata)
		{
			try
			{
				CudaIsErrorsEc(nvjpegDecodePhaseOne(nvparam->handle, nvparam->state, &idata[0], idata.size(), nvparam->outfmt, nvparam->stream));
				CudaIsErrorsEc(cudaStreamSynchronize(nvparam->stream));
				CudaIsErrorsEc(nvjpegDecodePhaseTwo(nvparam->handle, nvparam->state, nvparam->stream));
				CudaIsErrorsEc(nvjpegDecodePhaseThree(nvparam->handle, nvparam->state, &nvparam->imgout, nvparam->stream));
				CudaIsErrorsEc(cudaStreamSynchronize(nvparam->stream));
				imgbuffer(odata);
			}
			catch (...)
			{
#			        if defined(_DEBUG)
			        do
                {
                    uint32_t t = static_cast<uint32_t>(std::time(nullptr));
                    std::string s = "save-bad-jpg-source-";
                    s += std::to_string(t).c_str();
                    s += ".jpg";
                    std::ofstream f1(s, std::ios::binary | std::ios::ate);
                    if (!f1.is_open())
                        break;

                    f1.write(reinterpret_cast<char*>(const_cast<uint8_t*>(&idata[0])), idata.size());
                    f1.close();

                    if (!odata.size())
                        break;

                    s += ".raw";
                    std::ofstream f2(s, std::ios::binary | std::ios::ate);
                    if (!f2.is_open())
                        break;

                    f2.write(reinterpret_cast<char*>(const_cast<uint8_t*>(&odata[0])), odata.size());
                    f2.close();

                }
                while (0);
#			        endif
				clear();
				throw;
			}
		}

		void NVJpegDecoder::imgbuffer(ImageLite::ImgBuffer& odata)
		{
			int32_t const sz = (imgparam.height * imgparam.width);
			ImageLite::ImgBuffer Rc(sz);
			ImageLite::ImgBuffer Gc(sz);
			ImageLite::ImgBuffer Bc(sz);

			CudaIsErrors(cudaMemcpy2D(&Rc[0], static_cast<size_t>(imgparam.width), nvparam->imgout.channel[0], static_cast<size_t>(nvparam->imgout.pitch[0]), imgparam.width, imgparam.height, cudaMemcpyDeviceToHost));
			CudaIsErrors(cudaMemcpy2D(&Gc[0], static_cast<size_t>(imgparam.width), nvparam->imgout.channel[1], static_cast<size_t>(nvparam->imgout.pitch[1]), imgparam.width, imgparam.height, cudaMemcpyDeviceToHost));
			CudaIsErrors(cudaMemcpy2D(&Bc[0], static_cast<size_t>(imgparam.width), nvparam->imgout.channel[2], static_cast<size_t>(nvparam->imgout.pitch[2]), imgparam.width, imgparam.height, cudaMemcpyDeviceToHost));

			odata.resize(imgparam.size());
#			if (defined(_BUILD_AVX2) || defined(__AVX2__))
			if (Helper::intrin.getcpuid() == Helper::Intrin::CpuId::CPU_AVX2)
				ImageLite::JpegGpuAVX2::tobuffer_avx2(Rc, Gc, Bc, sz, imgparam, odata, error);
			else
#			endif
				tobuffer_native(Rc, Gc, Bc, sz, imgparam, odata, error);
		}
	}
}
