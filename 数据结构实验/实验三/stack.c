#include <stdlib.h>
#include "stack.h"

/* 初始化顺序栈 */
bool InitStack(SqStack *S)
{
    S->base = (SElemType *)malloc(STACK_INIT_SIZE * sizeof(SElemType));
    if (!S->base)
        return false;
    S->top = S->base;
    S->stackSize = STACK_INIT_SIZE;
    return true;
}

/* 销毁栈 */
void DestroyStack(SqStack *S)
{
    if (S->base)
    {
        free(S->base);
        S->base = S->top = NULL;
        S->stackSize = 0;
    }
}

/* 判空 */
bool StackEmpty(const SqStack *S)
{
    return S->top == S->base;
}

/* 栈长度 */
int StackLength(const SqStack *S)
{
    return (int)(S->top - S->base);
}

/* 取栈顶元素 */
bool GetTop(const SqStack *S, SElemType *e)
{
    if (StackEmpty(S))
        return false;
    *e = *(S->top - 1);
    return true;
}

/* 进栈 */
bool Push(SqStack *S, SElemType e)
{
    if (StackLength(S) >= S->stackSize)
    {
        int newSize = S->stackSize + STACK_INCREMENT;
        SElemType *newBase = (SElemType *)realloc(S->base, newSize * sizeof(SElemType));
        if (!newBase)
            return false;
        S->top = newBase + (S->top - S->base);
        S->base = newBase;
        S->stackSize = newSize;
    }
    *(S->top++) = e;
    return true;
}

/* 出栈 */
bool Pop(SqStack *S, SElemType *e)
{
    if (StackEmpty(S))
        return false;
    *e = *(--S->top);
    return true;
}
