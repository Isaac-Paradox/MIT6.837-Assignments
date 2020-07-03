#pragma once
#include"vectors.h"
#include"object3d.h"
#include<cassert>

#define M_PI 3.1416926f

class Sphere : public RenderObject3D {
public:
	Sphere(Vec3f center, float radius, Material* material)
		: m_vCenter(center), m_fRadius(radius), RenderObject3D(material) {
		assert(m_pMaterial);
	};

	bool intersect(const Ray& r, Hit& h, const float tmin) {
		Vec3f ro = r.getOrigin() - m_vCenter;
		float hb = r.getDirection().Dot3(ro);
		float c = ro.Dot3(ro) - m_fRadius * m_fRadius;
		float delta = hb * hb - c;
		if (delta > 0) {
			delta = sqrtf(delta);
			float t = -hb - delta;
			if (t >= tmin && t < h.getT()) {	
				h.set(t, m_pMaterial, (r.pointAtParameter(t) - m_vCenter) * (1.0f / m_fRadius), r);
				return true;
			}
			t = -hb + delta;
			if (t >= tmin && t < h.getT()) {
				Vec3f normal = (r.pointAtParameter(t) - m_vCenter) * (1.0f / m_fRadius);
				h.set(t, m_pMaterial, normal, r);
				
				return true;
			}
		}
		return false;
	}

protected:
	Vec3f m_vCenter;
	float m_fRadius;
};