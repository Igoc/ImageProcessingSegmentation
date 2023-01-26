// +-------------------------------------------< PREPROCESSING >--------------------------------------------+

#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
#endif

// +----------------------------------------------< INCLUDE >-----------------------------------------------+

#include <cassert>
#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <limits>

// +------------------------------------------< TYPE DEFINITION >-------------------------------------------+

typedef uint8_t byte_t;

// +------------------------------------------< GLOBAL VARIABLE >-------------------------------------------+

static const size_t WIDTH  = 303;
static const size_t HEIGHT = 243;

byte_t* inputImage;
byte_t* outputImage;

// +-------------------------------------< KAPUR THRESHOLD SELECTION >--------------------------------------+

byte_t KapurThresholdSelection(byte_t* inputImage, byte_t* outputImage)
{
    assert(inputImage  != NULL);
    assert(outputImage != NULL);

    double   histogram[256]   = { 0.0 };
    double   entropy[256]     = { 0.0 };

    uint32_t foregroundNumber = 0;
    uint32_t backgroundNumber = WIDTH * HEIGHT;

    byte_t   kapurThreshold   = 0;
    double   maxEntropy       = DBL_MIN;

    for (int iy = 0; iy < HEIGHT; ++iy)
        for (int ix = 0; ix < WIDTH; ++ix)
            histogram[inputImage[iy * WIDTH + ix]]++;

    for (int threshold = 0; threshold < 256; ++threshold)
    {
        foregroundNumber += histogram[threshold];
        backgroundNumber -= histogram[threshold];

        for (int brightness = 0; brightness <= threshold; ++brightness)
            if (histogram[brightness] / foregroundNumber != 0)
                entropy[threshold] -= (histogram[brightness] / foregroundNumber) * log2(histogram[brightness] / foregroundNumber);

        for (int brightness = threshold + 1; brightness < 256; ++brightness)
            if (histogram[brightness] / backgroundNumber != 0)
                entropy[threshold] -= (histogram[brightness] / backgroundNumber) * log2(histogram[brightness] / backgroundNumber);

        if (entropy[threshold] > maxEntropy)
        {
            kapurThreshold = threshold;
            maxEntropy     = entropy[threshold];
        }
    }

    for (int iy = 0; iy < HEIGHT; ++iy)
        for (int ix = 0; ix < WIDTH; ++ix)
            outputImage[iy * WIDTH + ix] = (inputImage[iy * WIDTH + ix] < kapurThreshold) ? (0) : (255);

    return kapurThreshold;
}

// +------------------------------------------------< MAIN >------------------------------------------------+

int main(void)
{
    static const char* INPUT_RAW_FILE_NAME  = "hand.raw";
    static const char* OUTPUT_RAW_FILE_NAME = "hand_KapurThresholdSelection.raw";

    FILE* fileStream;

    inputImage  = new byte_t[WIDTH * HEIGHT];
    outputImage = new byte_t[WIDTH * HEIGHT];

    fileStream = fopen(INPUT_RAW_FILE_NAME, "rb");
    fread(inputImage, sizeof(byte_t), WIDTH * HEIGHT, fileStream);
    fclose(fileStream);

    printf("[Kapur Threshold] %d\n", KapurThresholdSelection(inputImage, outputImage));

    fileStream = fopen(OUTPUT_RAW_FILE_NAME, "w+b");
    fwrite(outputImage, sizeof(byte_t), WIDTH * HEIGHT, fileStream);
    fclose(fileStream);

    delete[] inputImage;
    delete[] outputImage;

    return 0;
}

// +------------------------------------------------< END >-------------------------------------------------+