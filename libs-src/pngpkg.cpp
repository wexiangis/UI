
//原作: https://blog.csdn.net/wang93IT/article/details/85003730

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "png.h"
#include "pngpkg.h"

#define PNG_BYTES_TO_CHECK	8
#define HAVE_ALPHA			1
#define NOT_HAVE_ALPHA		0

int png_check(FILE **fp, const char *filename) //检查是否png文件
{
	char checkheader[PNG_BYTES_TO_CHECK]; //查询是否png头
	*fp = fopen(filename, "rb");
	if (*fp == NULL) {
		fprintf(stderr, "open [%s] failed\n", filename);
		return -1;
	}
	if (fread(checkheader, 1, PNG_BYTES_TO_CHECK, *fp) != PNG_BYTES_TO_CHECK) //读取png文件长度错误直接退出
		return 0;
	return png_sig_cmp((png_bytep)checkheader, 0, PNG_BYTES_TO_CHECK); //0正确, 非0错误
}

int png_read(const char *filename, PicData *out) //取出png文件中的rgb数据
{
	png_structp png_ptr; //png文件句柄
	png_infop	info_ptr;//png图像信息句柄
	FILE *fp;
	if (png_check(&fp, filename) != 0) {
		fprintf(stderr, "[%s] file is not png\n", filename);
		return -1;
	}

	// printf("launcher[%s] ...\n", PNG_LIBPNG_VER_STRING); //打印当前libpng版本号
 
	//1: 初始化libpng的数据结构 :png_ptr, info_ptr
	png_ptr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
	info_ptr = png_create_info_struct(png_ptr);
 
	//2: 设置错误的返回点
	setjmp(png_jmpbuf(png_ptr));
	rewind(fp); //等价fseek(fp, 0, SEEK_SET);
 
	//3: 把png结构体和文件流io进行绑定 
	png_init_io(png_ptr, fp);
	//4:读取png文件信息以及强转转换成RGBA:8888数据格式
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0); //读取文件信息
	int channels, color_type; 
	channels 	= png_get_channels(png_ptr, info_ptr); //通道数量
	color_type 	= png_get_color_type(png_ptr, info_ptr);//颜色类型
	out->bit_depth = png_get_bit_depth(png_ptr, info_ptr);//位深度	
	out->width 	 = png_get_image_width(png_ptr, info_ptr);//宽
	out->height  = png_get_image_height(png_ptr, info_ptr);//高
	
	//if(color_type == PNG_COLOR_TYPE_PALETTE)
	//	png_set_palette_to_rgb(png_ptr);//要求转换索引颜色到RGB
	//if(color_type == PNG_COLOR_TYPE_GRAY && out->bit_depth < 8)
	//	png_set_expand_gray_1_2_4_to_8(png_ptr);//要求位深度强制8bit
	//if(out->bit_depth == 16)
	//	png_set_strip_16(png_ptr);//要求位深度强制8bit
	//if(png_get_valid(png_ptr,info_ptr,PNG_INFO_tRNS))
	//	png_set_tRNS_to_alpha(png_ptr);
	//if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	//	png_set_gray_to_rgb(png_ptr);//灰度必须转换成RG

	// printf("channels = %d color_type = %d bit_depth = %d width = %d height = %d ...\n", channels, color_type, out->bit_depth, out->width, out->height);
 
	int i, j, k;
	int size, pos = 0;
	int temp;
	
	//5: 读取实际的rgb数据
	png_bytepp row_pointers; //实际存储rgb数据的buf
	row_pointers = png_get_rows(png_ptr, info_ptr); //也可以分别每一行获取png_get_rowbytes();
	size = out->width * out->height; //申请内存先计算空间
	if (channels == 4 || color_type == PNG_COLOR_TYPE_RGB_ALPHA) { //判断是24位还是32位
		out->alpha_flag = HAVE_ALPHA; //记录是否有透明通道
		size *= (sizeof(unsigned char) * 4); //size = out->width * out->height * channel
		out->rgba = (png_bytep)malloc(size);
		if (NULL == out->rgba) {
			// printf("malloc rgba faile ...\n");
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
			fclose(fp);
			return -1;
		}
		//从row_pointers里读出实际的rgb数据出来
		temp = channels - 1;
		for (i = 0; i < out->height; i++) 
			for (j = 0; j < out->width * 4; j += 4) 
				for (k = temp; k >= 0; k--)
					out->rgba[pos++] = row_pointers[i][j + k];
	} else if (channels == 3 || color_type == PNG_COLOR_TYPE_RGB) { //判断颜色深度是24位还是32位
		out->alpha_flag = NOT_HAVE_ALPHA;
		size *= (sizeof(unsigned char) * 3);
		out->rgba = (png_bytep)malloc(size);
		if (NULL == out->rgba) {
			// printf("malloc rgba faile ...\n");
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
			fclose(fp);
			return -1;
		}
		//从row_pointers里读出实际的rgb数据
		temp = (3 * out->width);
		for (i = 0; i < out->height; i ++) {
			for (j = 0; j < temp; j += 3) {
				out->rgba[pos++] = row_pointers[i][j+2];
				out->rgba[pos++] = row_pointers[i][j+1];
				out->rgba[pos++] = row_pointers[i][j+0];
			}
		}
	} else return -1; 
	//6:销毁内存
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	fclose(fp);
	//此时， 我们的out->rgba里面已经存储有实际的rgb数据了
	//处理完成以后free(out->rgba)
	return 0;
}

int RotationRight90(unsigned char * src, int srcW, int srcH, int channel) //顺时针旋转90度
{
	unsigned char * tempSrc = NULL; //临时的buf用来记录原始的图像(未旋转之前的图像)
	int mSize = srcW * srcH * sizeof(char) * channel;
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 3;
	int desW = 0;
	int desH = 0;

	desW = srcH;
	desH = srcW;

	tempSrc = (unsigned char *)malloc(sizeof(char) * srcW * srcH * channel);
	memcpy(tempSrc, src, mSize); //拷贝原始图像至tempbuf
	for(i = 0; i < desH; i ++)
	{
		for(j = 0; j < desW; j ++)
		{
			for(k = 0; k < channel; k ++)
			{
				src[(i * desW + j) * channel + k] = tempSrc[((srcH - 1 - j) * srcW + i) * channel + k]; //替换像素
			}
		}
	}
	free(tempSrc);
	return 0;
}

int png_write(const char *filename , PicData *out) //生成一个新的png图像
{
	png_structp png_ptr;
	png_infop 	info_ptr;
	png_byte color_type;
	png_bytep * row_pointers;
	FILE *fp = fopen(filename, "wb");
	if (NULL == fp) {
		fprintf(stderr, "open [%s] failed\n", filename);
		return -1;
	}
	//1: 初始化libpng结构体  
	png_ptr	= png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (!png_ptr) {
		fprintf(stderr, "png_create_write_struct failed\n");
		return -1;
	}
	//2: 初始化png_infop结构体 ， 
	//此结构体包含了图像的各种信息如尺寸，像素位深, 颜色类型等等
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		fprintf(stderr, "png_create_info_struct failed\n");
		return -1;
	}
	//3: 设置错误返回点
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "error during init_io\n");
		return -1;
	}
	//4:绑定文件IO到Png结构体
	png_init_io(png_ptr, fp);
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "error during init_io\n");
		return -1;
	}
	if (out->alpha_flag == HAVE_ALPHA) color_type = PNG_COLOR_TYPE_RGB_ALPHA;
	else color_type = PNG_COLOR_TYPE_RGB;
	//5：设置以及写入头部信息到Png文件
	png_set_IHDR(png_ptr, info_ptr, out->width, out->height, out->bit_depth,
	color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "error during init_io\n");
		return -1;
	}
	int channels, temp;
	int i, j, pos = 0;
	if (out->alpha_flag == HAVE_ALPHA) {
		channels = 4;
		temp = (4 * out->width);
		// printf("have alpha ...\n");
	} else {
		channels = 3;
		temp = (3 * out->width);
		// printf("not have alpha ...\n");
	}
	// 顺时针旋转90度 ， 旋转完了一定要把width 和height调换 不然得到的图像是花的  旋转三次就是逆时针旋转一次
	//RotationRight90(out->rgba, out->width, out->height, channels);
	//RotationRight90(out->rgba, out->height, out->width, channels);
	//RotationRight90(out->rgba, out->width, out->height, channels);
	row_pointers = (png_bytep*)malloc(out->height * sizeof(png_bytep));
	for (i = 0; i < out->height; i++) {
		row_pointers[i] = (png_bytep)malloc(temp* sizeof(unsigned char));
		for (j = 0; j < temp; j += channels) {
			if (channels == 4) {
				row_pointers[i][j+3] = out->rgba[pos++];
				row_pointers[i][j+2] = out->rgba[pos++];
				row_pointers[i][j+1] = out->rgba[pos++];
				row_pointers[i][j+0] = out->rgba[pos++];
			} else {
				row_pointers[i][j+2] = out->rgba[pos++];
				row_pointers[i][j+1] = out->rgba[pos++];
				row_pointers[i][j+0] = out->rgba[pos++];
			}
		}
	}
	//6: 写入rgb数据到Png文件
	png_write_image(png_ptr, (png_bytepp)row_pointers);
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "error during init_io\n");
		return -1;
	}
	//7: 写入尾部信息
	png_write_end(png_ptr, NULL);
	//8:释放内存 ,销毁png结构体
	for (i = 0; i < out->height; i ++)
		free(row_pointers[i]);
	free(row_pointers);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);
	return 0;
}

// int main(int argc, char **argv)
// {
// 	PicData out;
// 	if (argc == 3) {
// 		png_read(argv[1], &out);
// 		png_write(argv[2], &out);
// 		free(out.rgba);
// 	} else {
// 		puts("please input two file, \nargv[1]:source.png argv[2]:dest.png");	
// 	}
// 	return 0;
// }

