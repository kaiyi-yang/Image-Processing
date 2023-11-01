#ifndef BMP_HEADER
#define BMP_HEADER

struct BitBMP24bit {  //24bit pixel
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct BitBMP32bit {  //32bit pixel
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char S;
};

#pragma pack(1)
typedef struct {
	unsigned short type;    // type : Magic identifier BM(0x42,0x4d) 
	unsigned int size;     // File size in bytes
	unsigned short reserved1, reserved2;   // «O¯dÄæ¦ì 
	unsigned int offset;   // Offset to image data, bytes 
} FILEHEADER;

#pragma pack(1)
typedef struct {
	unsigned int size;   // Info Header size in bytes
	unsigned int width, height;  // Width and height of image 
	unsigned short planes;   // Number of colour planes
	unsigned short bits;   // Bits per pixel 
	unsigned int compression;  // Compression type
	unsigned int imagesize;  // Image size in bytes 
	unsigned int xresolution, yresolution;  // Pixels per meter
	unsigned int ncolours;  // Number of colours 
	unsigned int importantcolours;  // Important colours 
} INFOHEADER;

#endif 
