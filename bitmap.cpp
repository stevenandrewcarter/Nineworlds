#include "bitmap.h"

bitmap::bitmap(void)
{
	data = 0;
	width=height=0;
}


bool bitmap::create(int x, int y)
{	
	width  = x;
	height = y;
	data = new unsigned char[(width*height)*3];
	
	if(!data) 
		return false;
	else
		return true;
}


void bitmap::getcolor(int x, int y, BYTE &r, BYTE &g, BYTE &b) 
{
	if((x < width) && (y < height)) {
		r = data[(x + (y*width))*3 + 0];
		g = data[(x + (y*width))*3 + 1];
		b = data[(x + (y*width))*3 + 2];
	}
}


void bitmap::setcolor(int x, int y, BYTE r, BYTE g, BYTE b) 
{
	if((x < width) && (y < height)) {
		data[(x + (y*width))*3 + 0] = r;
		data[(x + (y*width))*3 + 1] = g;
		data[(x + (y*width))*3 + 2] = b;
	}
}


bool bitmap::load(char *filename)
{
	int x, y;
	bmpFHEAD h1;
	bmpIHEAD h2;
	BYTE color[3];
	BYTE swpclr;
	
	FILE *fp = fopen(filename, "rb");

	//FILEHEADER
	fread(&h1.bfType,			sizeof(h1.bfType),			1, fp);
	fread(&h1.bfSize,			sizeof(h1.bfSize),			1, fp);
	fread(&h1.bfReserved1,		sizeof(h1.bfReserved1),		1, fp);
	fread(&h1.bfReserved2,		sizeof(h1.bfReserved2),		1, fp);
	fread(&h1.bfOffBits,		sizeof(h1.bfOffBits),		1, fp);
	//FILEHEADER DONE

	//INFOHEADER
	fread(&h2,					sizeof(h2),					1, fp);
	//INFOHEADER DONE

	width  = h2.biWidth;
	height = h2.biHeight;

	data = new BYTE[(width*height)*3];
	
	if(!data) 
		return false;

	for(y=0; y<height; y++) 
	{
		for(x=0; x<width; x++) 
		{
			fread (color, sizeof(color), 1, fp);
			
			//Swap BGR to RGB
			swpclr=color[0];
			color[0]=color[2];
			color[2]=swpclr;

			setcolor (x, y, color[0], color[1], color[2]);
    	}

		//Add padding bytes
		//FIX: Move filepointer instead
		for (int a=0; a<(4-((3*width)%4))%4; a++)
			fread (&swpclr, sizeof(BYTE), 1, fp);
	}

	fclose(fp);

	if(!data)
		return false;
	else
		return true;
}


bool bitmap::save(char *filename)
{
	bmpFHEAD filehead;						//File Header
	bmpIHEAD infohead;						//Info Header
	BYTE color[3];
	BYTE swpclr;
	long bitsize;							//Size of the bitmap
	FILE *fp = fopen(filename, "wb");

	if (!data || !fp)						//If there's no data nor a file, don't even start writing
		{ fclose (fp); return false; }

	bitsize  = width*height*3;				//Get size of the bitmap (width*height*3 Bytes per Pixel)
	bitsize += bitsize%4;					//and round to next 4 bytes (see byte alignement in compiler options!)

	//FILEHEADER
	filehead.bfType      = 'MB';
    filehead.bfSize      = sizeof(bmpFHEAD)-2 + sizeof(bmpIHEAD) + bitsize;	// -2 due to byte alignment
    filehead.bfReserved1 = 0;
    filehead.bfReserved2 = 0;
    filehead.bfOffBits   = sizeof(bmpFHEAD)-2 + sizeof(bmpIHEAD);			// -2 due to byte alignment

    fwrite(&filehead.bfType,		sizeof(filehead.bfType),		1, fp);
	fwrite(&filehead.bfSize,		sizeof(filehead.bfSize),		1, fp);
	fwrite(&filehead.bfReserved1,	sizeof(filehead.bfReserved1),	1, fp);
	fwrite(&filehead.bfReserved2,	sizeof(filehead.bfReserved2),	1, fp);
	fwrite(&filehead.bfOffBits,		sizeof(filehead.bfOffBits),		1, fp);
	//FILEHEADER DONE

	//HEADER
	infohead.biSize			= sizeof (bmpIHEAD);
	infohead.biWidth		= width;
	infohead.biHeight		= height;
	infohead.biPlanes		= 1;
	infohead.biBitCount		= 24;			//24bit
	infohead.biCompression	= 0;			//RGB
	infohead.biSizeImage	= bitsize;
	infohead.biXPelsPerMeter= 2800;
	infohead.biYPelsPerMeter= 2800;
	infohead.biClrUsed		= 0;
	infohead.biClrImportant = 0;

	fwrite(&infohead, sizeof (infohead), 1, fp);
	//HEADER DONE


	//BITMAP
	for(int y=0; y<height; y++) 
	{
		for(int x=0; x<width; x++) 
		{
			getcolor(x, y, color[0], color[1], color[2]);
			
			//Swap BGR to RGB
			swpclr=color[0];
			color[0]=color[2];
			color[2]=swpclr;

			fwrite (color, sizeof (color), 1, fp);
    	}
		
		//Add padding bytes
		swpclr=0;
		for (int a=0; a<(4-((3*width)%4))%4; a++)
			fwrite (&swpclr, sizeof(BYTE), 1, fp);
	}

	//BITMAP DONE
	
	fclose (fp);

	return true;
}


bitmap::~bitmap(void)
{
	if (data) 
		delete data;
	data = 0;
}