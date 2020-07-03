#pragma once
#include <limits>
#include<cmath>

#include"ray.h"

class Camera {
public:
	Camera(const Vec3f& center, const Vec3f& direction, const Vec3f& up)
		: m_vCenter(center), m_vForward(direction), m_vUp(up) {
		m_vForward.Normalize();
		Vec3f::Cross3(m_vRight, m_vForward, m_vUp);
		m_vRight.Normalize();
		Vec3f::Cross3(m_vUp, m_vRight, m_vForward);
		m_vUp.Normalize();
	};

	virtual Ray generateRay(Vec2f point) = 0;

	virtual float getTMin() const = 0;

	const Vec3f& getForward() const { return m_vForward; }

	const Vec3f& getUp() const { return m_vUp; }

	const Vec3f& getRight() const { return m_vRight; }

protected:
	Vec3f m_vCenter; 
	Vec3f m_vForward; 
	Vec3f m_vUp;
	Vec3f m_vRight;
};

class OrthographicCamera : public Camera {
public:
	OrthographicCamera(const Vec3f& center, const Vec3f& direction, const Vec3f& up, float size)
		: Camera(center, direction, up), m_fSize(size) { };

	Ray generateRay(Vec2f point) { 
		point -= Vec2f(0.5f, 0.5f);
		Vec3f orig = m_vCenter + m_fSize * (m_vUp * point.y() + m_vRight * point.x());
		return Ray(orig, m_vForward);
	};

	float getTMin() const { return -numeric_limits<float>::max(); };

protected:
	
	float m_fSize;
};

class PerspectiveCamera : public Camera {
public:
	PerspectiveCamera(const Vec3f& center, const Vec3f& direction, const Vec3f& up, float angle_radians)
		: Camera(center, direction, up){
		m_vLeftDownSide = m_vForward * (0.5f/tan(angle_radians * 0.5f)) - (m_vUp + m_vRight) * 0.5;
		//m_fScreenScale = tan(angle_radians * 0.5f);
		//m_vLeftDownSide = m_vForward - ((m_vUp + m_vRight) * m_fScreenScale);
		//m_fScreenScale *= 2;
	};

	Ray generateRay(Vec2f point) {
		//Vec3f direction = m_vLeftDownSide + (point.x() * m_vRight + point.y() * m_vUp) * m_fScreenScale;
		Vec3f direction = m_vLeftDownSide + (point.x() * m_vRight + point.y() * m_vUp);
		direction.Normalize();
		return Ray(m_vCenter, direction);
	};

	float getTMin() const { return 1e-5f; };

protected:
	Vec3f m_vLeftDownSide;
	//float m_fScreenScale;
};