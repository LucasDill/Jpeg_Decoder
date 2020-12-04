#include "Structures.h"
#include "ReadFunctions.h"
#include "BitMapFunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

void _save_to_ppm(struct JpegInfo* j) {
	//std::cout << "\n THERE IS NO SCAN OUT\nj: " << j->height;
	if (j->scan) {
		//cout << j->pixels;
		//std::cout << "\nTHERE IS A SCAN OUT";
		
		FILE* fg = fopen("expo.ppm", "wb");
		assert(fg);
		std::cout << j;
		fprintf(fg, "P%d\n", j->blocks_mcu <= 1 ? 5 : 6);
		fprintf(fg, "%d %d\n255\n", j->width, j->height);
		fwrite(j->pixels, 1, j->pixel_length, fg);
		fclose(fg);
		
		printf_s( "\nImage saved as a ppm file\n");
		free(fg);
	}
}


int main(int argc, char* argv[])
{
	//exit(1);
	FILE* JpegFile;
	
	fopen_s(&JpegFile, "Divisible.jpg", "rb");//OPen the image as a file 

	if (!JpegFile) {// if the image is not valid 
		cout << "There Was an Error opening the image\n";
	}
	else {
		cout << "The Image was Opened\n";
	}

	fseek(JpegFile, 0, SEEK_END);//look for the end of the file 
	int JpgSize = (unsigned int)ftell(JpegFile);//get the position of the file to show the size of the origional file 
	cout << "The Origional file size is: " << JpgSize<<" Bytes\n";
	unsigned char* JpgBuffer = (unsigned char*)malloc(JpgSize);//create a buffer for the image file 
	cout << "\nSize by another method: " << sizeof(*JpegFile);
	fseek(JpegFile, 0, SEEK_SET);//go back to the start of the file 

	int size = (int)fread(JpgBuffer, 1, JpgSize, JpegFile);//read each element to the buffer and get the total amount read 
	
	if (size != JpgSize) {//if the size is different there was an issue reading the file 
		cout << "ERROR READING FILE TO BUFFER";
	}

	fclose(JpegFile);//close the file now we are done with it 
	
	///////////////CREATE THE BUFFERS TO READ FROM THE FILE 
	//create the bit buffer and fill with zeros apart from the length and data 

	
	struct BitBuffer* bitbuf = (BitBuffer*)malloc(sizeof(*bitbuf));
	if (bitbuf) {
		memset(bitbuf, 0, sizeof(*bitbuf));//fill with zeros 
		bitbuf->data = JpgBuffer;
		bitbuf->length = JpgSize;
	}
	else {
		cout << "\nERROR CREATING THE BIT BUFFER";
	}
	

	//Create the buffer for the jepg info and fill it with zeros 
	struct JpegInfo* info = (JpegInfo*)malloc(sizeof(*info));
	if (info)
	{
		memset(info, 0, sizeof(*info));
	}
	else {
		cout << "\nERROR CREATING THE JPEG INFO";
	}

	int a= Decode(bitbuf, info);
	
	//_save_to_ppm(info);
	if (!(WriteToBMP(info))) {
		cout << "\nWriting to BMP Failed";
	}
	else {
		cout << "\nWrote to BMP";
	}
	
	//_save_to_ppm(info);
	if (!(NewBetterBMP(info))) {
		cout << "\nWriting to BMP Failed";
	}
	else {
		cout << "\nWrote to BMP";
	}

	//cout << "Decode Status: "<<a; 
	
	//cout << "\nPixel 4: " << info->pixels[0];

	//free(bitbuf);
	//_save_to_ppm(info);

	//exit(1);
	
}