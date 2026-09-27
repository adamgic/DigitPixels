#include "digitpixels/DigitPixels.hpp"
#include "TestHelpers.hpp"

#include <opencv2/core.hpp>
#include <algorithm>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char *argv[])
{
    const std::string expectedResult = "98792364539";
    try
    {
        require(argc == 3, "Expected training directory and input image");
        cv::setNumThreads(1);
        cv::setRNGSeed(12345);
        DigitPixels recognizer(argv[1], false);
        const std::string result = recognizer.recognizeDigitsOnImage(argv[2]);
        std::cout << "Recognized: " << result << '\n';
        require(result == expectedResult, "Incorrect recognition result, expected: " + expectedResult + ", got: " + result);
        return 0;
    }
    catch (const std::exception &error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
