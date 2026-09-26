#pragma once

#include <opencv2/core.hpp>

cv::Mat cropUselessDataFromImage(cv::Mat image);
cv::Mat prepareNetworkDataFromImage(cv::Mat image);
