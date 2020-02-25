
#include "ImageLiteTest.h"

void tostream(ImageLite::MJpeg & mjpg, std::vector<char> & idata, int32_t part)
{
        uint32_t cnt = 1;
        mjpg.stream(&idata[0], idata.size(),
                    [&](ImageLite::ImgBuffer const & b,int32_t const w, int32_t const h)
                    {
                        if (!b.size())
                        {
                            std::cout << " -! NOT decode jepeg, RGB buffer size is null, " << w << "x" << h << std::endl;
                            return false;
                        }
                        if (mjpg.error)
                        {
                            std::cout << " -! MJpeg decode error: " << mjpg.error.value() << std::endl;
                            return false;
                        }
                        std::string s = "test\\save-mjpeg-rgb-";
                        s += std::to_string(part).c_str();
                        s += "-";
                        s += std::to_string(cnt).c_str();
                        s += ".raw";
                        std::cout << " -- "
                                  << cnt++
                                  << ", found decode jepeg, size: "
                                  << b.size()
                                  << ", save name: "
                                  << s.c_str()
                                  << std::endl;

                        writeRaw(s, b);
                        return true;
                    }
            );

        std::cout << " -- Part " << part << ", decode, count: " << (cnt - 1) << std::endl;
}

bool readFilePart(std::string_view s, std::vector<char> & data)
{
        std::ifstream fo(s.data(), std::ios::in | std::ios::binary | std::ios::ate);
        if (!fo.is_open())
        {
            std::cout << " -! Open '" << s.data() << "' error" << std::endl;
            return false;
        }
        uint32_t sz = static_cast<uint32_t>(fo.tellg());
        if (!sz)
            return false;

        data.resize(sz);

        fo.seekg(0, std::ios::beg);
        fo.read(&data[0], sz);
        fo.close();
        return true;
}

int main()
{
    try
    {
        std::vector<uint8_t> odata;
        ImageLite::JpegGpu::NVJpegDecoder decoder;
        if (!decoder.isenable())
            throw std::system_error(decoder.error);

        /*
        decoder.imgread("test-1.jpg", odata);
        decoder.imgread("test-2.jpg", odata);
        decoder.imgread("test-3.jpg", odata);
        decoder.imgread("test-4.jpg", odata);
        decoder.imgread("file_example_JPG_2500kB.jpg", odata);
        //
        std::ofstream f("test-raw.raw", std::ios::out | std::ios::binary | std::ios::ate);
        f.write(reinterpret_cast<char*>(&odata[0]), odata.size());
        f.close();
        std::cout << decoder.imgparam << std::endl;
        */

        std::vector<char> idata1;
        std::vector<char> idata2;
        std::vector<char> idata3;

        readFilePart("test/test-raw-part1.mjpeg", idata1);
        readFilePart("test/test-raw-part2.mjpeg", idata2);
        readFilePart("test/test-raw-part3.mjpeg", idata3);

        ImageLite::MJpeg mjpg;
        //
        tostream(mjpg, idata1, 1);
        tostream(mjpg, idata2, 2);
        //tostream(mjpg, idata3, 3);
    }
    catch (std::system_error const& ex_)
    {
        std::cout << " -! Except: " << ex_.what() << std::endl;
    }
    catch (std::runtime_error const& ex_)
    {
        std::cout << " -! Except: " << ex_.what() << std::endl;
    }
    catch (...)
    {
        std::cout << " -! Except unknown" << std::endl;
    }
    return 0;
}
