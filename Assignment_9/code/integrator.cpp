#include "integrator.h"

void Integrator::Update(Particle* particle, ForceField* forcefield, float t, float dt) {
	particle->increaseAge(dt);
}

//pn+1 = pn + vn * dt
//vn+1 = vn + a(pn, t) * dt
void EulerIntegrator::Update(Particle* particle, ForceField* forcefield, float t, float dt) {
	Integrator::Update(particle, forcefield, t, dt);
	const Vec3f& position = particle->getPosition();
	const Vec3f& velocity = particle->getVelocity();
	Vec3f acc = forcefield->getAcceleration(position, particle->getMass(), t);
	particle->setPosition(position + velocity * dt);
	particle->setVelocity(velocity + acc * dt);
}

Vec3f EulerIntegrator::getColor() {
	return Vec3f(1, 0, 0);
}

//pm = pn + vn * dt/2
//vm = vn + a(pn, t) * dt / 2
//pn + 1 = pn + vm * dt
//vn + 1 = vn + a(pm, t + dt / 2) * dt
void MidpointIntegrator::Update(Particle* particle, ForceField* forcefield, float t, float dt) {
	Integrator::Update(particle, forcefield, t, dt);
	const Vec3f& position = particle->getPosition();
	const Vec3f& velocity = particle->getVelocity();
	float mass = particle->getMass();
	Vec3f an = forcefield->getAcceleration(position, mass, t);
	Vec3f pm = position + velocity * 0.5f * dt;
	Vec3f vm = velocity + an * dt * 0.5f;
	Vec3f am = forcefield->getAcceleration(pm, mass, t + dt * 0.5f);
	particle->setPosition(position + vm * dt);
	particle->setVelocity(velocity + am * dt);
}

Vec3f MidpointIntegrator::getColor() {
	return Vec3f(0, 1, 0);
}


void RungeKuttaIntegrator::Update(Particle* particle, ForceField* forcefield, float t, float dt) {
	Integrator::Update(particle, forcefield, t, dt);
	Vec3f pk, vk, ak, vk_sum, ak_sum;
	const Vec3f& position = particle->getPosition();
	const Vec3f& velocity = particle->getVelocity();
	float mass = particle->getMass();

	//k1
	vk_sum = velocity;
	ak_sum = ak = forcefield->getAcceleration(position, mass, t + dt);

	//k2
	pk = position + velocity * dt * 0.5f;
	vk = velocity + ak * dt * 0.5f;
	ak = forcefield->getAcceleration(pk, mass, t + dt * 0.5f);
	
	ak_sum += 2 * ak;
	vk_sum += 2 * vk;

	//k3
	pk = position + vk * dt * 0.5f;
	vk = velocity + ak * dt * 0.5f;
	ak = forcefield->getAcceleration(pk, mass, t + dt * 0.5f);

	ak_sum += 2 * ak;
	vk_sum += 2 * vk;

	//k4
	pk = position + vk * dt;
	vk = velocity + ak * dt;
	ak = forcefield->getAcceleration(pk, mass, t + dt);
	
	ak_sum += ak;
	vk_sum += vk;

	particle->setPosition(position + vk_sum * (dt / 6.f));
	particle->setVelocity(velocity + ak_sum * (dt / 6.f));
}

Vec3f RungeKuttaIntegrator::getColor() {
	return Vec3f(0, 0, 1);
}
