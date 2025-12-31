#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

#define STACK_INIT_SIZE 100 /* 栈元素初始空间大小 */
#define STACK_INCREMENT 10  /* 栈扩容增量 */

typedef char SElemType;

/* 顺序栈结构 */
typedef struct
{
    SElemType *base; /* 栈底指针 */
    SElemType *top;  /* 栈顶指针 */
    int stackSize;   /* 当前分配的存储容量 */
} SqStack;

bool InitStack(SqStack *S);
void DestroyStack(SqStack *S);
bool StackEmpty(const SqStack *S);
int StackLength(const SqStack *S);
bool GetTop(const SqStack *S, SElemType *e);
bool Push(SqStack *S, SElemType e);
bool Pop(SqStack *S, SElemType *e);

#endif /* STACK_H */ 
