#pragma once
#include"vectors.h"
#include"particle.h"
#include"random.h"

class Generator {
public:
	// initialization
	void SetColors(const Vec3f& color, const Vec3f& dead_color, float color_randomness);
	void SetLifespan(float lifespan, float lifespan_randomness, int desired_num_particles);
	void SetMass(float mass, float mass_randomness);

	// on each timestep, create some particles
	virtual int numNewParticles(float current_time, float dt) const = 0;
	virtual Particle* Generate(float current_time, int i) = 0;

	// for the gui
	virtual void Paint() const {};
	void Restart() { m_rRandom = Random(); };
protected:
	inline Vec3f _GetColor(){ return m_vColor + _GetRandomVec3f(m_fColorRandomness); }

	inline float _GetLifeSpan(){ return m_fLifespan + _GetRandomfloat(m_fLifespanRandomness); }

	inline float _GetMass(){ return m_fMass + _GetRandomfloat(m_fLifespanRandomness); }

	inline Particle* _Generate(const Vec3f& p, const Vec3f& v) { return new Particle(p, v, _GetColor(), m_vDeadColor, _GetMass(), _GetLifeSpan()); }

	inline Vec3f _GetRandomVec3f(float randomness) { return randomness * m_rRandom.randomVector(); }

	inline float _GetRandomfloat(float randomness) { return randomness * m_rRandom.next(); }

protected:

	Vec3f m_vColor;
	Vec3f m_vDeadColor;
	float m_fColorRandomness = 0.f;

	float m_fLifespan = 1.f;
	float m_fLifespanRandomness = 0.f;
	int m_nDesiredNumParticles = 10;

	float m_fMass = 1.f;
	float m_fMassRandomness = 0.f;

	Random m_rRandom;
};

class HoseGenerator : public Generator {
public:
	HoseGenerator(const Vec3f& position, float position_randomness, const Vec3f& velocity, float velocity_randomness);

	int numNewParticles(float current_time, float dt) const override;

	Particle* Generate(float current_time, int i) override;

protected:
	Vec3f m_vPosition;
	float m_fPositionRandomness;

	Vec3f m_vVelocity;
	float m_fVelocityRandomness;
};

class RingGenerator : public Generator {
public:
	RingGenerator(float position_randomness, Vec3f velocity, float velocity_randomness);

	int numNewParticles(float current_time, float dt) const override;

	Particle* Generate(float current_time, int i) override;

	void Paint() const override;
protected:

	float m_fPositionRandomness;

	Vec3f m_vVelocity;
	float m_fVelocityRandomness;
};