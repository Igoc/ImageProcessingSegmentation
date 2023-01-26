// +-------------------------------------------< PREPROCESSING >--------------------------------------------+

#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
#endif

// +----------------------------------------------< INCLUDE >-----------------------------------------------+

#include <cassert>
#include <cinttypes>
#include <cstdio>

// +------------------------------------------< TYPE DEFINITION >-------------------------------------------+

typedef uint8_t byte_t;

// +------------------------------------------< GLOBAL VARIABLE >-------------------------------------------+

static const size_t WIDTH  = 303;
static const size_t HEIGHT = 243;

byte_t* inputImage;
byte_t* outputImage;

// +-----------------------------------< ITERATIVE THRESHOLD SELECTION >------------------------------------+

byte_t InitIterativeThresholdSelection(byte_t* image)
{
    assert(image != NULL);

    double foregroundMean = 0.0;
    double backgroundMean = 0.0;

    for (int iy = 0; iy < HEIGHT; ++iy)
        for (int ix = 0; ix < WIDTH; ++ix)
            foregroundMean += image[iy * WIDTH + ix];

    backgroundMean += image[0] + image[WIDTH - 1] + image[(HEIGHT - 1) * WIDTH] + image[(HEIGHT - 1) * WIDTH + (WIDTH - 1)];
    foregroundMean -= backgroundMean;

    foregroundMean /= (WIDTH - 2) * (HEIGHT - 2);
    backgroundMean /= 4.0;

    return static_cast<byte_t>((foregroundMean + backgroundMean) / 2.0 + 0.5);
}

byte_t ComputeIterativeThresholdSelection(uint32_t* histogram, byte_t threshold)
{
    assert(histogram != NULL);

    double   foregroundMean   = 0.0;
    double   backgroundMean   = 0.0;
    uint32_t foregroundNumber = 0;
    uint32_t backgroundNumber = 0;

    for (int brightness = 0; brightness <= threshold; ++brightness)
    {
        foregroundMean   += histogram[brightness] * brightness;
        foregroundNumber += histogram[brightness];
    }

    for (int brightness = threshold + 1; brightness < 256; ++brightness)
    {
        backgroundMean   += histogram[brightness] * brightness;
        backgroundNumber += histogram[brightness];
    }

    foregroundMean /= foregroundNumber;
    backgroundMean /= backgroundNumber;

    return static_cast<byte_t>((foregroundMean + backgroundMean) / 2.0 + 0.5);
}

byte_t IterativeThresholdSelection(byte_t* inputImage, byte_t* outputImage)
{
    assert(inputImage  != NULL);
    assert(outputImage != NULL);

    uint32_t histogram[256] = { 0 };
    byte_t   threshold      = 0;
    byte_t   prevThreshold  = 0;

    for (int iy = 0; iy < HEIGHT; ++iy)
        for (int ix = 0; ix < WIDTH; ++ix)
            histogram[inputImage[iy * WIDTH + ix]]++;

    threshold = InitIterativeThresholdSelection(inputImage);

    while (true)
    {
        prevThreshold = threshold;
        threshold     = ComputeIterativeThresholdSelection(histogram, prevThreshold);

        if (threshold == prevThreshold)
            break;
    }

    for (int iy = 0; iy < HEIGHT; ++iy)
        for (int ix = 0; ix < WIDTH; ++ix)
            outputImage[iy * WIDTH + ix] = (inputImage[iy * WIDTH + ix] < threshold) ? (0) : (255);

    return threshold;
}

// +------------------------------------------------< MAIN >------------------------------------------------+

int main(void)
{
    static const char* INPUT_RAW_FILE_NAME  = "hand.raw";
    static const char* OUTPUT_RAW_FILE_NAME = "hand_IterativeThresholdSelection.raw";

    FILE* fileStream;

    inputImage  = new byte_t[WIDTH * HEIGHT];
    outputImage = new byte_t[WIDTH * HEIGHT];

    fileStream = fopen(INPUT_RAW_FILE_NAME, "rb");
    fread(inputImage, sizeof(byte_t), WIDTH * HEIGHT, fileStream);
    fclose(fileStream);

    printf("[Iterative Threshold] %d\n", IterativeThresholdSelection(inputImage, outputImage));

    fileStream = fopen(OUTPUT_RAW_FILE_NAME, "w+b");
    fwrite(outputImage, sizeof(byte_t), WIDTH * HEIGHT, fileStream);
    fclose(fileStream);

    delete[] inputImage;
    delete[] outputImage;

    return 0;
}

// +------------------------------------------------< END >-------------------------------------------------+