#ifndef HEADER_3A181052299526D9
#define HEADER_3A181052299526D9

#pragma once

class App
{
    public:
        //
        struct aremote_t
        {
            int32_t rotate = 0, ratio = 0;
            static inline const char url[] = "http://127.0.0.1:53777/stream/live.mjpg";

            std::string get()
            {
                std::stringstream ss;
                ss << aremote_t::url;

                if ((rotate) || (ratio))
                    ss << "?";
                if (rotate)
                    ss << "rotate=" << rotate << ((ratio) ? "&" : "");
                if (ratio)
                    ss << "ratio=" << ratio;

                auto s = ss.str();
                return s;
            }
        };

    private:
        //
        bool                          m_isinit = false;
        std::atomic<bool>             m_isrun  = false;
        std::atomic<bool>             m_iscap  = false;
        //
        std::shared_ptr<SDL_Window>   m_win;
        std::shared_ptr<SDL_Renderer> m_rend;
        std::shared_ptr<SDL_Texture>  m_tex;
        std::shared_ptr<HWND__>       m_statb;
        std::shared_mutex             m_lock;
        std::thread                   m_thr;
        //
        SDL_Rect                      m_rview     = { 0, 0, 400, 400 };
        ImageLite::IPOINT<int32_t>    m_pscreen   = { 400, 400 };
        std::atomic<uint32_t>         m_uevent    = 0U;
        std::atomic<bool>             m_isresize  = false;
        std::atomic<bool>             m_isaremote = false;
        std::atomic<bool>             m_isfastcpu  = true;
        std::atomic<bool>             m_isstatb   = true;
        int32_t                       m_sbpad = 22;
        HBITMAP                       m_bitmap[5]{};
        //
        std::string                   m_url;
        aremote_t                     m_aremote{};
        //
        void resize(int32_t = 0, int32_t = 0);
        void loop();
        void update(ImageLite::ImgBuffer const&);
        void reload();
        void splash();
        void thdata();
        //
        void start();
        void stop();
        void pause();
        //
        void status_print(std::string_view, std::string_view) const;
        //
        void menu_show();
        void menu_setenable(HMENU, uint32_t, bool);
        void menu_setcheck(HMENU,  uint32_t, bool);
        void menu_setimage(HMENU,  uint32_t, uint32_t);


    public:
        //
        std::atomic<uint32_t>  m_reconnect = 10;
        //
        App();
        ~App();
        //
        bool run();
        void show(bool = true);


};

#endif

