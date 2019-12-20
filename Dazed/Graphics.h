#pragma once
#include "Maths.h"

extern mat ModelView;
extern mat Projection;
extern mat Viewport;

void ViewPort(int x,int y,int w,int h);
void ProjectMatrix(float coeff = 0.f);
void LookAt(vec3f eye,vec3f center,vec3f up);

