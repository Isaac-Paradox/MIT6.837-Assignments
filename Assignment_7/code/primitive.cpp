#include"transform.h"
#include"sphere.h"
#include"plane.h"
#include"triangle.h"
#include"Grid.h"

/// <summary>
/// ///Transform
/// </summary>

Transform::Transform(const Matrix& m, Object3D* o)
	: m_mM2WMatrix(m), m_pObject(o), m_bBoundBox(Vec3f(),Vec3f()){
	m_mM2WMatrix.Inverse(m_mW2MMatrix);
	m_mW2MMatrix.Transpose(m_mNormalM2WMatrix);
	//m_mVectorM2WMatrix.Inverse(m_mVectorW2MMatrix);
	m_mM2WMatrix.Transpose(m_mNormalW2MMatrix);
	m_bBoundBox = _TransformBoundingBox(*m_pObject->getBoundingBox(), m_mM2WMatrix);
	m_pBoundingBox = &m_bBoundBox;
}

bool Transform::intersect(const Ray& r, Hit& h, const float tmin) {
	Vec3f ori = r.getOrigin();
	Vec3f dir = r.getDirection();
	Vec4f tdir = Vec4f(dir, 0);
	m_mW2MMatrix.Transform(ori);
	m_mW2MMatrix.Transform(tdir);
	dir.Set(tdir.x(), tdir.y(), tdir.z());
	float tScale = dir.Length();
	dir.Normalize();
	Ray tRay(ori, dir);
	Hit tHit(h.getT() * tScale, nullptr, Vec3f(0, 0, -1.0f));
	if (m_pObject->intersect(tRay, tHit, tmin * tScale)) {
		Vec3f normal = tHit.getNormal();
		m_mNormalM2WMatrix.Transform(normal);
		normal.Normalize();
		h.set(tHit.getT() / tScale, tHit.getMaterial(), normal, r);
		return true;
	}
	return false;
}

void Transform::paint() {
	glPushMatrix();
	GLfloat* glMatrix = m_mM2WMatrix.glGet();
	glMultMatrixf(glMatrix);
	delete[] glMatrix;
	m_pObject->paint();
	glPopMatrix();
}

void Transform::insertIntoGrid(Grid* g, Matrix* m) {
	Matrix obj_matrix = m_mM2WMatrix;
	if (m) { obj_matrix = (*m) * obj_matrix; }
	m_pObject->insertIntoGrid(g, &obj_matrix);
}

/// <summary>
/// Sphere
/// </summary>
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
	if (!RenderObject3D::intersect(r, h, tmin)) {
		return false;
	}
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

BoundingBox Sphere::GetTransformedAABB(const Matrix& m) {
	Vec3f transformed_center = m_vCenter;
	m.Transform(transformed_center);
	BoundingBox bb(transformed_center, transformed_center);
	Vec3f offset(m_fRadius, m_fRadius, m_fRadius);
	m.TransformDirection(offset);
	bb.Extend(transformed_center + offset);
	bb.Extend(transformed_center - offset);
	return bb;
}

/// <summary>
/// Plane
/// </summary>

Plane::Plane(const Vec3f& normal, float d, Material* m)
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
}

bool Plane::intersect(const Ray& r, Hit& h, const float tmin) {
	if (!RenderObject3D::intersect(r, h, tmin)) {
		return false;
	}
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

void Plane::paint() {
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

void Plane::insertIntoGrid(Grid* g, Matrix* m) { g->InsertInfiniteObject(this); }


Triangle::Triangle(Vec3f& a, Vec3f& b, Vec3f& c, Material* m) 
	: m_vA(a), m_vB(b), m_vC(c), RenderObject3D(m) {
	Vec3f::Cross3(m_vNormal, (m_vB - m_vA), (m_vC - m_vB));
	m_vNormal.Normalize();

	m_pBoundingBox = new BoundingBox(_GetTriangleBoundingBox(m_vA, m_vB, m_vC));
}

void Triangle::paint() {
	RenderObject3D::paint();
	glBegin(GL_TRIANGLES);
	glNormal3f(m_vNormal.x(), m_vNormal.y(), m_vNormal.z());     // List of floats
	glVertex3f(m_vA.x(), m_vA.y(), m_vA.z());
	glVertex3f(m_vB.x(), m_vB.y(), m_vB.z());
	glVertex3f(m_vC.x(), m_vC.y(), m_vC.z());
	glEnd();
}

bool Triangle::intersect(const Ray& r, Hit& h, const float tmin) {
	if (!RenderObject3D::intersect(r, h, tmin)) {
		return false;
	}
	float a11, a12, a13, b1,
		a21, a22, a23, b2,
		a31, a32, a33, b3;
	a11 = m_vA.x() - m_vB.x();
	a21 = m_vA.y() - m_vB.y();
	a31 = m_vA.z() - m_vB.z();

	a12 = m_vA.x() - m_vC.x();
	a22 = m_vA.y() - m_vC.y();
	a32 = m_vA.z() - m_vC.z();

	const Vec3f& dir = r.getDirection();
	a13 = dir.x();
	a23 = dir.y();
	a33 = dir.z();

	const Vec3f& ori = r.getOrigin();
	b1 = m_vA.x() - ori.x();
	b2 = m_vA.y() - ori.y();
	b3 = m_vA.z() - ori.z();
	float A = DETERMINANT(a11, a12, a13, a21, a22, a23, a31, a32, a33);
	if (A) {
		float beta = DETERMINANT(b1, a12, a13, b2, a22, a23, b3, a32, a33) / A;
		float gama = DETERMINANT(a11, b1, a13, a21, b2, a23, a31, b3, a33) / A;
		float t = DETERMINANT(a11, a12, b1, a21, a22, b2, a31, a32, b3) / A;
		if (beta + gama <= 1 && beta >= 0 && gama >= 0 && t >= tmin && t < h.getT()) {
			if (r.getDirection().Dot3(m_vNormal) > 0) {
				h.set(t, m_pMaterial, -1.0f * m_vNormal, r);
			} else {
				h.set(t, m_pMaterial, m_vNormal, r);
			}
			h.set(t, m_pMaterial, m_vNormal, r);
			return true;
		}
	}
	return false;
}

BoundingBox Triangle::GetTransformedAABB(const Matrix& m) {
	Vec3f _a = m_vA, _b = m_vB, _c = m_vC;
	m.Transform(_a);
	m.Transform(_b);
	m.Transform(_c);

	return _GetTriangleBoundingBox(_a, _b, _c);
}

BoundingBox Triangle::_GetTriangleBoundingBox(const Vec3f& _a, const Vec3f& _b, const Vec3f& _c) {
	BoundingBox _out(_a, _a);
	_out.Extend(_b);
	_out.Extend(_c);
	return _out;
}
