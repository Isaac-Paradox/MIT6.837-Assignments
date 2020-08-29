#include "group.h"

void Group::addObject(int index, Object3D* obj) {
	assert(index < m_iObjectNum&& index >= 0);
	m_vObjects[index] = obj;
	BoundingBox* objBB = obj->getBoundingBox();
	if (objBB) {
		if (m_pBoundingBox) {
			m_pBoundingBox->Extend(objBB->getMax());
			m_pBoundingBox->Extend(objBB->getMin());
		} else {
			m_pBoundingBox = new BoundingBox(*objBB);
		}
	}
}

void Group::paint() {
	for (int i = 0; i < m_iObjectNum; i++) {
		m_vObjects[i]->paint();
	}
}

bool Group::intersect(const Ray& r, Hit& h, const float tmin) {
	bool hitted = false;
	for (int i = 0; i < m_iObjectNum; i++) {
		if (m_vObjects[i]->intersect(r, h, tmin)) {
			hitted = true;
		}
	}
	return hitted;
}

void Group::insertIntoGrid(Grid* g, Matrix* m) {
	for (int i = 0; i < m_iObjectNum; i++) {
		m_vObjects[i]->insertIntoGrid(g, m);
	}
}

bool Group::intersectShadowRay(const Ray& r, Hit& h, const float epsilon) {
	for (int i = 0; i < m_iObjectNum; i++) {
		if (m_vObjects[i]->intersect(r, h, epsilon)) {
			return true;
		}
	}
	return false;
}
