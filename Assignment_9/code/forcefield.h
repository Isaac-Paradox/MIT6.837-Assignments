#pragma once
#include"vectors.h"

class ForceField {
public:
	virtual Vec3f getAcceleration(const Vec3f& position, float mass, float t) const = 0;
};

class GravityForceField : public ForceField {
public:
	GravityForceField(const Vec3f& gravity);

	Vec3f getAcceleration(const Vec3f& position, float mass, float t) const override;

protected:
	Vec3f m_vGravity;
};

class ConstantForceField : public ForceField {
public:
	ConstantForceField(const Vec3f& force);

	Vec3f getAcceleration(const Vec3f& position, float mass, float t) const override;
protected:

	Vec3f m_vForce;
};

class RadialForceField : public ForceField {
public:
	RadialForceField(float magnitude);

	Vec3f getAcceleration(const Vec3f& position, float mass, float t) const override;

protected:
	float m_fMagnitude;
};

class VerticalForceField : public ForceField {
public:
	VerticalForceField(float magnitude);

	Vec3f getAcceleration(const Vec3f& position, float mass, float t) const override;

protected:
	float m_fMagnitude;
};

class WindForceField : public ForceField {
public:
	WindForceField(float magnitude);

	Vec3f getAcceleration(const Vec3f& position, float mass, float t) const override;


protected:
	float m_fMagnitude;
};