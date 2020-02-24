
#pragma once

namespace ImageLite
{
    /// Geometry DATA:

            template<typename Tval>
            struct IPOINT
            {
                Tval x, y;
                Tval *byindex[2] = { &x, &y };

                IPOINT()
                 : x(0), y(0) {}
                IPOINT(Tval x_, Tval y_)
                 : x(x_), y(y_) {}
                //
                bool empty() const
                {
                    if constexpr (std::is_unsigned<Tval>::value)
                        return ((!x) && (!y));
                    else
                        return ((x <= 0) && (y <= 0));
                }
                //
                template<typename Tobj, typename Ttype>
                Tobj get()
                {
                    return {
                        static_cast<Ttype>(x),
                        static_cast<Ttype>(y)
                    };
                }
                template<typename Tobj>
                Tobj get()
                {
                    return {
                        x,
                        y
                    };
                }
                //
                template<uint32_t Tn>
                Tval get()
                {
                    static_assert((Tn < 2), "index wrong, >= 2");
                    return *byindex[Tn];
                }
#               if defined(__WIN32)
                void set(POINT const & p)
                {
                    x = static_cast<Tval>(p.x);
                    y = static_cast<Tval>(p.y);
                }
#               if defined(_BUILD_GDIPLUS_ENABLE)
                void set(Gdiplus::Point const & p)
                {
                    x = static_cast<Tval>(p.X);
                    y = static_cast<Tval>(p.Y);
                }
#               endif
#               endif
#               if defined(_BUILD_SDL2_ENABLE)
                void set(SDL_Point const & p)
                {
                    x = static_cast<Tval>(p.x);
                    y = static_cast<Tval>(p.y);
                }
#               endif
                template<typename Ttype>
                void set(Ttype _x, Ttype _y)
                {
                    x = static_cast<Tval>(_x);
                    y = static_cast<Tval>(_y);
                }
                //
                template<typename Tobj>
                void set(Tobj const & obj)
                {
                    set<decltype(reinterpret_cast<Tobj*>(0)->x)>(obj.x , obj.y);
                }
            };
            //
            template<typename Tval>
            struct IRECT
            {
                Tval x, y, w, h;
                Tval *byindex[4] = { &x, &y, &w, &h };

                IRECT()
                 : x(0), y(0), w(0), h(0) {}
                IRECT(Tval x_, Tval y_, Tval w_, Tval h_)
                 : x(x_), y(y_), w(w_), h(h_) {}
                //
                bool empty() const
                {
                    if constexpr (std::is_unsigned<Tval>::value)
                        return ((!w) || (!h));
                    else
                        return ((w <= 0) || (h <= 0));
                }
                bool contains(IPOINT<Tval> const & ip, Tval pad = 0) const
                {
                    return (
                        (ip.x >= x)       && // w
                        (ip.x <= (x + w)) &&
                        (ip.y >= y)       && // h
                        (ip.y <= (y + ((pad) ? pad : h)))
                            );
                }
#               if defined(__WIN32)
                void convert(RECT const & r)
                {
                    x = static_cast<Tval>(r.left);
                    y = static_cast<Tval>(r.top);
                    w = static_cast<Tval>(r.right  - r.left);
                    h = static_cast<Tval>(r.bottom - r.top);
                }
                void convert(RECTL const & r)
                {
                    convert(reinterpret_cast<RECT const&>(r));
                }
#               if defined(_BUILD_GDIPLUS_ENABLE)
                void convert(Gdiplus::Rect const & r)
                {
                    set(r);
                }
#               endif
#               endif
#               if defined(_BUILD_SDL2_ENABLE)
                void convert(SDL_Rect const & r)
                {
                    set(r);
                }
#               endif
                //
                template<typename Tobj, typename Ttype>
                Tobj get()
                {
                    return {
                        static_cast<Ttype>(x),
                        static_cast<Ttype>(y),
                        static_cast<Ttype>(w),
                        static_cast<Ttype>(h)
                    };
                }
                template<typename Tobj>
                Tobj get()
                {
                    return { x, y, w, h };
                }
                //
                template<uint32_t Tn>
                Tval get()
                {
                    static_assert((Tn < 4), "index wrong, >= 4");
                    return *byindex[Tn];
                }
                //
                template<typename Ttype>
                IPOINT<Ttype> get()
                {
                    return {
                        static_cast<Ttype>(w),
                        static_cast<Ttype>(h)
                    };
                }
                //
#               if defined(__WIN32)
                void set(RECT const & r)
                {
                    x = static_cast<Tval>(r.left);
                    y = static_cast<Tval>(r.top);
                    w = static_cast<Tval>(r.right);
                    h = static_cast<Tval>(r.bottom);
                }
                void set(RECTL const & r)
                {
                    set(reinterpret_cast<RECT const&>(r));
                }
#               if defined(_BUILD_GDIPLUS_ENABLE)
                void set(Gdiplus::Rect const & r)
                {
                    x = static_cast<Tval>(r.X);
                    y = static_cast<Tval>(r.Y);
                    w = static_cast<Tval>(r.Width);
                    h = static_cast<Tval>(r.Height);
                }
#               endif
#               endif
#               if defined(_BUILD_SDL2_ENABLE)
                void set(SDL_Rect const & r)
                {
                    x = static_cast<Tval>(r.x);
                    y = static_cast<Tval>(r.y);
                    w = static_cast<Tval>(r.w);
                    h = static_cast<Tval>(r.h);
                }
#               endif
                template<typename Ttype>
                void set(IRECT<Ttype> const & ir)
                {
                    set<Ttype>(ir.x, ir.y, ir.w, ir.h);
                }
                //
                template<typename Ttype>
                void set(IPOINT<Ttype> const & ip)
                {
                    set<Ttype>(ip.x, ip.y);
                }
                //
                template<typename Ttype>
                void set(Ttype const _x, Ttype const _y, Ttype const _w, Ttype const _h)
                {
                    x = static_cast<Tval>(_x);
                    y = static_cast<Tval>(_y);
                    w = static_cast<Tval>(_w);
                    h = static_cast<Tval>(_h);
                }
                //
                template<typename Ttype>
                void set(Ttype const _w, Ttype const _h)
                {
                    set<Ttype>(0, 0, _w, _h);
                }
                //
                template<typename T1type, typename T2type>
                void set(T2type const & obj)
                {
                    static_assert((sizeof(T2type) == (sizeof(T1type) * 4)), "type different size");
                    uint8_t *xobj = reinterpret_cast<uint8_t*>(
                                    const_cast<T2type*>(&obj)
                                );
                    for (uint32_t i = 0U; i < 4; i++)
                        *byindex[i] =  static_cast<Tval>(
                                        *reinterpret_cast<T1type*>(
                                            xobj + (sizeof(T1type) * i)
                                        )
                                    );
                }
                //
                template<typename Ttype>
                bool compare(Ttype const _w, Ttype const _h) const
                {
                    return ((w == static_cast<Tval>(_w)) &&
                            (h == static_cast<Tval>(_h)));
                }
                template<typename Ttype>
                bool compare(IRECT<Ttype> const & ir) const
                {
                    return ((x == static_cast<Tval>(ir.x)) &&
                            (y == static_cast<Tval>(ir.y)) &&
                            (w == static_cast<Tval>(ir.w)) &&
                            (h == static_cast<Tval>(ir.h)));
                }
                template<typename Ttype>
                bool compare(IPOINT<Ttype> const & ip) const
                {
                    return compare<Ttype>(ip.x, ip.y);
                }
                //
            };
}

