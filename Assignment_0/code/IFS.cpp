#include"IFS.h"

IFS::IFS(FILE* input) {
    // read the number of transforms
    fscanf(input, "%d", &m_iNumTransforms);

    // < DO SOMETHING WITH num_transforms >
    m_vMats = new Matrix[m_iNumTransforms];
    m_vProbabilities = new float[m_iNumTransforms];
    // read in the transforms
    for (int i = 0; i < m_iNumTransforms; i++) {
        fscanf(input, "%f", &m_vProbabilities[i]);
		if (i) {
			m_vProbabilities[i] += m_vProbabilities[i - 1];
		}
        m_vMats[i].Read3x3(input);
    }

    // close the file
    fclose(input);
}

IFS::~IFS() {
	delete[] m_vMats;
	delete[] m_vProbabilities;
}

bool IFS::BuildIFS(int num_points, int num_iters, int size, Image& image) {
	image.SetAllPixels(WHITE);
	Vec2f point;
	for (int pointi = 0; pointi < num_points; pointi++) {
		point.Set(RandomFloat(), RandomFloat());
		Vec3f color = WHITE;
		int ite = num_iters;
		while(ite--){
			float rf = RandomFloat();
			int matIndex = 0;
			while (m_vProbabilities[matIndex] < rf) { ++matIndex; }
			Vec3f add(1 - matIndex, 0, matIndex);
			Vec3f::Average(color, add, color);
			m_vMats[matIndex].Transform(point);
		}
		if (point.x() >= -0.5 && point.x() < 1.5 && point.y() >= -1 && point.y() < 1) {
			image.SetPixel((point.x() + 0.5) * size * 0.5, (point.y() + 1) * size * 0.5, color);
		}
	}

	return true;
}

float IFS::RandomFloat() {
	return (float)rand() / 0x7FFF;
}
