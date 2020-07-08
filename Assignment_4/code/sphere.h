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
				Vec3f normal = (r.pointAtParameter(t) - m_vCenter) * (1.0f / m_fRadius);
				normal.Normalize();
				h.set(t, m_pMaterial, normal, r);
				return true;
			}
			t = -hb + delta;
			if (t >= tmin && t < h.getT()) {
				Vec3f normal = (r.pointAtParameter(t) - m_vCenter) * (1.0f / m_fRadius);
				normal.Normalize();
				h.set(t, m_pMaterial, normal, r);
				return true;
			}
		}
		return false;
	}

	void paint() override {
		RenderObject3D::paint();
		glBegin(GL_QUADS);
		float phi = M_PI * 0.5f, nphi = phi - s_fPhiDelta;
		float theta, ntheta;
		if (s_bGouraud) {
			for (int iPhi = 0; iPhi < s_nPhiSteps; ++iPhi, phi = nphi, nphi -= s_fPhiDelta) {
				theta = 0, ntheta = theta + s_fThetaDelta;
				for (int iTheta = 0; iTheta < s_nThetaSteps; ++iTheta, theta = ntheta, ntheta += s_fThetaDelta) {
					Vec3f normal = _GetPoint(theta, phi);
					Vec3f vertex = m_vCenter + normal;
					normal.Normalize();
					glNormal3f(normal.x(), normal.y(), normal.z());
					glVertex3f(vertex.x(), vertex.y(), vertex.z());
					normal = _GetPoint(ntheta, phi);
					vertex = m_vCenter + normal;
					normal.Normalize();
					glNormal3f(normal.x(), normal.y(), normal.z());
					glVertex3f(vertex.x(), vertex.y(), vertex.z());
					normal = _GetPoint(ntheta, nphi);
					vertex = m_vCenter + normal;
					normal.Normalize();
					glNormal3f(normal.x(), normal.y(), normal.z());
					glVertex3f(vertex.x(), vertex.y(), vertex.z());
					normal = _GetPoint(theta, nphi);
					vertex = m_vCenter + normal;
					normal.Normalize();
					glNormal3f(normal.x(), normal.y(), normal.z());
					glVertex3f(vertex.x(), vertex.y(), vertex.z());
				}
			}
		} else {
			for (int iPhi = 0; iPhi < s_nPhiSteps; ++iPhi, phi = nphi, nphi -= s_fPhiDelta) {
				float min_phi = phi - s_fPhiDelta * 0.5f;
				theta = 0, ntheta = theta + s_fThetaDelta;
				for (int iTheta = 0; iTheta < s_nThetaSteps; ++iTheta, theta = ntheta, ntheta += s_fThetaDelta) {
					float min_theta = theta + s_fThetaDelta * 0.5f;
					Vec3f normal = _GetPoint(min_theta, min_phi);
					glNormal3f(normal.x(), normal.y(), normal.z());
					Vec3f vertex = _GetPoint(theta, phi);
					vertex += m_vCenter;
					glVertex3f(vertex.x(), vertex.y(), vertex.z());
					vertex = _GetPoint(ntheta, phi);
					vertex += m_vCenter;
					glVertex3f(vertex.x(), vertex.y(), vertex.z());
					vertex = _GetPoint(ntheta, nphi);
					vertex += m_vCenter;
					glVertex3f(vertex.x(), vertex.y(), vertex.z());
					vertex = _GetPoint(theta, nphi);
					vertex += m_vCenter;
					glVertex3f(vertex.x(), vertex.y(), vertex.z());
				}
			}
		}
		glEnd();
	}

	static void SetSteps(uint32_t theta_steps, uint32_t phi_steps) {
		s_nThetaSteps = theta_steps;
		s_nPhiSteps = phi_steps;
		s_fThetaDelta = (M_PI * 2) / s_nThetaSteps;
		s_fPhiDelta = M_PI / s_nPhiSteps;
	}

protected:
	inline Vec3f _GetPoint(float theta, float phi) {
		float cosp = cos(phi);
		return m_fRadius * Vec3f(cosp * cos(theta), sin(phi), cosp * sin(theta));
	}

public:
	static bool s_bGouraud;

protected:
	Vec3f m_vCenter;
	float m_fRadius;

	static uint32_t s_nThetaSteps, s_nPhiSteps;
	static float s_fThetaDelta, s_fPhiDelta;
};

//uint32_t Sphere::s_nThetaSteps = 5;
//uint32_t Sphere::s_nPhiSteps = 5;
//float Sphere::s_fThetaDelta = 0;
//float Sphere::s_fPhiDelta = 0;