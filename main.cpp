//
//  Created by Adam Gic on 15.06.2015.
//  Copyright (c) 2015 UAM. All rights reserved.
//

#include "DigitPixels.h"

void help() {
    cout << "Usage: DigitPixels <training set directory path> <test image path>" << endl;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        help();
        return 1;
    }

    DigitPixels digitRecognizer = DigitPixels(argv[1]);
    string response = digitRecognizer.recognizeDigitsOnImage(argv[2]);

    cout << "Digits on image: " << response << endl;

    digitRecognizer.showNetworkResponseForImage(argv[2]);

    return 0;
}
