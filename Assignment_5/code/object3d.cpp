#include "object3d.h"
#include"Grid.h"
#include<array>
#include<limits>

void Object3D::insertIntoGrid(Grid* g, Matrix* m) {
	if (m_pBoundingBox) {
		_InsertBBIntoGrid(g, m ? GetTransformedBoundingBox(m) : (*m_pBoundingBox));
	}
}

BoundingBox Object3D::GetTransformedBoundingBox(Matrix* m) {
	return _TransformBoundingBox(*m_pBoundingBox, *m);
}

BoundingBox Object3D::_TransformBoundingBox(const BoundingBox& _bb, const Matrix& _m) {
	std::array<Vec3f,8> points = _GetBoxVertices(_bb.getMin(), _bb.getMax());
	const static float max_float = (std::numeric_limits<float>().max)();
	const static float min_float = (std::numeric_limits<float>().min)();
	BoundingBox _out(Vec3f(max_float, max_float, max_float), Vec3f(min_float, min_float, min_float));
	for (Vec3f& point : points) {
		_m.Transform(point);
		_out.Extend(point);
	}
	return _out;
}

void Object3D::_InsertBBIntoGrid(Grid* g, const BoundingBox& _bb) {
	Indexs from, to;
	{
		const Vec3f& grid_min = g->getBoundingBox()->getMin();
		const Vec3f& voxel_size = g->GetVoxelSize();

		Vec3f from_index = _bb.getMin() - grid_min;
		Vec3f to_index = _bb.getMax() - grid_min;
		from_index.Divide(voxel_size[0], voxel_size[1], voxel_size[2]);
		to_index.Divide(voxel_size[0], voxel_size[1], voxel_size[2]);
		const Indexs& grid_num = g->GetGridNum();
		for (int i = 0; i < 3; ++i) {
			if (from_index[i] == to_index[i]) {
				from[i] = to[i] = (std::max)(0, (std::min)(int(std::floorf(from_index[i])), g->GetGridNum()[i] - 1));
			} else {
				from[i] = (std::max)(0, int(std::ceilf((from_index[i] - 5e-4f) - 1)));
				to[i] = (std::min)(int(std::floorf(to_index[i] + 5e-4f)), g->GetGridNum()[i] - 1);
			}
			assert(from[i] <= to[i]);
		}
	}

	for (int i = from[0]; i <= to[0]; i++) {
		for (int j = from[1]; j <= to[1]; j++) {
			for (int k = from[2]; k <= to[2]; k++) {
				g->InsertObjectIntoVoxel({ i, j, k }, this);
			}
		}
	}
}

std::array<Vec3f, 8> Object3D::_GetBoxVertices(const Vec3f& _min, const Vec3f& _max) {
	std::array<Vec3f, 8> points;
	points[0].Set(_min.x(), _min.y(), _min.z());
	points[1].Set(_max.x(), _min.y(), _min.z());
	points[2].Set(_max.x(), _min.y(), _max.z());
	points[3].Set(_min.x(), _min.y(), _max.z());
	points[4].Set(_min.x(), _max.y(), _min.z());
	points[5].Set(_max.x(), _max.y(), _min.z());
	points[6].Set(_max.x(), _max.y(), _max.z());
	points[7].Set(_min.x(), _max.y(), _max.z());
	return points;
}
