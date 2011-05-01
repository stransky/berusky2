
/*************************************************************************
	
	file created on:	2002/08/30   19:33
	filename: 			Bmp.c
	author:				Andreas Hartl<andy@runicsoft.com>

		visit http://www.runicsoft.com for updates and more information

	purpose:	functions to load raw bmp data,
	                      to save raw bmp data,
						  to convert RGB data to raw bmp data,
						  to convert raw bmp data to RGB data
						  and to use the WinAPI to select
							a bitmap into a device context

The code in this file is subject to the RunicSoft source code licence 
	( http://www.runicsoft.com/sourcelicence.txt )

**************************************************************************/

#include <stdio.h>       // for memset
#include "3d_all.h"

/*******************************************************************
char* ConvertRGBToBMPBuffer ( char* Buffer, int width, 
		int height, long* newsize )


This function takes as input an array of RGB values, it's width
and height.
The buffer gets then transformed to an array that can be used
to write to a windows bitmap file. The size of the array
is returned in newsize, the array itself is the
return value of the function.
Both input and output buffers must be deleted by the
calling function.

The input buffer is expected to consist of width * height
RGB triplets. Thus the total size of the buffer is taken as
width * height * 3.

The function then transforms this buffer so that it can be written 
to a windows bitmap file:
First the RGB triplets are converted to BGR.
Then the buffer is swapped around since .bmps store
images uside-down.
Finally the buffer gets DWORD ( 32bit ) aligned, 
meaning that each scanline ( 3 * width chars ) gets
padded with 0x00 chars up to the next DWORD boundary


*******************************************************************/

byte* ConvertRGBToBMPBuffer ( byte* Buffer, int width, int height, int* newsize)
{
	int padding = 0;
	int scanlinebytes = width * 3;
  int psw;
	long bufpos = 0;
	long newpos = 0;
  byte* newbuf;
  int  x,y;

	// first make sure the parameters are valid
	if ( ( NULL == Buffer ) || ( width == 0 ) || ( height == 0 ) )
		return NULL;

	// now we have to find with how many bytes
	// we have to pad for the next DWORD boundary	

	while ( ( scanlinebytes + padding ) % 4 != 0 )     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	psw = scanlinebytes + padding;
	
	// we can already store the size of the new padded buffer
	*newsize = height * psw;

	// and create new buffer
	newbuf = (byte *)malloc(sizeof(byte)*(*newsize));
  	
	// fill the buffer with zero bytes then we dont have to add
	// extra padding zero bytes later on
	memset ( newbuf, 0, *newsize );

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	for ( y = 0; y < height; y++ )
		for ( x = 0; x < 3 * width; x+=3 )
		{
			bufpos = y * 3 * width + x;     // position in original buffer
			newpos = y * psw + x;           // position in padded buffer

			newbuf[newpos]   = Buffer[bufpos];   // swap r and b
			newbuf[newpos+1] = Buffer[bufpos+1]; // g stays
			newbuf[newpos+2] = Buffer[bufpos+2]; // swap b and r
		}

	return newbuf;
}

/***************************************************************
byte* ConvertBMPToRGBBuffer ( byte* Buffer, 
		int width, int height )

This function takes as input the data array
from a bitmap and its width and height.
It then converts the bmp data into an RGB array.
The calling function must delete both the input
and output arrays.
The size of the returned array will be 
width * height * 3
On error the returb value is NULL, else the
RGB array.


The Buffer is expected to be the exact data read out
from a .bmp file.  
The function will swap it around, since images
are stored upside-down in bmps.
The BGR triplets from the image data will
be converted to RGB.
And finally the function removes padding bytes.
The returned arraay consits then of
width * height RGB triplets.

*****************************************************************/

byte* ConvertBMPToRGBBuffer ( byte* Buffer, int width, int height )
{
	int  padding = 0;
	int  scanlinebytes = width * 3;
  int  psw;
  byte *newbuf;
  byte *p_p1, *p_p2;
  int  x,y;

  // first make sure the parameters are valid
	if ( ( NULL == Buffer ) || ( width == 0 ) || ( height == 0 ) )
		return NULL;

	// find the number of padding bytes
		
	while((scanlinebytes + padding)%4 != 0)     // DWORD = 4 bytes
		padding++;
	
  // get the padded scanline width
	psw = scanlinebytes + padding;

	// create new buffer
	newbuf = (byte *)malloc(sizeof(byte)*width*height*3);
  
  for (y = 0; y < height; y++ ) {
    p_p1 = newbuf+y*scanlinebytes;
    p_p2 = Buffer+y*psw;
    for (x = 0; x < width; x++, p_p1+=3, p_p2+=3) {
      p_p1[0] = p_p2[2];
      p_p1[1] = p_p2[1];
      p_p1[2] = p_p2[0];
		}
  }

	return newbuf;
}

/***************************************************************
int SaveBMP ( byte* Buffer, int width, int height, 
		long paddedsize, char* bmpfile )

Function takes a buffer of size <paddedsize> 
and saves it as a <width> * <height> sized bitmap 
under the supplied filename.
On error the return value is FALSE.

***************************************************************/

int SaveBMP(FILE *file, byte* Buffer, int width, int height, int paddedsize )
{
	// declare bmp structures 
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;
	
	// andinitialize them to zero
	memset ( &bmfh, 0, sizeof (BITMAPFILEHEADER ) );
	memset ( &info, 0, sizeof (BITMAPINFOHEADER ) );
	
	// fill the fileheader with data
	bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paddedsize;
	bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits
	
	// fill the infoheader

	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;			// we only have one bitplane
	info.biBitCount = 24;		// RGB mode is 24 bits
	info.biCompression = BI_RGB;	
	info.biSizeImage = 0;		// can be 0 for 24 bit images
	info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
	info.biYPelsPerMeter = 0x0ec4;     
	info.biClrUsed = 0;			// we are in RGB mode and have no palette
	info.biClrImportant = 0;    // all colors are important
	
	// write file header
	if(!fwrite(&bmfh,sizeof(BITMAPFILEHEADER), 1, file))
  {	
		return FALSE;
	}

	// write infoheader
	if(!fwrite(&info,sizeof(BITMAPINFOHEADER),1,file))
	{	
		return FALSE;
	}

	// write image data
	if(!fwrite(Buffer, paddedsize, 1, file))
	{	
		return FALSE;
	}

	return TRUE;
}

/*******************************************************************
byte* LoadBMP ( int* width, int* height, long* size 
		char* bmpfile )

The function loads a 24 bit bitmap from bmpfile, 
stores it's width and height in the supplied variables
and the whole size of the data (padded) in <size>
and returns a buffer of the image data 

On error the return value is NULL. 

  NOTE: make sure you [] delete the returned array at end of 
		program!!!
*******************************************************************/
byte* LoadBMP(KFILE *file, int* width, int* height, int* size)
{
	// declare bitmap structures
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
  byte* Buffer;
  	
  kread(&bmpheader, sizeof(BITMAPFILEHEADER), 1, file);
  kread(&bmpinfo, sizeof(BITMAPINFOHEADER), 1, file);
	
	// check if file is actually a bmp
	if ( bmpheader.bfType != 'MB' )
	{
		return(NULL);
	}

	// get image measurements
	*width   = bmpinfo.biWidth;
	*height  = abs ( bmpinfo.biHeight );

	// check if bmp is uncompressed
	if ( bmpinfo.biCompression != BI_RGB )
	{
		return(NULL);
	}

	// check if we have 24 bit bmp
	if(bmpinfo.biBitCount != 24)
	{
		return(NULL);
	}
	
	// create buffer to hold the data
	*size = bmpheader.bfSize - bmpheader.bfOffBits;
	Buffer = (byte *)mmalloc(sizeof(byte)*(*size));  

	// move file pointer to start of bitmap data
	//aseek(1,file, start+bmpheader.bfOffBits, SEEK_SET);
	
  // read bmp data	
  if(!kread(Buffer,*size,1,file)) {
    free(Buffer);
		return(NULL);
	}

	// everything successful here: close file and return buffer
	return(Buffer);
}
