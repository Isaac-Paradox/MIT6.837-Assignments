#ifndef _MATERIAL_H_
#define _MATERIAL_H_

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
    PhongMaterial(const Vec3f& diffuseColor,
        const Vec3f& specularColor,
        float exponent,
        const Vec3f& reflectiveColor,
        const Vec3f& transparentColor,
        float indexOfRefraction)
        : Material(diffuseColor), 
        m_vSpecularColor(specularColor), 
        m_fExponent(exponent),
        m_vReflectiveColor(reflectiveColor),
        m_vTransparentColor(transparentColor),
        m_fIndexOfRefraction(indexOfRefraction){}

    void glSetMaterial() const override {
        GLfloat one[4] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat zero[4] = { 0.0, 0.0, 0.0, 0.0 };
        GLfloat specular[4] = {
          getSpecularColor().r(),
          getSpecularColor().g(),
          getSpecularColor().b(),
          1.0 };
        GLfloat diffuse[4] = {
          getDiffuseColor().r(),
          getDiffuseColor().g(),
          getDiffuseColor().b(),
          1.0 };

        // NOTE: GL uses the Blinn Torrance version of Phong...      
        float glexponent = m_fExponent;
        if (glexponent < 0) glexponent = 0;
        if (glexponent > 128) glexponent = 128;

#if !SPECULAR_FIX 

        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

#else

        // OPTIONAL: 3 pass rendering to fix the specular highlight 
        // artifact for small specular exponents (wide specular lobe)

        if (SPECULAR_FIX_WHICH_PASS == 0) {
            // First pass, draw only the specular highlights
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

        } else if (SPECULAR_FIX_WHICH_PASS == 1) {
            // Second pass, compute normal dot light 
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
        } else {
            // Third pass, add ambient & diffuse terms
            assert(SPECULAR_FIX_WHICH_PASS == 2);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
        }

#endif
    };

    Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const override {
        Vec3f normal = (ray.getDirection().Dot3(hit.getNormal()) > 0 ? -1 : 1) * hit.getNormal();
        float cosine_alpha = normal.Dot3(dirToLight);
        Vec3f view = ray.getDirection();
        view *= -1.0f;
        Vec3f half = (view + dirToLight);
        half.Normalize();
        float cosine_beta = half.Dot3(normal);
        Vec3f _out(0, 0, 0);
        if (cosine_alpha > 0) {
            _out += cosine_alpha * diffuseColor;
            if (cosine_beta > 0) {
#if !SPECULAR_FIX 
                _out += std::pow(cosine_beta, m_fExponent) * m_vSpecularColor;
#else
                _out += cosine_alpha * std::pow(cosine_beta, m_fExponent) * m_vSpecularColor;
#endif
            }
        }

        return _out * lightColor;
    }

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

#endif
