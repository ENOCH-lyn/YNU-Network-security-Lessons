#include "stack.h"

// 初始化栈
void InitStack(Stack *S)
{
    S->top = NULL;
    S->size = 0;
}

// 判栈空
bool StackEmpty(Stack S)
{
    return S.size == 0;
}

// 入栈
bool Push(Stack *S, ElemType e)
{
    LinkType newNode = (LinkType)malloc(sizeof(NodeType));
    if (!newNode)
        return false;
    newNode->data = e;
    newNode->next = S->top;
    S->top = newNode;
    S->size++;
    return true;
}

// 出栈
bool Pop(Stack *S, ElemType *e)
{
    if (StackEmpty(*S))
        return false;
    LinkType p = S->top;
    *e = p->data;
    S->top = p->next;
    free(p);
    S->size--;
    return true;
}

// 取栈顶元素
bool GetTop(Stack S, ElemType *e)
{
    if (StackEmpty(S))
        return false;
    *e = S.top->data;
    return true;
}

// 销毁栈
void DestroyStack(Stack *S)
{
    ElemType e;
    while (!StackEmpty(*S))
    {
        Pop(S, &e);
    }
}