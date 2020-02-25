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


#pragma once

namespace Helper
{
	template <typename T, typename U>
	class Map
	{
		private:
			std::map<T, U> map_;

		public:
			Map(const T & key, const U & val)
			{
				map_[key] = val;
			}
			Map<T, U> & operator()(const T & key, const U & val)
			{
				map_[key] = val;
				return *this;
			}
			operator std::map<T, U>()
			{
				return map_;
			}
	};
	typedef std::map<std::string, std::string> StringMap;
	typedef Map<std::string, std::string> StringMapInit;
	//
#   if defined(__WIN32__)
	typedef std::map<uint32_t, std::tuple<uint32_t, HBITMAP>> ImageMap;
	typedef Map<uint32_t, std::tuple<uint32_t, HBITMAP>> ImageMapInit;
#   endif
	//
	typedef std::map<int32_t, std::string> CatalogMap;
	typedef Map<int32_t, std::string> CatalogMapInit;
}
