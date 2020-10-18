#pragma once
#include"object3d.h"

class Group : public Object3D {
public:
	Group(int object_num) : m_iObjectNum(object_num){
		m_vObjects = new Object3D*[m_iObjectNum];
	}

	~Group() { delete[] m_vObjects; }

	void addObject(int index, Object3D* obj);

	void paint() override;

	bool intersect(const Ray& r, Hit& h, const float tmin);

	void insertIntoGrid(Grid* g, Matrix* m) override;

protected:
	Object3D** m_vObjects = nullptr;

	int m_iObjectNum = 0;
};