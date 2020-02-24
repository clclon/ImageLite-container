
#pragma once

/** @file
 @page lua_data Lua data table API
 @brief Lua virtual tables help.
 */

#include <vector>
#include <array>

#if !defined(SOL_USING_CXX_LUA)
#  define SOL_USING_CXX_LUA 1
#endif

#if !defined(SOL_ALL_SAFETIES_ON)
#  define SOL_ALL_SAFETIES_ON 1
#endif

#if !__has_include(<sol/sol.hpp>)
#  error "require sol.hpp from https://github.com/ThePhD/sol2"
#endif

#if !__has_include(<ImageLite.h>)
#  error "require ImageLite.h from https://github.com/clclon/Android-Remote-Viewer/extern/ImageLite"
#endif

#include <sol/forward.hpp>
#include <sol/sol.hpp>
#include <ImageLite.h>

#if __has_include(<HelperLog.h>)
#  include <HelperLog.h>
#endif

#if !defined(LOGEXCEPT)
#  if defined(_DEBUG)
#    define LOGEXCEPT() throw
#  else
#    define LOGEXCEPT()
#  endif
#endif

namespace ImageLite
{
    /**
     @brief Image pixels array by position, image data container to Lua API.
            Position always increases by 3 with each step.
     @luadata { position, { R,G,B }}
     @luacode
     @verbatim
        local imagePixelsPositionTable = {
            {
                50678, { 255, 255, 255 }
            },
            {
                50681, { 255, 255, 127 }
            },
            {
                50684, { 255, 255, 80 }
            }
        }
        @endverbatim
    */
    struct LuaArray4
    {
        /**< data array, { position, { R,G,B }} */
        std::vector<std::tuple<uint32_t,std::array<uint8_t,3>>> data;
    };
    /**
     @brief Image pixels array by x/y coordinates, image data container to Lua API.
            Coordinates cannot exceed the physical size of the image in pixels.
     @luadata { x, y, { R,G,B }}
     @luacode
     @verbatim
        local imagePixelsCoordinatesTable = {
            {
                1021, 301, { 255, 255, 255 }
            },
            {
                1022, 301, { 255, 255, 127 }
            },
            {
                1023, 301, { 255, 255, 80 }
            }
        }
        @endverbatim
    */
    struct LuaArray5
    {
        /**< data array, { x, y, { R,G,B }} */
        std::vector<std::tuple<uint32_t, uint32_t, std::array<uint8_t,3>>> data;
    };

    // proxy ImageLite::ImageRGBbuffer
    /**
     @brief LuaImage object as proxy to C++ ImageLite::ImageRGBbuffer class
            Coordinates cannot exceed the physical size of the image in pixels.
     @luadata aremote.image.new(..)
     @verbatim
        -- AVAILABLE LUA CONSTRUCTORS
        local im = aremote.image.new()
        local im = aremote.image.new(string filename)
        local im = aremote.image.new(table ImageTable)
     @endverbatim
     @luacode Example using aremote.image methods

     IMAGE CREATE FROM IMAGETABLE
     @verbatim
        local imgTbl = aremote.screenGet()
        local im     = aremote.image.new(imgTbl)
        if im:empty() then
            -- image null length, exit
        end

        -- image buffer length
        local length = im:length()
        -- image dimensions
        local w, h   = im:size()
        -- image save
        im:save("filename.jpg")
     @endverbatim

     IMAGE APPLY SEPIA EFFECT
     @verbatim
        local im  = aremote.image.new("filename.jpg")
        im:sepia()
        im:save("filename.png")
     @endverbatim

     IMAGE GRAYSCALE
     @verbatim
        local im  = aremote.image.new("filename.jpg")
        -- image convert to grey
        im:grey()
        im:save("filename.png")
     @endverbatim

     IMAGE BLACK & WHITE
     @verbatim
        local im  = aremote.image.new("filename.jpg")
        -- image convert to black & white
        im:bw()
        im:save("filename.png")
     @endverbatim

     IMAGE CREATE FROM ANDROID FULL FRAME BUFFER, CAPTURE
     @verbatim
        local im  = aremote.image.new()
        im:capture()
        im:save("filename.bmp")
     @endverbatim

     IMAGE COMPARE
     @verbatim
        local imgTbl = aremote.screenGet()
        local im  = aremote.image.new("filename.jpg")
        -- image compare
        local x = im:compare(imgTbl)
        if x then
            -- images equals
        end
     @endverbatim

     IMAGE REGION COMPARE
     @verbatim
        local im1 =  aremote.image.new()
        local im2  = aremote.image.new("filename300x300.jpg")
        -- image compare region
        local x = im1:rcompare(im2:get(), 70, 100, 300, 300)
        if x then
            -- images region equals
        end
     @endverbatim
     @seeimgtbl
    */
    class LuaImage
    {
        private:
            //
            ImageLite::ImageRGBbuffer img_{};
            //
            void setimg(ImageLite::ImageTable&);

        public:
            //
            /**< default constructor */
            LuaImage();
            /**< load image from constructor */
            LuaImage(std::string);
            /**< constructor create image from ImageTable */
            LuaImage(ImageLite::ImageTable);
            //

            /**
             @brief Re-initialize LuaImage from ImageTable.
             @luadata aremote.image set(table ImageTable)
             @luacode
             @verbatim
                  local im = aremote.image.new()
                  local imgTbl = aremote.screenGet()
                  im:set(imgTbl)
             @endverbatim
             @seeimgtbl
             */
            void set(ImageLite::ImageTable);

            /**
             @brief Get ImageTable from LuaImage.
             @luadata aremote.image get()
             @luacode
             @verbatim
                  local im = aremote.image.new("filename.jpg")
                  local imgTbl = im:get()
             @endverbatim
             @seeimgtbl
             */
            ImageLite::ImageTable get();

            /**
             @brief Compare LuaImage.
             @luadata aremote.image compare(table ImageTable)
             @luacode
             @verbatim
                  local im1 = aremote.image.new("filename.jpg")
                  local im2 = aremote.image.new("filename.png")
                  local x = im1:compare(im2:get())
                  if x then
                     -- images equals
                  end
             @endverbatim
             @seeimgtbl
             */
            double compare(ImageLite::ImageTable);

            /**
             @brief Compare LuaImage region.
             @luadata aremote.image rcompare(table ImageTable, x, y, w, h)
             @luacode
             @verbatim
                  local im1 = aremote.image.new("filename.jpg")
                  local im2 = aremote.image.new("filename300x300.png")
                  local x = im1:rcompare(im2:get(), 70, 100, 300, 300)
                  if x then
                     -- images region equals
                  end
             @endverbatim
             @image{inline} html rcompare.png "region compare"
             @seeimgtbl
             */
            double compare_region(ImageLite::ImageTable, uint32_t, uint32_t, uint32_t, uint32_t);

            /**
             @brief Get pixel by x/y coordinates
             @luadata aremote.image pixel(x, y)
             @luacode
             @verbatim
                  local im = aremote.image.new("filename.jpg")
                  local r, g, b = im:pixel(19, 29)
             @endverbatim
             */
            std::tuple<uint8_t,uint8_t,uint8_t> getpixel_xy(int32_t, int32_t);

            /**
             @brief Get pixel by position
             @luadata aremote.image pixelp(position)
             @luacode
             @verbatim
                  local im = aremote.image.new("filename.jpg")
                  local r, g, b = im:pixelp(50291)
             @endverbatim
             */
            std::tuple<uint8_t,uint8_t,uint8_t> getpixel_pos(int32_t);

            /**
             @brief Get image dimensions
             @luadata aremote.image size()
             @luacode
             @verbatim
                  local im = aremote.image.new("filename.jpg")
                  local w, h = im:size()
             @endverbatim
             */
            std::tuple<int32_t,int32_t> size();

            /**
             @brief Get image buffer length
             @luadata aremote.image length()
             @luacode
             @verbatim
                  local im = aremote.image.new("filename.jpg")
                  local length = im:length()
             @endverbatim
             */
            uint32_t length();

            /**
             @brief Check image is empty
             @luadata aremote.image empty()
             @luacode
             @verbatim
                  local im = aremote.image.new("filename.jpg")
                  local x = im:empty()
                  if x then
                     -- image empty
                  end
             @endverbatim
             */
            bool empty();

            /**
             @brief Capture Android screen, get full frame buffer data
             @luadata aremote.image capture()
             @luacode
             @verbatim
                  local im = aremote.image.new("filename.jpg")
                  local x = im:capture()
                  if x then
                     -- image captured
                  else
                     -- image NOT captured
                  end
             @endverbatim
             */
            bool capture();

            /**
             @brief Load bmp, png, raw image
             @luadata aremote.image load("filename.jpg")
             @luacode
             @verbatim
                  local im = aremote.image.new()
                  im:load("filename.jpg")
             @endverbatim
             */
            void load(std::string const&);

            /**
             @brief Save image as bmp, png, raw
             @luadata aremote.image save("filename.jpg")
             @luacode
             @verbatim
                  local im = aremote.image.new()
                  im:capture()
                  im:save("filename.jpg")
             @endverbatim
             */
            void save(std::string const&);

            /**
             @brief Image apply sepia effect
             @luadata aremote.image sepia()
             @luacode
             @verbatim
                  -- .. create image object
                  im:sepia()
                  im:save("filename-sepia.jpg")
             @endverbatim
             */
            void sepia();

            /**
             @brief Image convert to grey scale
             @luadata aremote.image grey()
             @luacode
             @verbatim
                  -- .. create image object
                  im:grey()
                  im:save("filename-grey.jpg")
             @endverbatim
             */
            void grey();

            /**
             @brief Image convert to black & white
             @luadata aremote.image bw()
             @luacode
             @verbatim
                  -- .. create image object
                  im:bw()
                  im:save("filename-grey.jpg")
             @endverbatim
             */
            void bw();
    };

    namespace Lua
    {
        template <class T>
        static inline void register_lua_imageTable(T & lua)
        {
            lua.template new_usertype<ImageLite::ImageTable>(
                "ImageTable",
                "width", &ImageLite::ImageTable::width,
                "height",&ImageLite::ImageTable::height,
                "itype", &ImageLite::ImageTable::itype,
                "data",  &ImageLite::ImageTable::data
            );
        }

        template <class T>
        static inline void register_lua_LuaImage(T & lua)
        {
            lua.template new_usertype<ImageLite::LuaImage>(
                    "image",    sol::constructors<
                                ImageLite::LuaImage(),
                                ImageLite::LuaImage(std::string),
                                ImageLite::LuaImage(ImageLite::ImageTable)
                                >(),
                    "set",     &ImageLite::LuaImage::set,
                    "get",     &ImageLite::LuaImage::get,
                    "capture", &ImageLite::LuaImage::capture,
                    "load",    &ImageLite::LuaImage::load,
                    "save",    &ImageLite::LuaImage::save,
                    "sepia",   &ImageLite::LuaImage::sepia,
                    "grey",    &ImageLite::LuaImage::grey,
                    "bw",      &ImageLite::LuaImage::bw,
                    "compare", &ImageLite::LuaImage::compare,
                    "rcompare",&ImageLite::LuaImage::compare_region,
                    "pixel",   &ImageLite::LuaImage::getpixel_xy,
                    "pixelp",  &ImageLite::LuaImage::getpixel_pos,
                    "empty",   &ImageLite::LuaImage::empty,
                    "size",    &ImageLite::LuaImage::size,
                    "length",  &ImageLite::LuaImage::length
                );
        }
    }
}

/// ImageTable
bool                  sol_lua_check(sol::types<ImageLite::ImageTable>, lua_State*, int32_t, std::function<sol::check_handler_type>, sol::stack::record&);
ImageLite::ImageTable sol_lua_get(sol::types<ImageLite::ImageTable>,   lua_State*, int32_t, sol::stack::record&);
int32_t               sol_lua_push(sol::types<ImageLite::ImageTable>,  lua_State*, const ImageLite::ImageTable&);

/// LuaArray4
bool                  sol_lua_check(sol::types<ImageLite::LuaArray4>, lua_State*, int32_t, std::function<sol::check_handler_type>, sol::stack::record&);
ImageLite::LuaArray4  sol_lua_get(sol::types<ImageLite::LuaArray4>,   lua_State*, int32_t, sol::stack::record&);
int32_t               sol_lua_push(sol::types<ImageLite::LuaArray4>,  lua_State*, const ImageLite::LuaArray4&);

/// LuaArray5
bool                  sol_lua_check(sol::types<ImageLite::LuaArray5>, lua_State*, int32_t, std::function<sol::check_handler_type>, sol::stack::record&);
ImageLite::LuaArray5  sol_lua_get(sol::types<ImageLite::LuaArray5>,   lua_State*, int32_t, sol::stack::record&);
int32_t               sol_lua_push(sol::types<ImageLite::LuaArray5>,  lua_State*, const ImageLite::LuaArray5&);
