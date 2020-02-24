
#pragma once

namespace ImageLite
{
	class MJpeg
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
			using mjpeg_cb = std::function<bool(ImageLite::ImgBuffer const&)>;
			std::error_code error;
			//
			MJpeg();
			void stream(const char*, uint32_t, mjpeg_cb);
	};
}
