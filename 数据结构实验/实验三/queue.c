#include <stdlib.h>
#include "queue.h"

/* 初始化循环队列 */
bool InitQueue(SqQueue *Q)
{
    Q->base = (QElemType *)malloc(MAXQSIZE * sizeof(QElemType));
    if (!Q->base)
        return false;
    Q->front = Q->rear = 0;
    Q->maxSize = MAXQSIZE;
    return true;
}

/* 队列长度 */
int QueueLength(const SqQueue *Q)
{
    return (Q->rear - Q->front + Q->maxSize) % Q->maxSize;
}

/* 入队 */
bool EnQueue(SqQueue *Q, QElemType e)
{
    if ((Q->rear + 1) % Q->maxSize == Q->front)
    {
        /* 队满 */
        return false;
    }
    Q->base[Q->rear] = e;
    Q->rear = (Q->rear + 1) % Q->maxSize;
    return true;
}

/* 出队 */
bool DeQueue(SqQueue *Q, QElemType *e)
{
    if (Q->front == Q->rear)
    {
        /* 队空 */
        return false;
    }
    *e = Q->base[Q->front];
    Q->front = (Q->front + 1) % Q->maxSize;
    return true;
}

/* 清空队列 */
void ClearQueue(SqQueue *Q)
{
    Q->front = Q->rear = 0;
}

/* 销毁队列 */
void DestroyQueue(SqQueue *Q)
{
    if (Q->base)
    {
        free(Q->base);
        Q->base = NULL;
    }
    Q->front = Q->rear = Q->maxSize = 0;
}

/* 判空 */
bool QueueEmpty(const SqQueue *Q)
{
    return Q->front == Q->rear;
}
