#pragma once
#include"spline.h"
#include"curve.h"
#include<array>
#include<vector>

class Surface : public Spline {
public:

};

class SurfaceOfRevolution : public Surface {
public:
	SurfaceOfRevolution(Curve* curve);

	void Paint(ArgParser* args) override { m_pCurve->Paint(args); }

	void OutputBezier(FILE* file) override { fprintf(file, "surface_of_revolution\n"); m_pCurve->OutputBezier(file); }
	void OutputBSpline(FILE* file) override { fprintf(file, "surface_of_revolution\n"); m_pCurve->OutputBSpline(file); }

	int getNumVertices()  override { return m_pCurve->getNumVertices(); }
	Vec3f getVertex(int i) override { return m_pCurve->getVertex(i); }

	void moveControlPoint(int selectedPoint, float x, float y) override { m_pCurve->moveControlPoint(selectedPoint, x, y); }
	void addControlPoint(int selectedPoint, float x, float y)  override { m_pCurve->addControlPoint(selectedPoint, x, y); }
	void deleteControlPoint(int selectedPoint) override { m_pCurve->deleteControlPoint(selectedPoint); }

	virtual TriangleMesh* OutputTriangles(ArgParser* args);
protected:
	Curve* m_pCurve;
};

class BezierPatch : public Surface {
public:
	BezierPatch() {};

	void Paint(ArgParser* args) override;

	void OutputBezier(FILE* file) override { assert(0); fprintf(file, "bezier_patch\n"); }
	void OutputBSpline(FILE* file) override { assert(0); }

	int getNumVertices()  override { return 16; }
	Vec3f getVertex(int i) override { return m_vControlPoint[i]; }

	void moveControlPoint(int selectedPoint, float x, float y) override { assert(0); }
	void addControlPoint(int selectedPoint, float x, float y)  override { assert(0); }
	void deleteControlPoint(int selectedPoint) override { assert(0); }

	virtual TriangleMesh* OutputTriangles(ArgParser* args);

	void set(int i, const Vec3f& v) { m_vControlPoint[i] = v; }
protected:
	std::array<Vec3f, 16> m_vControlPoint;
};