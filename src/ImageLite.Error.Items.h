/*
    static inline const char l_err_01[] = "image buffer empty";
    static inline const char l_err_02[] = "image size mismatch";
    static inline const char l_err_03[] = "image pointer size mismatch";
    static inline const char l_err_04[] = "image pointer is null";
    static inline const char l_err_05[] = "image file name empty";
    static inline const char l_err_06[] = "image buffer bad index";
    static inline const char l_err_07[] = "image buffer bad size";
    static inline const char l_err_08[] = "image file not access";
    static inline const char l_err_09[] = "image file empty";
    static inline const char l_err_10[] = "image format not support";
    static inline const char l_err_11[] = "not support, use load_raw() function";
    static inline const char l_err_12[] = "image rectangle size is large";
    static inline const char l_err_13[] = "not crop image rectangle";
    static inline const char l_err_14[] = "image buffer not support type";
    // BMP errors
    static inline const char l_err_21[] = "image not bitmap format";
    static inline const char l_err_22[] = "bitmap image header not correct size";
    static inline const char l_err_23[] = "bitmap image color not 24";
    static inline const char l_err_24[] = "bitmap image format not RGB (BGR)";
    static inline const char l_err_25[] = "bitmap image header size mismatch";
    static inline const char l_err_26[] = "bitmap image format not convertible";
    static inline const char l_err_27[] = "bitmap image incorrect data size";

throw std::system_error(ImageLite::make_error_code(ImageLite::ErrorId::error_imgl_));
ImageLite::errCat.message(error_imgl_)
throw std::system_error(ImageLite::make_error_code(ImageLite::ErrorId::error_imgl_size_is_large));
throw std::system_error(ImageLite::make_error_code(ImageLite::ErrorId::error_imgl_png_lib_error), lodepng_error_text(err));

*/

__ERRITEM(buffer_empty,       "image buffer empty")
__ERRITEM(size_mismatch,      "image size mismatch")
__ERRITEM(pointer_size,       "image pointer size mismatch")
__ERRITEM(pointer_null,       "image pointer is null")
__ERRITEM(file_name_empty,    "image file name empty")
__ERRITEM(bad_index,          "image buffer bad index")
__ERRITEM(bad_size,           "image buffer bad size")
__ERRITEM(file_not_access,    "image file not access")
__ERRITEM(file_empty,         "image file empty")
__ERRITEM(format_not_support, "image format not support")
__ERRITEM(not_support,        "not support, use load_raw() function")
__ERRITEM(size_is_large,      "image rectangle size is large")
__ERRITEM(not_crop,           "not crop image rectangle")
__ERRITEM(not_support_type,   "image buffer not support type")
__ERRITEM(png_lib_error,      "png image error:")
__ERRITEM(jpg_lib_error,      "jpg library error")
    // BMP errors
__ERRITEM(bmp_not_format,     "image not bitmap format")
__ERRITEM(bmp_header_not_size,"bitmap image header not correct size")
__ERRITEM(bmp_color_not_24,   "bitmap image color not 24")
__ERRITEM(bmp_format_not_rgb, "bitmap image format not RGB (BGR)")
__ERRITEM(bmp_size_mismatch,  "bitmap image header size mismatch")
__ERRITEM(bmp_not_convertible,"bitmap image format not convertible")
__ERRITEM(bmp_incorrect_size, "bitmap image incorrect data size")

#undef __ERRITEM
