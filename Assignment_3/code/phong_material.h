#pragma once
#include"material.h"

class PhongMaterial : public Material {
public:
	PhongMaterial(Vec3f diffuseColor, Vec3f specularColor, float exponent) 
		: Material(diffuseColor), m_vSpecularColor(specularColor), m_fExponent(exponent){ }

	void glSetMaterial() const override;
protected:
	const Vec3f& getSpecularColor() const { return m_vSpecularColor; }

	Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const override;
	
protected:
	Vec3f m_vSpecularColor;
	float m_fExponent;
};