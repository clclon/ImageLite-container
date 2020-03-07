#ifndef HEADER_35EC05D7A99E57B7
#define HEADER_35EC05D7A99E57B7


#pragma once

namespace ImageLite
{
    namespace JpegGpu
    {
        enum ErrorId
        {
            error_begin = 0,
            error_nvjpeg_not_initialized,
            error_nvjpeg_invalid_parameter,
            error_nvjpeg_bad_jpeg,
            error_nvjpeg_jpeg_not_supported,
            error_nvjpeg_allocator_failure,
            error_nvjpeg_execution_failed,
            error_nvjpeg_arch_mismatch,
            error_nvjpeg_internal_error,
            error_nvjpeg_implementation_not_supported,
            error_bad_device_number = 1800,
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
        class DLL_EXPORT ErrorCat : public std::error_category
        {
            public:
                //
                const char* name() const noexcept override;
                std::string message(int32_t ev) const override;
        };
        const ErrorCat errCat;
        //
        std::error_code make_error_code(ErrorId);
    }
}

namespace std
{
    template <>
    struct is_error_code_enum<ImageLite::JpegGpu::ErrorId> : public true_type {};
}

#endif // header guard

