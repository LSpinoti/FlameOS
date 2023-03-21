#ifndef MATH_H
#define MATH_H

#define PI	3.14159
#define E	2.71828

static unsigned long __inf = (unsigned long) 0x7FF << 52;
static unsigned long __nan = (unsigned long) 0xFFFFFFFFFFFFFFFF;

#define INFINITY (*((double *) &__inf))
#define NAN (*((double *) &__nan))

int abs(int x);
double sqrt(double x);
double hypot(double x, double y);

#endif
