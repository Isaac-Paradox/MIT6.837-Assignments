#pragma once
#include<limits>
#include<cmath>

#include"vectors.h"
#include"ray.h"

class Camera {
public:
	Camera(const Vec3f& center, const Vec3f& direction, const Vec3f& up)
		: m_vCenter(center), m_vForward(direction), m_vOriginUp(up) { };

	virtual void glInit(int w, int h);

	virtual void rtInit() { _UpdateCameraDirection(); };

	virtual void glPlaceCamera(void);

	virtual void dollyCamera(float dist);

	virtual void truckCamera(float dx, float dy);

	virtual void rotateCamera(float rx, float ry);

	virtual Ray generateRay(Vec2f point) = 0;

	virtual float getTMin() const = 0;

	const Vec3f& getForward() const { return m_vForward; }

	const Vec3f& getUp() const { return m_vUp; }

	const Vec3f& getRight() const { return m_vRight; }
protected:
	virtual bool _UpdateCameraDirection();

protected:
	Vec3f m_vCenter; 
	Vec3f m_vForward; 
	Vec3f m_vUp;
	Vec3f m_vRight;
	Vec3f m_vOriginUp;
private:
	bool m_bCameraDirectionDirty = true;
};

class OrthographicCamera : public Camera {
public:
	OrthographicCamera(const Vec3f& center, const Vec3f& direction, const Vec3f& up, float size)
		: Camera(center, direction, up), m_fSize(size) { };

	void glInit(int w, int h) override;

	float getTMin() const { return 5e-5f; };

	Ray generateRay(Vec2f point) override ;
protected:
	
	float m_fSize;
};

class PerspectiveCamera : public Camera {
public:
	PerspectiveCamera(const Vec3f& center, const Vec3f& direction, const Vec3f& up, float angle_radians)
		: Camera(center, direction, up), m_fAngle(angle_radians){ };

	void glInit(int w, int h) override;

	float getTMin() const { return 5e-5f; };

	Ray generateRay(Vec2f point) override;
protected:
	bool _UpdateCameraDirection() override;

protected:
	Vec3f m_vLeftDownSide;

	float m_fAngle = 0;
};