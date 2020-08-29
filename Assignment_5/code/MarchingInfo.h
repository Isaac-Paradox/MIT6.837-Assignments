#pragma once
#include<array>

class MarchingInfo {
public:
	float tmin;//the current value of tmin
	std::array<int, 3> index;//the grid indices i, j, and k for the current grid cell
	std::array<float, 3> t_next;//the next values of intersection along each axis 
	std::array<float, 3> d_t;//the marching increments along each axis 
	std::array<bool, 3> sign;//the sign of the direction vector components
	/*To render the occupied grid cells for visualization 
	*you will also need to store the surface normal of the cell face 
	*which was crossed to enter the current cell. 
	*Write the appropriate accessors and modifiers.
	*/
	bool has_hit = false;

	int entered_face_index;

	void nextCell() {
		int next_grid_axis = -1;
		for (int i = 0; i < 3; i++) {
			if (t_next[i] >= tmin && (next_grid_axis == -1 || t_next[i] < t_next[next_grid_axis])) {
				next_grid_axis = i;
			}
		}
		index[next_grid_axis] += sign[next_grid_axis] ? 1 : -1;
		tmin = t_next[next_grid_axis];
		t_next[next_grid_axis] += d_t[next_grid_axis];
		entered_face_index = next_grid_axis * 2 + (sign[next_grid_axis] ? 0 : 1);
	}
};