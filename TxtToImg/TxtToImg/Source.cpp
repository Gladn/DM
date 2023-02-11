#define _CRT_SECURE_NO_WARNINGS
#include <string>;
#include <Windows.h>
#include <tchar.h>
#include <time.h>
#include <iostream>
#include <gdiplus.h>
using namespace std;
using namespace Gdiplus;
#pragma comment (lib, "Gdiplus.lib")

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 5; // number of image encoders
	UINT size = 1; // size of the image encoder array in bytes
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1; // Failure
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1; // Failure
	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j; // Success
		}
	}
	free(pImageCodecInfo);
	return -1; // Failure
}

VOID Example_SaveFile(const WCHAR* F, const WCHAR* T)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	CLSID encoderClsid;
	Status stat;
	Image* image = new Image(F);
	// Get the CLSID of the PNG
	//encoder.
		GetEncoderClsid(L"image/jpeg", &encoderClsid);
	stat = image->Save(T, &encoderClsid, NULL);
	if (stat == Ok)
		printf("img was saved successfully\n");
	else
		printf("Failure: stat = %d\n", stat);
	delete image;
	GdiplusShutdown(gdiplusToken);
}

#define DIB_RGB(r, g, b) ((DWORD)((r & 0xFF) << 16) | ((g & 0xFF) << 8) | ((b& 0xFF) << 0))

#define DIB_RGB2(r, g, b) ((DWORD)((r >> 16) & 0xFF) | ((g >> 8) & 0xFF) | (b & 0xFF))

void draw(__int32* arr, int width, int x, int y, DWORD color) {
	for (int r = y; r <= y; r++) {
		for (int c = x; c <= x; c++)
			arr[r * width + c] = color;
	}
}
// 24/32 бит
BOOL SaveArrFile(const TCHAR* filename, const __int32* arr,
	int width, int height, int bpp) {
	if ((bpp < 24) || (bpp > 32)) // только 24/32 бит
		return FALSE;
	DWORD p_row = (DWORD)((width * bpp + 31) & ~31) / 8uL;
	DWORD size = (DWORD)(height * p_row);
	// формируем файловый заголовок
	BITMAPFILEHEADER hdr;
	ZeroMemory(&hdr, sizeof(BITMAPFILEHEADER));
	hdr.bfType = 0x4D42;
	hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	hdr.bfSize = hdr.bfOffBits + size;
	// заголовок описателя растра
	BITMAPINFO dib;
	ZeroMemory(&dib, sizeof(BITMAPINFO));
	dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	dib.bmiHeader.biBitCount = bpp;
	dib.bmiHeader.biCompression = BI_RGB;
	dib.bmiHeader.biPlanes = 1u;
	dib.bmiHeader.biWidth = (long)width;
	dib.bmiHeader.biHeight = (long)-height;
	dib.bmiHeader.biSizeImage = size;
	dib.bmiHeader.biXPelsPerMeter = 11811L;
	dib.bmiHeader.biYPelsPerMeter = 11811L;
	dib.bmiHeader.biClrImportant = 0uL;
	dib.bmiHeader.biClrUsed = 0uL;
	// далее запись в файл
	HANDLE fp = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fp == INVALID_HANDLE_VALUE)
		return FALSE;
	// записываем заголовки...
	DWORD dwr = 0uL;
	WriteFile(fp, (LPCVOID)&hdr, sizeof(BITMAPFILEHEADER), &dwr, NULL);
	WriteFile(fp, (LPCVOID)&dib.bmiHeader, sizeof(BITMAPINFOHEADER), &dwr,
		NULL);
	// запись массива пикселей
		if (bpp == 32) // 32-бит
			WriteFile(fp, (LPCVOID)arr, size, &dwr, NULL);
		else if (bpp == 24) { // 24-бит с дополнением до 32-разрядной границы
			BYTE nil = 0u;
			int cb = sizeof(RGBQUAD);
			int align = ((cb - ((width * bpp + 7) / 8) % cb) % cb);
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++)
					WriteFile(fp, (LPCVOID)&arr[y * width + x],
						sizeof(RGBTRIPLE), &dwr, NULL);
				for (int i = 0; i < align; i++) // до границы DWORD
					WriteFile(fp, (LPCVOID)&nil, sizeof(BYTE), &dwr,
						NULL);
			}
		}
	FlushFileBuffers(fp);
	CloseHandle(fp);
	return TRUE;
}




int main(void) {
	string fnm;
	srand(time(NULL));
	//массив пикселей
	__int32 arr[200 * 200] = { 0 };
	int cw = 200;
	int ch = 200;
	int counter = 0;
	char s1, s2, s3;
	// нарисуем что-нибудь
	unsigned long rgb;
	FILE *ptrFile; fopen_s(&ptrFile, "ciphertext.txt", "rb");
	char label[] = "simon_";
	char FileName[256];
	for (int i = 1; i < 2 ; i++) {
		char folder1[] = "input\\Simon_1\\";
		char folder2[] = "output\\";
		if (CreateDirectory(folder2, NULL)) printf("directory created");
		//sprintf(FileName, "%s%s%i%s", folder1, label, i,".txt");

		try {
			//FILE* ptrFile;
			//if (fopen_s(&ptrFile, FileName, "rb"))
				//throw 1;
			unsigned char Red, Green, Blue;
			char abc = 'f';
			unsigned char abc2 = abc;
			
			//cout << (int)abc2;
			for (int y = 0; y < ch; y++) {
				for (int x = 0; x < cw; x++) {
					Red = fgetc(ptrFile);
					Green = fgetc(ptrFile);
					Blue = fgetc(ptrFile);
					rgb = DIB_RGB((Red), (Green), (Blue));

					//arr[x + y] = rgb;

					draw(arr, cw, x, y, rgb);
					if (feof(ptrFile)) rewind(ptrFile);
				}
			}
			char cipher[] = "simon_";
			char OutputFile[256];
			// сохраняем в файл
			sprintf(OutputFile, "%s%i%s", cipher, i,".bmp");
			fnm = cipher;
			fnm += to_string(i);
			printf("directory ");
			if (SaveArrFile(_T(OutputFile), arr, cw, ch, 32))
				cout << "Excellent save file N - " << i <<
				endl;
			else
				_putts(_T("Error save file !"));
			string F1 = fnm + ".bmp";
			string F2 = folder2 + fnm + ".bmp";
			wstring F11 = wstring(F1.begin(), F1.end());
			wstring F21 = wstring(F2.begin(), F2.end());
			const WCHAR* FNMBM = F11.c_str();
			const WCHAR* FNMJPG = F21.c_str();
			Example_SaveFile(FNMBM, FNMJPG);
			if (remove(F1.c_str())) {
				printf("Error removing file\n");
			}
		}
		catch (...) {
			break;
		}
	}
}