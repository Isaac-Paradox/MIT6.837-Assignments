#pragma once
#include"ray.h"
#include"hit.h"

class Object3D {
public:
	virtual bool intersect(const Ray& r, Hit& h, const float tmin) = 0;
};

class RenderObject3D : public Object3D {
protected:
	RenderObject3D(Material* material) : m_pMaterial(material) {};

	Material* m_pMaterial = nullptr;
};