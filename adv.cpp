#include <iostream>
//File stream
#include <fstream>
#include <cmath>
#include <cstring>
#include "windows.h"
using namespace std;

/********************************HXLBMPFILEH********************************/

#ifndef HXLBMPFILEH
#define HXLBMPFILEH
class HXLBMPFILE {
	BYTE *Imagedata;
public:
	int imagew, imageh;
	int iYRGBnum;//1:�Ҷ� 3����ɫ
	RGBQUAD palette[256];

	BYTE *pDataAt(int h, int Y0R0G1B2 = 0);
	BOOL AllocateMem();
	BOOL LoadBMPFILE(char * fname);
	BOOL SaveBMPFILE(char * fname);

	HXLBMPFILE();
	~HXLBMPFILE();
};
#endif // !HXLBMPFILEH

HXLBMPFILE::HXLBMPFILE() {
	Imagedata = NULL;
	for (int i = 0; i < 256; i++) {
		palette[i].rgbBlue =
			palette[i].rgbGreen =
			palette[i].rgbRed = i;
		palette[i].rgbReserved = 0;
	}
	iYRGBnum = 0;
	imagew = imageh = 0;
}

HXLBMPFILE :: ~HXLBMPFILE() {
	if (Imagedata) {
		delete Imagedata;
	}
}

BYTE * HXLBMPFILE::pDataAt(int h, int Y0R0G1B2/*RGB��һ��*/) {
	if (iYRGBnum <= Y0R0G1B2) {//?Ϊ��ɫͼ���ʱ���أ�����������
		return NULL;
	}
	int w = imagew * h + Y0R0G1B2 *imagew * imageh;
	return Imagedata + w;
}

BOOL HXLBMPFILE::AllocateMem() {
	int w = imagew *imageh*iYRGBnum;
	if (Imagedata) {
		delete Imagedata;
		Imagedata = NULL;
	}
	Imagedata = new BYTE[w];
	if (Imagedata) {
		memset(Imagedata, 0, w);
	}
	return (Imagedata != NULL);
}

BOOL HXLBMPFILE::LoadBMPFILE(char * cFilename) {
	FILE *f;
	if (strlen(cFilename) < 1) {
		return FALSE;
	}
	f = fopen(cFilename, "r+b");
	if (f == NULL) {
		return FALSE;
	}

	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;

	fread(&fh, sizeof(BITMAPFILEHEADER), 1, f);
	if (fh.bfType != 0x4d42) {
		fclose(f);
		return FALSE;
	}
	fread(&ih, sizeof(BITMAPINFOHEADER), 1, f);
	if ((ih.biBitCount != 8) && (ih.biBitCount != 24)) {
		fclose(f);
		return FALSE;
	}
	iYRGBnum = ih.biBitCount / 8;
	imagew = ih.biWidth;
	imageh = ih.biHeight;

	if (!AllocateMem()) {
		fclose(f);
		return FALSE;
	}
	if (iYRGBnum == 1) {
		fread(palette, sizeof(RGBQUAD), 256, f);
	}
	fseek(f, fh.bfOffBits, SEEK_SET);

	int w4b = (imagew *iYRGBnum + 3) / 4 * 4, i, j;
	BYTE *ptr;
	if (iYRGBnum == 1) {
		w4b -= imagew;
		for (i = imageh - 1; i >= 0; i--) {
			fread(pDataAt(i), imagew, 1, f);
			if (w4b > 0) {
				fseek(f, w4b, SEEK_CUR);
			}
		}
	}

	if (iYRGBnum == 3) {
		ptr = new BYTE[w4b];
		for (i = imageh - 1; i >= 0; i--) {
			fread(ptr, w4b, 1, f);
			for (int j = 0; j < imagew; j++) {
				*(pDataAt(i, 0) + j) = *(ptr + j * 3 + 0);
				*(pDataAt(i, 1) + j) = *(ptr + j * 3 + 1);
				*(pDataAt(i, 2) + j) = *(ptr + j * 3 + 2);
			}
		}
		delete ptr;
	}
	fclose(f);
	return TRUE;
}

BOOL HXLBMPFILE::SaveBMPFILE(char *cFilename) {
	if (!Imagedata) {
		return FALSE;
	}
	FILE *f;
	if (strlen(cFilename) < 1) {
		return FALSE;
	}
	f = fopen(cFilename, "w+b");
	if (f == NULL) {
		return FALSE;
	}

	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
	memset(&ih, 0, sizeof(BITMAPINFOHEADER));

	fh.bfType = 0x4d42;
	fh.bfReserved1 = fh.bfReserved2 = 0;
	fh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ((iYRGBnum == 1) ? 256 * sizeof(RGBQUAD) : 0);

	ih.biSize = 40;
	ih.biPlanes = 1;
	ih.biWidth = imagew;
	ih.biHeight = imageh;
	ih.biBitCount = 8 * iYRGBnum;

	int w4b = (imagew * iYRGBnum + 3) / 4 * 4;
	ih.biSizeImage = ih.biHeight * w4b;
	fh.bfSize = fh.bfOffBits + ih.biSizeImage;

	fwrite(&fh, sizeof(BITMAPFILEHEADER), 1, f);
	fwrite(&ih, sizeof(BITMAPINFOHEADER), 1, f);
	if (iYRGBnum == 1) {
		fwrite(palette, sizeof(RGBQUAD), 256, f);
	}

	BYTE *ptr;
	int i, j;
	if (iYRGBnum == 1) {
		ptr = new BYTE[10];
		memset(ptr, 0, 10);
		w4b -= ih.biWidth;
		for (i = ih.biHeight - 1; i >= 0; i--) {
			fwrite(pDataAt(i), ih.biWidth, 1, f);
			if (w4b > 0) {
				fwrite(ptr, w4b, 1, f);
			}
		}
		delete ptr;
	}
	if (iYRGBnum == 3) {
		ptr = new BYTE[w4b];
		memset(ptr, 0, w4b);
		for (i = ih.biHeight - 1; i >= 0; i--) {
			for (j = 0; j < ih.biWidth; j++) {
				*(ptr + j * 3 + 0) = *(pDataAt(i, 0) + j);
				*(ptr + j * 3 + 1) = *(pDataAt(i, 1) + j);
				*(ptr + j * 3 + 2) = *(pDataAt(i, 2) + j);
			}
			fwrite(ptr, w4b, 1, f);
		}
		delete ptr;
	}
	fclose(f);
	return TRUE;
}

/********************************HXLBMPFILEH********************************/

int advanced_file(char * inFilename, char * outFilename)
{
	/****************************** ���⻯ ******************************/
	HXLBMPFILE bmpfile;
	int i, j, v;
	if (!bmpfile.LoadBMPFILE(inFilename))
	{
		return 1;
	}

	double cnt[256], p[256], f[256];
	memset(cnt, 0, sizeof(double) * 256);

	// ͳ�Ƹ��Ҷ�ֵ���ִ���
	for (i = 0; i < bmpfile.imageh; i++)
	{
		for (j = 0; j < bmpfile.imagew; j++)
		{
			cnt[bmpfile.pDataAt(i)[j]]++;
		}
	}

	//������Ҷ�ֵ����Ƶ��
	for (int i = 0; i < 256; i++)
	{
		p[i] = cnt[i] / (bmpfile.imageh * bmpfile.imagew);
		//printf("%lf,", p[i]);
	}

	//�ۼ�
	f[0] = p[0];
	for (int i = 1; i < 256; i++)
	{
		f[i] = f[i - 1] + p[i];
		//printf("%lf,", f[i]);
	}

	//ת��Ϊ��0-255��
	for (int i = 0; i < 256; i++)
	{
		f[i] = int(255 * f[i] + 0.5);
		//printf("%lf,",f[i]);
	}

	/****************************** ���⻯ ******************************/

	/****************************** ���� G ******************************/

	//����һ����256�������ţ�0��2�����������
	double x_0to1[256];
	double y_0to1[256];
	memset(x_0to1, 0, sizeof(double) * 256);
	memset(y_0to1, 0, sizeof(double) * 256);
	//����һ����256�������ţ�0��256) ���������
	double G[256];
	memset(G, 0, sizeof(double) * 256);
	//����ϵ��
	double k;                  //����y��sumΪ1
	double sum_y = 0;
	for (int i = 0; i < 256; i++)
	{
		x_0to1[i] = 2 * i / 255.0;
		y_0to1[i] = 1 - pow((1 - x_0to1[i]), 2);
		sum_y = sum_y + y_0to1[i];
	}
	k = 1 / sum_y;
	//printf("%lf ,",sum_y);
	//printf("%lf ,", k);
	//��֪ͼ��ֱ��ͼG��
	for (int i = 0; i < 256; i++)
	{
		G[i] = k * y_0to1[i];
		//printf("%lf ,", G[i]);
	}

	/****************************** ���� G ******************************/

	/***************************** G ���⻯ *****************************/

	//��ʱ���� tmpΪ�任����
	double tmp[256];
	int G1[256];
	memset(tmp, 0, sizeof(double) * 256);
	memset(G1, 0, sizeof(int) * 256);

	//�ۼ�
	tmp[0] = G[0];
	for (int i = 1; i < 256; i++)
	{
		tmp[i] = tmp[i - 1] + G[i];
	}

	//ת��Ϊ�任����
	for (int i = 0; i < 256; i++)
	{
		tmp[i] = int(255 * tmp[i] + 0.5);
		//printf("%lf,",tmp[i]);
	}

	/****************************** G ���⻯ ******************************/

	/****************************** ӳ�亯�� ******************************/

	//�洢��Ӧ����
	int index_map[256];
	memset(index_map, 0, sizeof(int) * 256);

	for (int i = 0; i < 256; i++)
	{
		double smallest_diff = 256.0;
		for (int j = 0; j < 256; j++)
		{
			if (abs(f[i] - tmp[j]) < abs(smallest_diff))
			{
				smallest_diff = f[i] - tmp[j];
				index_map[i] = j;
			}
		}
		//printf("%d   %lf   %lf ", i, f[i],tmp[i]);
		//printf("%d : %d \n", i, index_map[i]);
	}

	/****************************** ӳ�亯�� ******************************/

	for (i = 0; i < bmpfile.imageh; i++)
	{
		for (j = 0; j < bmpfile.imagew; j++)
		{
			bmpfile.pDataAt(i)[j] = index_map[int(bmpfile.pDataAt(i)[j])];
		}
	}

	bmpfile.SaveBMPFILE(outFilename);
	return 0;
}

int main()
{
	advanced_file("bw1.bmp", "bw1_gdh.bmp");
	return 0;
}