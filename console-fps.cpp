#include "advancedConsole.h"
#include <unistd.h>
#include <stdio.h>
#include "imath.h"
#include <math.h>
#include <vector>

struct vec3d {
	vec3d() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}
	vec3d(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	vec3d(double x0, double y0, double z0,
	      double x1, double y1, double z1) {
		this->x0 = x0; this->y0 = y0; this->z0 = z0;
		this->x1 = x1; this->y1 = y1; this->z1 = z1;
	}
	vec3d& operator+ (vec3d const & obj) {
		vec3d r;
		r.x = x + obj.x;
		r.y = y + obj.y;
		r.z = z + obj.z;
		return r;
	}
	vec3d& operator+=(vec3d const & obj) {
		x += obj.x;
		y += obj.y;
		z += obj.z;
		return *this;
	}
	vec3d& operator-=(vec3d const & obj) {
		x -= obj.x;
		y -= obj.y;
		z -= obj.z;
		return *this;
	}
	vec3d& operator*=(vec3d const & obj) {
		x *= obj.x;
		y *= obj.y;
		z *= obj.z;
		return *this;
	}
	vec3d operator- (vec3d const & obj) {
		vec3d r;
		r.x = x - obj.x;
		r.y = y - obj.y;
		r.z = z - obj.z;
		return r;
	}
	vec3d operator* (vec3d const & obj) {
		vec3d r;
		r.x = x * obj.x;
		r.y = y * obj.y;
		r.z = z * obj.z;
		return r;
	}
	vec3d operator/ (vec3d const & obj) {
		vec3d r;
		r.x = x / obj.x;
		r.y = y / obj.y;
		r.z = z / obj.z;
		return r;
	}


	double x, y, z;
	double x0, y0, z0;
	double x1, y1, z1;
};

struct triangle {
	triangle() {	}
	triangle(vec3d p0, vec3d p1, vec3d p2) {
		p[0] = p0;
		p[1] = p1;
		p[2] = p2;
	}
	vec3d p[3];
};

/*
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
*/

static double camx,camy,camz;

struct camera {
	vec3d position;
};

struct mesh {
	mesh() {
	
	}
	
	mesh(const triangle* tris, int length) {
		for (int i = 0; i < length; i++)
			this->tris.push_back(tris[i]);
	}

	std::vector<triangle> tris;
	
	void translate(vec3d t) {
		for (auto& tri : tris) {
			tri.p[0] += t;
			tri.p[1] += t;
			tri.p[2] += t;
		}
	}
	
	void scale(vec3d s, vec3d origin) {
		for (auto& tri : tris) {
			for (int i = 0; i < 3; i++) {
				tri.p[i] -= origin;
				tri.p[i] *= s;
				tri.p[i] += origin;
			}
		}
	}
	
	void rotate(vec3d r, vec3d origin) {
		for (auto& tri : tris) {
			//Upon x
			for (int i = 0; i < 3; i++) {
				//tri.p[i].x = ((tri.p[i].x - origin.x) * cos(rad(r.x))) - ((origin.y - tri.p[i].y) * sin(rad(r.x))) + origin.x;
				//tri.p[i].y = ((origin.y - tri.p[i].y) * cos(rad(r.x))) - ((tri.p[i].x - origin.x) * sin(rad(r.x))) + origin.y;
				//-sin(theta)x == cos(theta)y
				//tri.p[i].x = tri.p[i].x * cos(rad(r.x));
				//tri.p[i].y = tri.p[i].y * sin(rad(r.x));
				
				//Half working
				//tri.p[i].y = tri.p[i].y * cos(rad(r.x)) - tri.p[i].z * sin(rad(r.x));
				//tri.p[i].z = tri.p[i].y * sin(rad(r.x)) + tri.p[i].z * cos(rad(r.x));
				
				vec3d old = tri.p[i];
				
				tri.p[i].y = old.y * cos(rad(r.x)) - old.z * sin(rad(r.x));
				tri.p[i].z = old.y * sin(rad(r.x)) + old.z * cos(rad(r.x));
			}
		}
	}
};

const triangle triangles_cube[] = {
	triangle({vec3d(0,1,0),vec3d(1,1,1),vec3d(1,1,0)}),
	triangle({vec3d(0,1,0),vec3d(0,1,1),vec3d(1,1,1)}),

	triangle({vec3d(1,0,0),vec3d(0,0,1),vec3d(0,0,0)}),
	triangle({vec3d(1,0,0),vec3d(1,0,1),vec3d(0,0,1)}),

	triangle({vec3d(0,1,0),vec3d(1,0,0),vec3d(0,0,0)}),
	triangle({vec3d(0,1,0),vec3d(1,1,0),vec3d(1,0,0)}),

	triangle({vec3d(1,1,1),vec3d(0,0,1),vec3d(1,0,1)}),
	triangle({vec3d(1,1,1),vec3d(0,1,1),vec3d(0,0,1)}),

	triangle({vec3d(0,1,1),vec3d(0,1,0),vec3d(0,0,0)}),
	triangle({vec3d(0,1,1),vec3d(0,0,0),vec3d(0,0,1)}),

	triangle({vec3d(1,1,0),vec3d(1,1,1),vec3d(1,0,1)}),
	triangle({vec3d(1,1,0),vec3d(1,0,1),vec3d(1,0,0)}),
};

static mesh cube = mesh(&triangles_cube[0], 12);

void rasterTriangle(const triangle* tri) {
	triangle translation; //Only store the x and y of the triangle here for raster

	const double fov = rad(90.0d);
	const double hFov = fov / 2.0d;
	const double tanFov = tan(hFov);
	double width = (adv::width / 4.0d) - 4.0d;
	double height = adv::height - 4.0d;

	//Translate the x via the z value
	{
		double oppo0 = tanFov * fabs(tri->p[0].z - camz);
		double oppo1 = tanFov * fabs(tri->p[1].z - camz);
		double oppo2 = tanFov * fabs(tri->p[2].z - camz);
		double length0 = oppo0 * 2;
		double length1 = oppo1 * 2;
		double length2 = oppo2 * 2;
		double minx0 = camx - oppo0;
		double maxx0 = camx + oppo0;
		double minx1 = camx - oppo1;
		double maxx1 = camx + oppo1;
		double minx2 = camx - oppo2;
		double maxx2 = camx + oppo2;
		double delta0 = tri->p[0].x - minx0;
		double delta1 = tri->p[1].x - minx1;
		double delta2 = tri->p[2].x - minx2;
		double scale0 = delta0 / length0;
		double scale1 = delta1 / length1;
		double scale2 = delta2 / length2;
		//x_trans0[i] = scale0 * width;
		translation.p[0].x = scale0 * width;
		translation.p[1].x = scale1 * width;
		translation.p[2].x = scale2 * width;
		//x_trans1[i] = scale1 * width;
		//x_trans2[i] = scale2 * width;
	}
	
	//Translate the y via the z value
	{
		double oppo0 = tanFov * fabs(tri->p[0].z - camz);
		double oppo1 = tanFov * fabs(tri->p[1].z - camz);
		double oppo2 = tanFov * fabs(tri->p[2].z - camz);
		double length0 = oppo0 * 2;
		double length1 = oppo1 * 2;
		double length2 = oppo2 * 2;
		double miny0 = camy - oppo0;
		double maxy0 = camy + oppo0;
		double miny1 = camy - oppo1;
		double maxy1 = camy + oppo1;
		double miny2 = camy - oppo2;
		double maxy2 = camy + oppo2;
		double delta0 = tri->p[0].y - miny0;
		double delta1 = tri->p[1].y - miny1;
		double delta2 = tri->p[2].y - miny2;
		double scale0 = delta0 / length0;
		double scale1 = delta1 / length1;
		double scale2 = delta2 / length2;
		//y_trans0[i] = scale0 * height;
		//y_trans1[i] = scale1 * height;
		translation.p[0].y = scale0 * height;
		translation.p[1].y = scale1 * height;
		translation.p[2].y = scale2 * height;
	}

	//Wireframe while debugging
	adv::line(translation.p[0].x, translation.p[0].y, translation.p[1].x, translation.p[1].y);
	adv::line(translation.p[1].x, translation.p[1].y, translation.p[2].x, translation.p[2].y);
	adv::line(translation.p[2].x, translation.p[2].y, translation.p[0].x, translation.p[0].y);
}

void rasterTriangles(const triangle* tri, int length) {
	adv::clear();
	for (int i = 0; i < length; i++)
		rasterTriangle(&tri[i]);
	adv::draw();
}

void renderMesh(const mesh* mesh) {
	for (auto t : mesh->tris)
		rasterTriangle(&t);
}

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
	cube.translate(vec3d({2.0d,0,-2.0d}));

	int key = 0;
	//double cmx = 2.0d, cmy = 2.0d, cmz = 0.0d;
	camx = 2.0d; camy = 2.0d; camz = 2.0d;
	while ((key = console::readKeyAsync()) != 27) {
		switch(key) {
			case 'W':
			case 'w':
			case VK_UP:
				camz-=0.1d;
				break;
			case 'd':
			case 'D':
			case VK_RIGHT:
				camx+=0.1d;
				break;
			case 'S':
			case 's':
			case VK_DOWN:
				camz+=0.1d;
				break;
			case 'A':
			case 'a':
			case VK_LEFT:
				camx-=0.1d;
				break;
			case 'l':
				camy+=0.1d;
				break;
			case 'o':
			case ' ':
				camy-=0.1d;
				break;
		}
		//squaredemo(cmx, cmy, cmz);
		//rasterTriangles(&cube[0], 12);
		adv::clear();
		renderMesh(&cube);
		//cube.translate(vec3d({0.0d,1.0d,0.0d}));
		cube.rotate(vec3d({10.0d,0,0}),vec3d({0,0,0}));
		//cube.scale(vec3d({1.1d,1,1}),vec3d({0,0,0}));
		adv::draw();
		console::sleep(33);
	}
}
