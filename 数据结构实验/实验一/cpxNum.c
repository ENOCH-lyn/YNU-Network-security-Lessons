#include "cpxNum.h"
#include <stdio.h>
#include <math.h>

// 初始化
void assign(cpxNum *c, double r, double i)
{
    c->_real = r;
    c->_imag = i;
}

// 获取实部
double getReal(const cpxNum *c)
{
    return c->_real;
}

// 获取虚部
double getImag(const cpxNum *c)
{
    return c->_imag;
}

// 打印复数
void print(const cpxNum *c)
{
    if (c->_imag >= 0)
    {
        printf("%.2f + %.2fi", c->_real, c->_imag);
    }
    else
    {
        printf("%.2f - %.2fi", c->_real, -c->_imag);
    }
}

// 复数加法
cpxNum cplus(const cpxNum *c1, const cpxNum *c2)
{
    cpxNum result;
    result._real = c1->_real + c2->_real;
    result._imag = c1->_imag + c2->_imag;
    return result;
}

// 复数减法
cpxNum cminus(const cpxNum *c1, const cpxNum *c2)
{
    cpxNum result;
    result._real = c1->_real - c2->_real;
    result._imag = c1->_imag - c2->_imag;
    return result;
}

// 复数乘法
cpxNum cmultiply(const cpxNum *c1, const cpxNum *c2)
{
    cpxNum result;
    result._real = c1->_real * c2->_real - c1->_imag * c2->_imag;
    result._imag = c1->_real * c2->_imag + c1->_imag * c2->_real;
    return result;
}

// 复数除法
cpxNum cdivide(const cpxNum *c1, const cpxNum *c2)
{
    cpxNum result;
    double denominator = c2->_real * c2->_real + c2->_imag * c2->_imag;

    if (fabs(denominator) < 1e-9)
    {
        printf("\n错误：除数不能为0！\n");
        result._real = 0;
        result._imag = 0;
    }
    else
    {
        result._real = (c1->_real * c2->_real + c1->_imag * c2->_imag) / denominator;
        result._imag = (c1->_imag * c2->_real - c1->_real * c2->_imag) / denominator;
    }
    return result;
}