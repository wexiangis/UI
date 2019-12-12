#ifndef _PNGPKG_H
#define _PNGPKG_H

#include <stdio.h>

typedef struct _PicData {
	int width, height; 	//长宽
	int bit_depth; 	   	//位深度
	int alpha_flag;		//是否有透明通道(0时rgba数据只有rgb)
	unsigned char *rgba;//实际rgb数据
}PicData;

//返回0正常
int png_write(const char *filename , PicData *out);

//返回0正常
int png_read(const char *filename, PicData *out);

//返回0正常
int png_rotationRight90(unsigned char * src, int srcW, int srcH, int channel);

//返回0正常
int png_check(FILE **fp, const char *filename);

#endif