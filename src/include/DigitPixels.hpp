//
//  Created by Adam Gic on 06.06.2015.
//

#ifndef __DigitRecognition__DigitPixels__
#define __DigitRecognition__DigitPixels__

#include <iostream>
#include <vector>

using namespace std;

class DigitPixels {

public:
    DigitPixels(string digitImagesDirectory);
    string recognizeDigitsOnImage(string fileName);
    void showNetworkResponseForImage(string fileName);
private:
    string digitImagesDirectory;
    vector<string> digitImagesNames( void );
};

#endif /* defined(__DigitRecognition__DigitPixels__) */
