#include "Grid.h"

void Grid::paint(void) {
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

bool Grid::intersect(const Ray& r, Hit& h, const float tmin) {
	MarchingInfo marching_info, marching_info1;
	_initializeRayMarch(marching_info, r, tmin);
	if (marching_info.has_hit) {
		marching_info.has_hit = false;
		m_nMaterialIndex = 0;
		while (true) {
			if (marching_info.index[0] < 0 || marching_info.index[0] >= m_nGridNum[0] ||
				marching_info.index[1] < 0 || marching_info.index[1] >= m_nGridNum[1] ||
				marching_info.index[2] < 0 || marching_info.index[2] >= m_nGridNum[2]) {
				break;
			}
			if (!marching_info.has_hit) {
				Object3DVector& objs = _GetVoxelObjectVector(marching_info.index);
				int obj_num = objs.getNumObjects();
				if (obj_num) {
					if (m_bVisualizeGrid) {
						h.set(marching_info.tmin,
							&c_vMaterials[(std::min)(obj_num, c_nMaterialNum) - 1],
							c_vNormals[marching_info.entered_face_index],
							r);
						marching_info.has_hit = true;
					} else {
						for (int i = 0; i < obj_num; i++) {
							Hit object_hit(h);
							if (objs.getObject(i)->intersect(r, object_hit, tmin)) {
								//if(object_hit.getT() >= marching_info.tmin - 1e-5f && object_hit.getT() <= marching_info.t_next[marching_info.GetNextGridAxis()] + 1e-5f){
								if (IsPointInsideGrid(object_hit.getIntersectionPoint(), marching_info.index)) {
									h = object_hit;
									marching_info.has_hit = true;
								}
							}
						}
					}
					
				}
			}

			_AddHitCellAndFace(marching_info.index, marching_info.entered_face_index);
			marching_info.nextCell();
		}
	} 
	if (!m_bVisualizeGrid) {
		int obj_num = m_vInfiniteObjects.getNumObjects();
		for (int i = 0; i < obj_num; i++) {
			if (m_vInfiniteObjects.getObject(i)->intersect(r, h, tmin)) {
				marching_info.has_hit = true;
			}
		}
	}
	return marching_info.has_hit;
}

void Grid::InsertObjectIntoVoxel(const Indexs& index, Object3D* obj, const Matrix* m) { 
	if (m) {
		Transformation* transform = new Transformation(*m, obj);
		m_vTransformations.addObject(transform);
		m_pVoxelObjects[_GetVoxelIndex(index)].addObject(transform);
	} else {
		m_pVoxelObjects[_GetVoxelIndex(index)].addObject(obj);
	}
}

bool Grid::IsPointInsideGrid(const Vec3f& point, const Indexs& index) {
	Vec3f min, max;
	_GetGridBoundary(min, max, index);
	for (int i = 0; i < 3; i++) {
		if (point[i] < min[i] - 1e-5f || point[i] > max[i] + 1e-5f) {
			return false;
		}
	}
    return true;
}

void Grid::_GetGridBoundary(Vec3f& min, Vec3f& max, const Indexs& index) {
	min = m_vBBMin + Vec3f(index[0], index[1], index[2]) * m_vVoxelSize;
	max = min + m_vVoxelSize;
}

void Grid::_initializeRayMarch(MarchingInfo& mi, const Ray& r, float tmin) {
	const Vec3f& ori = r.getOrigin();
	const Vec3f& dir = r.getDirection();
	float inv_rd[3]{ 1.f / dir.x(), 1.f / dir.y(), 1.f / dir.z() };
	mi.d_t[0] = std::abs(m_vVoxelSize[0] * inv_rd[0]);
	mi.d_t[1] = std::abs(m_vVoxelSize[1] * inv_rd[1]);
	mi.d_t[2] = std::abs(m_vVoxelSize[2] * inv_rd[2]);
	mi.sign[0] = !std::signbit(dir[0]);
	mi.sign[1] = !std::signbit(dir[1]);
	mi.sign[2] = !std::signbit(dir[2]);

	Vec3f hit_point;
	if (ori.x() > m_vBBMin.x() - 1e-5f && ori.x() < m_vBBMax.x() + 1e-5f &&
		ori.y() > m_vBBMin.y() - 1e-5f && ori.y() < m_vBBMax.y() + 1e-5f &&
		ori.z() > m_vBBMin.z() - 1e-5f && ori.z() < m_vBBMax.z() + 1e-5f) {
		// inside the box
		mi.has_hit = true;
		hit_point = ori;
		mi.tmin = 1e-5f;
		mi.entered_face_index = 0;
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

void Grid::_AddHitCellAndFace(const Indexs& index, int face) {
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

void Grid::_GetGridCellIndexFromPoint(const Vec3f& p, Indexs& index) {
	Vec3f ind = p - m_vBBMin;

	for (int i = 0; i < 3; ++i) {
		//index[i] = std::clamp(int(floorf(fi)), 0, m_nGridNum[i] - 1);
		index[i] = (std::max)(0, (std::min)(int(floorf(ind[i] / m_vVoxelSize[i])), m_nGridNum[i] - 1));
	}
}

Transformation::Transformation(const Matrix& m, Object3D* o) : Transform(m, o) {}

void Transformation::insertIntoGrid(Grid* g, Matrix* m) {}
