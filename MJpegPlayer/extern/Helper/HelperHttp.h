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

#ifndef HEADER_1B69119597342BF5
#define HEADER_1B69119597342BF5

#pragma once

#if defined(__WIN32__)
#  if !defined(_WIN32_WINDOWS)
#    define _WIN32_WINDOWS 1
#  endif
#  if !defined(_WIN32_WINNT)
#    define _WIN32_WINNT 0x0601
#  endif
#endif

#if !defined(USE_STANDALONE_ASIO)
#  define USE_STANDALONE_ASIO 1
#endif

#include <winsock2.h> //
#include <ws2tcpip.h> // fix ASIO socket_types.hpp ..
#include <asio.hpp>
#include <windows.h>
#include <HelperMap.h>

namespace Helper
{
    namespace Http
    {
        enum ErrorId
        {
            error_begin = 0,
            error_url_empty = 1400,
            error_url_bad_format,
            error_proto_not_support,
            error_asio_init,
            error_asio_read,
            error_connect,
            error_packet,
            error_packet_decode,
            error_packet_size,
            error_packet_not_http,
            error_reconnect_limit,
            error_unknown,
            error_end
        };

        class ErrorCat : public std::error_category
        {
            private:
                Helper::CatalogMap l_http_error = Helper::CatalogMapInit
                    (error_begin, "")
                    (error_url_empty, "URL empty")
                    (error_url_bad_format, "URL bad format")
                    (error_proto_not_support, "protocol not support")
                    (error_asio_init, "ASIO initialize error")
                    (error_asio_read, "ASIO socket read error")
                    (error_connect, "connect error")
                    (error_packet, "get packet error")
                    (error_packet_decode, "packet decode error")
                    (error_packet_size, "packet size is small")
                    (error_packet_not_http, "not HTTP packet")
                    (error_reconnect_limit, "reconnect limit")
                    (error_unknown, "error unknown")
                    (error_end, "");

            public:
                const char* name() const noexcept override
                {
                    return "Http Client";
                }
                std::string message(int32_t e) const override
                {
                    if ((e < ErrorId::error_begin) || (e > ErrorId::error_end))
                        e = ErrorId::error_unknown;

                    auto r = l_http_error.find(e);
                    if (r == l_http_error.end())
                        return "";
                    return r->second;
                }
        };
        const ErrorCat errCat{};

        static inline __attribute__((always_inline)) std::error_code make_error_code(ErrorId e)
        {
            return { static_cast<int32_t>(e), errCat };
        }

        class Client
        {
            public:
                //
                enum proto_e
                {
                    PROTO_NONE,
                    PROTO_HTTP,
                    PROTO_HTTPS
                };
                struct uri_t
                {
                    std::string host;
                    std::string port;
                    std::string uri;
                    proto_e     proto;
                };

            private:
                //
                uri_t            m_uri;
                asio::io_service m_ios;
                asio::ip::tcp::socket m_socket;
                asio::ip::tcp::resolver::iterator m_resolv;
                //
                bool parseurl(std::string_view url)
                {
                    do
                    {
                        m_uri.proto = PROTO_NONE;

                        auto pos = url.find_first_of("://");
                        if (pos == std::string::npos)
                            break;

                        if ((pos == 4) && (url.compare(0, pos, "http", 4) == 0))
                            m_uri.proto = PROTO_HTTP;
                        else if ((pos == 5) && (url.compare(0, pos, "https", 5) == 0))
                            m_uri.proto = PROTO_HTTPS;
                        else
                            break;

                        std::string_view tmp = url.substr(pos + 3, url.length());

                        auto posu = tmp.find_first_of("/");
                        if (posu != std::string::npos)
                        {
                            m_uri.uri = tmp.substr(posu, tmp.length());
                            tmp = tmp.substr(0, posu);
                        }

                        pos = tmp.find_first_of(":");
                        if (pos == std::string::npos)
                        {
                            m_uri.host = tmp;
                            m_uri.port = "80";
                        }
                        else
                        {
                            m_uri.port = tmp.substr(pos + 1, posu);
                            m_uri.host = tmp.substr(0, pos);
                        }
                        return true;
                    }
                    while (0);
                    return false;
                }

            public:
                //
                std::error_code error;
                //
                Client(std::string_view url)
                    : m_socket(m_ios)
                {
                    try
                    {
                        if (!parseurl(url))
                        {
                            error = make_error_code(ErrorId::error_url_bad_format);
                            return;
                        }
                        switch (m_uri.proto)
                        {
                            case proto_e::PROTO_NONE:
                            case proto_e::PROTO_HTTPS:
                                {
                                    error = make_error_code(ErrorId::error_proto_not_support);
                                    return;
                                }
                            default:
                                break;
                        }

                        asio::ip::tcp::resolver r(m_ios);
                        asio::ip::tcp::resolver::query q(m_uri.host, m_uri.port);
                        m_resolv = r.resolve(q, error);
                    }
                    catch (std::system_error const & ex_) { error = ex_.code(); }
                    catch (...)
                    {
                        error = make_error_code(ErrorId::error_asio_init);
                    }
                }

                ~Client()
                {
                    m_socket.shutdown(asio::socket_base::shutdown_both);
                    m_socket.close();
                }

                size_t get(std::vector<char> & v)
                {
                    try
                    {
                        auto sz = m_socket.read_some(asio::buffer(&v[0], v.size()), error);
                        if (error)
                            return 0;
                        return sz;
                    }
                    catch (std::system_error const & ex_) { error = ex_.code(); }
                    catch (...)
                    {
                        error = make_error_code(ErrorId::error_asio_read);
                    }
                    return 0;
                }

                bool connect()
                {
                    try
                    {
                        do
                        {
                            if (error)
                                break;

                            asio::streambuf r;
                            std::ostream rs(&r);

                            rs << "GET "
                               << ((m_uri.uri.empty()) ? "/" : m_uri.uri.c_str())
                               << " HTTP/1.0\r\n"
                               << "Host: " << m_uri.host.c_str() << ":" << m_uri.port.c_str() << "\r\n"
                               << "Accept: */*\r\n"
                               << "Connection: close\r\n\r\n";

                            asio::connect(m_socket, m_resolv, error);
                            if (error)
                                break;

                            asio::write(m_socket, r, error);
                            if (error)
                                break;

                            std::vector<char> v(50);
                            auto sz = get(v);
                            if (sz < 12)
                            {
                                error = make_error_code(ErrorId::error_packet_size);
                                break;
                            }

                            std::string_view s = &v[0];
                            if ((s.compare(0, 5, "HTTP/", 5) != 0) ||
                                (s.compare(9, 3, "200", 3) != 0))
                            {
                                error = make_error_code(ErrorId::error_packet_not_http);
                                break;
                            }
                            return true;
                        }
                        while (0);
                    }
                    catch (std::system_error const & ex_) { error = ex_.code(); }
                    catch (...)
                    {
                        error = make_error_code(ErrorId::error_connect);
                    }
                    return false;
                }
        };
    }
}

namespace std
{
    template <>
    struct is_error_code_enum<Helper::Http::ErrorId> : public true_type{};
};

using HttpError  = Helper::Http::ErrorId;
using HttpClient = Helper::Http::Client;

#endif

