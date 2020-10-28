#include "generator.h"

#include<GL/glut.h>


void Generator::SetColors(const Vec3f& color, const Vec3f& dead_color, float color_randomness) {
	m_vColor = color;
	m_vDeadColor = dead_color;
	m_fColorRandomness = color_randomness;
}

void Generator::SetLifespan(float lifespan, float lifespan_randomness, int desired_num_particles) {
	m_fLifespan = lifespan;
	m_fLifespanRandomness = lifespan_randomness;
	m_nDesiredNumParticles = desired_num_particles;
}

void Generator::SetMass(float mass, float mass_randomness) {
	m_fMass = mass;
	m_fMassRandomness = mass_randomness;
}

HoseGenerator::HoseGenerator(const Vec3f& position, 
	float position_randomness, 
	const Vec3f& velocity, 
	float velocity_randomness) :
	m_vPosition(position),
	m_fPositionRandomness(position_randomness),
	m_vVelocity(velocity),
	m_fVelocityRandomness(velocity_randomness){}

int HoseGenerator::numNewParticles(float current_time, float dt) const {
	return std::ceilf(dt * m_nDesiredNumParticles / m_fLifespan);
}

Particle* HoseGenerator::Generate(float current_time, int i) {
	return _Generate(m_vPosition + _GetRandomVec3f(m_fPositionRandomness),
		m_vVelocity + _GetRandomVec3f(m_fVelocityRandomness));
}

RingGenerator::RingGenerator(float position_randomness, 
	Vec3f velocity, 
	float velocity_randomness) :
	m_fPositionRandomness(position_randomness),
	m_vVelocity(velocity),
	m_fVelocityRandomness(velocity_randomness) {}

int RingGenerator::numNewParticles(float current_time, float dt) const {
	return std::ceilf(current_time * dt * m_nDesiredNumParticles / m_fLifespan);
}

Particle* RingGenerator::Generate(float current_time, int i) {
	float radius = current_time / m_fLifespan;
	float random_angle = _GetRandomfloat(2.f * 3.1415926f);
	return _Generate(Vec3f(radius * std::cos(random_angle), 0.f, radius * std::sin(random_angle)), m_vVelocity + _GetRandomVec3f(m_fVelocityRandomness));
}

void RingGenerator::Paint() const {
	glColor3f(0.3f, 0.3f, 0.3f);
	glBegin(GL_QUADS);
	glVertex3f(5.f, 0.f, 5.f);
	glVertex3f(-5.f, 0.f, 5.f);
	glVertex3f(-5.f, 0.f, -5.f);
	glVertex3f(5.f, 0.f, -5.f);
	glEnd();
}
