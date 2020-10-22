#pragma once
#include<vector>

#include"spline.h"

class Curve : public Spline {
public:
	Curve(int num_vertices) : m_nNumVertices(num_vertices) { m_vVertices.resize(num_vertices); }

	int getNumVertices() override { return m_nNumVertices; }

	Vec3f getVertex(int i) { return m_vVertices[i]; }

	void set(int i, const Vec3f& v) { m_vVertices[i] = v; }

	void OutputBezier(FILE* file) override;

	void OutputBSpline(FILE* file) override;

	void moveControlPoint(int selectedPoint, float x, float y) override;

	void addControlPoint(int selectedPoint, float x, float y) override;

	void deleteControlPoint(int selectedPoint) override;

	TriangleMesh* OutputTriangles(ArgParser* args) override { return nullptr; }

	virtual void GetCurvePoints(std::vector<Vec3f>& curve_points, int tessellation) = 0;

protected:
	virtual const std::vector<Vec3f> GetBezierControlPoint() = 0;

	virtual const std::vector<Vec3f> GetBSplineControlPoint() = 0;
protected:
	int m_nNumVertices;

	std::vector<Vec3f> m_vVertices;
};

class BezierCurve : public Curve {
public:
	BezierCurve(int num_vertices) : Curve(num_vertices) {}

	void Paint(ArgParser* args) override;

	void GetCurvePoints(std::vector<Vec3f>& curve_points, int tessellation) override;

	void static GetWindowCurve(std::array<Vec3f, 4> control_points, std::vector<Vec3f>& curve_points, int tessellation);
protected:
	const std::vector<Vec3f> GetBezierControlPoint() override { return m_vVertices; }

	const std::vector<Vec3f> GetBSplineControlPoint() override;

	
};

class BSplineCurve : public Curve {
public:
	BSplineCurve(int num_vertices) : Curve(num_vertices) {}

	void Paint(ArgParser* args) override;

	void GetCurvePoints(std::vector<Vec3f>& curve_points, int tessellation) override;

protected:
	const std::vector<Vec3f> GetBezierControlPoint() override;

	const std::vector<Vec3f> GetBSplineControlPoint() override { return m_vVertices; };

	void _GetWindowCurve(std::array<Vec3f, 4> control_points, std::vector<Vec3f>& curve_points, int tessellation);
};