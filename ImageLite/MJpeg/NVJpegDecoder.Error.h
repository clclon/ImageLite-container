
#pragma once

namespace ImageLite
{
    namespace JpegGpu
    {
        enum ErrorId
        {
            error_begin = 0,
            error_bad_device_number,
            error_bad_device_mode,
            error_device_not_support,
            error_unknown_chroma,
            error_bad_image,
            error_bad_channel,
            error_bad_imgstream,
            error_decompress,
            error_cuda_api,
            error_file_access,
            error_sys_objectnull,
            error_sys_execpt,
            error_not_support_avx,
            error_unknown,
            error_end
        };
        class ErrorCat : public std::error_category
        {
        public:
            //
            const char* name() const noexcept override;
            std::string message(int32_t ev) const override;
        };
        const ErrorCat errCat;
        //
        std::error_code make_error_code(ErrorId);
        std::string geterror(int32_t);
        const char* geterrorc(int32_t);
    }
}

namespace std
{
    template <>
    struct is_error_code_enum<ImageLite::JpegGpu::ErrorId> : public true_type {};
}

