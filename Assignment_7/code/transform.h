#pragma once
#include"object3d.h"
#include"matrix.h"

class Transform : public Object3D {
public:
	Transform(const Matrix& m, Object3D* o);

	bool intersect(const Ray& r, Hit& h, const float tmin);

	void paint();

	void insertIntoGrid(Grid* g, Matrix* m) override;

protected:
	Matrix m_mM2WMatrix;
	Matrix m_mW2MMatrix;
	Matrix m_mNormalM2WMatrix;
	Matrix m_mNormalW2MMatrix;
	Object3D* m_pObject = nullptr;
	BoundingBox m_bBoundBox;
};