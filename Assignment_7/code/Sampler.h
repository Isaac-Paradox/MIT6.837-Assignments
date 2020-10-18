#pragma once
#include"vectors.h"

class Sampler {
public:
	Sampler(int samples_per_pixel);

	virtual Vec2f getSamplePosition(int n) = 0;

protected:
	int m_nSamplesPerPixel;
};

class RandomSampler : public Sampler {
public:
	RandomSampler(int samples_per_pixel);

	Vec2f getSamplePosition(int n) override;
};

class UniformSampler : public Sampler {
public:
	UniformSampler(int samples_per_pixel);

	Vec2f getSamplePosition(int n) override;

protected:
	int m_nRootSPP;

	float m_fInvRootSPP;
};

class JitteredSampler : public UniformSampler {
public:
	JitteredSampler(int samples_per_pixel);

	Vec2f getSamplePosition(int n) override;
};