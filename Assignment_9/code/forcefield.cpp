#include "forcefield.h"
#include"perlin_noise.h"

GravityForceField::GravityForceField(const Vec3f& gravity) 
    : m_vGravity(gravity) {}

Vec3f GravityForceField::getAcceleration(const Vec3f& position, float mass, float t) const {
    return m_vGravity;
}

ConstantForceField::ConstantForceField(const Vec3f& force) 
    : m_vForce(force){}

Vec3f ConstantForceField::getAcceleration(const Vec3f& position, float mass, float t) const {
    return m_vForce * (1 / mass);
}

RadialForceField::RadialForceField(float magnitude) 
    : m_fMagnitude(magnitude){}

Vec3f RadialForceField::getAcceleration(const Vec3f& position, float mass, float t) const {
    return (-m_fMagnitude / mass) * position;
}

VerticalForceField::VerticalForceField(float magnitude) 
    : m_fMagnitude(magnitude) {}

Vec3f VerticalForceField::getAcceleration(const Vec3f& position, float mass, float t) const {
    return Vec3f(0, position.y(), 0) * (-m_fMagnitude / mass);
}

WindForceField::WindForceField(float magnitude) 
    : m_fMagnitude(magnitude) {}

Vec3f WindForceField::getAcceleration(const Vec3f& position, float mass, float t) const {
    float nx = PerlinNoise::noise(t, position.y(), position.z()) * 3.f;
    float ny = PerlinNoise::noise(position.x(), t, position.z()) + 0.5f;
    float nz = PerlinNoise::noise(position.x(), position.y(), t);
    return Vec3f(nx, ny, nz) * (m_fMagnitude / mass);
}
