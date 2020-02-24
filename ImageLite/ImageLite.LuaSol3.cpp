
#if __has_include(<ARemote.h>)
#  include <ARemote.h>
#  define _BUILD_APP_AREMOTE 1
#endif
#include "ImageLite.LuaSol3.h"

namespace ImageLite
{
    //
    // LUA PROXY ImageLite::ImageRGBbuffer as object
    //
    LuaImage::LuaImage() {}
    LuaImage::LuaImage(std::string s)
    {
        try
        {
            img_.load(s);
        }
        catch (...) { LOGEXCEPT(); }
    }
    LuaImage::LuaImage(ImageLite::ImageTable tbl)
    {
        setimg(tbl);
    }
    void LuaImage::set(ImageLite::ImageTable tbl)
    {
        setimg(tbl);
    }
    void LuaImage::setimg(ImageLite::ImageTable & tbl)
    {
        try
        {
            ImageLite::ImageRGBbuffer tmp(tbl, ImageLite::BufferType::IBT_RGB);
            img_ = std::move(tmp);
        }
        catch (...) { LOGEXCEPT(); }
    }
    ImageLite::ImageTable LuaImage::get()
    {
        try
        {
            return img_.get<ImageLite::ImageTable>();
        }
        catch (...) { LOGEXCEPT(); return ImageLite::ImageTable(); }
    }
    double LuaImage::compare(ImageLite::ImageTable tbl)
    {
        try
        {
            ImageLite::ImageRGBbuffer tmp(tbl, ImageLite::BufferType::IBT_RGB);
            return img_.icompare(tmp);
        }
        catch (...) { LOGEXCEPT(); return 0.0; }
    }
    double LuaImage::compare_region(ImageLite::ImageTable tbl, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
    {
        try
        {
            ImageLite::ImageRGBbuffer tmp(tbl, ImageLite::BufferType::IBT_RGB);
            ImageLite::IRECT<int32_t> r(x, y, w, h);
            return img_.icompare(tmp, r);
        }
        catch (...) { LOGEXCEPT(); return 0.0; }
    }
    std::tuple<uint8_t,uint8_t,uint8_t> LuaImage::getpixel_xy(int32_t x, int32_t y)
    {
        try
        {
            ImageLite::IPIX3 pix =  img_.getpixel(x, y);
            return { pix.c1, pix.c2, pix.c3 };
        }
        catch (...) { LOGEXCEPT(); return { 0U, 0U, 0U }; }
    }
    std::tuple<uint8_t,uint8_t,uint8_t> LuaImage::getpixel_pos(int32_t pos)
    {
        try
        {
            ImageLite::IPIX3 pix =  img_.getpixel(pos);
            return { pix.c1, pix.c2, pix.c3 };
        }
        catch (...) { LOGEXCEPT(); return { 0U, 0U, 0U }; }
    }
    std::tuple<int32_t,int32_t> LuaImage::size() // image dimension
    {
        try
        {
            ImageLite::IPOINT<int32_t> ip =  img_.getsize(ImageLite::SizeType::IST_IMAGE_WH_POINT);
            return { ip.x, ip.y };
        }
        catch (...) { LOGEXCEPT(); return { 0, 0 }; }
    }
    uint32_t LuaImage::length() // image buffer size
    {
        try
        {
            ImageLite::IPOINT<int32_t> ip =  img_.getsize(ImageLite::SizeType::IST_BUFFER_BYTES);
            return static_cast<uint32_t>(ip.x);
        }
        catch (...) { LOGEXCEPT(); return 0; }
    }
    bool LuaImage::empty()
    {
        try { return img_.empty(); }
        catch (...) { LOGEXCEPT(); return true; }
    }
    bool LuaImage::capture()
    {
#       if defined(_BUILD_APP_AREMOTE)
        if (!Conf.class_display)
            return false;

        try
        {

            App::CaptureTrigger cap;
            //
            ImageLite::ImgBuffer ifb;
            Conf.class_display->imageCapture(ifb, Event::ADisplay::ImgTypeOut::T_RAW_NOPAD);
            ImageLite::ImageRGBbuffer tmp(
                        ifb,
                        Conf.display.w,
                        Conf.display.h,
                        0U,
                        ImageLite::BufferType::IBT_RGB
                );
            if (tmp.empty())
                return false;

            img_ = std::move(tmp);
            return true;
        }
        catch (...) { LOGEXCEPT(); return false; }
#       else
        return false;
#       endif
    }
    void LuaImage::load(std::string const & s)
    {
        try { img_.load(s); }
        catch (...) { LOGEXCEPT(); }
    }
    void LuaImage::save(std::string const & s)
    {
        try { img_.save(s); }
        catch (...) { LOGEXCEPT(); }
    }
    void LuaImage::sepia()
    {
        try { img_.sepia(); }
        catch (...) { LOGEXCEPT(); }
    }
    void LuaImage::grey()
    {
        try { img_.grey(); }
        catch (...) { LOGEXCEPT(); }
    }
    void LuaImage::bw()
    {
        try { img_.bw(); }
        catch (...) { LOGEXCEPT(); }
    }
}

/// ImageTable
bool sol_lua_check(sol::types<ImageLite::ImageTable>, lua_State* L, int32_t index, std::function<sol::check_handler_type> handler, sol::stack::record& tracking)
{
	return sol::stack::check<sol::lua_table>(L, index, handler, tracking);
}
ImageLite::ImageTable sol_lua_get(sol::types<ImageLite::ImageTable>, lua_State* L, int32_t index, sol::stack::record& tracking)
{
	sol::lua_table imgtable = sol::stack::get<sol::lua_table>(L, index, tracking);
	int32_t w = imgtable["width"],
            h = imgtable["height"],
            i = imgtable["itype"];
    auto    v = imgtable["data"].get<std::vector<uint8_t>>();
	return ImageLite::ImageTable{ w, h, i, v };
}
int32_t sol_lua_push(sol::types<ImageLite::ImageTable>, lua_State* L, const ImageLite::ImageTable & v)
{
	sol::table imgtable = sol::table::create_with(
                            L,
                            "width", v.width,
                            "height",v.height,
                            "itype", v.itype,
                            "data",  v.data
        );
	return sol::stack::push(L, imgtable);
}

/// LuaArray4
bool sol_lua_check(sol::types<ImageLite::LuaArray4>, lua_State* L, int32_t index, std::function<sol::check_handler_type> handler, sol::stack::record& tracking)
{
	return sol::stack::check<sol::lua_table>(L, index, handler, tracking);
}
ImageLite::LuaArray4 sol_lua_get(sol::types<ImageLite::LuaArray4>, lua_State* L, int32_t index, sol::stack::record& tracking)
{
	sol::lua_table imgarray4 = sol::stack::get<sol::lua_table>(L, index, tracking);
    auto v = imgarray4["data"].get<std::vector<std::tuple<uint32_t, std::array<uint8_t,3>>>>();
	return ImageLite::LuaArray4{ v };
}
int32_t sol_lua_push(sol::types<ImageLite::LuaArray4>, lua_State* L, const ImageLite::LuaArray4 & v)
{
	sol::table imgarray4 = sol::table::create_with(L, "data", v.data);
	return sol::stack::push(L, imgarray4);
}

/// LuaArray5
bool sol_lua_check(sol::types<ImageLite::LuaArray5>, lua_State* L, int32_t index, std::function<sol::check_handler_type> handler, sol::stack::record& tracking)
{
	return sol::stack::check<sol::lua_table>(L, index, handler, tracking);
}
ImageLite::LuaArray5 sol_lua_get(sol::types<ImageLite::LuaArray5>, lua_State* L, int32_t index, sol::stack::record& tracking)
{
	sol::lua_table imgarray5 = sol::stack::get<sol::lua_table>(L, index, tracking);
    auto v = imgarray5["data"].get<std::vector<std::tuple<uint32_t, uint32_t, std::array<uint8_t,3>>>>();
	return ImageLite::LuaArray5{ v };
}
int32_t sol_lua_push(sol::types<ImageLite::LuaArray5>, lua_State* L, const ImageLite::LuaArray5 & v)
{
	sol::table imgarray5 = sol::table::create_with(L, "data", v.data);
	return sol::stack::push(L, imgarray5);
}
