#include "advancedConsole.h"
#include <unistd.h>
#include <stdio.h>
#include "imath.h"
#include <math.h>

struct vec3d {
	public:
		vec3d(double x0, double y0, double z0,
		      double x1, double y1, double z1)
		{
			this->x0 = x0; this->y0 = y0; this->z0 = z0;
			this->x1 = x1; this->y1 = y1; this->z1 = z1;
		}
		double x0, y0, z0;
		double x1, y1, z1;
};

void squaredemo(double camx, double camy, double camz) {
	const vec3d square[12]={vec3d(1,1,1,  1,3,1),
				vec3d(1,1,1,  3,1,1),
				vec3d(1,1,1,  1,1,3),
				vec3d(3,3,1,  3,1,1),
				vec3d(3,3,1,  1,3,1),
				vec3d(3,3,1,  3,3,3),
				vec3d(3,1,3,  3,1,1),
				vec3d(3,1,3,  1,1,3),
				vec3d(3,1,3,  3,3,3),
				vec3d(1,3,3,  1,3,1),
				vec3d(1,3,3,  1,1,3),
				vec3d(1,3,3,  3,3,3)};

	const vec3d square2[12]={vec3d(1,1,1,  1,3,1),
				vec3d(1,1,1,  3,1,1),
				vec3d(1,1,1,  1,1,3),
				vec3d(3,3,1,  3,1,1),
				vec3d(3,3,1,  1,3,1),
				vec3d(3,3,1,  3,3,3),
				vec3d(3,1,3,  3,1,1),
				vec3d(3,1,3,  1,1,3),
				vec3d(3,1,3,  3,3,3),
				vec3d(1,3,3,  1,3,1),
				vec3d(1,3,3,  1,1,3),
				vec3d(1,3,3,  3,1,3)};

	const double fov = rad(90.0d);
	const double hFov = fov / 2.0d;
	//const double hFov = fov;
	const double tanFov = tan(hFov);
	
	//const double width = double(CONSOLEWIDTH-4) / 2;
	//const double height = double(CONSOLEHEIGHT-4);
	double width = (adv::width / 4.0d) - 4.0d;
	double height = adv::height - 4.0d;

	double x_trans0[12] = {0.0d}, y_trans0[12] = {0.0d},
	       x_trans1[12] = {0.0d}, y_trans1[12] = {0.0d};
	
	vec3d* cur;
	
	//Translate the x via the z value
	for (int i = 0; i < 12; i++) {
		cur = (vec3d*)&square[i];
		double oppo0 = tanFov * fabs(cur->z0 - camz);
		double oppo1 = tanFov * fabs(cur->z1 - camz);
		double length0 = oppo0 * 2;
		double length1 = oppo1 * 2;
		double minx0 = camx - oppo0;
		double maxx0 = camx + oppo0;
		double minx1 = camx - oppo1;
		double maxx1 = camx + oppo1;
		double delta0 = cur->x0 - minx0;
		double delta1 = cur->x1 - minx1;
		double scale0 = delta0 / length0;
		double scale1 = delta1 / length1;
		x_trans0[i] = scale0 * width;
		x_trans1[i] = scale1 * width;
	}
	//Translate the y via the z value
	for (int i = 0; i < 12; i++) {
		cur = (vec3d*)&square[i];
		double oppo0 = tanFov * fabs(cur->z0 - camz);
		double oppo1 = tanFov * fabs(cur->z1 - camz);
		double length0 = oppo0 * 2;
		double length1 = oppo1 * 2;
		double miny0 = camy - oppo0;
		double maxy0 = camy + oppo0;
		double miny1 = camy - oppo1;
		double maxy1 = camy + oppo1;
		double delta0 = cur->y0 - miny0;
		double delta1 = cur->y1 - miny1;
		double scale0 = delta0 / length0;
		double scale1 = delta1 / length1;
		y_trans0[i] = scale0 * height;
		y_trans1[i] = scale1 * height;
	}
	//Raster
	adv::clear();
	for (int i = 0; i < 12; i++) {
		adv::line(x_trans0[i] + 1, y_trans0[i] + 2, x_trans1[i] + 1, y_trans1[i] + 2, L' ', BBLUE | FWHITE);
	}
	adv::draw();

}

int main(int argc, char** argv) {
	adv::setDrawingMode(DRAWINGMODE_COMPARE);
	adv::setDoubleWidth(true);
	adv::setThreadState(false);

	int key = 0;
	double cmx = 2.0d, cmy = 2.0d, cmz = 0.0d;
	while ((key = console::readKey()) != 27) {
		switch(key) {
			case 'W':
			case 'w':
			case KEY_UP:
				cmz+=0.1d;
				break;
			case 'd':
			case 'D':
			case KEY_LEFT:
				cmx+=0.1d;
				break;
			case 'S':
			case 's':
			case KEY_DOWN:
				cmz-=0.1d;
				break;
			case 'A':
			case 'a':
			case KEY_RIGHT:
				cmx-=0.1d;
				break;
		}
		squaredemo(cmx, cmy, cmz);
	}
}
