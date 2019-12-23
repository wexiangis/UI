#ifndef _VIEWTYPE_H
#define _VIEWTYPE_H

#include "viewValue.h"

/***** <注意> 以'_'下划线开头的变量为系统赋值 *****/

typedef struct ViewStruct View_Struct;
typedef struct ViewDraw View_Draw;
typedef struct ViewState View_State;
typedef struct ViewFocus View_Focus;
typedef struct ViewCallback View_Callback;

//========== 坐标和多边形 ==========

//坐标
typedef union{
    int xy[2];
    long long XY;
}Dot_Type;

//多边形
typedef struct PolygonType{
    int *point; //定点坐标数组
    int len;    //坐标数组长度 定点个数=len/2
    struct PolygonType *next;//支持多个多边形组成图像
}Polygon_Type;

//========== 形状封装 ==========

//形状列表
typedef enum ViewShapeType{
    VST_NULL = -1,
    VST_RECT = 0,   //矩形 
    VST_PARA,       //平行四边形
    VST_LINE,       //线条
    VST_CIRCLE,     //圆
    VST_SECTOR,     //扇形
    VST_PROGRESS_BAR,//进度条
    VST_SCROLL_BAR, //滚动条
    VST_SWITCH,     //开关
    VST_POLYGON,    //多边形
}ViewShape_Type;

//矩形
typedef struct ViewShapeRectType{
    int rad;//圆角半径
    int lineSize;//线宽
    int angle;//旋转角度(-360, +360)
}ViewShapeRect_Type;

//平行四边形
typedef struct ViewShapeParaType{
    int err;//矩形基础上,>0时左上和右下缩进量,<0时右上和左下缩进量
    int lineSize;//线宽
    int mode;//缩进模式 0:向内缩进(控件宽减少) 1:向外扩充(控件宽不变) 2:向外扩充(控件宽变大) 注意1,2会超出控件范围需要设置overDraw
}ViewShapePara_Type;

//线条
typedef struct ViewShapeLineType{
    int mode;//线条模式 0/左上-右下 1/左下-右上 2/左上-右上 3/右上-右下 4/左下-右下 5/左上-左下
    int lineSize;//线宽
    int space;//不为0时画 虚线
    // int sErr[2], eErr[2];//线段两端点在控件内的进一步偏移量 注意参照mode说明来定位起始和结束端点
}ViewShapeLine_Type;

//圆
typedef struct ViewShapeCircleType{
    int rad;//半径
    int rad2;//内径
    int div;//分块数
    int divStart;//分块起始
    int divEnd;//分块结束
}ViewShapeCircle_Type;

//扇形
typedef struct ViewShapeSector{
    int rad;//外径
    int rad2;//内径
    int rotary;//旋转角度0~359 //控件中心为圆心
    int angle;//开角
}ViewShapeSector_Type;

//进度条
typedef struct ViewShapeProcessBarType{
    int rad;//圆角半径
    int lineSize;//线宽
    int percent;//百分数值
    int mode;//0:左到右 1:右到左 2:下到上 3:上到下
    int edge;//精度条和边界间距
}ViewShapeProcessBar_Type;

//滚动条
typedef struct ViewShapeScrollBarType{
    int rad;//圆角半径
    int lineSize;//线宽
    int percent;//百分数值
    int percentOfTotal;//滚动部分占总长比例
    int mode;//0:左-右滚动 1:上-下滚动
}ViewShapeScrollBar_Type;

//开关
typedef struct ViewShapeSwitchType{
    int rad;//圆角半径
    int lineSize;//线宽
    int status;//0/关 1/开
    int printType;//字体 不填/默认240
    int mode;//0/[关/开] 1/圆心圆 2/保留
}ViewShapeSwitch_Type;

//多边形
typedef struct ViewShapePolygonType{
    Polygon_Type *pol;
    int lineSize;//线宽
    int angle;//旋转角度(-360, +360)
}ViewShapePolygon_Type;

//形状汇总
typedef union ViewShapeUnion{
    ViewShapeRect_Type rect;
    ViewShapePara_Type para;
    ViewShapeLine_Type line;
    ViewShapeCircle_Type circle;
    ViewShapeSector_Type sector;
    ViewShapeProcessBar_Type processBar;
    ViewShapeScrollBar_Type scrollBar;
    ViewShapeSwitch_Type sw;
    ViewShapePolygon_Type polygon;
}ViewShape_Union;

//========== 控件位置 ==========

typedef enum{
    VWHT_MATCH = -10000,   //和父控件同长, VWHT_MATCH*n-m 表示父控件的 m/n 长 // 注意: n,m < 100
    VWHT_FULL = -1000000,  //和屏幕同长, VWHT_FULL*n-m 表示屏幕的 m/n 长     // 注意: n,m < 100
}ViewWidthHeight_Type;

typedef enum{
    VRNT_PARENT = 0,
    VRNT_LAST = -10000,   //相对对象为前一个, VRNT_LAST-n 表示前 n 个  // 注意: n < 100
    VRNT_NEXT = -1000000, //相对对象为后一个, VRNT_NEXT-n 表示后 n 个  // 注意: n < 100
}ViewWRelativeNumber_Type;

typedef enum{
    VRT_TOP = 0x01,
    VRT_BOTTOM = 0x03,  //和 VRT_TOP 互斥使用
    VRT_LEFT = 0x10,
    VRT_RIGHT = 0x30,   //和 VRT_LEFT 互斥使用
}ViewRelative_Type;

//========== 控件绘图 ==========

struct ViewDraw{

    //---------- 位置参数 ----------

    //宽高 直接指定数值或使用如下宏
    //宏定义: VWHT_MATCH, VWHT_MATCH*n, VWHT_MATCH*n-m : 父级控件长,父级控件长的1/n,父级控件长的m/n
    //宏定义: VWHT_FULL, VWHT_FULL*n, VWHT_FULL*n-m : 屏幕长,屏幕长的1/n,屏幕长的m/n
    int width, height;

    //指定相对 view 的序号(从1数起),为0时相对父控件
    //宏定义: VRNT_LAST, VRNT_LAST*n : 前一个,前 n 个
    //宏定义: VRNT_NEXT, VRNT_NEXT*n : 后一个,后 n 个
    int rNumber;
    
    //相对位置关系,用 VRT_TOP/VRT_BOTTOM 和 VRT_LEFT/VRT_RIGHT 组合 
    //      例如左上: rType = VRT_LEFT | VRT_TOP;
    //      例如右下: rType = VRT_RIGHT | VRT_BOTTOM;
    //当 rType = 0 (也就是不填), 判定当前 view 的坐标和 rNumber 指定的 view 的左上角坐标一致
    int rType;

    //相对偏移量
    //相对对象为其它 view 时(rNumber != 0): 下面数值为外偏移量,也就是靠在一起时的间距
    //相对对象为父控件时(rNumber = 0): 下面数值为内偏移量,也就是在父控件里面的相对偏移量
    //      例如: rNumber=0; rType=VRT_BOTTOM; 
    //           当前 view 在父控件内部的左下角, rErrTopBottom 将指定与父控件底线的距离 rErrLeftRight 与左边的距离
    //      例如: rNumber=VRNT_LAST; rType=VRT_RIGHT; 
    //           当前 view 在上一个 view 的右边,且上边平齐, rErrTopBottom 将指定上边往下平移量 rErrLeftRight 往右平移量
    //      例如: rNumber=VRNT_LAST; rType=0; 
    //           当前 view 与上一个 view 的左上角坐标对齐, rErrTopBottom 将指定往下平移量 rErrLeftRight 往右平移量
    int rErrTopBottom;
    int rErrLeftRight;

    //横/纵向 相对父控件居中(优先于上面 rErrTopBottom rErrLeftRight 判定)
    bool centerHor, centerVer;

    //真实宽高 [系统赋值]
    int _absWidth, _absHeight;

    //控件在屏幕中的绝对位置范围 [系统赋值]
    int _absXY[2][2];

    //---------- 背景色 ----------

    //背景颜色的颜色,绘制范围为 absXY[2][2]
    ViewValue_Format *backgroundColor;
    int backgroundRad;//圆角
    float backgroundAlpha;//透明度 0~1 1表示完全透明
    bool backgroundOnTop;//覆盖在控件上

    //---------- 形状 ----------

    //形状 0 默认为矩形
    ViewShape_Type shapeType;

    //形状具体数据
    ViewShape_Union shape;

    //相对于控件在4个方向上的缩进
    int shapeEdgeTop, shapeEdgeBottom;
    int shapeEdgeLeft, shapeEdgeRight;

    //形状的真正绘制范围 [系统赋值]
    int _shapeAbsXY[2][2];

    //不同形状 shapeColorPrint 参数意义不同(看上面"104行"的说明)
    ViewValue_Format *shapeColorPrint;   //默认为打印色
    ViewValue_Format *shapeColorBackground;  //默认为打底色

    //透明度 0~1, 1表示完全透明
    float shapeAlpha;

    //---------- 图片 ----------

    //图片路径,不为NULL时启用
    char *picPath;

    //文字输出框相对于控件在4个方向上的缩进,用以确定输出范围
    int picEdgeTop, picEdgeBottom;
    int picEdgeLeft, picEdgeRight;

    //图片实际输出范围 [系统赋值]
    int _picAbsXY[2][2];

    //图片原始数据 [系统赋值]
    unsigned char *_pic;
    //指向图片原始数据的指针矩阵 [系统赋值]
    unsigned char ***_picMap;

    //图片的实际 宽/高/像素字节 [系统赋值]
    int _picWidth, _picHeight, _picPW;

    //使用替换颜色
    int picReplaceColor;//目标颜色值
    ViewValue_Format *picReplaceColorBy;//替换成
    bool picReplaceColorOn;//启用替换

    //使用透过颜色
    int picInvisibleColor;//指定透明色
    bool picInvisibleColorOn;//启用透明色

    //透明度 0~1, 1表示完全透明
    float picAlpha;

    //---------- 内容 ----------

    //数据
    ViewValue_Format *value;

    //文字输出框相对于控件在4个方向上的缩进,用以确定输出范围
    int valueEdgeTop, valueEdgeBottom;
    int valueEdgeLeft, valueEdgeRight;

    //文字输出范围 [系统赋值]
    int _valueAbsXY[2][2];

    //文字在输出范围内的 横/纵向 相对位置 0/居中(默认) 1/向上对齐 2/向下对齐
    int valueCenterHor, valueCenterVer;

    //字体代号,例如:160,240,320,400,480,560,640 前两位表示字号,最后位表示线宽(0表示最细)
    int valueType;

    //字颜色
    ViewValue_Format *valueColor;

    //横向字间距
    int valueXEdge;

    //纵向字间距,>0时启用自动换行,并作为行间隔
    int valueYEdge;

    //透明度 0~1, 1表示完全透明
    float valueAlpha;

    //---------- 内容滚动 ----------

    //滚动(和 valueEdge 互斥使用) >0时启用并作为平移像素量
    int scroll;
    //多少个绘制周期算一次计数
    int scrollPeriod;
    //计数,当前平移量 = scrollCount * scroll [系统赋值]
    int _scrollCount;
    //计数,满 scrollPeriod 时 scrollCount+1 [系统赋值]
    int _scrollCount2;

    //---------- 下划线 ----------

    //下划线 >0启用并表示线宽,与控件的 width 同长度
    int bottomLine;
    ViewValue_Format *bottomLineColor;
    bool bottomLineOnValue;//放置到内容底下,与内容同长度
    float bottomLineAlpha;

    //---------- 描边 ----------

    //描边,>0时启用,并作为描边的宽度
    int side;
    ViewValue_Format *sideColor;
    //透明度 0~1, 1表示完全透明
    float sideAlpha;

    //---------- 其它标注 ----------

    //右上角特殊标记,例如未读的邮件
    bool mark;

};

//========== 控件状态 ==========

struct ViewState{

    //不绘当前 view, 不绘制子 view (回调函数失能)
    bool hideAll;

    //不绘制子 view
    bool hide;
    
    //锁定状态,不能操作,变成灰色 (回调函数失能)
    bool lock;

    //焦点遍历时是否可在此停留
    bool focusStop;

    //焦点系统的指针 [系统赋值]
    View_Focus *_focus;

};

//========== 控件回调 ==========

struct ViewCallback{

};

//========== 控件焦点 ==========

typedef enum{
    VFE_NULL = 0,
    VFE_RETURN,
    VFE_ENTER,
    VFE_NEXT,
    VFE_LAST,
    VFE_HOME,
}ViewFocus_Event;

typedef struct ViewFocus{
    View_Struct *topView;
    View_Struct *view;   //当前view
    ViewValue_Format *color;    //聚焦时绘制颜色
    int lineSize;
    float alpha;
};

//========== 控件结构体 ==========

struct ViewStruct{
    //名称
    char name[64];

    //----- 同级链表 -----
    int _order;//在同级链表中的序号(序号从1数起)
    View_Struct *_next;
    View_Struct *_last;
    View_Struct *_parent;
    //----- 子级链表 -----
    int _total;      //子 view 的总数
    bool hideChild;  //不绘制子 view
    View_Struct *_child;    //子 view 链表的头
    View_Struct *_lastChild;//子 view 链表的尾
    //----- 跳转和替换 -----
    //跳转 view, 启用时后面的 绘图 和 子 view 不绘制
    View_Struct *jumpView;
    //在当前 view 按下确认后,有 jumpView 则跳转,并自动置为 true; 返回时自动置 false [半系统赋值]
    bool jumpViewOn;
    //默认在绘制 jumpView 时屏蔽当前 view,要同时可见置为 true [主要用于 Home 界面]
    bool jumpViewKeepOn;
    //自己作为上一级的 jumpView 时,通过 backView 来返回上一级
    View_Struct *backView;

    //----- 绘图相关 -----
    View_Draw draw;

    //----- 状态相关 -----
    ViewState state;

    //----- 各种回调函数 -----
    ViewCallback callback;
};

#endif