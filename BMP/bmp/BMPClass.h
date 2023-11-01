#ifndef BMPCLass_H
#define BMPCLass_H

#include <string>
#include <vector>

#include "bmp_header.h"

using namespace std;

class BMPImage {

public:
    BMPImage();

    int WriteBMPImage(string fileName); // Create a bmp file with the info
    int ReadBMPImage(string fileName);  // Read a bmp file
    void DumpBMPInfo(); // list all of the information in the headers
    void resolution(string fileName, unsigned int xresolution, unsigned int yresolution); //Write a bmp file with different resolution
    void BilinearScale(double scale, string fileName); // the scale value could be larger/smaller than 1.
    void WriteBMPFromRawData(int newImageWidth, int newImageHeight,
        unsigned char* R, unsigned char* G, unsigned char* B, int BitsDepth,
        FILEHEADER newFileHeader, INFOHEADER newInfoHeader, string fileName);


private:

    unsigned int ImageWidth;
    unsigned int ImageHeight;

    int bitsDepth;

    FILEHEADER bmpHeader;
    INFOHEADER bmpInfoHeader;

    vector<unsigned char> colorTable;
    vector<unsigned char> PixelValue;

    string FileName;

    vector<char> ColorTableandPixel;

    vector<BitBMP32bit> BMPImage32bit;
    BitBMP32bit Dummy32;

    vector<BitBMP24bit> BMPImage24bit;
    BitBMP24bit Dummy24;

    int ByteBitsRatio;

    void ConvertRawDataIntoImage();

    int DistanceCalculate(int x1, int x2, int y1, int y2);
};

#endif 


