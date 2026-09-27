#include "Preprocessing.hpp"
#include "TestHelpers.hpp"

#include <opencv2/core.hpp>
#include <cmath>
#include <iostream>
#include <stdexcept>

int main()
{
    try
    {
        // Black source pixels become maximum foreground after inversion.
        cv::Mat black(20, 20, CV_8UC1, cv::Scalar(0));
        cv::Mat result = prepareNetworkDataFromImage(black);
        require(result.rows == 1 && result.cols == 100,
                "Expected one row with 100 network inputs");
        require(result.type() == CV_32FC1, "Expected single-channel float data");
        require(cv::checkRange(result), "Expected finite output values");
        double low = 0.0;
        double high = 0.0;
        cv::minMaxLoc(result, &low, &high);
        require(std::abs(low - 1.0) < 1e-6 && std::abs(high - 1.0) < 1e-6,
                "Black input should become normalized foreground value 1");

        cv::Mat glyph(30, 30, CV_8UC1, cv::Scalar(255));
        glyph(cv::Rect(8, 5, 10, 20)).setTo(0);
        cv::Mat sample = prepareNetworkDataFromImage(glyph);
        require(sample.rows == 1 && sample.cols == 100, "Invalid sample shape");
        require(cv::checkRange(sample), "Non-finite sample values");
        cv::minMaxLoc(sample, &low, &high);
        require(low >= 0.0 && high <= 1.0 && high > 0.0,
                "Expected foreground data in [0, 1]");

        bool emptyRejected = false;
        try
        {
            prepareNetworkDataFromImage(cv::Mat{});
        }
        catch (const std::invalid_argument &)
        {
            emptyRejected = true;
        }
        require(emptyRejected, "Empty input must be rejected");

        bool colourRejected = false;
        try
        {
            prepareNetworkDataFromImage(cv::Mat(20, 20, CV_8UC3, cv::Scalar(0)));
        }
        catch (const std::invalid_argument &)
        {
            colourRejected = true;
        }
        require(colourRejected, "Colour input must be rejected");
        std::cout << "Preprocessing checks passed\n";
        return 0;
    }
    catch (const std::exception &error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
