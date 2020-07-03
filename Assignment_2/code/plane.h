#pragma once
#include"object3d.h"
class Plane : RenderObject3D {
public:
	Plane(const Vec3f& normal, float d, Material* m)
		: RenderObject3D(m), m_vNormal(normal), m_fD(-d) {
		//m_vNormal.Normalize();
	};

	bool intersect(const Ray& r, Hit& h, const float tmin) {
		float t = -(m_fD + m_vNormal.Dot3(r.getOrigin())) / m_vNormal.Dot3(r.getDirection());
		
		if (t >= tmin && t < h.getT()) {
			h.set(t, m_pMaterial, m_vNormal, r);
			return true;
		}
		return false;
	}

public:
	Vec3f m_vNormal;
	float m_fD;
};