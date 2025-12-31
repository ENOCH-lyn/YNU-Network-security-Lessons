#include <stdio.h>
#include "cpxNum.h"
#include <windows.h>
int main()
{
    SetConsoleOutputCP(65001);
    cpxNum c1, c2, result;
    double real, imag;

    printf("----------------------------------------\n");
    printf("         复数运算程序\n");
    printf("----------------------------------------\n");

    printf("请输入第一个复数的实部和虚部 (以空格分隔): ");
    scanf("%lf %lf", &real, &imag);
    assign(&c1, real, imag);
    printf("您生成的第一个复数是: ");print(&c1);printf("\n\n");
    printf("请输入第二个复数的实部和虚部 (以空格分隔): ");
    scanf("%lf %lf", &real, &imag);
    assign(&c2, real, imag);
    printf("您生成的第二个复数是: ");print(&c2);printf("\n");
    printf("\n*********** 复数运算测试 *************\n");
    // 求和
    result = cplus(&c1, &c2);
    printf("c1 + c2 的结果是: ");print(&result);printf("\n");
    // 求差
    result = cminus(&c1, &c2);
    printf("c1 - c2 的结果是: ");print(&result);printf("\n");
    // 求积
    result = cmultiply(&c1, &c2);
    printf("c1 * c2 的结果是: ");print(&result);printf("\n");
    // 求商
    result = cdivide(&c1, &c2);
    printf("c1 / c2 的结果是: ");print(&result);printf("\n");
    return 0;
}