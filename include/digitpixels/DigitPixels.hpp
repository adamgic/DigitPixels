#pragma once

#include <string>
#include <vector>

class DigitPixels
{
public:
    explicit DigitPixels(std::string digitImagesDirectory, bool showTrainingWindows = false);
    std::string recognizeDigitsOnImage(std::string fileName);
    void showNetworkResponseForImage(std::string fileName);

private:
    std::string digitImagesDirectory;
    std::vector<std::string> digitImagesNames();
};
