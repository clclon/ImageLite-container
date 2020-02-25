#ifndef HEADER_56CE205B424D49E9
#define HEADER_56CE205B424D49E9


#pragma once

namespace ImageLite
{
	namespace JpegGpu
	{
		struct gpuparams_t;
		//
		class DLL_EXPORT NVJpegDecoder
		{
		public:
			//
			struct gpuimage_t
			{
				int32_t height, width, channel, pitch, pad;
				int32_t size() noexcept
				{
					return ((pitch + pad) * height);
				}
				bool empty() noexcept
				{
					return (!height || !width || !channel || !pitch);
				}
				void clear() noexcept
				{
					height = width = channel = pitch = pad = 0;
				}
				void info(std::ostream& os) const
				{
					os << "width: " << width;
					os << ", height: " << height;
					os << ", channel: " << channel;
					os << ", pitch: " << pitch;
					os << ", pad: " << pad;
				}
				friend std::ostream& operator << (std::ostream& os, gpuimage_t const& obj)
				{
					obj.info(os);
					return os;
				}
			};
		private:
			//
			std::unique_ptr<gpuparams_t> nvparam;
			bool                         nvisenable = false;
			//
			void init() noexcept;
			void clean();
			void imgalloc();
			void imginfo(ImageLite::ImgBuffer const&);
			void imgbuffer(ImageLite::ImgBuffer&);
			void imgdecompress(ImageLite::ImgBuffer const&, ImageLite::ImgBuffer&);

		public:
			//
			gpuimage_t imgparam{};
			std::error_code error;
			//
			NVJpegDecoder() noexcept;
			NVJpegDecoder(int32_t) noexcept;
			~NVJpegDecoder();
			//
			bool isenable();
			bool imgread(std::string const&, ImageLite::ImgBuffer&);
			bool imgstream(ImageLite::ImgBuffer const&, ImageLite::ImgBuffer&);
			void clear();
			//
			static const char* cudaGetError(uint32_t);
		};
	}
}
#endif // header guard

