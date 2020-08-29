#pragma once
#include<cmath>
#include<array>
#include"object3d.h"

#define DETERMINANT(a11, a12, a13, a21, a22, a23, a31, a32, a33) (a11 * a22 * a33 + a12 * a23 * a31 + a13 * a21 * a32 - a13 * a22 * a31 - a12 * a21 * a33 - a11 * a23 * a32)

class Triangle : public RenderObject3D {
public:
	Triangle(Vec3f& a, Vec3f& b, Vec3f& c, Material* m) 
		: m_vA(a), m_vB(b), m_vC(c), RenderObject3D(m) {
		Vec3f::Cross3(m_vNormal, (m_vB - m_vA), (m_vC - m_vB));
		m_vNormal.Normalize();
		
		m_pBoundingBox = new BoundingBox(_GetTriangleBoundingBox(m_vA, m_vB, m_vC));
	};

	void paint() override {
		RenderObject3D::paint();
		glBegin(GL_TRIANGLES);
		glNormal3f(m_vNormal.x(), m_vNormal.y(), m_vNormal.z());     // List of floats
		glVertex3f(m_vA.x(), m_vA.y(), m_vA.z());
		glVertex3f(m_vB.x(), m_vB.y(), m_vB.z());
		glVertex3f(m_vC.x(), m_vC.y(), m_vC.z());
		glEnd();
	}

	bool intersect(const Ray& r, Hit& h, const float tmin) {
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
	};

	void insertIntoGrid(Grid* g, Matrix* m) override {
		_InsertBBIntoGrid(g, m ? GetTransformedBoundingBox(m) : (*m_pBoundingBox));
	}

	BoundingBox GetTransformedBoundingBox(Matrix* m) override {
		Vec3f _a = m_vA, _b = m_vB, _c = m_vC;
		m->Transform(_a);
		m->Transform(_b);
		m->Transform(_c);

		return _GetTriangleBoundingBox(_a, _b, _c);
	}

protected:

	BoundingBox _GetTriangleBoundingBox(const Vec3f& _a, const Vec3f& _b, const Vec3f& _c) {
		BoundingBox _out(_a, _a);
		_out.Extend(_b);
		_out.Extend(_c);
		return _out;
	}

protected:

	Vec3f m_vNormal;

	Vec3f m_vA;

	Vec3f m_vB;

	Vec3f m_vC;
};