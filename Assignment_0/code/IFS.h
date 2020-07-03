#pragma once
#include"matrix.h"
#include"image.h"

const Vec3f WHITE(1, 1, 1);
const Vec3f BLACK(0, 0, 0);

class IFS {
protected:
    Matrix* m_vMats;
    float* m_vProbabilities;
    int m_iNumTransforms;
public:
    IFS(FILE* input);
    ~IFS();

    bool BuildIFS(int num_points,
        int num_iters,
        int size,
        Image& image);
private:
    float RandomFloat();
};