#ifndef Structures
#define Structures

struct BitBuffer {
	unsigned char* data;
	int length;
	int read_position;
	int read_buffer;
	int read_bits;
	int read_endOfFile;
};


struct hArray {
	unsigned char count;
	struct hVlc* v;
};

struct hTable {
	unsigned char count;
	struct hArray arr[16];
};

struct hVlc {
	unsigned short val;
	unsigned char code;

};

struct sampleInfo {
	int id;
	int Hor_sample;
	int Vert_sample;
	int Hor_MCU;
	int Vert_MCU;
	int tableID;
	int dc_id;
	int ac_id;
	int dc;
	int vector[64];
	unsigned char pixels[64];
};

struct JpegInfo {
	int width, height;
	unsigned char* table[2];//quantization tables 
	int table_count;
	int component_count;
	struct hTable htab[4];//Huffman tables 
	struct sampleInfo comp;

	//////////////this is all of the info for the frame of bits 
	int mcu_width;
	int mcu_height;
	int hors_mcu;
	int vert_mcu;
	int blocks_mcu;

	int rest;
	int restNext;
	int restCount;

	unsigned char* scan;
	int scan_length;

	unsigned char* pixels;
	int pixel_length;// length of the pixels
};



static unsigned char ZigMat[64] = {
	0,  1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63,
};

#define W1 2841
#define W2 2676
#define W3 2408
#define W5 1609
#define W6 1108
#define W7 565
////////////////////////////////////////////////////////NEXT SECTION IS FOR BMP FILE CONVERSION 
struct BMPFileHeader {
	long filesize;
	char reserved[2];
	long headsize;
	long datsize;
	long width;
	long height;
	short Planes;
	short bits;
	long Compression;
	long ImageSize;
	long xpix=1000;
	long ypix=1000;
	long clrused;
	long Important;
};



#endif
