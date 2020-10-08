#pragma once
#include"object3d.h"
#include"raytracing_stats.h"

class Plane : RenderObject3D {
public:
	Plane(const Vec3f& normal, float d, Material* m);

	bool intersect(const Ray& r, Hit& h, const float tmin) override;

	void paint() override;

	void insertIntoGrid(Grid* g, Matrix* m) override;
protected:
	const float c_nBig = 1e6;

	Vec3f m_vNormal;
	Vec3f m_vU, m_vV;
	float m_fD;
};