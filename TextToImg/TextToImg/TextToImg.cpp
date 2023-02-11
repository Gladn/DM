#define _CRT_SECURE_NO_WARNINGS
#include <string>;
#include <Windows.h>
#include <tchar.h>
#include <time.h>
#include <iostream>

using namespace std;


//BOOL SaveFile(const TCHAR* filename, const __int32* arr,
//	int width, int height, int bpp) {
//	if ((bpp < 24) || (bpp > 32)) // только 24/32 бит
//		return FALSE;
//	DWORD p_row = (DWORD)((width * bpp + 31) & ~31) / 8uL;
//	DWORD size = (DWORD)(height * p_row);
//
//	// Сформируем файловый заголовок BMP
//	BITMAPFILEHEADER bmHeader;
//	ZeroMemory(&bmHeader, sizeof(BITMAPFILEHEADER));
//	bmHeader.bfType = 0x4D42;
//	bmHeader.bfSize = bmHeader.bfOffBits + size;
//	bmHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
//	
//
//	BITMAPINFO dib;
//	ZeroMemory(&dib, sizeof(BITMAPINFO));
//	dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//	dib.bmiHeader.biBitCount = bpp;
//	dib.bmiHeader.biCompression = BI_RGB;
//	dib.bmiHeader.biPlanes = 1u;
//	dib.bmiHeader.biWidth = (long)width;
//	dib.bmiHeader.biHeight = (long)-height;
//	dib.bmiHeader.biSizeImage = size;
//	dib.bmiHeader.biXPelsPerMeter = 11811L;
//	dib.bmiHeader.biYPelsPerMeter = 11811L;
//	dib.bmiHeader.biClrImportant = 0uL;
//	dib.bmiHeader.biClrUsed = 0uL;
//
//
//	HANDLE fp = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
//		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (fp == INVALID_HANDLE_VALUE)
//		return FALSE;
//	DWORD dwr = 0uL;
//	WriteFile(fp, (LPCVOID)&bmHeader, sizeof(BITMAPFILEHEADER), &dwr, NULL);
//	WriteFile(fp, (LPCVOID)&dib.bmiHeader, sizeof(BITMAPINFOHEADER), &dwr, NULL);
//	if (bpp == 32) WriteFile(fp, (LPCVOID)arr, size, &dwr, NULL);	
//	
//	FlushFileBuffers(fp);
//	CloseHandle(fp);
//	return TRUE;
//}




int main()
{
    int cw = 200;
    int ch = 200;
    int arr[200 * 200] = { 0 };
	int rs, gs, bs;
    unsigned char Red, Green, Blue;
    unsigned long rgb;
	//char folder1[] = "/blowfish16r//";
	//char folder2[] = "output\\blowfish16rbmp\\";
	//CreateDirectory(folder2, NULL);
	char FileName[256];
	char label[] = "blowfish8r_";
	for (int i = 0; i < 1000; i++) {
		
		FILE* File;
		//fopen_s(&File, "ciphertext.txt", "rb");
		sprintf(FileName, "%s%i%s", label, i, ".txt");
		fopen_s(&File, FileName, "rb");


		//printf(getc(File) == EOF ? "Empty file\n" : "");
		//char outputFolder[] = "output\\";
		//if (CreateDirectory(outputFolder, NULL)) printf("Output directory created\n");


		for (int y = 0; y < ch; y++) {
			for (int x = 0; x < cw; x++) {
				//if (fgetc(File) == 'EOF') rewind(File);
				rs = fgetc(File); gs = fgetc(File); bs = fgetc(File);
				if (rs == EOF) {
					fseek(File, 0L, SEEK_SET);
					rs = fgetc(File);
				}
				if (gs == EOF) {
					fseek(File, 0L, SEEK_SET);
					gs = fgetc(File);
				}
				if (bs == EOF) {
					fseek(File, 0L, SEEK_SET);
					bs = fgetc(File);
				}
				Red = rs;
				Green = gs;
				Blue = bs;
				arr[y * cw + x] = ((Red & 0xFF) << 16) | ((Green & 0xFF) << 8) | ((Blue & 0xFF));

			}
		}


		char cipher[] = "blowfish8rb_";
		char OutputFile[256];
		//sprintf_s(OutputFile, "%s%s", cipher, ".bmp");
		sprintf_s(OutputFile, "%s%i%s", cipher, i, ".bmp");
		//if (SaveFile(_T(OutputFile), arr, cw, ch, 32)) cout << "File saved" << endl;
		//else _putts(_T("Error save file !"));

		DWORD p_row = (DWORD)((200 * 32 + 31) & ~31) / 8uL;
		DWORD size = (DWORD)(200 * p_row);

		// Сформируем файловый заголовок BMP
		BITMAPFILEHEADER bmHeader;
		ZeroMemory(&bmHeader, sizeof(BITMAPFILEHEADER));
		bmHeader.bfType = 0x4D42;
		bmHeader.bfSize = bmHeader.bfOffBits + size;
		bmHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);


		BITMAPINFO dib;
		ZeroMemory(&dib, sizeof(BITMAPINFO));
		dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		dib.bmiHeader.biBitCount = 32;
		dib.bmiHeader.biCompression = BI_RGB;
		dib.bmiHeader.biPlanes = 1u;
		dib.bmiHeader.biWidth = (long)200;
		dib.bmiHeader.biHeight = (long)-200;
		dib.bmiHeader.biSizeImage = size;
		dib.bmiHeader.biXPelsPerMeter = 11811L;
		dib.bmiHeader.biYPelsPerMeter = 11811L;
		dib.bmiHeader.biClrImportant = 0uL;
		dib.bmiHeader.biClrUsed = 0uL;


		HANDLE fp = CreateFile(OutputFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (fp == INVALID_HANDLE_VALUE)
			return FALSE;
		DWORD dwr = 0uL;
		WriteFile(fp, (LPCVOID)&bmHeader, sizeof(BITMAPFILEHEADER), &dwr, NULL);
		WriteFile(fp, (LPCVOID)&dib.bmiHeader, sizeof(BITMAPINFOHEADER), &dwr, NULL);
		WriteFile(fp, (LPCVOID)arr, size, &dwr, NULL);
		fclose(File);
		FlushFileBuffers(fp);
		CloseHandle(fp);
	}
    
}

