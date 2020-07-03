#pragma once
#include"object3d.h"
#include"matrix.h"

class Transform : public Object3D {
public:
	Transform(Matrix& m, Object3D* o) 
		: m_mM2WMatrix(m), m_pObject(o){
		m_mM2WMatrix.Inverse(m_mW2MMatrix);
		m_mW2MMatrix.Transpose(m_mNormalM2WMatrix);
		//m_mVectorM2WMatrix.Inverse(m_mVectorW2MMatrix);
		m_mM2WMatrix.Transpose(m_mNormalW2MMatrix);
	}

	bool intersect(const Ray& r, Hit& h, const float tmin) {
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

protected:
	Matrix m_mM2WMatrix;
	Matrix m_mW2MMatrix;
	Matrix m_mNormalM2WMatrix;
	Matrix m_mNormalW2MMatrix;
	Object3D* m_pObject;
};