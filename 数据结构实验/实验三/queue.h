#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

#define MAXQSIZE 100 /* 循环队列容量 */

typedef char QElemType;

/* 循环队列结构 */
typedef struct
{
    QElemType *base; /* 存储空间基址 */
    int front;       /* 队头指针 */
    int rear;        /* 队尾指针 */
    int maxSize;     /* 容量 */
} SqQueue;

bool InitQueue(SqQueue *Q);
int QueueLength(const SqQueue *Q);
bool EnQueue(SqQueue *Q, QElemType e);
bool DeQueue(SqQueue *Q, QElemType *e);
void ClearQueue(SqQueue *Q);
void DestroyQueue(SqQueue *Q);
bool QueueEmpty(const SqQueue *Q);

#endif /* QUEUE_H */
