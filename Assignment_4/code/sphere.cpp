#include"sphere.h"

uint32_t Sphere::s_nThetaSteps = 20;
uint32_t Sphere::s_nPhiSteps = 20;
float Sphere::s_fThetaDelta = (M_PI * 2) / s_nThetaSteps;
float Sphere::s_fPhiDelta = M_PI / s_nPhiSteps;
bool Sphere::s_bGouraud = false;