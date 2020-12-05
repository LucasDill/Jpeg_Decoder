#ifndef BitMapFunctions
#define BitMapFunctions
#include "Structures.h"
#include "ReadFunctions.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstdint>
//#include <fstream>

int intializeHeader(struct JpegInfo* info, struct BMPFileHeader* head) {
	memset((char*) head, 0, sizeof(BMPFileHeader)); /* sets everything to 0 */
	head->headsize = 54L;
	head->width = (long)info->width;
	head->datsize = 0x28L;//predefined for this kind of image 
	head->height = (long)info->height;
	head->Planes = 1;
	head->bits = 24;//CHANGE TO SEE WHAT HAPPENS 
	head->Compression = 0L;
	int BytesPerLine = head->width*3;//This is for 24 bit change it for others

	if (BytesPerLine & 0x0003)
	{
		BytesPerLine |= 0x0003;
		++BytesPerLine;
	}
	head->filesize = head->headsize + (long(head->width)*long(head->height)*3);

	cout << "\n Bytes Per Line: " << BytesPerLine<<"\n";
	return BytesPerLine;
}


 int WriteBMP_Org(struct JpegInfo* info)
{
	struct BMPFileHeader* Header=(BMPFileHeader*)malloc(sizeof(*Header));

	//cout << sizeof(*Header);
	//cout <<sizeof(BMPFileHeader*);
	if (Header)
	{
		memset(Header, 0, sizeof(Header));
	}
	else {
		cout << "Error Handling Header Buffer: ";
		return 0;
	}


	int bytes=intializeHeader(info, Header);//get the header ready for the new image 
	FILE* outBmp= fopen("NewBit.bmp", "wb");

	if (!outBmp)
	{
		cout << "\nThere is an issue opening up the output file";
		return 0;
	}
	int width = Header->width;
	fwrite("BM", 1, 2, outBmp);//write the first lines in 
	fwrite((char*) Header, 1, sizeof(*Header), outBmp);
	   
	char* lineContainer;

	 lineContainer = (char*)calloc(1, bytes);
	if (!lineContainer)
	{
		cout << "\nProblem Assigning Memory for Line Container";
		return 0;
	}



	for (int line = Header->height-1; line>=0; line--) {
		for (int x = 0; x < Header->width; x++)
		{
			*(lineContainer + x * 3 +2) = *(info->pixels + (x + line * width));
			*(lineContainer + x * 3 + 1) = *(info->pixels + (x + line * width));
			*(lineContainer + x * 3 + 0) = *(info->pixels + (x + line * width));
		}
		fwrite(lineContainer, 1, bytes, outBmp);
	}
	free(lineContainer);
	fclose(outBmp);
	return Header->filesize;
}
 /*
 int WriteBMP(struct JpegInfo* info)
 {
	
	 
	 int wid, len;

	
		   wid = info->width;
		   len = info->height;

	  

	  int* greyscale=(int*)malloc(sizeof(*greyscale)*wid*len*3);
	  

	 FILE* f;
	 unsigned char* img = NULL;
	 int filesize = 54 + 3 * wid* len;  //w is your image width, h is image height, both int

	 img = (unsigned char*)malloc(filesize);
	 memset(img, 0, filesize);

	 for (int i = 0; i < wid; i++)
	 {
		 for (int j = 0; j < len; j++)
		 {
			 
			 int y = (len - 1) - j;//reverse the 
			 
			 img[(i +y * wid) * 3 + 2] = (unsigned char)(info->pixels[(y * wid) + i]);
			 img[(i +y * wid) * 3 + 1] = (unsigned char)(info->pixels[(y * wid) + i]);
			 img[(i +y * wid) * 3 + 0] = (unsigned char)(info->pixels[(y * wid) + i]);
		 }
	 }

	 unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
	 unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };
	 unsigned char bmppad[3] = { 0,0,0 };

	 bmpfileheader[2] = (unsigned char)(filesize);
	 bmpfileheader[3] = (unsigned char)(filesize >> 8);
	 bmpfileheader[4] = (unsigned char)(filesize >> 16);
	 bmpfileheader[5] = (unsigned char)(filesize >> 24);

	 bmpinfoheader[4] = (unsigned char)(wid);
	 bmpinfoheader[5] = (unsigned char)(wid >> 8);
	 bmpinfoheader[6] = (unsigned char)(wid >> 16);
	 bmpinfoheader[7] = (unsigned char)(wid >> 24);
	 bmpinfoheader[8] = (unsigned char)(len);
	 bmpinfoheader[9] = (unsigned char)(len >> 8);
	 bmpinfoheader[10] = (unsigned char)(len >> 16);
	 bmpinfoheader[11] = (unsigned char)(len >> 24);

	 f = fopen("img.bmp", "wb");
	 fwrite(bmpfileheader, 1, 14, f);
	 fwrite(bmpinfoheader, 1, 40, f);
	 for (int i = 0; i < wid; i++)
	 {
		 fwrite(img + (wid * (len - i - 1) * 3), 3, wid, f);
		 fwrite(bmppad, 1, (4 - (wid * 3) % 4) % 4, f);
	 }

	 free(img);
	 fclose(f);


	 return filesize;
 }
 */





#endif // !BitMapFunctions
