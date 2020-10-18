#pragma once
#include<cmath>
#include<array>
#include"object3d.h"

#define DETERMINANT(a11, a12, a13, a21, a22, a23, a31, a32, a33) (a11 * a22 * a33 + a12 * a23 * a31 + a13 * a21 * a32 - a13 * a22 * a31 - a12 * a21 * a33 - a11 * a23 * a32)

class Triangle : public RenderObject3D {
public:
	Triangle(Vec3f& a, Vec3f& b, Vec3f& c, Material* m);

	void paint() override;

	bool intersect(const Ray& r, Hit& h, const float tmin);

	BoundingBox GetTransformedAABB(const Matrix& m) override;

protected:

	BoundingBox _GetTriangleBoundingBox(const Vec3f& _a, const Vec3f& _b, const Vec3f& _c);

protected:
	Vec3f m_vNormal;

	Vec3f m_vA;

	Vec3f m_vB;

	Vec3f m_vC;
};