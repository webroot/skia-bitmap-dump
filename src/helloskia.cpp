//============================================================================
// Name        : helloskia.cpp
// Author      : abhishek
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>

#include "SkCanvas.h"
#include "SkGraphics.h"
#include "SkSurface.h"
#include "SkString.h"
#include "SkTime.h"
#include "SkImageInfo.h"
#include "SkImageDecoder.h"
#include "SkImageEncoder.h"

static void saveSkBitmapToBMPFile(const SkBitmap& skBitmap, const char* path);

using namespace std;

int main() {
	cout << "Hello skia example" << endl;

	SkBitmap bitmap;
//	bitmap.setConfig(SkBitmap::kARGB_8888_Config, 200, 200);
//	bitmap.allocN32Pixels(300, 200);
	SkImageInfo info;
	info = SkImageInfo::Make(300, 300, kRGB_565_SkColorType,
			kOpaque_SkAlphaType);

	bitmap.allocPixels(info);
	SkCanvas canvas(bitmap);
	canvas.drawColor(SK_ColorWHITE);
	SkPaint paint;
	paint.setAntiAlias(true);
	paint.setColor(SK_ColorRED);
	canvas.drawCircle(SkIntToScalar(150), SkIntToScalar(150),
	SkIntToScalar(150), paint);

	paint.setColor(SK_ColorBLUE);
	canvas.drawLine(10, 150, 140, 150, paint);

	int textsize = 20;
	paint.setColor(SK_ColorBLACK);
	paint.setTextSize(textsize);

	SkRect bounds;
	string s = "This is Some String A";
	int w = paint.measureText(s.c_str(), s.size(), &bounds);
	cout << "w=" << w << endl;
	cout << "h=" << bounds.height() << endl;
	canvas.drawText(s.c_str(), s.size(), SkIntToScalar(10),
			SkIntToScalar(150 + bounds.height()), paint);

	// save png file
	SkImageEncoder::EncodeFile("out.png", bitmap, SkImageEncoder::kPNG_Type,
			100);

	return 0;
}

static void saveSkBitmapToBMPFile(const SkBitmap& skBitmap, const char* path) {
	typedef unsigned char UINT8;
	typedef signed char SINT8;
	typedef unsigned short UINT16;
	typedef signed short SINT16;
	typedef unsigned int UINT32;
	typedef signed int SINT32;

	struct BMP_FILEHDR // BMP file header
	{
		UINT32 bfSize; // size of file
		UINT16 bfReserved1;
		UINT16 bfReserved2;
		UINT32 bfOffBits; // pointer to the pixmap bits
	};

	struct BMP_INFOHDR // BMP information header
	{
		UINT32 biSize; // size of this struct
		UINT32 biWidth; // pixmap width
		UINT32 biHeight; // pixmap height
		UINT16 biPlanes; // should be 1
		UINT16 biBitCount; // number of bits per pixel
		UINT32 biCompression; // compression method
		UINT32 biSizeImage; // size of image
		UINT32 biXPelsPerMeter; // horizontal resolution
		UINT32 biYPelsPerMeter; // vertical resolution
		UINT32 biClrUsed; // number of colors used
		UINT32 biClrImportant; // number of important colors
	};
#define BitmapColorGetA(color) (((color) >> 24) & 0xFF)
#define BitmapColorGetR(color) (((color) >> 16) & 0xFF)
#define BitmapColorGetG(color) (((color) >> 8) & 0xFF)
#define BitmapColorGetB(color) (((color) >> 0) & 0xFF)

	int bmpWidth = skBitmap.width();
	int bmpHeight = skBitmap.height();
	int stride = skBitmap.rowBytes();
	char* m_pmap = (char*) skBitmap.getPixels();
	//virtual PixelFormat& GetPixelFormat() =0; //assume pf is ARGB;
	FILE* fp = fopen(path, "wb");
	if (!fp) {
		printf("saveSkBitmapToBMPFile: fopen %s Error!\n", path);
	}
	SINT32 bpl = bmpWidth * 4;
	// BMP file header.
	BMP_FILEHDR fhdr;
	fputc('B', fp);
	fputc('M', fp);
	fhdr.bfReserved1 = fhdr.bfReserved2 = 0;
	fhdr.bfOffBits = 14 + 40; // File header size + header size.
	fhdr.bfSize = fhdr.bfOffBits + bpl * bmpHeight;
	fwrite(&fhdr, 1, 12, fp);

	// BMP header.
	BMP_INFOHDR bhdr;
	bhdr.biSize = 40;
	bhdr.biBitCount = 32;
	bhdr.biCompression = 0; // RGB Format.
	bhdr.biPlanes = 1;
	bhdr.biWidth = bmpWidth;
	bhdr.biHeight = bmpHeight;
	bhdr.biClrImportant = 0;
	bhdr.biClrUsed = 0;
	bhdr.biXPelsPerMeter = 2384;
	bhdr.biYPelsPerMeter = 2384;
	bhdr.biSizeImage = bpl * bmpHeight;
	fwrite(&bhdr, 1, 40, fp);

	// BMP data.
	//for(UINT32 y=0; y<m_height; y++)
	for (SINT32 y = bmpHeight - 1; y >= 0; y--) {
		SINT32 base = y * stride;
		for (SINT32 x = 0; x < (SINT32) bmpWidth; x++) {
			UINT32 i = base + x * 4;
			UINT32 pixelData = *(UINT32*) (m_pmap + i);
			UINT8 b1 = BitmapColorGetB(pixelData);
			UINT8 g1 = BitmapColorGetG(pixelData);
			UINT8 r1 = BitmapColorGetR(pixelData);
			UINT8 a1 = BitmapColorGetA(pixelData);
			r1 = r1 * a1 / 255;
			g1 = g1 * a1 / 255;
			b1 = b1 * a1 / 255;
			UINT32 temp = (a1 << 24) | (r1 << 16) | (g1 << 8) | b1; //a bmp pixel in little endian is B、G、R、A
			fwrite(&temp, 4, 1, fp);
		}
	}
	fflush(fp);
	fclose(fp);
}
