#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string_view>
#include <memory>
#include <ImageLite.h>

//#include <NVJpegDecoder.h>

void readRaw(std::string_view s, ImageLite::ImgBuffer & b)
{
    std::ifstream f(s.data(), std::ios::binary | std::ios::ate);
    if (!f.is_open())
        throw std::runtime_error("open read file");


    f.seekg (0, std::ios::end);
    std::size_t sz = f.tellg();
    if (!sz)
        throw std::runtime_error("file null size");

    f.seekg(0, std::ios::beg);
    b.resize(sz);
    f.read(reinterpret_cast<char*>(&b[0]), sz);
    f.close();
}

void writeRaw(std::string_view s, ImageLite::ImgBuffer const & b)
{
    std::ofstream f(s.data(), std::ios::binary | std::ios::ate);
    if (!f.is_open())
        throw std::runtime_error("open write file");

    f.write(reinterpret_cast<char*>(const_cast<uint8_t*>(&b[0])), b.size());
    f.close();
}
