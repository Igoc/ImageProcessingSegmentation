// +-------------------------------------------< PREPROCESSING >--------------------------------------------+

#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
#endif

// +----------------------------------------------< INCLUDE >-----------------------------------------------+

#include <cassert>
#include <cinttypes>
#include <cstdio>
#include <limits>

// +------------------------------------------< TYPE DEFINITION >-------------------------------------------+

typedef uint8_t byte_t;

// +------------------------------------------< GLOBAL VARIABLE >-------------------------------------------+

static const size_t WIDTH  = 303;
static const size_t HEIGHT = 243;

byte_t* inputImage;
byte_t* outputImage;

// +--------------------------------------< OTSU THRESHOLD SELECTION >--------------------------------------+

byte_t OtsuThresholdSelection(byte_t* inputImage, byte_t* outputImage)
{
    assert(inputImage  != NULL);
    assert(outputImage != NULL);

    double histogram[256]   = { 0.0 };
    double variance[256]    = { 0.0 };

    double foregroundMean   = 0.0;
    double backgroundMean   = 0.0;
    double foregroundNumber = 0.0;
    double backgroundNumber = WIDTH * HEIGHT;

    byte_t otsuThreshold    = 0;
    double maxVariance      = DBL_MIN;

    for (int iy = 0; iy < HEIGHT; ++iy)
        for (int ix = 0; ix < WIDTH; ++ix)
            histogram[inputImage[iy * WIDTH + ix]]++;

    for (int threshold = 0; threshold < 256; ++threshold)
    {
        foregroundMean    = 0.0;
        backgroundMean    = 0.0;
        foregroundNumber += histogram[threshold];
        backgroundNumber -= histogram[threshold];

        for (int brightness = 0; brightness <= threshold; ++brightness)
            foregroundMean += histogram[brightness] * brightness;
        foregroundMean /= foregroundNumber;

        for (int brightness = threshold + 1; brightness < 256; ++brightness)
            backgroundMean += histogram[brightness] * brightness;
        backgroundMean /= backgroundNumber;

        variance[threshold] = (foregroundNumber / (WIDTH * HEIGHT)) * (backgroundNumber / (WIDTH * HEIGHT)) *
                              (foregroundMean - backgroundMean) * (foregroundMean - backgroundMean);

        if (variance[threshold] > maxVariance)
        {
            otsuThreshold = threshold;
            maxVariance   = variance[threshold];
        }
    }

    for (int iy = 0; iy < HEIGHT; ++iy)
        for (int ix = 0; ix < WIDTH; ++ix)
            outputImage[iy * WIDTH + ix] = (inputImage[iy * WIDTH + ix] < otsuThreshold) ? (0) : (255);

    return otsuThreshold;
}

// +------------------------------------------------< MAIN >------------------------------------------------+

int main(void)
{
    static const char* INPUT_RAW_FILE_NAME  = "hand.raw";
    static const char* OUTPUT_RAW_FILE_NAME = "hand_OtsuThresholdSelection.raw";

    FILE* fileStream;

    inputImage  = new byte_t[WIDTH * HEIGHT];
    outputImage = new byte_t[WIDTH * HEIGHT];

    fileStream = fopen(INPUT_RAW_FILE_NAME, "rb");
    fread(inputImage, sizeof(byte_t), WIDTH * HEIGHT, fileStream);
    fclose(fileStream);

    printf("[Otsu Threshold] %d\n", OtsuThresholdSelection(inputImage, outputImage));

    fileStream = fopen(OUTPUT_RAW_FILE_NAME, "w+b");
    fwrite(outputImage, sizeof(byte_t), WIDTH * HEIGHT, fileStream);
    fclose(fileStream);

    delete[] inputImage;
    delete[] outputImage;

    return 0;
}

// +------------------------------------------------< END >-------------------------------------------------+