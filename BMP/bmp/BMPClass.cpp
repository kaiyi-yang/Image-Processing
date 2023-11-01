#include <fstream>
#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

#include "BMPClass.h"

BMPImage::BMPImage()
{
    ImageWidth = 0;
    ImageHeight = 0;
    bitsDepth = 0;
    ColorTableandPixel.clear();
}

int BMPImage::ReadBMPImage(string fileName)  //read bmp file
{
    FileName = fileName;

    ifstream in(fileName.c_str(), ios::in | ios::binary);  //open

    if (!in)
    {
        cerr << "Unable to read this file" << endl;
        return false;
    }

    in.seekg(0, in.end);  // go to the end of the file
    int Filelength = in.tellg();
    in.seekg(0, in.beg);

    in.read((char*)&bmpHeader, sizeof(bmpHeader));  // read 14bytes
    in.read((char*)&bmpInfoHeader, sizeof(bmpInfoHeader));  // read 40bytes


    bitsDepth = bmpInfoHeader.bits;  // bits per pixel
    ImageWidth = bmpInfoHeader.width;
    ImageHeight = bmpInfoHeader.height;

    ByteBitsRatio = bitsDepth / 8;   // 3 or 4

    int totalHeaderLength = in.tellg();
    int residueLength = Filelength - totalHeaderLength;

    char* data = new char[residueLength]();  //初始化為0
    in.read(data, residueLength);

    for (auto i = 0; i < residueLength; i++)
    {
        ColorTableandPixel.push_back(data[i]);  // put in the vector
    }

    delete[]data;


    switch (bmpInfoHeader.bits)
    {
    case 32: // This bitmap is a 32-bit image

        this->ConvertRawDataIntoImage();
        break;

    case 24: // This is a 24-bit image

        this->ConvertRawDataIntoImage();
        break;

    default:
        cout << "strange bits info" << endl;
    }

    in.close();

}


void BMPImage::DumpBMPInfo()
{
    cout << "File name:" << FileName << endl;
    cout << "File Size:" << bmpHeader.size << endl;
    cout << "Image size:" << bmpInfoHeader.imagesize << endl;
    cout << "Image Width:" << bmpInfoHeader.width << endl;
    cout << "Image Height:" << bmpInfoHeader.height << endl;
    cout << "Bit depth:" << bmpInfoHeader.bits << endl;
    cout << "Color number:" << bmpInfoHeader.ncolours << endl;
}

int BMPImage::WriteBMPImage(string fileName)
{

    ofstream out(fileName.c_str(), ios::out | ios::binary);   //save

    if (!out)
    {
        cerr << "Unable to write the BMP file" << endl;
    }



    out.write((char*)&bmpHeader, sizeof(bmpHeader));
    out.write((char*)&bmpInfoHeader, sizeof(bmpInfoHeader));


    int dataLength = ColorTableandPixel.size();

    char* tempWriteData = new char[dataLength]();

    int Index = 0;
    for (auto charData : ColorTableandPixel)  // put vector in array
    {
        tempWriteData[Index] = charData;
        Index++;
    }

    out.write(tempWriteData, dataLength);
    out.close();

    delete[]tempWriteData;

    return 0;
}


void BMPImage::ConvertRawDataIntoImage()
{
    // This function is used to avoid padding issue

    int residue = (ImageWidth) % 4;
    int paddingNmber = 0;

    if (residue && bitsDepth == 24)
    {
        paddingNmber = residue;
    }

    int PixelIndex = 0;


    // read
    for (int j = 0; j < ImageHeight; j++)
    {
        for (int i = 0; i < ImageWidth * (bitsDepth >> 3); i += (bitsDepth) >> 3)   // 24位元>>3 = 3 ; 32位元>>3 = 4
        {
            if ((bitsDepth >> 3) == 4) // 32bit
            {

                if ((PixelIndex + i + 3) < ColorTableandPixel.size())
                {
                    Dummy32.b = ColorTableandPixel.at(PixelIndex + i);
                    Dummy32.g = ColorTableandPixel.at(PixelIndex + i + 1);
                    Dummy32.r = ColorTableandPixel.at(PixelIndex + i + 2);
                    Dummy32.S = ColorTableandPixel.at(PixelIndex + i + 3);
                    BMPImage32bit.push_back(Dummy32);
                }
                else {
                    cout << "Out of index";
                    return;
                }
            }


            if ((bitsDepth >> 3) == 3)   // 24bit
            {
                if ((PixelIndex + i + 2) < ColorTableandPixel.size())
                {
                    Dummy24.b = ColorTableandPixel.at(PixelIndex + i);
                    Dummy24.g = ColorTableandPixel.at(PixelIndex + i + 1);
                    Dummy24.r = ColorTableandPixel.at(PixelIndex + i + 2);
                    BMPImage24bit.push_back(Dummy24);
                }
                else {
                    cout << "Out of index";
                    return;
                }
            }

        }

        PixelIndex += ImageWidth * (bitsDepth >> 3);  //換下一列

        if (bitsDepth == 24)   //補0
            PixelIndex += paddingNmber;

    }

    cout << "PixelIndex:" << PixelIndex << endl;

}

void BMPImage::resolution(string fileName, unsigned int xresolution, unsigned int yresolution)
{
    ofstream out(fileName.c_str(), ios::out | ios::binary);

    if (!out)
    {
        cerr << "Unable to write the BMP file" << endl;
    }

    bmpInfoHeader.xresolution = xresolution; //set resolution
    bmpInfoHeader.yresolution = yresolution; //set resolution

    out.write((char*)&bmpHeader, sizeof(bmpHeader));
    out.write((char*)&bmpInfoHeader, sizeof(bmpInfoHeader));


    int dataLength = ColorTableandPixel.size();

    char* tempWriteData = new char[dataLength]();

    int Index = 0;
    for (auto charData : ColorTableandPixel)
    {
        tempWriteData[Index] = charData;
        Index++;
    }

    out.write(tempWriteData, dataLength);
    out.close();

    delete[]tempWriteData;

}


void BMPImage::BilinearScale(double scale, string fileName)  //scale up and down
{

    // first create a blank image plane
    int counter = 0;

    if (scale > 1)  // scale up
    {

        int intScale = (int)scale;

        int newImageWidth = int(ImageWidth * intScale);
        int newImageHeight = int(ImageHeight * intScale);


        unsigned char* ScaleUpImage = new unsigned char[newImageWidth * newImageHeight * (bitsDepth >> 3)]();   // 24位元>>3 = 3 ; 32位元>>3 = 4


        unsigned char* newSizeRed = new unsigned char[newImageWidth * newImageHeight]();
        unsigned char* newSizeGreen = new unsigned char[newImageWidth * newImageHeight]();
        unsigned char* newSizeBlue = new unsigned char[newImageWidth * newImageHeight]();


        int index = 0;

        // put the seed for interpolating

        for (int j = 0; j < newImageHeight; j += intScale)
        {

            for (int i = 0; i < newImageWidth; i += intScale)
            {

                if (bmpInfoHeader.bits == 24)
                {
                    newSizeRed[i + j * newImageWidth] = BMPImage24bit.at(index).r;
                    newSizeGreen[i + j * newImageWidth] = BMPImage24bit.at(index).g;
                    newSizeBlue[i + j * newImageWidth] = BMPImage24bit.at(index).b;
                }


                if (bmpInfoHeader.bits == 32)
                {
                    newSizeRed[i + j * newImageWidth] = BMPImage32bit.at(index).r;
                    newSizeGreen[i + j * newImageWidth] = BMPImage32bit.at(index).g;
                    newSizeBlue[i + j * newImageWidth] = BMPImage32bit.at(index).b;
                }
                index++;

            }


        }


        int w[4];  // one pixel has four weights
        int xReference[4];
        int yReference[4];

        for (int p = 0; p < 4; p++)
        {
            xReference[p] = 0;
            yReference[p] = 0;
            w[p] = 0;
        }


        
           // start interpolating

        for (int j = 0; j < newImageHeight - 1; j++)  //caculate weight 左上到右下
        {

            for (int i = 0; i < newImageWidth; i++)
            {
                counter++;
                
                if ((i % intScale) != 0 || (j % intScale) != 0)  // find w[0], if it's the seed then equal to zero
                {

                    xReference[0] = i - (i % intScale);
                    yReference[0] = j - (j % intScale);

                    w[0] = 2 * intScale - DistanceCalculate(xReference[0], i, yReference[0], j);
  


                    if (i > (newImageWidth - intScale))  // find w[1], if it's boundary then equal to zero
                    {
                        w[1] = 0;
                        xReference[1] = 0;
                        yReference[1] = 0;;

                    }
                    else {
                        xReference[1] = i - (i % intScale) + intScale;
                        yReference[1] = j - (j % intScale);

                        w[1] = 2 * intScale - DistanceCalculate(xReference[1], i, yReference[1], j);


                    }



                    if (j >= (newImageHeight - intScale))  // find w[2], if it's boundary then equal to zero
                    {
                        w[2] = 0;
                        xReference[2] = 0;
                        yReference[2] = 0;;

                    }
                    else {
                        xReference[2] = i - (i % intScale);
                        yReference[2] = j - (j % intScale) + intScale;

                        w[2] = 2 * intScale - DistanceCalculate(xReference[2], i, yReference[2], j);
                    }



                    if (j >= (newImageHeight - intScale) || i >= (newImageWidth - intScale))  // find w[3], if it's boundary then equal to zero
                    {
                        w[3] = 0;
                        xReference[3] = 0;
                        yReference[3] = 0;

                    }
                    else {

                        xReference[3] = i - (i % intScale) + intScale;
                        yReference[3] = j - (j % intScale) + intScale;

                        w[3] = 2 * intScale - DistanceCalculate(xReference[3], i, yReference[3], j);
                    }



                    int NewRed = 0;
                    int NewGreen = 0;
                    int NewBlue = 0;
                    int sumWeighting = 1;



                    for (int k = 0; k < 4; k++)  // caculate the new color
                    {
                        if (xReference[k] + yReference[k] * newImageWidth >= newImageWidth * newImageHeight)
                        {
                            cout << i << "  " << j << " " << k << " " << xReference[k] << "  " << yReference[k] << "   " << newImageWidth << "   " << newImageHeight << endl;
                            cout << "Problem happen" << endl;
                        }

                        NewRed += w[k] * newSizeRed[xReference[k] + yReference[k] * newImageWidth];
                        NewGreen += w[k] * newSizeGreen[xReference[k] + yReference[k] * newImageWidth];
                        NewBlue += w[k] * newSizeBlue[xReference[k] + yReference[k] * newImageWidth];

                        sumWeighting += w[k];

                    }

                    //put new color in pixel
                    newSizeRed[i + j * newImageWidth] = int(NewRed / sumWeighting);
                    newSizeGreen[i + j * newImageWidth] = int(NewGreen / sumWeighting);
                    newSizeBlue[i + j * newImageWidth] = int(NewBlue / sumWeighting);


                }

            }

        }


        FILEHEADER newFileHeader = bmpHeader;
        INFOHEADER newInfoHeader = bmpInfoHeader;


        newInfoHeader.width = newImageWidth;
        newInfoHeader.height = newImageHeight;

        WriteBMPFromRawData(newImageWidth, newImageHeight, newSizeRed, newSizeGreen, newSizeBlue, newInfoHeader.bits,
            newFileHeader, newInfoHeader, fileName);


    }


    if (scale <= 1 && scale > 0)  //scale down
    {
        int intRatio = int(1 / scale);


        cout << "IntRatio:" << intRatio << endl;

        int newImageWidth = int(ImageWidth / intRatio);
        int newImageHeight = int(ImageHeight / intRatio);

        int ColorIndex = 0;

        unsigned char* newSizeRed = new unsigned char[newImageWidth * newImageHeight]();
        unsigned char* newSizeGreen = new unsigned char[newImageWidth * newImageHeight]();
        unsigned char* newSizeBlue = new unsigned char[newImageWidth * newImageHeight]();


        for (int j = 0; j < newImageHeight; j++)
        {
            for (int i = 0; i < newImageWidth; i++)
            {

                if (bitsDepth == 24)
                {
                    newSizeRed[i + j * newImageWidth] = BMPImage24bit.at(ColorIndex).r;
                    newSizeGreen[i + j * newImageWidth] = BMPImage24bit.at(ColorIndex).g;
                    newSizeBlue[i + j * newImageWidth] = BMPImage24bit.at(ColorIndex).b;
                }

                if (bitsDepth == 32)
                {
                    newSizeRed[i + j * newImageWidth] = BMPImage32bit.at(ColorIndex).r;
                    newSizeGreen[i + j * newImageWidth] = BMPImage32bit.at(ColorIndex).g;
                    newSizeBlue[i + j * newImageWidth] = BMPImage32bit.at(ColorIndex).b;
                }


                ColorIndex += intRatio;  // pick up the pixels

            }
        }


        cout << "Create new bmp image" << endl;


        FILEHEADER newFileHeader = bmpHeader;
        INFOHEADER newInfoHeader = bmpInfoHeader;


        newInfoHeader.width = newImageWidth;
        newInfoHeader.height = newImageHeight;

        WriteBMPFromRawData(newImageWidth, newImageHeight, newSizeRed, newSizeGreen, newSizeBlue, newInfoHeader.bits,
            newFileHeader, newInfoHeader, fileName);


    }

}

int BMPImage::DistanceCalculate(int x1, int x2, int y1, int y2)
{
    return int((sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2))));
}


void BMPImage::WriteBMPFromRawData(int newImageWidth, int newImageHeight,
    unsigned char* R, unsigned char* G, unsigned char* B, int BitsDepth,
    FILEHEADER newFileHeader, INFOHEADER newInfoHeader, string fileName)
{
    int FinalImageSize = 0;

    newInfoHeader.bits = BitsDepth;
    newInfoHeader.width = newImageWidth;
    newInfoHeader.height = newImageHeight;


    newInfoHeader.size = 40;
    newInfoHeader.imagesize = newImageWidth * newImageHeight * (BitsDepth >> 3);  // 3 or 4


    newFileHeader.size = newInfoHeader.imagesize + 0x36;    
    newFileHeader.offset = 0x36;


    cout << BitsDepth << endl;


    ofstream out(fileName.c_str(), ios::out | ios::binary);

    if (!out)
    {
        cerr << "Unable to write file" << endl;
        exit(0);
    }

    out.write((char*)&newFileHeader, sizeof(newFileHeader));
    out.write((char*)&newInfoHeader, sizeof(newInfoHeader));


    char paddingZero = 0x00;

    unsigned char* FinalImagePlan = new unsigned char[FinalImageSize]();

    int IndexIncrement = 0;

    if (BitsDepth == 24)
    {
        FinalImageSize = (newImageWidth * newImageHeight * 3) + (newImageWidth % 4) * newImageHeight;
        FinalImagePlan = new unsigned char[FinalImageSize]();  // create image plan
        IndexIncrement = 3;
    }


    if (BitsDepth == 32)
    {
        FinalImageSize = (newImageWidth * newImageHeight * 4);
        FinalImagePlan = new unsigned char[FinalImageSize]();  // create image plan
        IndexIncrement = 4;
    }

    int Index = 0;

    cout << "Mode number:" << newImageWidth % 4 << endl;
 
    // put the RGB in plan
    for (int j = 0; j < newImageHeight; j++)
    {
        for (int i = 0; i < newImageWidth; i++)
        {
            FinalImagePlan[Index] = B[i + j * newImageWidth];
            FinalImagePlan[Index + 1] = G[i + j * newImageWidth];
            FinalImagePlan[Index + 2] = R[i + j * newImageWidth];

            if (BitsDepth == 32)
            {
                FinalImagePlan[Index + 3] = 0x00;  // S 
            }

            Index += IndexIncrement;
            if (Index > FinalImageSize)
            {
                cout << i << "   " << j << " " << newImageWidth
                    << "    " << newImageHeight << "   " << Index << "   " << FinalImageSize << "   Error at size" << endl;

            }


        }

        if (BitsDepth == 24 && (newImageWidth % 4) > 0) {
            // section for padding
            for (int p = 0; p < (newImageWidth % 4); p++)

                FinalImagePlan[Index] = paddingZero;
            Index++;

            // cout << Index << endl;

            if (Index >= FinalImageSize)
            {
                cout << " padding Error at size" << endl;
            }

        }

    }

    out.write((char*)FinalImagePlan, FinalImageSize);

    out.close();

}
