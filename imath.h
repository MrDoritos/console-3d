#pragma once

#define PI_SQ 9.86960440109
#define D_PI 6.28318530718
//#define M_PI 3.14159265359 /* USE M_PI in <math.h> */

double dist(double x0, double y0, double x1, double y1);
float distf(float x0, float y0, float x1, float y1);
double rad(double deg);
double deg(double rad);
float radf(float deg);
float degf(float rad);
double wrap(double p, double m);