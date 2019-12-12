#ifndef _VIEWVALUE_H
#define _VIEWVALUE_H

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h> //互斥锁
#include <stdarg.h>  //变长参数

//========== 数据类型代号 ==========
typedef enum{
    VT_NULL = 0,
    VT_CHAR = 1,
    VT_STRING,
    VT_STRING_ARRAY,
    VT_INT,
    VT_INT_ARRAY,
    VT_DOUBLE,
    VT_DOUBLE_ARRAY,
    VT_BOOL,
    VT_BOOL_ARRAY,
    VT_POINT,       //不参与文件读/写
    VT_POINT_ARRAY, //不参与文件读/写
    VT_END
}ViewValue_Type;

//========== 类型+名称+数据存储 ==========
#define VIEW_VALUE_NAME_LEN     64 //变量名称限长32字节
typedef struct ViewValueFormat{
    //[互斥锁]
    pthread_mutex_t lock;
    //[类型]
    ViewValue_Type type;
    //[名称]
    char name[VIEW_VALUE_NAME_LEN];
    //[这是一块固定大小的内存,用来存储任意类型数据]
    union {
        char Char;
        char *String;
        char **StringArray;
        int Int;
        int *IntArray;
        double Double;
        double *DoubleArray;
        bool Bool;
        bool *BoolArray;
        void *Point;
        void **PointArray;
    }value;
    //[描述value内存大小的变量]
    //value 为普通变量时:
    //      vSize 值为变量大小字节数
    //      例如: sizeof(char), sizeof(int), sizeof(double)
    //value 为指针时:
    //      vSize 反映分配内存字节数,可用于计算有效数据长度
    //      例如: IntArray, 数组长度为 vSize/sizeof(int)
    //value 为 指针数组 StringArray, PointArray 时:
    //      vSize = 数组长度 * sizeof(void *)
    //      StringArray 里的每个字符串长度是任意的
    int vSize;
    //单向链表
    struct ViewValueFormat *next;
    //[多用途参数]
    //param[0]: value 为数组时(**StringArray,*DoubleArray,*BoolArray), 指定打印的分隔符
    //      例如: param[0]='/', Api输出时: "Tom/25/Man"
    //      0/默认: 使用逗号','
    //param[1]: value 为浮点数时(Double,*DoubleArray), 指定保留小数位数
    //      例如: param[0]=' ',param[1]=2, Api输出时: "12.34 98.01 1.20"
    //      0/默认:  浮点数保留6位小数
    char param[2];
}ViewValue_Format;

ViewValue_Format *viewValue_init(char *name, ViewValue_Type type, int valueNum, ...);
ViewValue_Format *viewValue_reset(ViewValue_Format *vvf, char *name, ViewValue_Type type, int valueNum, ...);
void viewValue_release(ViewValue_Format *vvf);
bool viewValue_compare(ViewValue_Format *vvf, ViewValue_Format *vvfArray, int *retNum);
int viewValue_find(ViewValue_Format *vvfArray, ...);
ViewValue_Format *viewValue_copy(ViewValue_Format *vvf1, ViewValue_Format *vvf2);
ViewValue_Format *viewValue_arrayAdd(ViewValue_Format *vvf, ...);
ViewValue_Format *viewValue_arrayRemoveByNum(ViewValue_Format *vvf, int num);

//ViewValue_Format 的文件读/写
int viewValue_save(char *filePath, ViewValue_Format **array, int arrayLen);
int viewValue_load(char *filePath, ViewValue_Format **array, int arrayLen);

#endif