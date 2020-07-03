#pragma once
#include <limits>

#include"ray.h"

class Camera {
public:
	Camera(Vec3f center, Vec3f direction, Vec3f up)
		: m_vCenter(center), m_vDirection(direction), m_vUp(up) {
		m_vDirection.Normalize();
		Vec3f::Cross3(m_vHorizontal, m_vDirection, m_vUp);
		m_vHorizontal.Normalize();
		Vec3f::Cross3(m_vUp, m_vHorizontal, m_vDirection);
		m_vUp.Normalize();
	};

	virtual Ray generateRay(Vec2f point) = 0;

	virtual float getTMin() const = 0;

protected:
	Vec3f m_vCenter; 
	Vec3f m_vDirection; 
	Vec3f m_vUp;
	Vec3f m_vHorizontal;
};

class OrthographicCamera : public Camera {
public:
	OrthographicCamera(Vec3f center, Vec3f direction, Vec3f up, float size) 
		: Camera(center, direction, up), m_fSize(size) { };
	Ray generateRay(Vec2f point) { 
		point -= Vec2f(0.5f, 0.5f);
		assert(point.x() >= -0.5, point.x() <= 0.5);
		Vec3f orig = m_vCenter + m_fSize * (m_vUp * point.y() + m_vHorizontal * point.x());
		return Ray(orig, m_vDirection);
	};
	float getTMin() const { return -numeric_limits<float>::max(); };

protected:
	
	float m_fSize;
};