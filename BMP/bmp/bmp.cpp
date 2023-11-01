#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <stdio.h>

#include "bmp_header.h"
#include "BMPClass.h"

using namespace std;
using std::stoi;

int main(int argc, char* argv[])
{

    if (argc < 7)
    {
        cout << "Unable to parse the file names" << endl;
        return 0;
    }

    BMPImage Image1;
    string tempFile(argv[1]);
    string outFile(argv[2]);
    string method(argv[3]);
    string xresolution(argv[4]);
    string yresolution(argv[5]);
    string scale(argv[6]);

    if (method == "r")  // Read and Write a bmp file with different resolution
    {
        Image1.ReadBMPImage(tempFile);
        Image1.DumpBMPInfo();
        Image1.resolution(outFile, stoi(xresolution), stoi(yresolution));
        return 0;
    }

    if (method == "s")  // Read and Write a bmp file 
    {
        Image1.ReadBMPImage(tempFile);
        Image1.DumpBMPInfo();
        Image1.WriteBMPImage(outFile);
        return 0;
    }

    if (method == "b")  // bilinear scale
    {
        Image1.ReadBMPImage(tempFile);
        Image1.DumpBMPInfo();
        Image1.BilinearScale(stod(scale), outFile);
        return 0;
    }

    return 0;
}
