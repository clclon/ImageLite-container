#ifndef HEADER_BE850F592D131FB9
#define HEADER_BE850F592D131FB9


#pragma once

namespace ImageLite
{
	class DLL_EXPORT MJpeg
	{
		public:
			//
			enum StageType
			{
			    MST_BEGIN,
			    MST_END,
			    MST_UNKNOWN
			};

		private:
			//
			StageType              m_state = StageType::MST_UNKNOWN;
			ImageLite::ImgBuffer   m_bimg;
			JpegGpu::NVJpegDecoder m_dec;
			std::array<char, 2>    m_prev{};
			//
			bool parse(const char*, const int32_t, ImageLite::ImgBuffer&, int32_t&);

		public:
			//
			using mjpeg_cb = std::function<bool(ImageLite::ImgBuffer const&,int32_t const, int32_t const)>;
			std::error_code error;
			//
			MJpeg();
			void stream(const char*, uint32_t, mjpeg_cb);
			void clear();
	};
}
#endif // header guard

