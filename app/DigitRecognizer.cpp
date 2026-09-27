#include "digitpixels/DigitPixels.hpp"

#include <exception>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    const bool headless = argc == 4 && std::string(argv[3]) == "--headless";
    if (argc < 3 || argc > 4 || (argc == 4 && !headless))
    {
        std::cerr << "Usage: DigitRecognizer <training directory> <input image>"
                     " [--headless]\n";
        return 1;
    }

    try
    {
        DigitPixels recognizer(argv[1], !headless);
        std::cout << "Digits on image: "
                  << recognizer.recognizeDigitsOnImage(argv[2]) << '\n';
        if (!headless)
        {
            recognizer.showNetworkResponseForImage(argv[2]);
        }
        return 0;
    }
    catch (const std::exception &error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 2;
    }
}
