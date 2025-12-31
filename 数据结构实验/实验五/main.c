#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef char TElemType;

typedef struct BiTNode {
    TElemType data;
    struct BiTNode *lchild;
    struct BiTNode *rchild;
} BiTNode, *BiTree;

static int preIndex = 0;

BiTree CreateBiTreeFromPre(const char *pre) {
    char ch = pre[preIndex++];
    if (ch == '#' || ch == '\0') {
        return NULL;
    }
    BiTree T = (BiTree)malloc(sizeof(BiTNode));
    if (!T) exit(1);
    T->data = ch;
    T->lchild = CreateBiTreeFromPre(pre);
    T->rchild = CreateBiTreeFromPre(pre);
    return T;
}

void PreOrder(BiTree T) {
    if (!T) return;
    putchar(T->data);
    PreOrder(T->lchild);
    PreOrder(T->rchild);
}

void InOrder(BiTree T) {
    if (!T) return;
    InOrder(T->lchild);
    putchar(T->data);
    InOrder(T->rchild);
}

void PostOrder(BiTree T) {
    if (!T) return;
    PostOrder(T->lchild);
    PostOrder(T->rchild);
    putchar(T->data);
}

void DestroyBiTree(BiTree T) {
    if (!T) return;
    DestroyBiTree(T->lchild);
    DestroyBiTree(T->rchild);
    free(T);
}

typedef struct CSNode {
    TElemType data;
    struct CSNode *firstchild;   // 第一个孩子
    struct CSNode *nextsibling;  // 右兄弟
} CSNode, *CSTree;

CSTree BiTreeToCSTree(BiTree T) {
    if (!T) return NULL;
    CSTree p = (CSTree)malloc(sizeof(CSNode));
    if (!p) exit(1);
    p->data = T->data;
    p->firstchild = BiTreeToCSTree(T->lchild);
    p->nextsibling = BiTreeToCSTree(T->rchild);
    return p;
}

BiTree CSTreeToBiTree(CSTree T) {
    if (!T) return NULL;
    BiTree p = (BiTree)malloc(sizeof(BiTNode));
    if (!p) exit(1);
    p->data = T->data;
    p->lchild = CSTreeToBiTree(T->firstchild);
    p->rchild = CSTreeToBiTree(T->nextsibling);
    return p;
}

// 以广义表形式输出树
void PrintCSTree(CSTree T) {
    if (!T) return;
    putchar(T->data);
    if (T->firstchild) {
        putchar('(');
        CSTree child = T->firstchild;
        while (child) {
            PrintCSTree(child);
            if (child->nextsibling) putchar(',');
            child = child->nextsibling;
        }
        putchar(')');
    }
}

void DestroyCSTree(CSTree T) {
    if (!T) return;
    DestroyCSTree(T->firstchild);
    DestroyCSTree(T->nextsibling);
    free(T);
}

int main(void) {
    SetConsoleOutputCP(65001);
    char pre[256];
    BiTree T = NULL, T2 = NULL;
    CSTree G = NULL;

    printf("输入二叉树的先序序列(用#表示空结点)，例如 ABC##DE#G##F###\n");
    if (scanf("%255s", pre) != 1) {
        return 0;
    }

    preIndex = 0;
    T = CreateBiTreeFromPre(pre);

    printf("\n先序遍历: ");
    PreOrder(T);
    printf("\n中序遍历: ");
    InOrder(T);
    printf("\n后序遍历: ");
    PostOrder(T);
    printf("\n");

    // 二叉树 -> 树
    G = BiTreeToCSTree(T);
    printf("\n转换得到的一般树(孩子-兄弟结构, 广义表表示): ");
    PrintCSTree(G);
    printf("\n");

    // 树 -> 二叉树
    T2 = CSTreeToBiTree(G);
    printf("\n从一般树再转换回来的二叉树中序遍历: ");
    InOrder(T2);
    printf("\n");

    DestroyBiTree(T);
    DestroyBiTree(T2);
    DestroyCSTree(G);

    return 0;
}
