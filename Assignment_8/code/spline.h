#pragma once
#include"vectors.h"
#include"arg_parser.h"
#include"triangle_mesh.h"
#include<vector>


class Spline {
public:
	// FOR VISUALIZATION
	virtual void Paint(ArgParser* args) = 0;

	// FOR CONVERTING BETWEEN SPLINE TYPES
	virtual void OutputBezier(FILE* file) = 0;
	virtual void OutputBSpline(FILE* file) = 0;

	// FOR CONTROL POINT PICKING
	virtual int getNumVertices() = 0;
	virtual Vec3f getVertex(int i) = 0;

	// FOR EDITING OPERATIONS
	virtual void moveControlPoint(int selectedPoint, float x, float y) = 0;
	virtual void addControlPoint(int selectedPoint, float x, float y) = 0;
	virtual void deleteControlPoint(int selectedPoint) = 0;

	// FOR GENERATING TRIANGLES
	virtual TriangleMesh* OutputTriangles(ArgParser* args) = 0;
protected:
	void PaintPoints(const std::vector<Vec3f>& control_points);

	void PaintCurveLine(const std::vector<Vec3f>& curve_points);

	void PaintControlLine(const std::vector<Vec3f>& control_points);
protected:
	const static float c_fPointSize;
	const static float c_fLineWidth;
	const static float c_fCurveWidth;
};