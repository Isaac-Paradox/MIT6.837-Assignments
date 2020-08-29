#include"sphere.h"

uint32_t Sphere::s_nThetaSteps = 20;
uint32_t Sphere::s_nPhiSteps = 20;
float Sphere::s_fThetaDelta = (M_PI * 2) / s_nThetaSteps;
float Sphere::s_fPhiDelta = M_PI / s_nPhiSteps;
bool Sphere::s_bGouraud = false;

Sphere::Sphere(Vec3f center, float radius, Material* material)
	: m_vCenter(center), m_fRadius(radius), RenderObject3D(material) {
	Vec3f _max(m_vCenter.x() + m_fRadius,
		m_vCenter.y() + m_fRadius,
		m_vCenter.z() + m_fRadius);
	Vec3f _min(m_vCenter.x() - m_fRadius,
		m_vCenter.y() - m_fRadius,
		m_vCenter.z() - m_fRadius);

	m_pBoundingBox = new BoundingBox(_min, _max);
}

bool Sphere::intersect(const Ray& r, Hit& h, const float tmin) {
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

void Sphere::paint() {
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

void Sphere::insertIntoGrid(Grid* g, Matrix* m) {
	if (m) {
		Object3D::insertIntoGrid(g, m);
	} else {
		const Vec3f& grid_min = g->getBoundingBox()->getMin();
		const Vec3f& voxel_size = g->GetVoxelSize();
		float half_diagonal = 0.5f * voxel_size.Length();
		Indexs size = g->GetGridNum();
		float center_x = 0.5f * voxel_size.x();
		for (int i = 0; i < size[0]; i++, center_x += voxel_size.x()) {
			float center_y = 0.5f * voxel_size.y();
			for (int j = 0; j < size[1]; j++, center_y += voxel_size.y()) {
				float center_z = 0.5f * voxel_size.z();
				for (int k = 0; k < size[2]; k++, center_z += voxel_size.z()) {
					if ((grid_min + Vec3f(center_x, center_y, center_z) - m_vCenter).Length() < m_fRadius + half_diagonal) {
						g->InsertObjectIntoVoxel({ i, j, k }, this);
					}
				}
			}
		}
	}
}

void Sphere::SetSphereSteps(uint32_t theta_steps, uint32_t phi_steps) {
	s_nThetaSteps = theta_steps;
	s_nPhiSteps = phi_steps;
	s_fThetaDelta = (M_PI * 2) / s_nThetaSteps;
	s_fPhiDelta = M_PI / s_nPhiSteps;
}

BoundingBox Sphere::GetTransformedBoundingBox(Matrix* m) {
	Vec3f transformed_center = m_vCenter;
	m->Transform(transformed_center);
	BoundingBox bb(transformed_center, transformed_center);
	Vec3f offset(m_fRadius, m_fRadius, m_fRadius);
	m->TransformDirection(offset);
	bb.Extend(transformed_center + offset);
	bb.Extend(transformed_center - offset);
	return bb;
}
