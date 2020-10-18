#include "filter.h"

Vec3f Filter::getColor(int i, int j, Film* film) {
    int spp = film->getNumSamples();
    int width = film->getWidth();
    int height = film->getHeight();
    int support_radius = getSupportRadius();
    Vec3f _out;
    float total_weight = 0.f;
    for (int offset_y = -support_radius; offset_y <= support_radius; offset_y++) {
        int y = j + offset_y;
        if (y < 0) { continue; } if (y >= height) { break; }
        for (int offset_x = -support_radius; offset_x <= support_radius; offset_x++) {
            int x = i + offset_x;
            if (x < 0) { continue; } if (x >= width) { break; }
            for (int s = 0; s < spp; s++) {
                Sample sample = film->getSample(x, y, s);
                Vec2f sample_pos = sample.getPosition();
                float weight = getWeight(offset_x + sample_pos.x() - 0.5f, offset_y + sample_pos.y() - 0.5f);
                total_weight += weight;
                _out += sample.getColor() * weight;
            }
        }
    }
    _out /= total_weight;
    return _out;
}

float BoxFilter::getWeight(float x, float y) {
    return std::fabsf(x) < m_fRadius && std::fabsf(y) < m_fRadius;
}

int BoxFilter::getSupportRadius() {
    return (int)std::ceil(m_fRadius - 0.5f);
}

float TentFilter::getWeight(float x, float y) {
    return (std::max)(0.f, 1.f - std::sqrt(x * x + y * y) / m_fRadius);
}

int TentFilter::getSupportRadius() {
    return (int)std::ceil(m_fRadius - 0.5f);
}

float GaussianFilter::getWeight(float x, float y) {
    float index = (x * x + y * y) / (2.f * m_fSigma * m_fSigma);
    return std::exp(-index);
}

int GaussianFilter::getSupportRadius() {
    return 2.f * m_fSigma;
}