#include <cmath>

#define PI 3.1415926
#define D_PI 6.2831853

#define DIST__ (sqrt(((x1 - x0) * (x1 - x0)) + ((y1 - y0) * (y1 - y0))))
#define RAD__(xs) (xs * PI / 180)
#define DEG__(xs) (xs * 180 / PI)

double dist(double x0, double y0, double x1, double y1) {
	return DIST__;
}

float distf(float x0, float y0, float x1, float y1) {
	return DIST__;
}

double rad(double deg) {
	return RAD__(deg);
}

float radf(float deg) {
	return RAD__(deg);
}

double deg(double rad) {
	return DEG__(rad);
}

float degf(float rad) {
	return DEG__(rad);
}

double wrap(double p, double m) {
	while (p >= m) {
		p -= m;
	}
	
	while (p < 0) {
		p += m;
	}
	
	return p;
}

#undef DIST__
#undef RAD__
#undef DEG__