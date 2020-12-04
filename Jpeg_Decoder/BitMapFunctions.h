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


 int WriteToBMP(struct JpegInfo* info)
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
	Header->ypix = 1000;
	Header->xpix = 1000;
	FILE* outBmp= fopen("NewBitmap.bmp", "wb");

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

	//This is where we write the data from the info to the image
	//To get the raw image we can take the pixels from the info class 
	unsigned char* raw_image;
	 raw_image = ( unsigned char*)info->pixels;
	//cout << Header->width;
	
	/*cout << "\n"<< sizeof(unsigned char *);
	cout <<"\n" <<*(img + (48576));
	cout << "\n" <<*(img + (4 + 32 * 2) * 700 + 2);
	cout <<"\n" <<*(img+7279);
	int a = 0;*/	
	
	
	int bytes_per_pixel =1;
	
	cout << "\n " << Header->filesize;
	cout << "\n " << Header->filesize/3;
	const int b = Header->filesize;
	int* arr = (int*)malloc(sizeof(*arr)*(Header->filesize/3));

	for (int l = 0; l < (Header->filesize/3); l++)
	{
		int greyscale = (int)raw_image[l];
		greyscale = greyscale + 128;
		arr[l] = greyscale;
		//cout << arr[l]<<" ";
	}
	//cout << arr;
	int a=0;
	while (info->pixels[a] != NULL)
	{
		//cout <<  arr[a];
		a++;
	}
	//cout << "\n\n\n\n\n\n\n\n\n\n\n\n\The Final Number for a was:" << a;

	int line, x;
	for ( line = Header->height; line>=0; line--) {
		for ( x = 0; x < width; x++)
		{
			*(lineContainer + x * bytes_per_pixel) = *(raw_image + (x + line * width) * bytes_per_pixel +2);
			*(lineContainer + x * bytes_per_pixel + 1) = *(raw_image + (x + line * width) * bytes_per_pixel + 1);
			*(lineContainer + x * bytes_per_pixel + 2) = *(raw_image + (x + line * width) * bytes_per_pixel + 0);
		}
		fwrite(lineContainer, 1, bytes, outBmp);
	}
	free(lineContainer);
	fclose(outBmp);
	return 1;
}

 int NewBetterBMP(struct JpegInfo* info)
 {
	/* BmpHeader Mainhead;
	 BmpInfoHead infoHead;

	 Mainhead.sizeofFile = 54 + (info->width * info->height * 3);

	 infoHead.width = info->width;
	 infoHead.height = info->height;
	 infoHead.hor_rez = round(info->width * 7);
	 infoHead.ver_rez = round(info->height * 7);

	 ofstream fout("Output.bmp", ios::binary);
		 
	 fout.write((char *) &Mainhead, 14);
	 fout.write((char*) &infoHead, 40);
	 size_t numberOfPixels = info->width * info->height;
	
	 cout <<"\n" <<sizeof(&info->pixels[3]);
	 &info->pixels[3];
	 cout << "\n" << sizeof(char);
	 cout << "\n" << sizeof(char*);
	 for (int i = 0; i < numberOfPixels; i++)
	 {
		 &info->pixels[i];
		 fout.write((char*)	&info->pixels[i], sizeof(&info->pixels[i]));
	 }
	 fout.close();
	 //cout<<"\na="<<a;

	 return 1;*/
	 
	 int wid, len;

	  //if (info->width >= info->height)
	//  {
		   wid = info->width;
		   len = info->height;
	  //}
	  //else {
		 //  wid = info->height;
		 //  len = info->width;
	//  }
	  

	  int* greyscale=(int*)malloc(sizeof(*greyscale)*wid*len*3);
	  int r, g, b;
	 for (int l = 0; l < len; l++)
	 {
		 for (int m = 0; m < wid; m++)
		 {
			 greyscale[(l*len)+m] = (int)info->pixels[(l*len)+m];
			 greyscale[(l * len) + m] = greyscale[(l * len) + m] + 128;
		 }
		 
	 }


	 FILE* f;
	 unsigned char* img = NULL;
	 int filesize = 54 + 3 * wid* len;  //w is your image width, h is image height, both int

	 img = (unsigned char*)malloc(3 * wid * len);
	 memset(img, 0, 3 * wid * len);

	 for (int i = 0; i < wid; i++)
	 {
		 for (int j = 0; j < len; j++)
		 {
			 
			 int x = i; int y = (len - 1) - j;
			 
			 /*r = greyscale[(i * wid) + j] * 255;
			 g = greyscale[(i * wid) + j] * 255;
			 b = greyscale[(i * wid) + j] * 255;
			 if (r > 255) r = 255;
			 if (g > 255) g = 255;
			 if (b > 255) b = 255;
			 */

			 img[(x +y * wid) * 3 + 2] = (unsigned char)(info->pixels[(y * wid) + x]);
			 img[(x +y * wid) * 3 + 1] = (unsigned char)(info->pixels[(y * wid) + x]);
			 img[(x +y * wid) * 3 + 0] = (unsigned char)(info->pixels[(y * wid) + x]);
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






#endif // !BitMapFunctions
