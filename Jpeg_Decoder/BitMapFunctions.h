#ifndef BitMapFunctions
#define BitMapFunctions
#include "Structures.h"
#include "ReadFunctions.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstdint>
#include <fstream>

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
	head->filesize = head->headsize + (long)BytesPerLine * head->height;

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
	FILE* outBmp= fopen("NewBitmap.bmp", "wb");

	if (!outBmp)
	{
		cout << "\nThere is an issue opening up the output file";
		return 0;
	}

	fwrite("BM", 1, 2, outBmp);//write the first lines in 
	fwrite((char*) Header, 1, sizeof(*Header), outBmp);

	char* lineContainer = (char*)calloc(1, bytes);
	if (!lineContainer)
	{
		cout << "\nProblem Assigning Memory for Line Container";
		return 0;
	}

	//This is where we write the data from the info to the image
	//To get the raw image we can take the pixels from the info class 

	unsigned char* img = info->pixels;
	//cout << Header->width;
	int width = Header->width;
	/*cout << "\n"<< sizeof(unsigned char *);
	cout <<"\n" <<*(img + (48576));
	cout << "\n" <<*(img + (4 + 32 * 2) * 700 + 2);
	cout <<"\n" <<*(img+7279);
	int a = 0;
	/while (*(img + a) != NULL)
	{
		cout << *(img + a);
		a++;
	}
	cout << "\n\n\n\n\n\n\n\n\n\n\n\n\The Final Number for a was:" << a;
	*/
	int bit_pix = 1;

	for (int line = Header->height-1; line>=0; line--) {
		for (int i = 0; i < width; i++)
		{
			*(lineContainer + i * bit_pix) = *(img + (i + line * width) * bit_pix + 0);
			//*(lineContainer + i * bit_pix + 1) = *(img+ (i + line *width) * bit_pix + 1);
			//*(lineContainer + i * bit_pix + 2) = *(img + (i + line *width) * bit_pix + 2);
		}
		fwrite(lineContainer, 1, bytes, outBmp);
	}
	free(lineContainer);
	fclose(outBmp);
	return 1;
}

 int NewBetterBMP(struct JpegInfo* info)
 {
	 BmpHeader Mainhead;
	 BmpInfoHead infoHead;

	 Mainhead.sizeofFile = 54 + (info->width * info->height * 3);

	 infoHead.width = info->width;
	 infoHead.height = info->height;
	 infoHead.hor_rez = round(info->width * 7);
	 infoHead.ver_rez = round(info->height * 7);

	 ofstream fout("Output.bmp", ios::binary);
		 
	 fout.write((char *) &Mainhead, 14);
	 fout.write((char*)& infoHead, 40);
	 size_t numberOfPixels = info->width * info->height;
	
	 cout <<"\n" <<sizeof(&info->pixels[3]);
	 &info->pixels[3];

	 for (int i = 0; i < numberOfPixels; i++)
	 {
		 fout.write((char*)	&info->pixels[i], sizeof(&info->pixels[i]));
	 }
	 fout.close();
	 //cout<<"\na="<<a;

	 return 1;
 }






#endif // !BitMapFunctions
