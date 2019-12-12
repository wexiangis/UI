#ifndef _VIEWTYPE_H
#define _VIEWTYPE_H

#include "viewValue.h"

/***** <注意> 以'_'下划线开头的变量为系统赋值 *****/

typedef struct ViewStruct View_Struct;
typedef struct ViewDraw View_Draw;
typedef struct ViewState View_State;
typedef struct ViewCallback View_Callback;

struct ViewDraw{

};

struct ViewState{

};

struct ViewCallback{

};

struct ViewStruct{

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