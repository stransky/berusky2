
/*************************************************************************
	
	file created on:	2002/08/30   19:33
	filename: 			Bmp.h
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

typedef struct tagBITMAPFILEHEADER {
	WORD    bfType __attribute__ ((packed));
	DWORD   bfSize __attribute__ ((packed));
	WORD    bfReserved1 __attribute__ ((packed));
	WORD    bfReserved2 __attribute__ ((packed));
	DWORD   bfOffBits __attribute__ ((packed));
} BITMAPFILEHEADER ;

typedef struct tagBITMAPINFOHEADER{
	DWORD  biSize;
	DWORD  biWidth;
	DWORD  biHeight;
	WORD   biPlanes;
	WORD   biBitCount;
	DWORD  biCompression;
	DWORD  biSizeImage;
	DWORD  biXPelsPerMeter;
	DWORD  biYPelsPerMeter;
	DWORD  biClrUsed;
	DWORD  biClrImportant;
} BITMAPINFOHEADER;

#define BI_RGB 0

byte* ConvertRGBToBMPBuffer ( byte* Buffer, int width, int height, int* newsize);
byte* ConvertBMPToRGBBuffer ( byte* Buffer, int width, int height );
int   SaveBMP(FILE *file, byte* Buffer, int width, int height, int paddedsize );
byte* LoadBMP(KFILE *file, int* width, int* height, int* size);
