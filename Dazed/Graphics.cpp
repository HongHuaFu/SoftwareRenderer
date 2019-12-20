#include "Graphics.h"

mat ModelView;
mat Viewport;
mat Projection;

void ViewPort(int x,int y,int w,int h)
{
	Viewport = mat::identity(4);
	Viewport[0][3] = x+w/2.f;
	Viewport[1][3] = y+h/2.f;
	Viewport[2][3] = 1.f;
	Viewport[0][0] = w/2.f;
	Viewport[1][1] = h/2.f;
	Viewport[2][2] = 0;
}

void ProjectMatrix(float coeff) {
	Projection = mat::identity(4);
	Projection[3][2] = coeff;
}

void LookAt(vec3f eye,vec3f center,vec3f up) {
	vec3f z = (eye-center).normalize();
	vec3f x = (up^z).normalize();
	vec3f y = (z^x).normalize();
	mat Minv = mat::identity(4);
	mat Tr   = mat::identity(4);
	for (int i=0; i<3; i++) {
		Minv[0][i] = x.raw[i];
		Minv[1][i] = y.raw[i];
		Minv[2][i] = z.raw[i];
		Tr[i][3] = -center.raw[i];
	}
	ModelView = Minv*Tr;
}