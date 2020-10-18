#include "Sampler.h"
#include<random>

Sampler::Sampler(int samples_per_pixel)
	: m_nSamplesPerPixel(samples_per_pixel) { }

RandomSampler::RandomSampler(int samples_per_pixel)
	: Sampler(samples_per_pixel) { }

Vec2f RandomSampler::getSamplePosition(int n) {
	return Vec2f((float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
}

UniformSampler::UniformSampler(int samples_per_pixel) 
	: Sampler(samples_per_pixel), 
	m_nRootSPP(std::sqrt(m_nSamplesPerPixel)),
	m_fInvRootSPP(1.0f / m_nRootSPP){ }
       
Vec2f UniformSampler::getSamplePosition(int n) {
	return Vec2f(m_fInvRootSPP * ((n / m_nRootSPP) + 0.5f), m_fInvRootSPP * ((n % m_nRootSPP) + 0.5f));
}

JitteredSampler::JitteredSampler(int samples_per_pixel)
	: UniformSampler(samples_per_pixel) {}

Vec2f JitteredSampler::getSamplePosition(int n) {
	return Vec2f(m_fInvRootSPP * ((n / m_nRootSPP) + (float)rand() / RAND_MAX), 
		m_fInvRootSPP * ((n % m_nRootSPP) + (float)rand() / RAND_MAX));
}
