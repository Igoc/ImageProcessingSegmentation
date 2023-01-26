// +-------------------------------------------< PREPROCESSING >--------------------------------------------+

#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
#endif

// +----------------------------------------------< INCLUDE >-----------------------------------------------+

#include <algorithm>
#include <cassert>
#include <cinttypes>
#include <cstdio>
#include <cstring>

// +------------------------------------------< TYPE DEFINITION >-------------------------------------------+

typedef uint8_t byte_t;

// +------------------------------------------< GLOBAL VARIABLE >-------------------------------------------+

static const size_t WIDTH  = 303;
static const size_t HEIGHT = 243;

byte_t* inputImage;
byte_t* outputImage;

// +------------------------------------------< EFFICIENT 2-PASS >------------------------------------------+

uint32_t* TopDownPass(byte_t* image, uint32_t* label)
{
    assert(image != NULL);
    assert(label != NULL);

    uint32_t minLabel = 0;

    for (int iy = 0; iy < HEIGHT - 1; ++iy)
        for (int ix = 1; ix < WIDTH - 1; ++ix)
            if (label[iy * WIDTH + ix] != 0)
            {
                minLabel = label[iy * WIDTH + ix];

                if (image[iy * WIDTH + ix] == image[iy * WIDTH + (ix + 1)])
                {
                    minLabel = (minLabel < label[iy * WIDTH + (ix + 1)]) ?
                               (minLabel) : (label[iy * WIDTH + (ix + 1)]);

                    label[iy * WIDTH + ix]       = minLabel;
                    label[iy * WIDTH + (ix + 1)] = minLabel;
                }
                
                if (image[iy * WIDTH + ix] == image[(iy + 1) * WIDTH + (ix + 1)])
                {
                    minLabel = (minLabel < label[(iy + 1) * WIDTH + (ix + 1)]) ?
                               (minLabel) : (label[(iy + 1) * WIDTH + (ix + 1)]);

                    label[iy * WIDTH + ix]             = minLabel;
                    label[(iy + 1) * WIDTH + (ix + 1)] = minLabel;
                }
                
                if (image[iy * WIDTH + ix] == image[(iy + 1) * WIDTH + ix])
                {
                    minLabel = (minLabel < label[(iy + 1) * WIDTH + ix]) ?
                               (minLabel) : (label[(iy + 1) * WIDTH + ix]);

                    label[iy * WIDTH + ix]       = minLabel;
                    label[(iy + 1) * WIDTH + ix] = minLabel;
                }

                if (image[iy * WIDTH + ix] == image[(iy + 1) * WIDTH + (ix - 1)])
                {
                    minLabel = (minLabel < label[(iy + 1) * WIDTH + (ix - 1)]) ?
                               (minLabel) : (label[(iy + 1) * WIDTH + (ix - 1)]);

                    label[iy * WIDTH + ix]             = minLabel;
                    label[(iy + 1) * WIDTH + (ix - 1)] = minLabel;
                }
            }

    return label;
}

uint32_t* BottomUpPass(byte_t* image, uint32_t* label)
{
    assert(image != NULL);
    assert(label != NULL);

    uint32_t minLabel = 0;

    for (int iy = HEIGHT - 1; iy > 0; --iy)
        for (int ix = WIDTH - 2; ix > 0; --ix)
            if (label[iy * WIDTH + ix] != 0)
            {
                minLabel = label[iy * WIDTH + ix];

                if (image[iy * WIDTH + ix] == image[iy * WIDTH + (ix - 1)])
                {
                    minLabel = (minLabel < label[iy * WIDTH + (ix - 1)]) ?
                               (minLabel) : (label[iy * WIDTH + (ix - 1)]);

                    label[iy * WIDTH + ix]       = minLabel;
                    label[iy * WIDTH + (ix - 1)] = minLabel;
                }
                
                if (image[iy * WIDTH + ix] == image[(iy - 1) * WIDTH + (ix - 1)])
                {
                    minLabel = (minLabel < label[(iy - 1) * WIDTH + (ix - 1)]) ?
                               (minLabel) : (label[(iy - 1) * WIDTH + (ix - 1)]);

                    label[iy * WIDTH + ix]             = minLabel;
                    label[(iy - 1) * WIDTH + (ix - 1)] = minLabel;
                }
                
                if (image[iy * WIDTH + ix] == image[(iy - 1) * WIDTH + ix])
                {
                    minLabel = (minLabel < label[(iy - 1) * WIDTH + ix]) ?
                               (minLabel) : (label[(iy - 1) * WIDTH + ix]);

                    label[iy * WIDTH + ix]       = minLabel;
                    label[(iy - 1) * WIDTH + ix] = minLabel;
                }

                if (image[iy * WIDTH + ix] == image[(iy - 1) * WIDTH + (ix + 1)])
                {
                    minLabel = (minLabel < label[(iy - 1) * WIDTH + (ix + 1)]) ?
                               (minLabel) : (label[(iy - 1) * WIDTH + (ix + 1)]);

                    label[iy * WIDTH + ix]             = minLabel;
                    label[(iy - 1) * WIDTH + (ix + 1)] = minLabel;
                }
            }

    return label;
}

uint32_t LabelRenumbering(uint32_t* label, uint32_t labelNumber)
{
    assert(label != NULL);

    uint32_t* sortedLabel           = NULL;
    uint32_t* renumberedLabel       = NULL;
    uint32_t  sortedlabelNumber     = 0;
    uint32_t  renumberedLabelNumber = 0;

    sortedLabel = new uint32_t[labelNumber]();

    for (unsigned int index = 0; index < WIDTH * HEIGHT; ++index)
        if (label[index] != 0)
            sortedLabel[sortedlabelNumber++] = label[index];

    std::sort(sortedLabel, sortedLabel + sortedlabelNumber);

    renumberedLabel                 = new uint32_t[sortedlabelNumber]();
    renumberedLabel[sortedLabel[0]] = renumberedLabelNumber++;

    for (unsigned int index = 1; index < sortedlabelNumber; ++index)
        if (sortedLabel[index] != sortedLabel[index - 1])
            renumberedLabel[sortedLabel[index]] = renumberedLabelNumber++;

    for (unsigned int index = 0; index < WIDTH * HEIGHT; ++index)
        if (label[index] != 0)
            label[index] = renumberedLabel[label[index]];

    delete[] sortedLabel;
    delete[] renumberedLabel;

    return renumberedLabelNumber;
}

uint32_t* ExtractLargeAreaLabel(uint32_t* inputLabel, uint32_t labelNumber, uint32_t* outputLabel, uint32_t areaExtractNumber)
{
    assert(inputLabel  != NULL);
    assert(outputLabel != NULL);
    assert(areaExtractNumber > 0);

    uint32_t* labelHistogram    = NULL;
    uint32_t* extractedAreaSize = NULL;

    labelHistogram    = new uint32_t[labelNumber]();
    extractedAreaSize = new uint32_t[areaExtractNumber]();

    for (unsigned int index = 0; index < WIDTH * HEIGHT; ++index)
        if (inputLabel[index] != 0)
            labelHistogram[inputLabel[index]]++;
    
    for (unsigned int labelIndex = 0; labelIndex < labelNumber; ++labelIndex)
        if (labelHistogram[labelIndex] > extractedAreaSize[0])
        {
            outputLabel[0]       = labelIndex;
            extractedAreaSize[0] = labelHistogram[labelIndex];
        }

    for (unsigned int extractIndex = 1; extractIndex < areaExtractNumber; ++extractIndex)
        for (unsigned int labelIndex = 0; labelIndex < labelNumber; ++labelIndex)
            if (labelHistogram[labelIndex] <= extractedAreaSize[extractIndex - 1] && labelIndex != outputLabel[extractIndex - 1])
                if (labelHistogram[labelIndex] > extractedAreaSize[extractIndex])
                {
                    outputLabel[extractIndex]       = labelIndex;
                    extractedAreaSize[extractIndex] = labelHistogram[labelIndex];
                }

    delete[] labelHistogram;
    delete[] extractedAreaSize;

    return outputLabel;
}

byte_t* Efficient2Pass(byte_t* inputImage, byte_t* outputImage, uint32_t areaExtractNumber = 1)
{
    assert(inputImage  != NULL);
    assert(outputImage != NULL);
    assert(areaExtractNumber > 0);

    uint32_t* label          = NULL;
    uint32_t* prevLabel      = NULL;
    uint32_t* extractedLabel = NULL;
    uint32_t  labelNumber    = 1;
    bool      difference     = false;

    memset(outputImage, 0, sizeof(byte_t) * WIDTH * HEIGHT);

    label          = new uint32_t[WIDTH * HEIGHT]();
    prevLabel      = new uint32_t[WIDTH * HEIGHT]();
    extractedLabel = new uint32_t[areaExtractNumber]();

    for (int iy = 0; iy < HEIGHT; ++iy)
        for (int ix = 0; ix < WIDTH; ++ix)
            if (inputImage[iy * WIDTH + ix] != 0)
                label[iy * WIDTH + ix] = labelNumber++;

    while (true)
    {
        memcpy(prevLabel, label, sizeof(uint32_t) * WIDTH * HEIGHT);
        difference = false;

        TopDownPass(inputImage, label);
        BottomUpPass(inputImage, label);

        for (unsigned int index = 0; index < WIDTH * HEIGHT; ++index)
            if (label[index] != prevLabel[index])
            {
                difference = true;
                break;
            }

        if (difference == false)
            break;
    }

    labelNumber = LabelRenumbering(label, labelNumber);
    ExtractLargeAreaLabel(label, labelNumber, extractedLabel, areaExtractNumber);
    
    for (unsigned int index = 0; index < WIDTH * HEIGHT; ++index)
        for (unsigned int extractIndex = 0; extractIndex < areaExtractNumber; ++extractIndex)
            if (label[index] == extractedLabel[extractIndex])
            {
                outputImage[index] = 255;
                break;
            }

    delete[] label;
    delete[] prevLabel;
    delete[] extractedLabel;

    return outputImage;
}

// +------------------------------------------------< MAIN >------------------------------------------------+

int main(void)
{
    static const char* INPUT_RAW_FILE_NAME  = "hand_OtsuThresholdSelection.raw";
    static const char* OUTPUT_RAW_FILE_NAME = "hand_Efficient2Pass.raw";

    FILE* fileStream;

    inputImage  = new byte_t[WIDTH * HEIGHT];
    outputImage = new byte_t[WIDTH * HEIGHT];

    fileStream = fopen(INPUT_RAW_FILE_NAME, "rb");
    fread(inputImage, sizeof(byte_t), WIDTH * HEIGHT, fileStream);
    fclose(fileStream);

    Efficient2Pass(inputImage, outputImage, 2);

    fileStream = fopen(OUTPUT_RAW_FILE_NAME, "w+b");
    fwrite(outputImage, sizeof(byte_t), WIDTH * HEIGHT, fileStream);
    fclose(fileStream);

    delete[] inputImage;
    delete[] outputImage;

    return 0;
}

// +------------------------------------------------< END >-------------------------------------------------+