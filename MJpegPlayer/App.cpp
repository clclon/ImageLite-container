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

#include "SDLMJpegPlayer_internal.h"
#include "App.h"
#include "resource/resource.h"
#include "resource/nvplayer400x400.h"
#include "resource/play48x48.h"
#include "resource/pause48x48.h"
#include "resource/stop48x48.h"

static const char l_appname[] = "MJPEG Player";

    App::App()
    {
        SDL_Init(SDL_INIT_VIDEO);

        do
        {
            m_win = Helper::auto_shared(
                SDL_CreateWindow(
                    l_appname,
                    SDL_WINDOWPOS_UNDEFINED,
                    SDL_WINDOWPOS_UNDEFINED,
                    m_pscreen.x,
                    m_pscreen.y + m_sbpad,
                    SDL_WINDOW_HIDDEN |
                    SDL_WINDOW_ALLOW_HIGHDPI
                )
            );
            if (!m_win)
                break;

            m_rend = Helper::auto_shared(
                SDL_CreateRenderer(
                    m_win.get(),
                    -1,
                    SDL_RENDERER_ACCELERATED
                )
            );
            if (!m_rend)
                break;

            m_tex = Helper::auto_shared(
                SDL_CreateTexture(
                    m_rend.get(),
                    SDL_PIXELFORMAT_RGB24,
                    SDL_TEXTUREACCESS_STREAMING,
                    m_pscreen.x,
                    m_pscreen.y
                )
            );
            if (!m_tex)
                break;

            SDL_SetRenderDrawColor(m_rend.get(), 0xff, 0xff, 0xff, 0xff);

            m_uevent = SDL_RegisterEvents(1);
            if (m_uevent.load() == ((uint32_t)-1))
                break;

            m_statb = Helper::auto_shared(
                ::CreateStatusWindowW(
                    WS_CHILD | SBARS_TOOLTIPS,
                    L"",
                    Helper::SDL2::get_hwnd(m_win.get()),
                    APP_STATUSBAR
                )
            );
            if (!m_statb)
                break;

            ::SendMessage(m_statb.get(), SB_SETBKCOLOR, 0, (LPARAM)RGB(255,255,255));

            Helper::error_exception::instance().set_print(
                        std::bind(App::status_print, this, std::placeholders::_1, std::placeholders::_2)
                    );
            reload();
            m_isinit = true;
            //
            static uint32_t bmpid[] =
            {
                ID_BMPCONN16,
                ID_BMPLOAD16,
                ID_BMPPLAY16,
                ID_BMPSTOP16,
                ID_BMPPAUSE16
            };
            for (uint32_t i = 0; i < std::size(m_bitmap); i++)
            {
                m_bitmap[i] = reinterpret_cast<HBITMAP>(
                            ::LoadImage(
                                ::GetModuleHandle(nullptr),
                                MAKEINTRESOURCE(bmpid[i]),
                                IMAGE_BITMAP,
                                16, 16,
                                LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS
                            )
                        );
            }
            return;
        }
        while (0);
        return;
    }

    App::~App()
    {
        m_isrun = false;
        if (m_thr.joinable())
            m_thr.join();
        for (uint32_t i = 0; i < std::size(m_bitmap); i++)
            if (m_bitmap[i])
                DeleteObject(m_bitmap[i]);
        SDL_Quit();
    }

    void App::status_print(std::string_view s, std::string_view t) const
    {
        if (!m_statb)
        {
            SDL_ShowSimpleMessageBox(0, t.data(), s.data(), nullptr);
            return;
        }
        std::wstringstream ss;
        ss << t.data() << " - " << s.data();
        auto str = ss.str();
        if (!str.empty())
            ::SendMessageW(
                m_statb.get(),
                SB_SETTEXTW,
                MAKEWPARAM(SBT_NOBORDERS, SB_SIMPLEID),
                reinterpret_cast<LPARAM>(str.data())
            );
    }

    void App::update(ImageLite::ImgBuffer const & b)
    {
        void   *pix = nullptr;
        int32_t pitch = 0;
        //
        std::lock_guard<std::shared_mutex> l_lock(m_lock);
        SDL_LockTexture(m_tex.get(), nullptr, &pix, &pitch);
        if ((pix) && (pitch))
            ::memcpy(pix, &b[0], b.size());
        SDL_UnlockTexture(m_tex.get());
    }

    void App::resize(int32_t w, int32_t h)
    {
        m_pscreen.x = ((w) ? w : m_pscreen.x);
        m_pscreen.y = ((h) ? h : m_pscreen.y);
        m_rview = { 0, 0, m_pscreen.x, m_pscreen.y };
        //
        do
        {
            if (m_isstatb.load())
            {
                RECT rc{};
                ::GetClientRect(m_statb.get(), &rc);
                m_sbpad = rc.bottom;
            }
            else
            {
                m_sbpad = 0;
                ::ShowWindow(m_statb.get(), SW_HIDE);
            }

            SDL_SetWindowSize(m_win.get(), m_pscreen.x, m_pscreen.y + m_sbpad);

            if (!m_isstatb.load())
                break;

            ::ShowWindow(m_statb.get(),  SW_SHOW);
            ::SendMessage(m_statb.get(), WM_SIZE, m_pscreen.x, m_pscreen.y);
        }
        while (0);
    }

    void App::reload()
    {
        std::lock_guard<std::shared_mutex> l_lock(m_lock);
        {
            if (m_iscap.load())
                Helper::auto_reset(
                    m_tex,
                    SDL_CreateTexture(
                        m_rend.get(),
                        SDL_PIXELFORMAT_RGB24,
                        SDL_TEXTUREACCESS_STREAMING,
                        m_pscreen.x,
                        m_pscreen.y
                    )
                );
            else
            {
                auto surf = Helper::SDL2::image_resource(img_nvplayer);
                Helper::auto_reset(
                    m_tex,
                    SDL_CreateTextureFromSurface(
                        m_rend.get(),
                        surf.get()
                    )
                );
            }
        }
        m_isresize = false;
    }

    void App::splash()
    {
        m_isresize  = true;
        m_pscreen.x = img_nvplayer.w;
        m_pscreen.y = img_nvplayer.h;
        SDL_SetWindowSize(m_win.get(), m_pscreen.x, m_pscreen.y + m_sbpad);
        Helper::SDL2::push_event(m_uevent.load(), APP_SPLASH);
    }

    void App::loop()
    {
        SDL_Event evt{};

        while (m_isrun.load())
        {
            while (SDL_PollEvent(&evt) != 0)
            {
                if (evt.type == m_uevent)
                {
                    switch (evt.user.code)
                    {
                        case APP_RESIZE:
                        case APP_SPLASH:
                            {
                                resize();
                                break;
                            }
                        case IDM_VIDEO_AREMOTE:
                            {
                                m_url.assign(
                                        m_aremote.get()
                                    );
                                start();
                                break;
                            }

                        case IDM_AREMOTE_RATIO11:
                        case IDM_AREMOTE_RATIO12:
                        case IDM_AREMOTE_RATIO13:
                            {
                                switch (evt.user.code)
                                {
                                    case IDM_AREMOTE_RATIO11:
                                        m_aremote.ratio = 0;
                                        break;
                                    case IDM_AREMOTE_RATIO12:
                                        m_aremote.ratio = 2;
                                        break;
                                    case IDM_AREMOTE_RATIO13:
                                        m_aremote.ratio = 3;
                                        break;
                                    default:
                                        break;
                                }
                                m_url.assign(
                                        m_aremote.get()
                                    );
                                break;
                            }
                        case IDM_AREMOTE_ROTATE0:
                        case IDM_AREMOTE_ROTATE90:
                        case IDM_AREMOTE_ROTATE180:
                        case IDM_AREMOTE_ROTATE270:
                        case IDM_AREMOTE_ROTATE360:
                            {
                                switch (evt.user.code)
                                {
                                    case IDM_AREMOTE_ROTATE0:
                                        m_aremote.rotate = 0;
                                        break;
                                    case IDM_AREMOTE_ROTATE90:
                                        m_aremote.rotate = 90;
                                        break;
                                    case IDM_AREMOTE_ROTATE180:
                                        m_aremote.rotate = 180;
                                        break;
                                    case IDM_AREMOTE_ROTATE270:
                                        m_aremote.rotate = 270;
                                        break;
                                    case IDM_AREMOTE_ROTATE360:
                                        m_aremote.rotate = 360;
                                        break;
                                    default:
                                        break;
                                }
                                m_url.assign(
                                        m_aremote.get()
                                    );
                                break;
                            }
                        case IDM_VIDEO_OPEN:
                            {
                                try
                                {
                                    GUI::CustomDialog cdlg;
                                    cdlg.SetTitle("Open URL (http)");
                                    cdlg.SetCtrl(
                                        {
                                            { true, IDC_EDIT1, std::string(), "http://localhost:53777/stream/live.mjpg" }
                                        }
                                    );
                                    if (cdlg.Show(
                                            ::GetModuleHandle(nullptr),
                                            Helper::SDL2::get_hwnd(m_win.get()),
                                            IDD_DIALOG_OURL
                                        ) != IDOK)
                                        break;

                                    auto v = cdlg.GetCtrlValue();
                                    if (!v.size())
                                        break;

                                    m_url.assign(v[0]);
                                    start();
                                }
                                catch (...) { error_except(); }
                                break;
                            }
                        case IDM_VIDEO_PLAY:
                            {
                                start();
                                break;
                            }
                        case IDM_VIDEO_STOP:
                            {
                                stop();
                                break;
                            }
                        case IDM_VIDEO_PAUSE:
                            {
                                pause();
                                break;
                            }
                        case IDM_APP_STATUSBAR:
                            {
                                m_isstatb = !(m_isstatb.load());
                                resize();
                                break;
                            }
                        case IDM_APP_FASTDECODE:
                            {
                                m_isfastcpu = !(m_isfastcpu.load());
                                break;
                            }
                        case IDM_APP_EXIT:
                            {
                                Helper::SDL2::push_event(SDL_QUIT, 0);
                                break;
                            }
                        default:
                            break;
                    }
                    continue;
                }
                switch (evt.type)
                {
                    case SDL_QUIT:
                    case SDL_WINDOWEVENT_CLOSE:
                        {
                            m_iscap = false;
                            m_isrun = false;
                            while (m_iscap.load())
                            {
                                std::this_thread::yield();
                                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                            }
                            break;
                        }
                    case SDL_RENDER_TARGETS_RESET:
                    case SDL_RENDER_DEVICE_RESET:
                        {
                            m_isresize = true;
                            reload();
                            continue;
                        }
                    case SDL_MOUSEBUTTONDOWN:
                        {
                            if (evt.button.button == SDL_BUTTON_RIGHT)
                            {
                                menu_show();
                            }
                            break;
                        }
                    case SDL_KEYDOWN:
                        {
                            if (m_isaremote.load())
                            {
                                if (evt.key.keysym.mod != KMOD_NONE)
                                {
                                }
                            }
                            switch (evt.key.keysym.sym)
                            {
                                case SDLK_ESCAPE:
                                    break;
                                default:
                                    break;
                            }
                            break;
                        }
                    case SDL_WINDOWEVENT:
                        {
                            /*
                            if (evt.window.event == SDL_WINDOWEVENT_RESIZED)
                            {
                                resize(false, evt.window.data1, evt.window.data2);
                            }
                            */
                            break;
                        }
                }
            }
            //
            {
                std::shared_lock<std::shared_mutex> l_lock(m_lock);
                SDL_RenderClear(m_rend.get());
                if (!m_isresize.load())
                    SDL_RenderCopy(m_rend.get(), m_tex.get(), nullptr, &m_rview);
                SDL_RenderPresent(m_rend.get());
            }
            if ((!m_iscap.load()) || (m_isfastcpu.load()))
            {
                std::this_thread::yield();
                std::this_thread::sleep_for(std::chrono::milliseconds(
                                    ((m_isfastcpu.load()) ? 50 : 200)
                                ));
            }
        }
    }

    void App::thdata()
    {
        if (m_thr.joinable())
            m_thr.join();

        std::thread thcap
        {
            [&]()
            {
                try
                {
                    uint32_t cnt = 0;
                    std::vector<char> v(65536);
                    ImageLite::MJpeg mjpg;

                    if (mjpg.error)
                        throw std::system_error(mjpg.error);

                    while ((m_iscap.load()) && (m_isrun.load()) && (cnt++ < m_reconnect.load()))
                    {
                        HttpClient http(m_url);
                        std::string ssssss = Helper::Http::errCat.message(HttpError::error_connect);
                        if (!http.connect())
                            throw std::system_error(
                                        http.error,
                                        Helper::Http::errCat.message(HttpError::error_connect)
                                    );

                        while (m_iscap.load())
                        {
                            auto sz = http.get(v);
                            if (!sz)
                                throw std::system_error(
                                        http.error,
                                        Helper::Http::errCat.message(HttpError::error_packet)
                                    );
                            try
                            {
                                mjpg.stream(&v[0], sz,
                                [&](ImageLite::ImgBuffer const & b, int32_t const w, int32_t const h)
                                {
                                    try
                                    {
                                        if ((!b.size()) || (mjpg.error))
                                            throw std::system_error(
                                                    http.error,
                                                    Helper::Http::errCat.message(HttpError::error_packet_decode)
                                                );
                                        if ((m_pscreen.x != w) || (m_pscreen.y != h))
                                        {
                                            m_pscreen.x = w;
                                            m_pscreen.y = h;
                                            m_isresize  = true;
                                            Helper::SDL2::push_event(m_uevent.load(), APP_RESIZE);
                                        }
                                        if (!m_isresize.load())
                                            update(b);
                                        return m_iscap.load();
                                    }
                                    catch (...) { error_except(); }
                                    return false;
                                });
                            }
                            catch (std::system_error const & ex_)
                            {
                                switch (ex_.code().value())
                                {
                                    case 3:
                                    case 4:
                                        {
                                            mjpg.clear();
                                            continue;
                                        }
                                    default:
                                        {
                                            mjpg.clear();
                                            throw;
                                        }
                                }
                            }
                            catch (...) { throw; }
                        }
                    }
                    if (cnt >= m_reconnect.load())
                    {
                        throw std::system_error(
                                    Helper::Http::make_error_code(HttpError::error_reconnect_limit)
                                );
                    }
                }
                catch (...) { error_except(); }
                m_iscap = false;
            }
        };
        m_thr = std::move(thcap);
    }

    void App::show(bool x)
    {
        if (x)
        {
            SDL_ShowWindow(m_win.get());
            if (m_isstatb.load())
                ::ShowWindow(m_statb.get(), SW_SHOW);
        }
        else
        {
            ::ShowWindow(m_statb.get(), SW_HIDE);
            SDL_HideWindow(m_win.get());
        }

    }

    bool App::run()
    {
        if (!m_isinit)
            return false;

        try
        {
            m_isrun = true;
            loop();
            m_isrun = false;
            return true;
        }
        catch (...) { error_except(); }
        m_isrun = false;
        return false;
    }

    void App::start()
    {
        if (m_iscap.load())
            return;

        try
        {
            if (m_url.empty())
                throw std::system_error(
                        Helper::Http::make_error_code(HttpError::error_url_empty)
                    );
            m_iscap = true;
            if (m_isstatb.load())
                info_show(m_url, "Play");

            {
                auto img = Helper::SDL2::image_resource(img_play);
                SDL_SetWindowIcon(m_win.get(), img.get());
                //
                std::stringstream ss;
                ss << l_appname << " : " << m_url.c_str();
                auto s = ss.str();
                SDL_SetWindowTitle(m_win.get(), s.c_str());
            }
            thdata();
        }
        catch (...) { error_except(); }
    }

    void App::stop()
    {
        try
        {
            m_iscap = false;
            if (m_isstatb.load())
                info_show(m_url, "Stop");

            {
                auto img = Helper::SDL2::image_resource(img_stop);
                SDL_SetWindowIcon(m_win.get(), img.get());
                SDL_SetWindowTitle(m_win.get(), l_appname);
            }
            splash();
        }
        catch (...) { error_except(); }
    }

    void App::pause()
    {
        try
        {
            m_iscap = false;
            if (m_isstatb.load())
                info_show(m_url, "Pause");

            {
                auto img = Helper::SDL2::image_resource(img_pause);
                SDL_SetWindowIcon(m_win.get(), img.get());
            }
        }
        catch (...) { error_except(); }
    }
