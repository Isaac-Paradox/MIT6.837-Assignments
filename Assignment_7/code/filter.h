#pragma once
#include"vectors.h"
#include"film.h"

class Filter {
public:
	Vec3f getColor(int i, int j, Film* film);

	virtual float getWeight(float x, float y) = 0;

	virtual int getSupportRadius() = 0;

};

class BoxFilter : public Filter {
public:
	BoxFilter(float radius) : m_fRadius(radius) {};

	float getWeight(float x, float y) override;

	int getSupportRadius() override;
protected:
	float m_fRadius;
};

class TentFilter : public Filter {
public:
	TentFilter(float radius) : m_fRadius(radius) {};

	float getWeight(float x, float y) override;

	int getSupportRadius() override;
protected:
	float m_fRadius;
};

class GaussianFilter : public Filter {
public:
	GaussianFilter(float sigma) : m_fSigma(sigma) {};

	float getWeight(float x, float y) override;

	int getSupportRadius() override;

protected:
	float m_fSigma;
};