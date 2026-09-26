#include "Preprocessing.hpp"

#include <opencv2/imgproc.hpp>
#include <stdexcept>
#include <vector>

using namespace cv;
using namespace std;

Mat prepareNetworkDataFromImage(Mat img)
{
    if (img.empty() || img.type() != CV_8UC1)
    {
        throw std::invalid_argument("Expected a non-empty 8-bit grayscale image");
    }
    Mat image = cropUselessDataFromImage(255 - img);

    GaussianBlur(image, image, Size(9, 9), 0);
    Size newSize(10, 10);
    resize(image, image, newSize);

    image = image.reshape(1, 1);

    image.convertTo(image, CV_32FC1);

    return image / 255;
}

Mat cropUselessDataFromImage(Mat image)
{
    // calc rows, and cols max. vals
    vector<double> rowMax(image.rows);
    vector<double> columnMax(image.cols);
    for (int i = 0; i < image.rows; i++)
    {
        Mat row = image(Rect(0, i, image.cols, 1));
        minMaxLoc(row, NULL, &rowMax[i]);
    }
    for (int i = 0; i < image.cols; i++)
    {
        Mat column = image(Rect(i, 0, 1, image.rows));
        minMaxLoc(column, NULL, &columnMax[i]);
    }

    double threshold = 128.0;
    Rect roi = Rect(0, 0, image.cols, image.rows);

    // crop image top, bottom, left margins containing no data
    while (rowMax[roi.y] < threshold && roi.y + 1 < image.rows)
    {
        roi.y++;
    }
    while (rowMax[roi.height - 1] < threshold && roi.y < roi.height - 1)
    {
        roi.height--;
    }
    while (columnMax[roi.x] < threshold && roi.x + 1 < image.cols)
    {
        roi.x++;
    }

    // crop following digit image data
    while (columnMax[roi.width - 1] > threshold && roi.x < roi.width - 1)
    {
        roi.width--;
    }
    // crop image right margin containing no data
    while (columnMax[roi.width - 1] < threshold && roi.x < roi.width - 1)
    {
        roi.width--;
    }

    roi.width -= roi.x;
    roi.height -= roi.y;
    return image(roi).clone();
}
