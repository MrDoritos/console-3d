#include "advancedConsole.h"
#include <unistd.h>
#include <stdio.h>
#include "imath.h"
#include <math.h>
#include <vector>
#include <fstream>
#include <string>
#include <strstream>

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
static bool showTriangles;

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
				//x
				if (r.x != 0.0d) {
					tri.p[i] -= origin;
					vec3d old = tri.p[i];
					tri.p[i].y = old.y * cos(rad(r.x)) - old.z * sin(rad(r.x));
					tri.p[i].z = old.y * sin(rad(r.x)) + old.z * cos(rad(r.x));
					tri.p[i] += origin;
				}
				//y
				if (r.y != 0.0d) {
					tri.p[i] -= origin;
					vec3d old = tri.p[i];
					tri.p[i].x = old.x * cos(rad(r.y)) + old.z * sin(rad(r.y));
					tri.p[i].z = -old.x * sin(rad(r.y)) + old.z * cos(rad(r.y));
					tri.p[i] += origin;
				}
				//z
				if (r.z != 0.0d) {
					tri.p[i] -= origin;
					vec3d old = tri.p[i];
					tri.p[i].x = old.x * cos(rad(r.z)) - old.y * sin(rad(r.z));
					tri.p[i].y = old.x * sin(rad(r.z)) + old.y * cos(rad(r.z));
					tri.p[i] += origin;
				}								
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
	double width;
	if (console::getImage() == IMAGE_LINUX)
		width = (adv::width / 4.0d) - 4.0d;
	else
		width = adv::width - 4.0d;//(adv::width / 2.0d) - 4.0d;
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
		translation.p[0].x = scale0 * width;
		translation.p[1].x = scale1 * width;
		translation.p[2].x = scale2 * width;
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
		translation.p[0].y = scale0 * height;
		translation.p[1].y = scale1 * height;
		translation.p[2].y = scale2 * height;
	}

	/*
	if (dist(translation.p[0].x, translation.p[0].y, translation.p[1].x, translation.p[1].y) > 1000 ||
		dist(translation.p[1].x, translation.p[1].y, translation.p[2].x, translation.p[2].y) > 1000 ||
		dist(translation.p[2].x, translation.p[2].y, translation.p[1].x, translation.p[1].y) > 1000);
		return;
	*/
	
	//z depth change
	const char zdepth[] = "#+-. ";
	
	//Wireframe while debugging
	//if (adv::bound(translation.p[0].x, translation.p[0].y) &&
	//	adv::bound(translation.p[1].x, translation.p[1].y) &&
	//	adv::bound(translation.p[2].x, translation.p[2].y)) {
		if (showTriangles && (camz - tri->p[0].z) < 20) {
			char c = zdepth[(int)floor(((camz - tri->p[0].z) / 20) * sizeof(zdepth))];
			char color = FWHITE | BBLACK;
			adv::line(translation.p[0].x, translation.p[0].y, translation.p[1].x, translation.p[1].y, c, color);
			adv::line(translation.p[1].x, translation.p[1].y, translation.p[2].x, translation.p[2].y, c, color);
			adv::line(translation.p[2].x, translation.p[2].y, translation.p[0].x, translation.p[0].y, c, color);
		} else {
			adv::triangle(translation.p[0].x,translation.p[0].y,translation.p[1].x,translation.p[1].y,translation.p[2].x,translation.p[2].y);
		}
	//}
}

bool inView(const triangle& tri) {
	for (int i = 0; i < 3; i++) {
		if (tri.p[i].z > camz)
			return false;
	}
	return true;
}

void rasterTriangles(const triangle* tri, int length) {
	adv::clear();
	for (int i = 0; i < length; i++)
		if (inView(tri[i]))
			rasterTriangle(&tri[i]);
	adv::draw();
}

void renderMesh(mesh* mesh) {
	//z depth sort
	std::sort(mesh->tris.begin(), mesh->tris.end(), [](triangle& a, triangle& b)
	{
		return (a.p[0].z < b.p[0].z);
	});
	for (auto t : mesh->tris)
		if (inView(t))
			rasterTriangle(&t);
}

bool loadObject(const char* file, mesh& mesh) {
	std::ifstream f(file);
	if (!f.is_open())
		return false;
	
		std::vector<vec3d> verts;
	while (!f.eof()) {
			char line[128];
			f.getline(line, 128);

			std::strstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
					vec3d v;
					s >> junk >> v.x >> v.y >> v.z;
					verts.push_back(v);
			}

				if (line[0] == 'f')
				{
					int f[3];
					s >> junk >> f[0] >> f[1] >> f[2];
					mesh.tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
				}
	}
return true;	
}

int main(int argc, char** argv) {
	mesh* currentMesh;
	mesh load;
	if (argc < 2 || !loadObject(argv[1], load))
		currentMesh = &cube;
	else
		currentMesh = &load;
	
	while (!adv::ready)
		console::sleep(50);
	if (console::getImage() == IMAGE_LINUX) {
		adv::setDrawingMode(DRAWINGMODE_COMPARE);
		adv::setDoubleWidth(true);
	}
	adv::setThreadState(false);
	currentMesh->translate(vec3d({0.0d,0,-2.0d}));

	int key = 0;
	//double cmx = 2.0d, cmy = 2.0d, cmz = 0.0d;
	camx = 0.0d; camy = 0.0d; camz = 0.0d;
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
			case 'k':
				currentMesh->rotate(vec3d({0,10.0d,0}),vec3d({2,0,2}));
				break;
			case ';':
				break;
			case '0':
				showTriangles = !showTriangles;
				break;
		}
		//squaredemo(cmx, cmy, cmz);
		//rasterTriangles(&cube[0], 12);
		adv::clear();
		renderMesh(currentMesh);
		//cube.translate(vec3d({0.0d,1.0d,0.0d}));
		//currentMesh->rotate(vec3d({0.0d,00.0,5.0}),vec3d({0,0,0}));
		//cube.scale(vec3d({1.1d,1,1}),vec3d({0,0,0}));
		adv::draw();
		console::sleep(16);
	}
}
