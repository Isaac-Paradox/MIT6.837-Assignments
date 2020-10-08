#include "rayTracer.h"
#include "light.h"

RayTracer::RayTracer(SceneParser* s, 
	Grid* grid, 
	int max_bounces, 
	float cutoff_weight, 
	bool shadows, 
	bool shade_back, 
	bool cast_transparent_shadows) 

	:m_nMaxBounces(max_bounces),
	m_pGrid(grid),
	m_fCutoffWeight(cutoff_weight),
	m_bShadows(shadows),
	m_bShadeBack(shade_back),
	m_pScene(s),
	m_pGroup(s->getGroup()),
	m_nLightNum(s->getNumLights()),
	m_vAmbientColor(s->getAmbientLight()),
	m_bTransparentShadows(cast_transparent_shadows) {}

Vec3f RayTracer::traceRay(Ray& ray, 
    float tmin, 
    int bounces, 
    float weight, 
    float indexOfRefraction, 
    Hit& hit, 
    bool fast) const {

    Vec3f color(0, 0, 0);
    RayTracingStats::IncrementNumNonShadowRays();
    Object3D* obj = fast ? (Object3D*)m_pGrid : (Object3D*)m_pGroup;
    if (obj->intersect(ray, hit, tmin)) {
        Vec3f normal = hit.getNormal();
        bool hit_back = !std::signbit(ray.getDirection().Dot3(normal));
        if (!hit_back || m_bShadeBack) {
            if (hit_back) { normal *= -1; }
            Vec3f intersection_point = hit.getIntersectionPoint();
            color = m_vAmbientColor * hit.getMaterial()->getDiffuseColor();
            for (int l = 0; l < m_nLightNum; l++) {
                Light* light = m_pScene->getLight(l);
                Vec3f light_dir, light_col;
                float dist_2_light;
                light->getIllumination(intersection_point, light_dir, light_col, dist_2_light);
                if (m_bShadows) {
                    Ray shadowRay(intersection_point, light_dir);
                    Hit shadowHit(dist_2_light, nullptr, Vec3f(0, 0, -1.0f));
                    RayTracingStats::IncrementNumShadowRays();
                    if (!obj->intersect(shadowRay, shadowHit, 1e-3)) {
                        color += hit.getMaterial()->Shade(ray, hit, light_dir, light_col);
                    }
                    RayTree::AddShadowSegment(shadowRay, 0, shadowHit.getT());

                } else if (m_bTransparentShadows) {
                    Ray shadowRay(intersection_point, light_dir);
                    Hit shadowHit(dist_2_light, nullptr, Vec3f(0, 0, -1.0f));
                    Vec3f color_mask(1.0f, 1.0f, 1.0f);
                    RayTracingStats::IncrementNumShadowRays();
                    if (_traceShadowRay(shadowRay, shadowHit, color_mask, nullptr, 0, fast)) {
                        color += color_mask * hit.getMaterial()->Shade(ray, hit, light_dir, light_col);
                    }
                    RayTree::AddShadowSegment(shadowRay, 0, shadowHit.getT());
                } else {
                    color += hit.getMaterial()->Shade(ray, hit, light_dir, light_col);
                }
            }

            const Vec3f& reflectiveColor = hit.getMaterial()->getReflectiveColor();
            if (reflectiveColor.Length() > 0 && bounces < m_nMaxBounces && weight >= m_fCutoffWeight) {
                Vec3f mirrorDir = mirrorDirection(normal, ray.getDirection());
                mirrorDir.Normalize();
                Ray refractionRay(intersection_point, mirrorDir);
                Hit refractionHit((numeric_limits<float>::max)(), nullptr, Vec3f(0, 0, -1.0f));
                RayTracingStats::IncrementNumNonShadowRays();
                color += reflectiveColor * traceRay(refractionRay, tmin, bounces + 1, weight * reflectiveColor.Length(), indexOfRefraction, refractionHit, fast);//反射是返回相同介质，应该使用同样的折射系数
                RayTree::AddReflectedSegment(refractionRay, 0, refractionHit.getT());
            }

            const Vec3f& transparentColor = hit.getMaterial()->getTransparentColor();
            if (transparentColor.Length() > 0) {
                float index_i;
                float index_t;
                if (!hit_back) {
                    index_i = indexOfRefraction;
                    index_t = hit.getMaterial()->getIndexOfRefraction();
                } else {
                    index_i = hit.getMaterial()->getIndexOfRefraction();
                    index_t = indexOfRefraction;
                }
                Vec3f transmitted;
                if (bounces < m_nMaxBounces && weight >= m_fCutoffWeight && transmittedDirection(normal, ray.getDirection(), index_i, index_t, transmitted)) {
                    Ray transparentRay(intersection_point, transmitted);
                    Hit transparentHit((numeric_limits<float>::max)(), nullptr, Vec3f(0, 0, -1.0f));
                    RayTracingStats::IncrementNumNonShadowRays();
                    color += transparentColor * traceRay(transparentRay, tmin, bounces + 1, weight * transparentColor.Length(), index_i, transparentHit, fast);
                    RayTree::AddTransmittedSegment(transparentRay, 0, transparentHit.getT());
                }
            }
        }
        return color;
    } else {
        return m_pScene->getBackgroundColor();
    }
}

bool RayTracer::_traceShadowRay(Ray& shadow_ray, 
    Hit& shadow_hit, 
    Vec3f& color_mask, 
    Material* last_material, 
    float last_t, 
    bool fast) const {

    Object3D* obj = fast ? (Object3D*)m_pGrid : (Object3D*)m_pGroup;
    float light_dist = shadow_hit.getT();

    if (obj->intersect(shadow_ray, shadow_hit, last_t + 5e-5f)) {
        Material* hitMaterial = shadow_hit.getMaterial();
        if (hitMaterial->getTransparentColor().Length() > 0) {
            if (last_material == hitMaterial) {
                float distance = (shadow_hit.getT() - last_t);
                //color_mask = color_mask * distance * hitMaterial->getTransparentColor();
                color_mask = color_mask *
                    (Vec3f(1, 1, 1) * (1 - distance) + distance * hitMaterial->getTransparentColor());//why?
            }
            last_t = shadow_hit.getT();
            shadow_hit.set(light_dist, nullptr, Vec3f(), shadow_ray);
            return _traceShadowRay(shadow_ray, shadow_hit, color_mask, hitMaterial, last_t, fast);
        } else {
            return false;
        }
    } else {
        return true;
    }
}

Vec3f RayTracer::mirrorDirection(const Vec3f& normal, const Vec3f& incoming) const { return 2 * normal.Dot3(-1 * incoming) * normal + incoming; }

bool RayTracer::transmittedDirection(const Vec3f& normal,
    const Vec3f& incoming,
    float index_i,
    float index_t,
    Vec3f& transmitted) const {

    //I=-incoming
    float cos_i = normal.Dot3(-1.0f * incoming);
    float eta_r = index_i / index_t;
    float squ_cos_t = 1.0f - eta_r * eta_r * (1.0f - cos_i * cos_i);
    if (squ_cos_t < 0) { return false; }
    transmitted = normal * (eta_r * cos_i - std::sqrt(squ_cos_t)) + incoming * eta_r;
    transmitted.Normalize();
    return true;
}
