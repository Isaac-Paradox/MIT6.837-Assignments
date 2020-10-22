#include"spline.h"
#include<GL/GL.h>

const float Spline::c_fPointSize = 5.f;
const float Spline::c_fLineWidth = 1.f;
const float Spline::c_fCurveWidth = 3.f;


void Spline::PaintPoints(const std::vector<Vec3f>& control_points) {
	glPointSize(c_fPointSize);
	glColor3f(1, 0, 0);
	glBegin(GL_POINTS); {
		for (const Vec3f& v : control_points) {
			glVertex2f(v.x(), v.y());
		}
	}
	glEnd();
}

void Spline::PaintControlLine(const std::vector<Vec3f>& control_points) {
	glLineWidth(c_fLineWidth);
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP); {
		for (const Vec3f& v : control_points) {
			glVertex2f(v.x(), v.y());
		}
	}glEnd();
}

void Spline::PaintCurveLine(const std::vector<Vec3f>& curve_points) {
	glLineWidth(c_fCurveWidth);
	glBegin(GL_LINE_STRIP); {
		glColor3f(0, 1, 0);
		for (const Vec3f& v : curve_points) {
			glVertex2f(v.x(), v.y());
		}
	}glEnd();
}