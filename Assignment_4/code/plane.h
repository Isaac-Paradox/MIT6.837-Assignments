#pragma once
#include"object3d.h"
class Plane : RenderObject3D {
public:
	Plane(const Vec3f& normal, float d, Material* m)
		: RenderObject3D(m), m_vNormal(normal), m_fD(-d) {
		m_vNormal.Normalize();
		if (m_vNormal.x() == 1) {
			Vec3f::Cross3(m_vU, m_vNormal, Vec3f(0, 1, 0));
		} else {
			Vec3f::Cross3(m_vU, m_vNormal, Vec3f(1, 0, 0));
		}
		m_vU.Normalize();
		Vec3f::Cross3(m_vV, m_vNormal, m_vU);
		m_vV.Normalize();
		m_vU *= c_nBig;
		m_vV *= c_nBig;
	};

	bool intersect(const Ray& r, Hit& h, const float tmin) {
		float t = -(m_fD + m_vNormal.Dot3(r.getOrigin())) / m_vNormal.Dot3(r.getDirection());
		
		if (t >= tmin && t < h.getT()) {
			if (r.getDirection().Dot3(m_vNormal) > 0) {
				h.set(t, m_pMaterial, -1.0f * m_vNormal, r);
			} else {
				h.set(t, m_pMaterial, m_vNormal, r);
			}
			return true;
		}
		return false;
	}

	void paint() {
		RenderObject3D::paint();
		glBegin(GL_QUADS);
		glNormal3f(m_vNormal.x(), m_vNormal.y(), m_vNormal.z());
		//glVertex3f(m_vU.x() + m_vV.x(), m_vU.y() + m_vV.y(), m_vU.z() + m_vV.z());
		//glVertex3f(m_vU.x() - m_vV.x(), m_vU.y() - m_vV.y(), m_vU.z() - m_vV.z());
		//glVertex3f(-m_vU.x() - m_vV.x(), -m_vU.y() - m_vV.y(), -m_vU.z() - m_vV.z());
		//glVertex3f(-m_vU.x() + m_vV.x(), -m_vU.y() + m_vV.y(), -m_vU.z() + m_vV.z());
		Vec3f vertex = (m_vU + m_vV) - (m_fD * m_vNormal);
		glVertex3f(vertex.x(), vertex.y(), vertex.z());
		vertex = (m_vU - m_vV) - (m_fD * m_vNormal);
		glVertex3f(vertex.x(), vertex.y(), vertex.z());
		vertex = (-1 * (m_vU + m_vV)) - (m_fD * m_vNormal);
		glVertex3f(vertex.x(), vertex.y(), vertex.z());
		vertex = ((-1 * m_vU) + m_vV) - (m_fD * m_vNormal);
		glVertex3f(vertex.x(), vertex.y(), vertex.z());
		glEnd();
	}

protected:
	const float c_nBig = 1e6;

	Vec3f m_vNormal;
	Vec3f m_vU, m_vV;
	float m_fD;
};