
#include "ImageLiteTest.h"

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

        std::ifstream fo("test/test-raw4.mjpeg", std::ios::in | std::ios::binary | std::ios::ate);
        if (!fo.is_open())
        {
            std::cout << " -! Open 'test/test-raw.mjpeg' error" << std::endl;
            return 1;
        }
        uint32_t const sz = static_cast<uint32_t>(fo.tellg());
        uint32_t cnt = 1;
        std::vector<char> idata(sz);

        fo.seekg(0, std::ios::beg);
        fo.read(&idata[0], sz);
        fo.close();

        ImageLite::MJpeg mjpg;
        mjpg.stream(&idata[0], idata.size(),
                    [&](ImageLite::ImgBuffer const & b)
                    {
                        if (!b.size())
                        {
                            std::cout << " -! NOT decode jepeg, RGB buffer size is null" << std::endl;
                            return false;
                        }
                        if (mjpg.error)
                        {
                            std::cout << " -! MJpeg decode error: " << mjpg.error.value() << std::endl;
                            return false;
                        }
                        std::string s = "test\\save-mjpeg-rgb-";
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

        std::cout << " -- I'm end decode, count: " << (cnt - 1) << std::endl;
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
