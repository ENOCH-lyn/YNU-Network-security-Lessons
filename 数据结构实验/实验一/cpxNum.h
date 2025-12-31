#ifndef CPXNUM_H
#define CPXNUM_H

typedef struct {
    double _real; // 实部
    double _imag; // 虚部
} cpxNum;
void assign(cpxNum* c, double r, double i);
double getReal(const cpxNum* c);
double getImag(const cpxNum* c);
void print(const cpxNum* c);
cpxNum cplus(const cpxNum* c1, const cpxNum* c2);
cpxNum cminus(const cpxNum* c1, const cpxNum* c2);
cpxNum cmultiply(const cpxNum* c1, const cpxNum* c2);
cpxNum cdivide(const cpxNum* c1, const cpxNum* c2);
#endif