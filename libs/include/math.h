#ifndef MATH_H
#define MATH_H

#define PI	3.1415926535897932384626
#define E	2.7182818284590452353603

static unsigned long __inf = (unsigned long) 0x7FF << 52;
static unsigned long __nan = (unsigned long) 0xFFFFFFFFFFFFFFFF;

#define INFINITY (*((double *) &__inf))
#define NAN (*((double *) &__nan))

#define signbit(A) ((A) < 0)

int abs(int x);
double fabs(double x);
double fmod(double numer, double denom);
double sqrt(double x);
double cbrt(double x);
double exp(double x);
double log(double x);
double hypot(double x, double y);
double floor(double x);
double ceil(double x);
double trunc(double x);

double sin(double x);
double cos(double x);
double tan(double x);

#endif
