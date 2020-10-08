#include "material.h"
#include"perlin_noise.h"
#include"matrix.h"
#include<cmath>

PhongMaterial::PhongMaterial(const Vec3f& diffuseColor, 
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
    m_fIndexOfRefraction(indexOfRefraction) {}

void PhongMaterial::glSetMaterial() const {
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

Vec3f PhongMaterial::Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const {
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

ProceduralTextureMaterial::ProceduralTextureMaterial(Matrix* m, Material* mat1, Material* mat2) 
    : Material(Vec3f()), m_pMaterial_1(mat1), m_pMaterial_2(mat2), m_pMatrix(m) {}

void ProceduralTextureMaterial::glSetMaterial() const { m_pMaterial_1->glSetMaterial(); }

Vec3f ProceduralTextureMaterial::_GetShadePoint(const Hit& hit) const {
    Vec3f sp = hit.getIntersectionPoint();
    m_pMatrix->Transform(sp);
    return sp;
}

Checkerboard::Checkerboard(Matrix* m, Material* mat1, Material* mat2)
    : ProceduralTextureMaterial(m, mat1, mat2){}


Vec3f Checkerboard::Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const {
    Material* mat = _3DCheckerBoard(_GetShadePoint(hit)) ? m_pMaterial_1 : m_pMaterial_2;
    return mat->Shade(ray, hit, dirToLight, lightColor);
}

bool Checkerboard::_1DCheckerBoard(float x) const{
    return ((int)std::floorf(x) % 2) == 0;
}

bool Checkerboard::_3DCheckerBoard(const Vec3f& ip) const{
    return _1DCheckerBoard(ip.x())^ _1DCheckerBoard(ip.y())^ _1DCheckerBoard(ip.z());
}

Noise::Noise(Matrix* m, Material* mat1, Material* mat2, int octaves) 
    : ProceduralTextureMaterial(m, mat1, mat2), m_nOctaves(octaves) { }

Vec3f Noise::Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const {
    return _InterpolationShading(_Noise(_GetShadePoint(hit)), m_pMaterial_1->Shade(ray, hit, dirToLight, lightColor)
        , m_pMaterial_2->Shade(ray, hit, dirToLight, lightColor));
}

float Noise::_Noise(const Vec3f& ts_coord) const {
    double _out = 0;
    for (int i = 0; i < m_nOctaves; i++) {
        int sample_period = 1;
        for (int j = 0; j < i; j++) {
            sample_period *= 2;
        }
        _out += PerlinNoise::noise((double)sample_period * ts_coord.x(), (double)sample_period * ts_coord.y(), (double)sample_period * ts_coord.z()) / sample_period;
    }
    return _out;
}

float Noise::_GetOffset() const { return 0.5f; }

float Noise::_GetScale() const { return 1.0f; }

Vec3f Noise::_InterpolationShading(float alpha, const Vec3f& color1, const Vec3f& color2) const {
    alpha = std::fmax(0.f, std::fmin(1.f, (_GetOffset() - alpha))) * _GetScale();
    return (1 - alpha) * color1 + alpha * color2;
}

Marble::Marble(Matrix* m, Material* mat1, Material* mat2, int octaves, float frequency, float amplitude) 
    : Noise(m, mat1, mat2, octaves), m_fFrequency(frequency), m_fAmplitude(amplitude) { }

float Marble::_Noise(const Vec3f& ts_coord) const {
    return std::sinf(m_fFrequency * ts_coord.x() + m_fAmplitude * Noise::_Noise(ts_coord));
}

float Marble::_GetOffset() const { return 0.5f; }

float Marble::_GetScale() const { return 1.0f; }

Wood::Wood(Matrix* m, Material* mat1, Material* mat2, int octaves, float frequency, float amplitude)
    : Marble(m, mat1, mat2, octaves, frequency, amplitude) {}

float Wood::_Noise(const Vec3f& ts_coord) const {
    float l = ts_coord.x() * ts_coord.x() + ts_coord.z() * ts_coord.z();
    float s = sinf(m_fFrequency * sqrtf(l) + m_fAmplitude * Noise::_Noise(ts_coord) + ts_coord.y() * 0.4f);
    float sign = s < 0 ? -1.f : 1.f;
    s = fabsf(s);
    return sign * (1 - std::powf(1 - s, m_fFrequency));
}

float Wood::_GetOffset() const { return 0.5f; }

float Wood::_GetScale() const { return 1.0f; }