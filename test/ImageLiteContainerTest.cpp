
#include "ImageLiteTest.h"

int main()
{
    try
    {
        double d;
        //
        ImageLite::ImageRGBbuffer img1;
        img1.load("test\\test-container.png");
        img1.save("test\\test-container.raw");
        //
        ImageLite::ImageRGBbuffer img2;
        img2.load_raw("test\\test-container.raw", 600, 1024, 0);
        //
        ImageLite::ImageRGBbuffer img3;
        img3.load("test\\test-crop-300x300.png");
        //
        d = img1.icompare(img2);
        std::cout << " -- compare 1 <-> 2: " << d << std::endl;
        //

        ImageLite::IRECT<int32_t> r3{ 0, 0, 300, 300 };
        d = img1.icompare(img3, r3);
        std::cout << " -- compare 1 <-> 3 (region): " << d << std::endl;

        img1.grey();
        img1.save("test\\save-test-grey.png");
        //
        img2.save("test\\save-test.bmp");
        img2.save("test\\save-test.png");
        img2.save("test\\save-test.jpg");
        img2.sepia();
        img2.save("test\\save-test-sepia.png");
        //
        img3.bw();
        img3.save("test\\save-test-bw.png");

        ///
        ImageLite::ImgBuffer b;
        readRaw("test\\test-container.raw", b);
        ImageLite::ImageRGBbuffer img4(b, 600, 1024, 0, ImageLite::BufferType::IBT_RGB);
        d = img1.icompare(img4);
        std::cout << " -- compare 1 <-> 4: " << d << std::endl;
        std::cout << " -- pad image 4: " << img4.getpad() << std::endl;
    }
    catch (std::system_error const & ex_)
    {
        std::cout << " -! exception sys: " << ex_.what() << std::endl;
    }
    catch (std::runtime_error const & ex_)
    {
        std::cout << " -! exception run: " << ex_.what() << std::endl;
    }
    return 0;
}
