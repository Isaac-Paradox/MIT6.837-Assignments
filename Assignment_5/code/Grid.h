#pragma once
#include"object3d.h"
#include"MarchingInfo.h"
#include"rayTree.h"
#include"plane.h"
#include"object3dvector.h"
#include<memory.h>
#include<limits>
#include<cmath>
#include<algorithm>
#include<array>

class Grid : public Object3D {
public:
	Grid(BoundingBox* bb, int nx, int ny, int nz)
		:m_nGridNum{ nx, ny,nz } {
		m_bVoxelObjects = new Object3DVector[m_nGridNum[0] * m_nGridNum[1] * m_nGridNum[2]];
		m_pBoundingBox = bb;
		m_vBBMax = m_pBoundingBox->getMax();
		m_vBBMin = m_pBoundingBox->getMin();
		m_vVoxelSize = m_vBBMax - m_vBBMin;
		m_vVoxelSize.Divide(m_nGridNum[0], m_nGridNum[1], m_nGridNum[2]);
	}

	~Grid() {
		delete[] m_bVoxelObjects;
	}

	void paint(void) override {
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		for (int i = 0; i < m_nGridNum[0]; i++) {
			for (int j = 0; j < m_nGridNum[1]; j++) {
				for (int k = 0; k < m_nGridNum[2]; k++) {
					Indexs index = { i, j, k };
					int obj_num = _GetVoxelObjectVector(index).getNumObjects();
					if (obj_num) {
						GLfloat diffuse[4];
						c_vDiffuseColor[(std::min)(obj_num, c_nMaterialNum) - 1].Get(diffuse[0], diffuse[1], diffuse[2]);
						diffuse[3] = 1;
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
						glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
						std::array<Vec3f, 8> points = GetGridVertices(index);
						for (int f = 0; f < 6; f++) {
							//{
							//	int axis = f / 2;
							//	Indexs adjoin_index = index;
							//	adjoin_index[axis] += c_vNormals[f][axis];
							//	if (adjoin_index[axis] >= 0 &&
							//		adjoin_index[axis] < m_nGridNum[axis] &&
							//		_GetVoxelObjectVector(adjoin_index).getNumObjects()) {
							//		continue;
							//	}
							//}
							
							glNormal3f(c_vNormals[f].x(), c_vNormals[f].y(), c_vNormals[f].z());
							for (int p = 0; p < 4; p++) {
								Vec3f& vertex = points[c_vQuads[4 * f + p]];
								glVertex3f(vertex.x(), vertex.y(), vertex.z());
							}
						}
					}
				}
			}
		}
		glEnd();
	}

	bool intersect(const Ray& r, Hit& h, const float tmin) override {
		MarchingInfo marching_info, marching_info1;
		initializeRayMarch(marching_info, r, tmin);
		if (marching_info.has_hit) {
			marching_info.has_hit = false;
			m_nMaterialIndex = 0;
			while (true) {
				for (int i = 0; i < 3; i++) {
					if (marching_info.index[i] < 0 || marching_info.index[i] >= m_nGridNum[i]) {
						return marching_info.has_hit;
					}
				}
				if (!marching_info.has_hit){
					int obj_num = _GetVoxelObjectVector(marching_info.index).getNumObjects();
					if (obj_num) {
						h.set(marching_info.tmin,
							&c_vMaterials[(std::min)(obj_num, c_nMaterialNum) - 1],
							c_vNormals[marching_info.entered_face_index],
							r);
						marching_info.has_hit = true;
					}
				}

				_AddHitCellAndFace(marching_info.index, marching_info.entered_face_index);
				marching_info.nextCell();
			}
		} else {
			return false;
		}
	}

	const Indexs& GetGridNum() const {
		return m_nGridNum;
	}

	void initializeRayMarch(MarchingInfo& mi, const Ray& r, float tmin) {
		const Vec3f& ori = r.getOrigin();
		const Vec3f& dir = r.getDirection();
		float inv_rd[3]{ 1.f / dir.x(), 1.f / dir.y(), 1.f / dir.z() };
		for (int i = 0; i < 3; ++i) {
			mi.d_t[i] = std::abs(m_vVoxelSize[i] * inv_rd[i]);
			mi.sign[i] = !std::signbit(dir[i]);
		}

		Vec3f hit_point;
		if (ori.x() > m_vBBMin.x() && ori.x() < m_vBBMax.x() &&
			ori.y() > m_vBBMin.y() && ori.y() < m_vBBMax.y() &&
			ori.z() > m_vBBMin.z() && ori.z() < m_vBBMax.z()) {
			// inside the box
			mi.has_hit = true;
			hit_point = ori;
		} else {
			// outside the box
			float t1, t2, tn, tf;
			int NI1, NI2, NI_near, NI_far;//normal index
			mi.has_hit = true;
			for (int i = 0; i < 3; i++) {
				if (dir[i] == 0) {
					if (ori[i] < m_vBBMin[i] || ori[i] > m_vBBMax[i]) {
						mi.has_hit = false;
						break;
					}
				}
			}

			if (mi.has_hit) {
				mi.has_hit = false;

				t1 = (m_vBBMin[0] - ori[0]) * inv_rd[0];
				t2 = (m_vBBMax[0] - ori[0]) * inv_rd[0];
				NI1 = 0;
				NI2 = 1;
				if (t1 > t2) { std::swap(t1, t2); std::swap(NI1, NI2); }
				tn = t1; NI_near = NI1; tf = t2; NI_far = NI2;
				t1 = (m_vBBMin[1] - ori[1]) * inv_rd[1];
				t2 = (m_vBBMax[1] - ori[1]) * inv_rd[1];
				NI1 = 2;
				NI2 = 3;
				if (t1 > t2) { std::swap(t1, t2); std::swap(NI1, NI2); }
				if (t1 > tn) { tn = t1; NI_near = NI1; } if (t2 < tf) { tf = t2; NI_far = NI2; }
				t1 = (m_vBBMin[2] - ori[2]) * inv_rd[2];
				t2 = (m_vBBMax[2] - ori[2]) * inv_rd[2];
				NI1 = 4;
				NI2 = 5;
				if (t1 > t2) { std::swap(t1, t2); std::swap(NI1, NI2); }
				if (t1 > tn) { tn = t1; NI_near = NI1; } if (t2 < tf) { tf = t2; NI_far = NI2; }
				if (tn <= tf && tf >= tmin) {
					if (tn > tmin) {
						t1 = tn;
						NI1 = NI_near;
					} else {
						t1 = tf;
						NI1 = NI_far;
					}
					mi.tmin = t1;
					hit_point = r.pointAtParameter(t1);
					mi.has_hit = true;
					mi.entered_face_index = NI1;
				}
			}
		}

		_GetGridCellIndexFromPoint(hit_point, mi.index);
		for (int i = 0; i < 3; ++i) {
			mi.t_next[i] = (m_vBBMin[i] + m_vVoxelSize[i] * (mi.index[i] + (mi.sign[i] ? 1 : 0)) - ori[i]) * inv_rd[i];
		}
	}

	const Vec3f& GetVoxelSize() const {
		return m_vVoxelSize;
	}

	void InsertObjectIntoVoxel(const Indexs& index, Object3D* obj) {
		m_bVoxelObjects[_GetVoxelIndex(index)].addObject(obj);
	}

	std::array<Vec3f, 8> GetGridVertices(const Indexs& index) {
		Vec3f min = m_vBBMin + Vec3f(index[0], index[1], index[2]) * m_vVoxelSize;
		Vec3f max = min + m_vVoxelSize;
		return _GetBoxVertices(min, max);
	}
protected:
	inline Object3DVector& _GetVoxelObjectVector(const Indexs& index) const {
		return m_bVoxelObjects[_GetVoxelIndex(index)];
	}

	inline int _GetVoxelIndex(const Indexs& index) const {
		return index[0] * m_nGridNum[1] * m_nGridNum[2] + index[1] * m_nGridNum[2] + index[2];
	}

	void _AddHitCellAndFace(const Indexs& index, int face) {
		std::array<Vec3f, 8> points = GetGridVertices(index);
		Material* mat = _GetHitGridMaterial();
		for (int f = 0; f < 6; f++) {
			if (f == face) {
				RayTree::AddEnteredFace(points[c_vQuads[4 * f + 0]]
					, points[c_vQuads[4 * f + 1]]
					, points[c_vQuads[4 * f + 2]]
					, points[c_vQuads[4 * f + 3]]
					, c_vNormals[f]
					, mat);
			}
			RayTree::AddHitCellFace(points[c_vQuads[4 * f + 0]]
				, points[c_vQuads[4 * f + 1]]
				, points[c_vQuads[4 * f + 2]]
				, points[c_vQuads[4 * f + 3]]
				, c_vNormals[f]
				, mat);
		}
	}

	Material* _GetHitGridMaterial() {
		if (m_nMaterialIndex == c_nMaterialNum) { m_nMaterialIndex = 0; }
		return &c_vMaterials[m_nMaterialIndex++];
	}

	//BoundingBox _GetVoxel(const Indexs& index) const {
	//	Vec3f start = m_pBoundingBox->getMin();
	//	return BoundingBox(
	//		Vec3f(start.x() + m_vVoxelSize[0] * index[0],
	//			start.y() + m_vVoxelSize[1] * index[1],
	//			start.z() + m_vVoxelSize[2] * index[2]),
	//		Vec3f(start.x() + m_vVoxelSize[0] * (index[0] + 1),
	//			start.y() + m_vVoxelSize[1] * (index[1] + 1),
	//			start.z() + m_vVoxelSize[2] * (index[2] + 1)));
	//}

	void _GetGridCellIndexFromPoint(const Vec3f& p, Indexs& index) {
		Vec3f ind = p - m_vBBMin;

		for (int i = 0; i < 3; ++i) {
			//index[i] = std::clamp(int(floorf(fi)), 0, m_nGridNum[i] - 1);
			index[i] = (std::max)(0, (std::min)(int(floorf(ind[i] / m_vVoxelSize[i])), m_nGridNum[i] - 1));
		}
	}

protected:
	Indexs m_nGridNum;

	Vec3f m_vVoxelSize;

	Object3DVector* m_bVoxelObjects;

	int m_nMaterialIndex = 0;

	Vec3f m_vBBMin;

	Vec3f m_vBBMax;
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

	const Vec3f HSL2RGB(float h, float s, float l) {
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