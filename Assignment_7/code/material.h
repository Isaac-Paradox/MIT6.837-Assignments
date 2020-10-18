#pragma once

#include "vectors.h"
#include "ray.h"
#include "hit.h"
#include "glCanvas.h"  
#include <gl/GL.h>

// ====================================================================
// ====================================================================

// You will extend this class in later assignments

class Material {

public:

	// CONSTRUCTORS & DESTRUCTOR
	Material(const Vec3f &d_color) { diffuseColor = d_color; }
	virtual ~Material() {}

	// ACCESSORS
	virtual Vec3f getDiffuseColor() const { return diffuseColor; }

	virtual Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const = 0;

	virtual void glSetMaterial() const = 0;

    virtual const Vec3f& getSpecularColor() const { return Vec3f(0, 0, 0); }

    virtual const Vec3f& getReflectiveColor() const { return Vec3f(0, 0, 0); }

    virtual const Vec3f& getTransparentColor() const { return Vec3f(0, 0, 0); }

    virtual const float getIndexOfRefraction() const { return 0.0f; }
protected:

  // REPRESENTATION
  Vec3f diffuseColor;
  
};

// ====================================================================
// ====================================================================

class PhongMaterial : public Material {
public:
    PhongMaterial(const Vec3f& diffuseColor, const Vec3f& specularColor, float exponent, const Vec3f& reflectiveColor, const Vec3f& transparentColor, float indexOfRefraction);

    void glSetMaterial() const override;

    Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const override;

    const Vec3f& getSpecularColor() const override { return m_vSpecularColor; }

    const Vec3f& getReflectiveColor() const override { return m_vReflectiveColor; }

    const Vec3f& getTransparentColor() const override { return m_vTransparentColor; }

    const float getIndexOfRefraction() const { return m_fIndexOfRefraction; }
protected:
    Vec3f m_vSpecularColor;

    float m_fExponent;

    Vec3f m_vReflectiveColor;

    Vec3f m_vTransparentColor;

    float m_fIndexOfRefraction;
};

class ProceduralTextureMaterial : public Material {
public:
    ProceduralTextureMaterial(Matrix* m, Material* mat1, Material* mat2);

    void glSetMaterial() const override;

protected:
    Vec3f _GetShadePoint(const Hit& hit) const;

protected:
    Material* m_pMaterial_1 = nullptr;
    Material* m_pMaterial_2 = nullptr;
    Matrix* m_pMatrix = nullptr;
};

class Checkerboard : public ProceduralTextureMaterial {
public:
    Checkerboard(Matrix* m, Material* mat1, Material* mat2);

    Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const override;

protected:
    bool _1DCheckerBoard(float x) const;

    bool _3DCheckerBoard(const Vec3f& ip) const;
};

class Noise : public ProceduralTextureMaterial {
public:
    Noise(Matrix* m, Material* mat1, Material* mat2, int octaves);

    Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const override;

protected:
    virtual float _Noise(const Vec3f& ts_coord) const;

    virtual float _GetOffset() const;

    virtual float _GetScale() const;

    Vec3f _InterpolationShading(float alpha, const Vec3f& color1, const Vec3f& color2) const;

protected:
    int m_nOctaves;
};

class Marble : public Noise {
public:
    Marble(Matrix* m, Material* mat1, Material* mat2, int octaves, float frequency, float amplitude);

protected:
    float _Noise(const Vec3f& ts_coord) const override;

    virtual float _GetOffset() const override;

    virtual float _GetScale() const override;
protected:
    float m_fFrequency;

    float m_fAmplitude;
};

class Wood : public Marble {
public:
    Wood(Matrix* m, Material* mat1, Material* mat2, int octaves, float frequency, float amplitude);

protected:
    float _Noise(const Vec3f& ts_coord) const override;

    virtual float _GetOffset() const override;

    virtual float _GetScale() const override;
};