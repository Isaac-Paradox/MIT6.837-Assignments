#pragma once
#include"object3d.h"

class Group : public Object3D {
public:
	Group(int object_num) : m_iObjectNum(object_num) {
		m_vObjects = new Object3D*[m_iObjectNum];
	}
	~Group() {
		delete[] m_vObjects;
	}
	void addObject(int index, Object3D* obj) {
		assert(index < m_iObjectNum && index >= 0);
		m_vObjects[index] = obj;
	};

	bool intersect(const Ray& r, Hit& h, const float tmin) {
		bool hitted = false;
		for (int i = 0; i < m_iObjectNum; i++) {
			if (m_vObjects[i]->intersect(r, h, tmin)) {
				hitted = true;
			}
		}
		return hitted;
	};

	//bool intersectShadowRay(const Ray& r, Hit& h, const float epsilon = 1e-08) {
	//	for (int i = 0; i < m_iObjectNum; i++) {
	//		if (m_vObjects[i]->intersect(r, h, epsilon)) {
	//			return true;
	//		}
	//	}
	//	return false;
	//}

protected:
	Object3D** m_vObjects = nullptr;
	int m_iObjectNum = 0;
};