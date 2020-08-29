#pragma once
#include"vectors.h"
#include"object3d.h"
#include"Grid.h"

#include<cassert>
#include<cmath>

#define M_PI 3.1416926f


class Sphere : public RenderObject3D {
public:
	Sphere(Vec3f center, float radius, Material* material);

	bool intersect(const Ray& r, Hit& h, const float tmin) override;

	void paint() override;

	void insertIntoGrid(Grid* g, Matrix* m) override;

	static void SetSphereSteps(uint32_t theta_steps, uint32_t phi_steps);

	BoundingBox GetTransformedBoundingBox(Matrix* m) override;
protected:
	inline Vec3f _GetPoint(float theta, float phi) { float cosp = cos(phi); return m_fRadius * Vec3f(cosp * cos(theta), sin(phi), cosp * sin(theta)); }


public:
	static bool s_bGouraud;

protected:
	Vec3f m_vCenter;
	float m_fRadius;

	static uint32_t s_nThetaSteps, s_nPhiSteps;
	static float s_fThetaDelta, s_fPhiDelta;
};