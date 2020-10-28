#pragma once
#include"particle.h"
#include"forcefield.h"

class Integrator {
public:
	virtual void Update(Particle* particle, ForceField* forcefield, float t, float dt);

	virtual Vec3f getColor() = 0;
};

class EulerIntegrator : public Integrator {
public:
	void Update(Particle* particle, ForceField* forcefield, float t, float dt) override;

	Vec3f getColor() override;
};

class MidpointIntegrator : public Integrator {
public:
	void Update(Particle* particle, ForceField* forcefield, float t, float dt) override;

	Vec3f getColor() override;
};

class RungeKuttaIntegrator : public Integrator {
public:
	void Update(Particle* particle, ForceField* forcefield, float t, float dt) override;

	Vec3f getColor() override;
};