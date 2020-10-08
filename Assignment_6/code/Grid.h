#pragma once
#include"object3d.h"
#include"MarchingInfo.h"
#include"rayTree.h"
#include"plane.h"
#include"object3dvector.h"
#include"matrix.h"
#include"transform.h"
#include<memory.h>
#include<limits>
#include<cmath>
#include<algorithm>
#include<array>
#include<vector>

class Transformation : public Transform {
public:
	Transformation(const Matrix& m, Object3D* o);

	void insertIntoGrid(Grid* g, Matrix* m) override;

protected:
	Matrix m_mMatrix;
};

class Grid : public Object3D {
public:
	Grid(BoundingBox* bb, int nx, int ny, int nz)
		:m_nGridNum{ nx, ny,nz } {
		m_pVoxelObjects = new Object3DVector[m_nGridNum[0] * m_nGridNum[1] * m_nGridNum[2]];
		m_pBoundingBox = bb;
		m_vBBMax = m_pBoundingBox->getMax();
		m_vBBMin = m_pBoundingBox->getMin();
		m_vVoxelSize = m_vBBMax - m_vBBMin;
		m_vVoxelSize.Divide(m_nGridNum[0], m_nGridNum[1], m_nGridNum[2]);
	}

	~Grid() {
		delete[] m_pVoxelObjects;
		for (int i = 0; i < m_vTransformations.getNumObjects(); ++i) {
			delete m_vTransformations.getObject(i);
		}
	}

	void paint(void) override;

	bool intersect(const Ray& r, Hit& h, const float tmin) override;

	const Indexs& GetGridNum() const { return m_nGridNum; }

	const Vec3f& GetVoxelSize() const { return m_vVoxelSize; }

	void InsertObjectIntoVoxel(const Indexs& index, Object3D* obj, const Matrix* m = nullptr);

	void InsertInfiniteObject(Object3D* obj) { m_vInfiniteObjects.addObject(obj); }

	std::array<Vec3f, 8> GetGridVertices(const Indexs& index) {
		Vec3f min, max;
		_GetGridBoundary(min, max, index);
		return _GetBoxVertices(min, max);
	}

	void SetGridVisualize(bool active) { m_bVisualizeGrid = active; }

	void insertIntoGrid(Grid* g, Matrix* m) override {};

	bool IsPointInsideGrid(const Vec3f& point, const Indexs& index);
protected:
	inline Object3DVector& _GetVoxelObjectVector(const Indexs& index) const { return m_pVoxelObjects[_GetVoxelIndex(index)]; }

	inline int _GetVoxelIndex(const Indexs& index) const { return index[0] * m_nGridNum[1] * m_nGridNum[2] + index[1] * m_nGridNum[2] + index[2]; }

	inline Material* _GetHitGridMaterial() { if (m_nMaterialIndex == c_nMaterialNum) { m_nMaterialIndex = 0; } return &c_vMaterials[m_nMaterialIndex++]; }

	inline void _GetGridBoundary(Vec3f& min, Vec3f& max, const Indexs& index);

	void _initializeRayMarch(MarchingInfo& mi, const Ray& r, float tmin);

	void _AddHitCellAndFace(const Indexs& index, int face);

	void _GetGridCellIndexFromPoint(const Vec3f& p, Indexs& index);

protected:
	Indexs m_nGridNum;

	Vec3f m_vVoxelSize;

	Object3DVector* m_pVoxelObjects;

	Object3DVector m_vInfiniteObjects;

	int m_nMaterialIndex = 0;

	Vec3f m_vBBMin;

	Vec3f m_vBBMax;

	bool m_bVisualizeGrid = false;

	Object3DVector m_vTransformations;
private:
	const int c_vQuads[24] = {
		0, 3, 7, 4,//right
		6, 2, 1, 5,//left
		0, 1, 2, 3,//down
		6, 5, 4, 7,//up
		0, 4, 5, 1,//back
		6, 7, 3, 2,//front
	};

	const Vec3f c_vNormals[6] = {
		Vec3f(-1, 0, 0),
		Vec3f(1, 0, 0),
		Vec3f(0, -1, 0),
		Vec3f(0, 1, 0),
		Vec3f(0, 0, -1),
		Vec3f(0, 0, 1),
	};

	static const int c_nMaterialNum = 16;

	const Vec3f HSL2RGB(float h, float s, float l)  {
		float rgb[3];
		float q = l < 0.5f ? (l * (1.f + s)) : (l + s - (l * s));
		float p = 2 * l - q;
		h /= 360.f;
		rgb[0] = h + 1.f / 3; rgb[1] = h; rgb[2] = h - 1.f / 3;
		for (int i = 0; i < 3; i++) {
			if (rgb[i] < 0.f) {
				rgb[i] += 1.f;
			} else if (rgb[i] > 1.f) {
				rgb[i] -= 1.f;
			}
			if (rgb[i] < 1.f / 6) {
				rgb[i] = p + (q - p) * 6 * rgb[i];
			} else if (rgb[i] < 0.5f) {
				rgb[i] = q;
			} else if (rgb[i] < 2.f / 3) {
				rgb[i] = p + (q - p) * 6 * (2.f / 3 - rgb[i]);
			} else {
				rgb[i] = p;
			}
		}
		return Vec3f(rgb[0], rgb[1], rgb[2]);
	}

	const PhongMaterial GetMaterial(int i) {
		return PhongMaterial(c_vDiffuseColor[i],
			Vec3f(0, 0, 0), 0, Vec3f(0, 0, 0), Vec3f(0, 0, 0), 0);
	}

	Vec3f c_vDiffuseColor[c_nMaterialNum] = {
		{1, 1, 1},
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 2, 1.f, 0.5f),
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 3, 1.f, 0.5f),
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 4, 1.f, 0.5f),
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 5, 1.f, 0.5f),
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 6, 1.f, 0.5f),
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 7, 1.f, 0.5f),
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 8, 1.f, 0.5f),
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 9, 1.f, 0.5f),
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 10, 1.f, 0.5f),
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 11, 1.f, 0.5f),
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 12, 1.f, 0.5f),
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 13, 1.f, 0.5f),
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 14, 1.f, 0.5f),
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 15, 1.f, 0.5f),
		HSL2RGB(360.f - 360.f / c_nMaterialNum * 16, 1.f, 0.5f),
	};

	//memory efficient using static
	PhongMaterial c_vMaterials[c_nMaterialNum] = {
		GetMaterial(0),
		GetMaterial(1),
		GetMaterial(2),
		GetMaterial(3),
		GetMaterial(4),
		GetMaterial(5),
		GetMaterial(6),
		GetMaterial(7),
		GetMaterial(8),
		GetMaterial(9),
		GetMaterial(10),
		GetMaterial(11),
		GetMaterial(12),
		GetMaterial(13),
		GetMaterial(14),
		GetMaterial(15),
	};
};