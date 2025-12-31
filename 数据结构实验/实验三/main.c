#include <stdio.h>
#include <locale.h>
#include "translate.h"
#include <windows.h>

int main(void)
{

    SetConsoleOutputCP(65001);

    char magic[256];
    char human[512];
    char chinese[512];

    printf("魔王语言翻译器\n");
    printf("产生式规则：\n");
    printf("  B -> tAdA\n");
    printf("  A -> sae\n");
    printf("  (ehnxgz) -> ezegexenehe\n");
    printf("  M -> 魔\n");
    printf("  W -> 王\n");
    printf("  Y -> 语\n");
    printf("  X -> 言\n");
    printf("  F -> 翻\n");
    printf("  C -> 译\n");
    printf("  Q -> 器\n");

    printf("请输入魔王语言串 (例如 B(ehnxgz)B 或 MWYXFCQ): ");
    if (scanf("%255s", magic) != 1)
    {
        fprintf(stderr, "错误\n");
        return 1;
    }
    translate(magic, human, sizeof(human));
    printf("\n翻译: %s\n", human);
    return 0;
}
 