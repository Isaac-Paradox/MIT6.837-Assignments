#pragma once
#include"scene_parser.h"
#include"rayTree.h"
#include"Grid.h"
#include"raytracing_stats.h"
#include<limits>

class RayTracer {
public:
    RayTracer(SceneParser* s, Grid* grid, int max_bounces, float cutoff_weight, bool shadows, bool shade_back, bool cast_transparent_shadows);
		
    Vec3f traceRay(Ray& ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit& hit, bool fast) const;

protected:
    bool _traceShadowRay(Ray& shadow_ray, Hit& shadow_hit, Vec3f& color_mask, Material* last_material, float last_t, bool fast) const;

    Vec3f mirrorDirection(const Vec3f& normal, const Vec3f& incoming) const;

    bool transmittedDirection(const Vec3f& normal, const Vec3f& incoming, float index_i, float index_t, Vec3f& transmitted) const;

protected:
	int m_nMaxBounces;

	float m_fCutoffWeight;

	bool m_bShadows;

    bool m_bTransparentShadows;

    bool m_bShadeBack;

    Grid* m_pGrid;

    SceneParser* m_pScene;

	Group* m_pGroup;

    int m_nLightNum;

    Vec3f m_vAmbientColor;
};