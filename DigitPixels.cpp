//
//  Created by Adam Gic on 06.06.2015.
//  Copyright (c) 2015 UAM. All rights reserved.
//

#include "DigitPixels.h"
#include "opencv/cv.h"
#include "opencv/ml.h"
#include "opencv/highgui.h"

using namespace cv;

CvANN_MLP neuralNetwork;

const char* digitNames[] = {
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

Mat cropUselessDataFromImage(Mat image);
Mat prepareNetworkDataFromImage(Mat img);
void createNetwork();
void trainNetworkWithImages(vector<string> imagesPaths);

DigitPixels::DigitPixels(string digitImagesDirectory) {
    this->digitImagesDirectory = digitImagesDirectory;
    createNetwork();
    trainNetworkWithImages(this->digitImagesNames());
}

vector<string> DigitPixels::digitImagesNames() {
    vector<string> imagesNames = vector<string>();

    for (int i = 0; i < 10; i++) {
        imagesNames.push_back(this->digitImagesDirectory + digitNames[i] + ".png");
    }

    return imagesNames;
}

string DigitPixels::recognizeDigitsOnImage(string imageFilePath) {
    Mat image = imread(imageFilePath, IMREAD_GRAYSCALE);

    // find spaces between digits image data
    vector<double> spaces = vector<double>();
    double max, previousMax = 0.0;
    double threshold = 128.0;
    Mat imageInv = 255 - image;
    for (int i = 0; i < image.cols - image.rows; i++) {
        Mat column = imageInv(Rect(i, 0, 1, image.rows));
        minMaxLoc(column, NULL, &max);
        if (previousMax < threshold && max > threshold) {
            spaces.push_back(i);
        }
        previousMax = max;
    }

    // get network response for each input image digit
    string digitsString;
    Rect windowRect(0, 0, image.rows, image.rows);
    for (int i = 0; i < spaces.size(); i++) {
        windowRect.x = spaces[i];
        Mat window = image(windowRect).clone();

        Mat windowInputData = prepareNetworkDataFromImage(window);
        Mat response;
        neuralNetwork.predict(windowInputData, response);
        Point maxLocation;
        minMaxLoc(response, NULL, NULL, NULL, &maxLocation);
        digitsString += to_string(maxLocation.x);
    }
    return digitsString;
}

void DigitPixels::showNetworkResponseForImage(string imageFilePath) {
    Mat image = imread(imageFilePath, IMREAD_GRAYSCALE);

    imshow("input image", image);
    
    Rect windowRect(0, 0, image.rows, image.rows);

    int iterCount = image.cols - image.rows + 1;
    Mat responses(0, 10, CV_32F);

    for (int i = 0; i < iterCount; i++) {
        windowRect.x = i;
        Mat window = image(windowRect).clone();

        Mat windowInputData = prepareNetworkDataFromImage(window);

        Mat response;
        neuralNetwork.predict(windowInputData, response);
        responses.push_back(response);
    }

    normalize(responses, responses, 0, 20, CV_MINMAX);
    int outputHeight = 280;
    int legendWidth = 20;
    Mat output = Mat::ones(outputHeight + 5, iterCount + legendWidth, CV_8U) * 255;
    // draw digit labels
    for (int j = 0; j < 10; j++) {
        putText(output, to_string(j), Point(0, outputHeight - j * outputHeight / 10), 2, 1, 0);
    }
    // draw network response
    for (int i = 0; i < iterCount; i++) {
        for (int j = 0; j < 10; j++) {
            int responseInt = (int)floor(responses.at<float>(i, j));
            output.at<uchar>(outputHeight - 1 - j * outputHeight / 10 - responseInt, i + legendWidth) = 0;
        }
    }
    transpose(responses, responses);
    imshow("responses", responses);
    imshow("output", output);

    waitKey();
}

void createNetwork() {
    Mat networkLayersSizes = Mat(4, 1, CV_32SC1);
    networkLayersSizes.row(0) = Scalar(100);
    networkLayersSizes.row(1) = Scalar(50);
    networkLayersSizes.row(2) = Scalar(30);
    networkLayersSizes.row(3) = Scalar(10);
    neuralNetwork.create(networkLayersSizes);
}

void trainNetworkWithImages(vector<string> imagesPaths) {
    Mat trainingData(0, 100, CV_32F);
    Mat trainingLabels(0, 10, CV_32F);

    for (int i=0; i<imagesPaths.size(); i++) {

        Mat trainingImage  = imread(imagesPaths[i], IMREAD_GRAYSCALE);

        Mat digitData = prepareNetworkDataFromImage(trainingImage);

        trainingData.push_back(digitData);

        Mat labels = Mat::zeros(1, 10, CV_32F);
        labels.at<float>(0, i) = 1.0f;
        trainingLabels.push_back(labels);
    }

    imshow("Trainging Data", trainingData);
    imshow("Training Labels", trainingLabels);

    CvANN_MLP_TrainParams trainParams;
    CvTermCriteria termCriteria;
    termCriteria.max_iter = 100000;
    termCriteria.epsilon = 0.00001;
    termCriteria.type = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;
    trainParams.train_method = CvANN_MLP_TrainParams::BACKPROP;
    trainParams.bp_dw_scale = 0.05;
    trainParams.bp_moment_scale = 0.05;
    trainParams.term_crit = termCriteria;

    neuralNetwork.train(trainingData, trainingLabels, Mat(), Mat(), trainParams);
}

Mat prepareNetworkDataFromImage(Mat img) {
    Mat image = cropUselessDataFromImage(255-img);

    GaussianBlur(image, image, Size(9,9), 0);
    Size newSize(10,10);
    resize(image, image, newSize);
    
    image = image.reshape(1,1);
    
    image.convertTo(image, CV_32FC1);

    return image / 255;
}

Mat cropUselessDataFromImage(Mat image) {
    // calc rows, and cols max. vals
    vector<double> rowMax(image.rows);
    vector<double> columnMax(image.cols);
    for (int i = 0; i < image.rows; i++) {
        Mat row = image(Rect(0, i, image.cols, 1));
        minMaxLoc(row, NULL, &rowMax[i]);
    }
    for (int i = 0; i < image.cols; i++) {
        Mat column = image(Rect(i, 0, 1, image.rows));
        minMaxLoc(column, NULL, &columnMax[i]);
    }

    double threshold = 128.0;
    Rect roi = Rect(0, 0, image.cols, image.rows);

    // crop image top, bottom, left margins containing no data
    while (rowMax[roi.y] < threshold && roi.y + 1 < image.rows) {
        roi.y++;
    }
    while (rowMax[roi.height - 1] < threshold && roi.y < roi.height - 1) {
        roi.height--;
    }
    while (columnMax[roi.x] < threshold && roi.x + 1 < image.cols) {
        roi.x++;
    }

    // crop following digit image data
    while (columnMax[roi.width - 1] > threshold && roi.x < roi.width - 1) {
        roi.width--;
    }
    // crop image right margin containing no data
    while (columnMax[roi.width - 1] < threshold && roi.x < roi.width - 1) {
        roi.width--;
    }

    roi.width -= roi.x;
    roi.height -= roi.y;
    return image(roi).clone();
}
