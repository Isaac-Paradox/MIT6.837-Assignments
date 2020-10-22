#include<GL/GL.h>
#include<array>
#include "curve.h"

void Curve::OutputBezier(FILE* file) {
	fprintf(file, "bezier\nnum_vertices %d\n", m_nNumVertices);
	const std::vector<Vec3f> control_point = GetBezierControlPoint();
	for (int i = 0; i < m_nNumVertices; i++) {
		const Vec3f& p = control_point[i];
		fprintf(file, "%f %f %f\n", p.x(), p.y(), p.z());
	}
}

void Curve::OutputBSpline(FILE* file) {
	fprintf(file, "bspline\nnum_vertices %d\n", m_nNumVertices);
	const std::vector<Vec3f> control_point = GetBSplineControlPoint();
	for (int i = 0; i < m_nNumVertices; i++) {
		const Vec3f& p = control_point[i];
		fprintf(file, "%f %f %f\n", p.x(), p.y(), p.z());
	}
}

void Curve::moveControlPoint(int selectedPoint, float x, float y) {
	m_vVertices[selectedPoint].Set(x, y, 0.f);
}

void Curve::addControlPoint(int selectedPoint, float x, float y) {
	m_vVertices.insert(m_vVertices.begin() + selectedPoint, Vec3f(x, y, 0.f));
	++m_nNumVertices;
}

void Curve::deleteControlPoint(int selectedPoint) {
	m_vVertices.erase(m_vVertices.begin() + selectedPoint);
	--m_nNumVertices;
}

void BezierCurve::Paint(ArgParser* args) {
	PaintControlLine(m_vVertices);

	std::vector<Vec3f> curve_points;
	GetCurvePoints(curve_points, args->curve_tessellation);
	PaintCurveLine(curve_points);
	
	PaintPoints(m_vVertices);
}

void BezierCurve::GetCurvePoints(std::vector<Vec3f>& curve_points, int tessellation) {
	for (int i = 0; i + 3 < m_nNumVertices; i += 3) {
		BezierCurve::GetWindowCurve({ m_vVertices[i], m_vVertices[i + 1], m_vVertices[i + 2], m_vVertices[i + 3] }
		, curve_points, tessellation);
	}
}

const std::vector<Vec3f> BezierCurve::GetBSplineControlPoint() {
	assert(m_nNumVertices == 4);
	std::vector<Vec3f> _out(m_nNumVertices);
	_out[0] = 6 * m_vVertices[0] - 7 * m_vVertices[1] + 2 * m_vVertices[2];
	_out[1] = 2 * m_vVertices[1] - m_vVertices[2];
	_out[2] = -1 * m_vVertices[1] + 2 * m_vVertices[2];
	_out[3] = 2 * m_vVertices[1] - 7 * m_vVertices[2] + 6 * m_vVertices[3];
	return _out;
}

void BezierCurve::GetWindowCurve(std::array<Vec3f, 4> control_points, 
	std::vector<Vec3f>& curve_points, 
	int tessellation) {
	float delta_t = 1.f / tessellation;
	float t = 0;
	for (int l = 0; l <= tessellation; l++) {
		std::array<Vec3f, 4> points = control_points;
		for (int i = 3; i > 0; i--) {
			for (int j = 0; j < i; j++) {
				points[j] = (1 - t) * points[j] + t * points[j + 1];
			}
		}
		curve_points.push_back(points[0]);
		t += delta_t;
	}
}

void BSplineCurve::Paint(ArgParser* args) {
	PaintControlLine(m_vVertices);

	std::vector<Vec3f> curve_points;
	GetCurvePoints(curve_points, args->curve_tessellation);
	PaintCurveLine(curve_points);

	PaintPoints(m_vVertices);
}

void BSplineCurve::GetCurvePoints(std::vector<Vec3f>& curve_points, int tessellation) {
	for (int i = 0; i + 3 < m_nNumVertices; i++) {
		_GetWindowCurve({ m_vVertices[i],m_vVertices[i + 1], m_vVertices[i + 2], m_vVertices[i + 3] }
		, curve_points, tessellation);
	}
}

const std::vector<Vec3f> BSplineCurve::GetBezierControlPoint() {
	assert(m_nNumVertices == 4);
	std::vector<Vec3f> _out(m_nNumVertices);
	float div = 1.f / 6.f;
	_out[0] = div * (m_vVertices[0] + 4 * m_vVertices[1] + m_vVertices[2]);
	_out[1] = div * (4 * m_vVertices[1] + 2 * m_vVertices[2]);
	_out[2] = div * (2 * m_vVertices[1] + 4 * m_vVertices[2]);
	_out[3] = div * (m_vVertices[1] + 4 * m_vVertices[2] + m_vVertices[3]);
	return _out;
}

void BSplineCurve::_GetWindowCurve(std::array<Vec3f, 4> control_points,
	std::vector<Vec3f>& curve_points,
	int tessellation) {
	float delta_t = 1.f / tessellation;
	float t = 0;
	for (int l = 0; l <= tessellation; l++) {
		float one_minus_t = 1 - t;
		float quadric_t = t * t, cubic_t = quadric_t * t;
		float a1 = one_minus_t * one_minus_t * one_minus_t,
			a2 = 3 * cubic_t - 6 * quadric_t + 4,
			a3 = -3 * cubic_t + 3 * quadric_t + 3 * t + 1,
			a4 = cubic_t;
		Vec3f curve_point = a1 * control_points[0] + 
			a2 * control_points[1] + 
			a3 * control_points[2] + 
			a4 * control_points[3];
		curve_point /= 6;
		curve_points.push_back(curve_point);
		t += delta_t;
	}
}
