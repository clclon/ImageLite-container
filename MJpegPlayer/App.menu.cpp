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

    static const uint32_t l_flags[] =
    {
        (MF_BYCOMMAND | MF_ENABLED),
        (MF_BYCOMMAND | MF_DISABLED | MF_GRAYED),
        (MF_BYCOMMAND | MF_ENABLED  | MF_CHECKED),
        (MF_BYCOMMAND | MF_ENABLED  | MF_UNCHECKED)
    };
    enum FlagsId
    {
        FID_ENABLED = 0,
        FID_DISABLED,
        FID_CHECKED,
        FID_UNCHECKED
    };

    void App::menu_setenable(HMENU hm, uint32_t cmd, bool isenable)
    {
        ::EnableMenuItem(hm, cmd, ((isenable) ? l_flags[FlagsId::FID_ENABLED] : l_flags[FlagsId::FID_DISABLED]));
    }

    void App::menu_setcheck(HMENU hm, uint32_t cmd, bool ischeck)
    {
        ::CheckMenuItem(hm, cmd, ((ischeck) ? l_flags[FlagsId::FID_CHECKED] : l_flags[FlagsId::FID_UNCHECKED]));
    }

    void App::menu_setimage(HMENU hm, uint32_t cmd, uint32_t id)
    {
        ::SetMenuItemBitmaps(hm, cmd, MF_BITMAP | MF_BYCOMMAND, m_bitmap[id], m_bitmap[id]);
    }

    void App::menu_show()
    {
        HMENU hpm = nullptr;

        do
        {
            if (!(hpm = ::LoadMenu(
                    ::GetModuleHandle(nullptr),
                    MAKEINTRESOURCE(ID_CONTEXT_MENU)
                )))
                break;

            menu_setcheck(hpm, IDM_APP_STATUSBAR,  m_isstatb.load());
            menu_setcheck(hpm, IDM_APP_FASTDECODE, m_isfastcpu.load());

            menu_setimage(hpm, IDM_VIDEO_OPEN,    0);
            menu_setimage(hpm, IDM_VIDEO_PLAY,    2);
            menu_setimage(hpm, IDM_VIDEO_STOP,    3);
            menu_setimage(hpm, IDM_VIDEO_PAUSE,   4);

            if (m_iscap.load())
            {
                menu_setenable(hpm, IDM_VIDEO_AREMOTE, false);
                menu_setenable(hpm, IDM_VIDEO_OPEN,    false);
                menu_setenable(hpm, IDM_VIDEO_PLAY,    false);
                menu_setenable(hpm, IDM_VIDEO_STOP,    true);
                menu_setenable(hpm, IDM_VIDEO_PAUSE,   true);
            }
            else
            {
                if (!m_url.empty())
                    menu_setenable(hpm, IDM_VIDEO_PLAY, true);
                else
                    menu_setenable(hpm, IDM_VIDEO_PLAY, false);

                menu_setenable(hpm, IDM_VIDEO_AREMOTE,  true);
                menu_setenable(hpm, IDM_VIDEO_OPEN,     true);
                menu_setenable(hpm, IDM_VIDEO_STOP,     false);
                menu_setenable(hpm, IDM_VIDEO_PAUSE,    false);
            }

            HMENU hpmt = GetSubMenu(hpm, 0);
            ImageLite::IPOINT<int32_t> ip = Helper::SDL2::get_mouse_display(m_win.get());

            uint32_t idx = ::TrackPopupMenuEx(
                    hpmt,
                    TPM_RETURNCMD | TPM_NONOTIFY |
                    TPM_RIGHTBUTTON | TPM_TOPALIGN | TPM_LEFTALIGN,
                    ip.x,
                    ip.y,
                    Helper::SDL2::get_hwnd(m_win.get()),
                    nullptr
                );
            if (!idx)
                break;

            Helper::SDL2::push_event(m_uevent.load(), idx);

        } while (0);

        if (hpm)
            ::DestroyMenu(hpm);
    }
