#ifndef STACK_H
#define STACK_H

#include "common.h"

typedef struct
{
    int step;     // 路径上的序号
    PosType seat; // 当前坐标
    int di;       // 往下一坐标位置的方向
} ElemType;

typedef struct NodeType
{
    ElemType data;         // 数据
    struct NodeType *next; // 指针
} NodeType, *LinkType;

typedef struct
{
    LinkType top; // 栈顶指针
    int size;     // 栈大小
} Stack;

void InitStack(Stack *S);
bool StackEmpty(Stack S);
bool Push(Stack *S, ElemType e);
bool Pop(Stack *S, ElemType *e);
bool GetTop(Stack S, ElemType *e);
void DestroyStack(Stack *S);

#endif