#pragma once
#include<gl/GL.h>

#include"material.h"
#include"ray.h"
#include"hit.h"

class Object3D {
public:
	virtual void paint(void) = 0;

	virtual bool intersect(const Ray& r, Hit& h, const float tmin) = 0;
};

class RenderObject3D : public Object3D {
public:
	void paint() override {
		m_pMaterial->glSetMaterial();
	}

protected:
	RenderObject3D(Material* material) : m_pMaterial(material) {};

	Material* m_pMaterial = nullptr;
};