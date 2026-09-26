#include "digitpixels/DigitPixels.hpp"
#include "Preprocessing.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>
#include <cmath>
#include <stdexcept>

using namespace cv;
using namespace std;

cv::Ptr<cv::ml::ANN_MLP> neuralNetwork;

const char *digitNames[] = {
    "zero",
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine",
};

void createNetwork();
void trainNetworkWithImages(vector<string> imagesPaths, bool showTrainingWindows);
void validateImage(Mat image, const string &imageFilePath);

DigitPixels::DigitPixels(string directory, bool showTrainingWindows)
    : digitImagesDirectory(directory)
{
    if (digitImagesDirectory.empty())
    {
        throw invalid_argument("Training directory must not be empty");
    }
    if (digitImagesDirectory.back() != '/')
    {
        digitImagesDirectory += '/';
    }
    createNetwork();
    trainNetworkWithImages(digitImagesNames(), showTrainingWindows);
}

vector<string> DigitPixels::digitImagesNames()
{
    vector<string> imagesNames = vector<string>();

    for (int i = 0; i < 10; i++)
    {
        imagesNames.push_back(this->digitImagesDirectory + digitNames[i] + ".png");
    }

    return imagesNames;
}

string DigitPixels::recognizeDigitsOnImage(string imageFilePath)
{
    Mat image = imread(imageFilePath, IMREAD_GRAYSCALE);
    validateImage(image, imageFilePath);

    // find spaces between digits image data
    vector<double> spaces = vector<double>();
    double max, previousMax = 0.0;
    double threshold = 128.0;
    Mat imageInv = 255 - image;
    for (int i = 0; i < image.cols - image.rows; i++)
    {
        Mat column = imageInv(Rect(i, 0, 1, image.rows));
        minMaxLoc(column, NULL, &max);
        if (previousMax < threshold && max > threshold)
        {
            spaces.push_back(i);
        }
        previousMax = max;
    }

    // get network response for each input image digit
    string digitsString;
    Rect windowRect(0, 0, image.rows, image.rows);
    for (int i = 0; i < spaces.size(); i++)
    {
        windowRect.x = spaces[i];
        Mat window = image(windowRect).clone();

        Mat windowInputData = prepareNetworkDataFromImage(window);
        Mat response;
        neuralNetwork->predict(windowInputData, response);
        Point maxLocation;
        minMaxLoc(response, NULL, NULL, NULL, &maxLocation);
        digitsString += to_string(maxLocation.x);
    }
    return digitsString;
}

void DigitPixels::showNetworkResponseForImage(string imageFilePath)
{
    Mat image = imread(imageFilePath, IMREAD_GRAYSCALE);
    validateImage(image, imageFilePath);

    imshow("input image", image);

    Rect windowRect(0, 0, image.rows, image.rows);

    int iterCount = image.cols - image.rows + 1;
    Mat responses(0, 10, CV_32F);

    for (int i = 0; i < iterCount; i++)
    {
        windowRect.x = i;
        Mat window = image(windowRect).clone();

        Mat windowInputData = prepareNetworkDataFromImage(window);

        Mat response;
        neuralNetwork->predict(windowInputData, response);
        responses.push_back(response);
    }

    normalize(responses, responses, 0, 20, cv::NORM_MINMAX);
    int outputHeight = 280;
    int legendWidth = 20;
    Mat output = Mat::ones(outputHeight + 5, iterCount + legendWidth, CV_8U) * 255;
    // draw digit labels
    for (int j = 0; j < 10; j++)
    {
        putText(output, to_string(j), Point(0, outputHeight - j * outputHeight / 10), 2, 1, 0);
    }
    // draw network response
    for (int i = 0; i < iterCount; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            int responseInt = (int)floor(responses.at<float>(i, j));
            output.at<uchar>(outputHeight - 1 - j * outputHeight / 10 - responseInt, i + legendWidth) = 0;
        }
    }
    transpose(responses, responses);
    imshow("responses", responses);
    imshow("output", output);

    waitKey();
}

void createNetwork()
{
    neuralNetwork = cv::ml::ANN_MLP::create();
    Mat layerSizes = (Mat_<int>(1, 4) << 100, 50, 30, 10);
    neuralNetwork->setLayerSizes(layerSizes);
    neuralNetwork->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM);
    neuralNetwork->setTrainMethod(cv::ml::ANN_MLP::BACKPROP, 0.05, 0.05);
    neuralNetwork->setTermCriteria(
        TermCriteria(TermCriteria::MAX_ITER | TermCriteria::EPS,
                     100000, 0.00001));
}

void trainNetworkWithImages(vector<string> imagesPaths,
                            bool showTrainingWindows)
{
    Mat trainingData(0, 100, CV_32F);
    Mat trainingLabels(0, 10, CV_32F);

    for (size_t i = 0; i < imagesPaths.size(); ++i)
    {
        Mat trainingImage = imread(imagesPaths[i], IMREAD_GRAYSCALE);
        if (trainingImage.empty())
        {
            throw runtime_error("Cannot load training image: " + imagesPaths[i]);
        }
        trainingData.push_back(prepareNetworkDataFromImage(trainingImage));
        Mat labels = Mat::zeros(1, 10, CV_32F);
        labels.at<float>(0, static_cast<int>(i)) = 1.0f;
        trainingLabels.push_back(labels);
    }

    if (showTrainingWindows)
    {
        imshow("Training Data", trainingData);
        imshow("Training Labels", trainingLabels);
    }

    if (!neuralNetwork->train(trainingData, cv::ml::ROW_SAMPLE, trainingLabels))
    {
        throw runtime_error("Neural network training failed");
    }
}

void validateImage(Mat image, const string &imageFilePath)
{
    if (image.empty())
    {
        throw runtime_error("Cannot load input image: " + imageFilePath);
    }
    if (image.cols <= image.rows)
    {
        throw invalid_argument("Expected a horizontal strip wider than its height");
    }
}