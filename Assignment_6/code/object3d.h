#pragma once
#include<gl/GL.h>
#include<algorithm>

#include"matrix.h"
#include"boundingbox.h"
#include"material.h"
#include"ray.h"
#include"hit.h"
#include"raytracing_stats.h"

class Grid;
typedef std::array<int, 3> Indexs;
class Object3D {
public:
	virtual BoundingBox* getBoundingBox() { return m_pBoundingBox; }

	virtual void paint(void) = 0;

	virtual bool intersect(const Ray& r, Hit& h, const float tmin) = 0;

	virtual void insertIntoGrid(Grid* g, Matrix* m);

	virtual BoundingBox GetTransformedAABB(const Matrix& m);

protected:
	Object3D() { };

	Object3D(BoundingBox* bb) :m_pBoundingBox(bb) { };

	BoundingBox _TransformBoundingBox(const BoundingBox& _bb, const Matrix& _m);

	inline std::array<Vec3f, 8> _GetBoxVertices(const Vec3f& min, const Vec3f& max);

protected:
	BoundingBox* m_pBoundingBox = nullptr;
};

class RenderObject3D : public Object3D {
public:
	void paint() override { m_pMaterial->glSetMaterial(); }

protected:
	RenderObject3D(Material* material)
		: m_pMaterial(material) { };

	bool intersect(const Ray& r, Hit& h, const float tmin) override {
		RayTracingStats::IncrementNumIntersections();
		return true;
	}

	Material* m_pMaterial = nullptr;
};