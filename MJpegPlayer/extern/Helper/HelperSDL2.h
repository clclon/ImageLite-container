/*
    MIT License

    Android Aremote Viewer, GUI ADB tools

    Android Viewer developed to view and control your android device from a PC.
    ADB exchange Android Viewer, support scale view, input tap from mouse,
    input swipe from keyboard, save/copy screenshot, etc..

    Copyright (c) 2016-2020 PS
    GitHub: https://github.com/ClClon

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


#ifndef HEADER_42253E72FBAD4BF8
#define HEADER_42253E72FBAD4BF8

#pragma once

#include <exception>
#include <memory>
#include <functional>
#include <string>
#include <string_view>
#include <placeholders.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <ImageLite.h>


namespace deleter
{
    static inline void __attribute__((always_inline)) freeobj(SDL_Window   *w) { SDL_DestroyWindow(w);   }
    static inline void __attribute__((always_inline)) freeobj(SDL_Renderer *r) { SDL_DestroyRenderer(r); }
    static inline void __attribute__((always_inline)) freeobj(SDL_Texture  *t) { SDL_DestroyTexture(t);  }
    static inline void __attribute__((always_inline)) freeobj(SDL_Surface  *s) { SDL_FreeSurface(s);     }
    static inline void __attribute__((always_inline)) freeobj(SDL_Cursor   *c) { SDL_FreeCursor(c);      }
    static inline void __attribute__((always_inline)) freeobj(SDL_mutex    *m) { SDL_DestroyMutex(m);    }
    static inline void __attribute__((always_inline)) freeobj(SDL_cond     *c) { SDL_DestroyCond(c);     }
    static inline void __attribute__((always_inline)) freeobj(HWND          h) { DestroyWindow(h);       }
}

namespace Helper
{
    template <typename T>
    std::shared_ptr<T> auto_shared(T *t)
    {
        return std::shared_ptr<T>(t, [](T *t) { deleter::freeobj(t); });
    }
    template <typename T>
    void auto_reset(std::shared_ptr<T> & ptr, T *t)
    {
        ptr.reset(t, [](T *t) { deleter::freeobj(t); });
    }

    class on_exit
    {
        private:
            std::function<void()> fun_ = [](){};
        public:
            on_exit(std::function<void()> f) noexcept { fun_ = f; }
            ~on_exit()                                { fun_(); }
    };

    class sdl_error : public std::runtime_error
    {
        public:
            sdl_error()
                : std::runtime_error(SDL_GetError()) { SDL_ClearError(); }
    };

    class error_exception
    {
        private:
            static inline const char *l_head = "Oops.. ";
            static inline const char *l_uerr = "unknown exception";
            //
            error_exception() {}
            error_exception(const error_exception&) = delete;
            error_exception& operator=(const error_exception&) = delete;
            //
            std::function<void(std::string_view, std::string_view)> f_print =
                [](std::string_view s, std::string_view t)
                {
                    SDL_ShowSimpleMessageBox(0, t.data(), s.data(), nullptr);
                };

        public:
            static error_exception& instance()
            {
                static error_exception m_instance{};
                return m_instance;
            }

            void set_print(const std::function<void(std::string_view, std::string_view)> fun)
            {
                f_print = fun;
            }
            void show(std::string_view s, std::string_view t) const
            {
                f_print(s, t);
            }
            void show(
                    std::exception_ptr const & pe,
                    std::string_view func,
                    std::string_view file,
                    int32_t line) const
            {
                std::stringstream ss;
                std::stringstream st;

                try
                {
                    st << error_exception::l_head;
                    //
                    auto pos = file.find_last_of("/\\");
                    if (pos != std::string::npos)
                        st << file.substr(pos + 1, file.length()) << ":" << line;
                    else
                        st << file.data() << ":" << line;
                    st << " [" << func.data() << "] ";
                    if (pe)
                    {
#                       if (defined(__GNUC__) && !defined(__clang__))
                        std::string extype = pe.__cxa_exception_type()->name();
                        if (!extype.empty())
                            ss << extype.c_str() << " : ";
#                       endif
                        std::rethrow_exception(pe);
                    }
                }
                catch(std::system_error const & e)  { ss << e.what(); }
                catch(std::runtime_error const & e) { ss << e.what(); }
                catch(std::exception const & e)     { ss << e.what(); }
                catch(std::string const & s)        { ss << s.c_str();}
                catch(intmax_t i)                   { ss << i; }
                catch(uintmax_t u)                  { ss << u; }
                catch(...)                          { ss << error_exception::l_uerr; }

                auto s = ss.str();
                auto t = st.str();
                if (!s.empty())
                    f_print(s, t);
                else
                    f_print(error_exception::l_uerr, t);
            }
            const error_exception & operator << (std::exception_ptr const & pe) const
            {
                show(pe, __func__, __FILE__, __LINE__);
                return *this;
            }
    };
    //const
    //constexpr error_exception exceptlog;

    namespace SDL2
    {
        typedef struct
        {
            int32_t w;
            int32_t h;
            int32_t bpp;
            uint8_t *data;
        } SDL2_ImageResource_t;

        static inline void push_event(uint32_t evid, int32_t id, void *data = nullptr)
        {
            SDL_Event ev{};
            ev.type = evid;
            ev.user.code = id;
            ev.user.data1 = data;
            SDL_PushEvent(&ev);
        }

        static inline ImageLite::IPOINT<int32_t> get_mouse_display(SDL_Window *win)
        {
            int32_t x, y;
            ImageLite::IPOINT<int32_t> ip{};
            ::SDL_GetWindowPosition(win, &ip.x, &ip.y);
            ::SDL_GetMouseState(&x, &y);
            ip.x += x;
            ip.y += y;
            return ip;
        }

        static inline std::shared_ptr<SDL_Surface> image_resource(SDL2_ImageResource_t const & ires, SDL_Color *transparent = nullptr)
        {
            uint32_t rmask, gmask, bmask, amask;
#           if SDL_BYTEORDER == SDL_BIG_ENDIAN
            int32_t shift = (ires.bpp == 3) ? 8 : 0;
            rmask = 0xff000000 >> shift;
            gmask = 0x00ff0000 >> shift;
            bmask = 0x0000ff00 >> shift;
            amask = 0x000000ff >> shift;
#           else
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            amask = (ires.bpp == 3) ? 0 : 0xff000000;
#           endif

            auto ptr = Helper::auto_shared<SDL_Surface>(
                            SDL_CreateRGBSurfaceFrom(
                                static_cast<void*>(ires.data),
                                ires.w,
                                ires.h,
                                (ires.bpp * 8),
                                (ires.bpp * ires.w),
                                rmask, gmask, bmask, amask
                            )
                        );

            if (transparent)
            {
                ptr->format->Amask = 0xFF000000;
                ptr->format->Ashift = 24;

                SDL_SetColorKey(
                    ptr.get(),
                    SDL_TRUE,
                    SDL_MapRGB(
                        ptr->format,
                        transparent->r,
                        transparent->g,
                        transparent->b
                    )
                );
            }
            return ptr;
        }

        static inline HWND get_hwnd(SDL_Window *win)
        {
            SDL_SysWMinfo wmInfo{};
            SDL_VERSION(&wmInfo.version);
            if (!SDL_GetWindowWMInfo(win, &wmInfo))
                return nullptr;
            return wmInfo.info.win.window;
        }

    }
}
#define error_except(x) Helper::error_exception::instance().show(std::current_exception(), __func__, __FILE__, __LINE__)
#define error_show(a,b) Helper::error_exception::instance().show(a,b)
#define info_show(a,b)  error_show(a,b)
#endif

