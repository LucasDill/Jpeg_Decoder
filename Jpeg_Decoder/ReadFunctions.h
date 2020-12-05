#ifndef ReadFunctions
#define ReadFunctions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Structures.h"
#include <iostream>

using namespace std;

unsigned char MoveByte(struct BitBuffer* bits) {
	return bits->data[bits->read_position++];
}

unsigned short MoveWord(struct BitBuffer* bits)
{
	unsigned char byte = MoveByte(bits);//get the byte 
	unsigned short a = byte << 8 | MoveByte(bits);//shift the 
	return a;
}

void GetQuantTable(struct BitBuffer* bits, struct JpegInfo* info)//get the quantization table 
{
	unsigned short length;
	length = MoveWord(bits);//find the total length needed 
	int off = bits->read_position;//get the current position in the file structure
	int max = off + length - 2;
	while (off < max) {//loop for the whole length of the section 
		unsigned char buffer =(unsigned char) MoveByte(bits);
		unsigned char midbuf = buffer >> 4;
		unsigned char id = buffer & 0xf;//add the buffer value to the id
		info->table[id] = &bits->data[bits->read_position];//save the current position in the array element 
		bits->read_position += 64;
		off = bits->read_position;
	}
}

void DecodeFrame(struct BitBuffer* bits, struct JpegInfo* info)//get the general file information as well as the frame for grayscale
{
	unsigned short length = MoveWord(bits);//get to the start of the information 
	unsigned char byte = MoveByte(bits);
	
	info->height = MoveWord(bits);
	info->width = MoveWord(bits);
	info->component_count = MoveByte(bits);

	
	struct sampleInfo* samp = &info->comp;
	samp->id = MoveByte(bits);
	unsigned char byteholder = MoveByte(bits);
	samp->Hor_sample = byteholder >> 4;
	samp->Vert_sample = byteholder & 0xf;
	samp->tableID = MoveByte(bits);

	//cout << "\n Horizontal; " << samp->Hor_sample<<"\n Vertical: "<<samp->Vert_sample;

	info->mcu_height = samp->Hor_sample << 3;
	info->mcu_width = samp->Vert_sample << 3;
	info->vert_mcu = (info->height + info->mcu_height - 1) / info->mcu_height;
	info->hors_mcu = (info->width + info->mcu_width - 1) / info->mcu_width;
	
	info->blocks_mcu = samp->Hor_sample * samp->Vert_sample + info->component_count-1;
	info->scan_length = info->width * info->mcu_height* info->component_count;
	info->scan = (unsigned char*)malloc(info->scan_length);
	info->pixel_length = info->width * info->height * info->component_count;
	info->pixels = (unsigned char*)malloc(info->pixel_length);
	//cout << "\nMADE IT TO THE END OF DECODE TABLE";
	//cout << "\nMade it to the end of the frame pix count: " << info->pixel_length;
}




void GetHuffman(struct BitBuffer* bits, struct JpegInfo* info)
{
	unsigned short length = MoveWord(bits);
	unsigned int start = bits->read_position;//find the total length of the section 
	unsigned int end = start + length - 2;
	while (start < end)
	{
		unsigned char position = MoveByte(bits);
		unsigned char tableid = (position >> 3) | (position & 0xf);
		struct hTable* tab = &info->htab[tableid];//create a new poiner object with the id generated 
		for (int i = 0; i < 16; i++)//fill the table with arrays 
		{
			int counts = MoveByte(bits);
			tab->count += counts;
			struct hArray arry = { 0, NULL };//allocate the memory for each object 
			if (counts > 0) {
				arry.count = counts;
				arry.v = (hVlc*)malloc(sizeof(*arry.v) * counts);
				memset(arry.v, 0, sizeof(*arry.v) * counts);
			}
			tab->arr[i] = arry;
		}
		int base, b;
		for ( b = 0,base=0; b < 16; b++) 
		{
			struct hArray* arrv = &tab->arr[b];//create and fill the values for each table 
			for (int c = 0; c < arrv->count; c++)
			{
				struct hVlc* v = & arrv->v[c];
				v->code = MoveByte(bits);
				v->val = base;
				base++;
			}
			if (arrv->count < (b + 1) * (b + 1))
			{
				base <<= 1;
			}
		}
		info->table_count++;
		start = bits->read_position;
	}
	//cout << "\nMADE IT TO THE END OF HUFFMAN table_count: " << info->htab[1].count;
}

unsigned int LookBits(struct BitBuffer* bits, int i)
{
	unsigned char buffer;
	unsigned int dat = 0;
	assert(i < 32);
	while (bits->read_bits < i)
	{
		if (bits->read_endOfFile || bits->read_position >= bits->length)
		{
			bits->read_buffer = (bits->read_buffer << 8) | 0xff;
			bits->read_bits += 8;
			continue;
		}
		buffer = MoveByte(bits);
		bits->read_buffer <<= 8;
		bits->read_buffer |= buffer;
		bits->read_bits += 8;
		if (buffer == 0xff)
		{
			unsigned short mark = MoveByte(bits);
			switch (mark) {
			case 0x00:
				//cout << "\n0x00";
			case 0xff:
				break;
			case(0xffd9 & 0xff):
				bits->read_position += -2;
				bits->read_endOfFile = 1;
				break;
			default:
				cout << "\nMADE IT TO CHECK DEFAULT THIS IS NOT GOOD";
			}
		}
	}
	dat = bits->read_buffer >> (bits->read_bits - i);
	return dat;
}

void SkipBits(struct BitBuffer* bits, int n) {
	if (bits->read_bits >= n) {
		bits->read_bits -= n;
		bits->read_buffer &= ((1 << bits->read_bits) - 1);
	}
}

/////////////////////////////////////////////////////////////////////////////
void BufferDump(const unsigned char* buffer, int stride) {
	int i, base = 0;
	for (i = 0; i < 64; i++) {
		//printf("%02x ", buffer[base++]);
		if (i && (i + 1) % 8 == 0) {
			//printf("\n");
			base += stride - 8;
		}
	}
}


int CheckValue(struct BitBuffer* bits, struct hTable* tab, unsigned char* code)
{
	int value,i;
	struct hArray* ar = NULL;
	for (i = 1; i <= 16; i++)
	{
		
		//cout << " " << i;
		value = LookBits(bits, i);
		ar = &tab->arr[i - 1];

		assert(ar);

		for (int m = 0; m < ar->count; m++)
		{
			struct hVlc* v = &ar->v[m];
			if (value == v->val)
			{
				SkipBits(bits, i);
				if (code) {
					//cout << "\n code if";
					*code = (unsigned char)v->code;
				}
				i = v->code & 0xf;
				if (!i)
				{
					//cout << "\n not if";
					return 0;
				}
				int dat = LookBits(bits, i);
				SkipBits(bits,i);
				value = dat;

				if (value < (1 << (i - 1)))
				{
					//cout << "\n long if";
					value += (int)(~(1 << i)) + 2;
					
				}
				return value;
			}
		}
	}
	//cout << "\n code i";
	BufferDump(&bits->data[bits->read_position],8);
	return 0;
}

unsigned char limit(const signed int a)
{
	return (a < 0) ? 0 : ((a > 0xff) ? 0xff : (unsigned char)a);
}

void ConvertRow(signed int* blk) {//inverse cosine transform 
	int  x0, x1, x2, x3, x4, x5, x6, x7, x8;
	if (!((x1 = blk[4] << 11)
		| (x2 = blk[6])
		| (x3 = blk[2])
		| (x4 = blk[1])
		| (x5 = blk[7])
		| (x6 = blk[5])
		| (x7 = blk[3])))
	{
		blk[0] = blk[1] = blk[2] = blk[3] = blk[4] = blk[5] = blk[6] = blk[7] = blk[0] << 3;
		return;
	}
	x0 = (blk[0] << 11) + 128;
	x8 = W7 * (x4 + x5);
	x4 = x8 + (W1 - W7) * x4;
	x5 = x8 - (W1 + W7) * x5;
	x8 = W3 * (x6 + x7);
	x6 = x8 - (W3 - W5) * x6;
	x7 = x8 - (W3 + W5) * x7;
	x8 = x0 + x1;
	x0 -= x1;
	x1 = W6 * (x3 + x2);
	x2 = x1 - (W2 + W6) * x2;
	x3 = x1 + (W2 - W6) * x3;
	x1 = x4 + x6;
	x4 -= x6;
	x6 = x5 + x7;
	x5 -= x7;
	x7 = x8 + x3;
	x8 -= x3;
	x3 = x0 + x2;
	x0 -= x2;
	x2 = (181 * (x4 + x5) + 128) >> 8;
	x4 = (181 * (x4 - x5) + 128) >> 8;
	blk[0] = (x7 + x1) >> 8;
	blk[1] = (x3 + x2) >> 8;
	blk[2] = (x0 + x4) >> 8;
	blk[3] = (x8 + x6) >> 8;
	blk[4] = (x8 - x6) >> 8;
	blk[5] = (x0 - x4) >> 8;
	blk[6] = (x3 - x2) >> 8;
	blk[7] = (x7 - x1) >> 8;
}

void ConvertCol(const signed int* blk, unsigned char* out, int stride) {//perfrom inverse descrete cosine transform 
	int x0, x1, x2, x3, x4, x5, x6, x7, x8;
	if (!((x1 = blk[8 * 4] << 8)
		| (x2 = blk[8 * 6])
		| (x3 = blk[8 * 2])
		| (x4 = blk[8 * 1])
		| (x5 = blk[8 * 7])
		| (x6 = blk[8 * 5])
		| (x7 = blk[8 * 3])))
	{
		x1 = limit(((blk[0] + 32) >> 6) + 128);
		for (x0 = 8; x0; --x0) {
			*out = (unsigned char)x1;
			out += stride;
		}
		return;
	}
	x0 = (blk[0] << 8) + 8192;
	x8 = W7 * (x4 + x5) + 4;
	x4 = (x8 + (W1 - W7) * x4) >> 3;
	x5 = (x8 - (W1 + W7) * x5) >> 3;
	x8 = W3 * (x6 + x7) + 4;
	x6 = (x8 - (W3 - W5) * x6) >> 3;
	x7 = (x8 - (W3 + W5) * x7) >> 3;
	x8 = x0 + x1;
	x0 -= x1;
	x1 = W6 * (x3 + x2) + 4;
	x2 = (x1 - (W2 + W6) * x2) >> 3;
	x3 = (x1 + (W2 - W6) * x3) >> 3;
	x1 = x4 + x6;
	x4 -= x6;
	x6 = x5 + x7;
	x5 -= x7;
	x7 = x8 + x3;
	x8 -= x3;
	x3 = x0 + x2;
	x0 -= x2;
	x2 = (181 * (x4 + x5) + 128) >> 8;
	x4 = (181 * (x4 - x5) + 128) >> 8;
	*out = limit(((x7 + x1) >> 14) + 128);  out += stride;
	*out = limit(((x3 + x2) >> 14) + 128);  out += stride;
	*out = limit(((x0 + x4) >> 14) + 128);  out += stride;
	*out = limit(((x8 + x6) >> 14) + 128);  out += stride;
	*out = limit(((x8 - x6) >> 14) + 128);  out += stride;
	*out = limit(((x0 - x4) >> 14) + 128);  out += stride;
	*out = limit(((x3 - x2) >> 14) + 128);  out += stride;
	*out = limit(((x7 - x1) >> 14) + 128);
}

void DecodeColour(struct BitBuffer* bits, struct JpegInfo* info)
{
	int id = 0;
	struct hTable* tabl = NULL;
	int val,a;
	struct sampleInfo* inf = &info->comp;
	const unsigned char* tabif = info->table[inf->tableID];
	assert(tabif);
	if (tabif == 0)
	{
		cout << "\nERROR WITH THE TABLE IN BLOCKS";
		return;
	}
	memset(inf->vector, 0, 64 * sizeof(inf->vector[0]));

	tabl = &info->htab[inf->dc_id];

	val = CheckValue(bits, tabl, NULL);
	inf->dc += val;
	inf->vector[0] = inf->dc * tabif[0];

	tabl = &info->htab[inf->ac_id];
	if (tabl == 0)
	{
		cout << "\nERROR WITH THE TABLE IN BLOCKS";
		return;
	}
	for (a = 1; a < 64; a++)
	{
		unsigned char code = 0;
		val = CheckValue(bits, tabl, &code);
		if (!code)
		{
			break;
		}
		else {
			a += (code >> 4);
			inf->vector[(signed int)ZigMat[a]] = val * tabif[a];
		}
	}

	int t;

	for ( t = 0; t < 64; t += 8)
	{
		ConvertRow(&inf->vector[t]);
	}
	for (t = 0; t < 8; t++)
	{
		ConvertCol(&inf->vector[t], &inf->pixels[t], 8);
	}
	//cout << "\nEnd of Colour";
}

void ConvertGrayscale(struct JpegInfo* info, int num)
{
	//cout << "\nConverting that grey";
	unsigned char* output = info->scan;
	unsigned char* pix = info->comp.pixels;
	int pixbas = 0;
	int outbas = num * info->mcu_width;///////////////
	for (int i = 0; i < info->mcu_height; i++)/////////////
	{
		memcpy(&output[outbas], &pix[pixbas], 8);//////////////////
		pixbas += 8;
		outbas += info->width;
	}
}

void ScanDecode(struct BitBuffer* bits, struct JpegInfo* info)
{
	//cout << "\n SCAN DECODE CALLED";
	unsigned short length = MoveWord(bits);
	unsigned char scanlen = MoveByte(bits);//move the object along past the 
	//printf_s("\nScan header %d, %d\n", length, scanlen);
	
	struct sampleInfo* si = &info->comp;
	unsigned char id = MoveByte(bits);
	unsigned char buffer = MoveByte(bits);
	si->dc_id = buffer >> 4;//set the alternating and direct id's 
	si->ac_id = (buffer & 1) | 2;
	
	
	unsigned char s1 = MoveByte(bits);
	unsigned char s2 = MoveByte(bits);
	unsigned char buffer2 = MoveByte(bits);


	for (int b = 0; b < info->vert_mcu; b++)//loop though the height and width of the frame 
	{
		for (int c = 0; c < info->hors_mcu; c++)
		{
				DecodeColour(bits, info);
			
			//cout << "\nBlocks for colour: " << info->blocks_mcu;
			if (info->blocks_mcu == 1) {
				//cout << "\nGray All the Way";
				//cout << "\nb= " << b << "  c=" << c;
				ConvertGrayscale(info, c);///////////////////////////////BIG CHANGE
				//cout << "end";
			}
			else{
				//ConvertGrayscale(info, c);
			 cout << "\nThis Program Only accepts Greyscale Images Please Enter a Different Image";
			 exit(1);
			}
			if (info->rest && !(--info->restCount)) {
				//cout << "\nRest If";
				unsigned short nex = MoveWord(bits);
				bits->read_bits = 0;
				bits->read_buffer = 0;
				if (nex == 0xffd9)
				{
					SkipBits(bits, -2);
					goto ScanLine;
				}
				if (((nex & 0xfff8) != 0xffd0) || ((nex & 0x7) != info->restNext)) {
					BufferDump(&bits->data[bits->read_position], 8);
					assert(0);
				}
				info->restNext = (nex + 1) & 0x7;
				info->restCount = info->rest;
				
				info->comp.dc = 0;
				
			}
		ScanLine:
			//cout << "\nWOO scan Line";
			//cout << "\nInfo b:" << b << " Scan_Length: " << info->scan_length << " Length of pixels: " << sizeof(info->pixels) << "Pixels at: " << b * info->scan_length << " " << &info->pixels[b * info->scan_length];
			memcpy(&info->pixels[b * info->scan_length], info->scan, info->scan_length);
			//cout << "\nEND OF DECODE SECOND";
		}
	}
}



void Decode(struct BitBuffer* bits, struct JpegInfo* info) {
	//cout << "\nDecodeCall";
	//cout << "\nOUTSIDE OF WHILE "<<bits->read_endOfFile<<"\n"<<bits->read_position<<"\n"<<bits->length;
	//bits->read_endOfFile = 1;
	int quant=0, decodefr=0, decoderes=0, gethuf=0, scande=0;
	
	while (!(bits->read_endOfFile || bits->read_position >= bits->length))//loop as long as there is no end of file
	{
		//cout << "\nBit Info" << bits->read_position << " " << bits->length << "\n" << bits->read_endOfFile;
		//cout << "\nINSIDE WHILE";
		unsigned short marker = MoveWord(bits);//get the next word from the file information 
		//cout << marker;
		switch (marker) {
		case 0xffd8://This is the start of the image 
			cout<<"Start of Image\n";
			break;
		case 0xffdb://If the next word is the quantization table retreive it
			GetQuantTable(bits, info);//read the quantization table 
			cout << "\nGet Quantization Table";
			quant++;
			break;
		case 0xffc0:
			DecodeFrame(bits, info);///get file information from the header such as the height and width 
			cout << "\nGet Data Frame";
			decodefr++;
			break;
		case 0xffc4:
			GetHuffman(bits, info);//Get the function headers which are stored in the file header 
			cout << "\nGet Huffman Tables";
			gethuf++;
			break;
		case 0xffda:
			ScanDecode(bits, info);//Decode the file information
			cout << "\nStart the Decode";
			scande++;
		break;
		default:
			if (marker >= 0xffe0 && marker <= 0xffef)
			{
				unsigned short length = MoveWord(bits);//move the word along 
				bits->read_position += (length - 2);//move the current position along to the next length subtracted by 2
				cout << "\nAPP SEGMENT size: "<<length;
			}
			
		
			break;
	}
		
	}
	
	
 }



#endif