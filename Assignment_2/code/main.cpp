#include <iostream>
#include <cassert>
#include <limits>

#include "camera.h"
#include "group.h"
#include "material.h"
#include "scene_parser.h"
#include "image.h"
#include "light.h"


int main(int argc, char** argv)
{
    char* input_file = NULL;
    int width = 100;
    int height = 100;
    char* output_file = NULL;
    float depth_min = 0;
    float depth_max = 1;
    char* depth_file = NULL;
    char* normals_file = NULL;
    bool shade_back = false;
    bool output_depth = false;
    bool output_normals = false;
    // sample command line:
// raytracer -input scene1_1.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-input")) {
            i++; assert(i < argc);
            input_file = argv[i];
        } else if (!strcmp(argv[i], "-size")) {
            i++; assert(i < argc);
            width = atoi(argv[i]);
            i++; assert(i < argc);
            height = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-output")) {
            i++; assert(i < argc);
            output_file = argv[i];
        } else if (!strcmp(argv[i], "-normals")) {
            i++; assert(i < argc);
            normals_file = argv[i];
            output_normals = true;
        } else if (!strcmp(argv[i], "-shade_back")) {
            shade_back = true;
        } else if (!strcmp(argv[i], "-depth")) {
            i++; assert(i < argc);
            depth_min = atof(argv[i]);
            i++; assert(i < argc);
            depth_max = atof(argv[i]);
            i++; assert(i < argc);
            depth_file = argv[i];
            output_depth = true;
        } else {
            printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }

    SceneParser scene(input_file);
    
    Camera *camera = scene.getCamera();
    Group* group = scene.getGroup();
    Image output(width, height);
    Image* normals = nullptr;
    if (output_normals) {
        normals = new Image(width, height);
    }
    output.SetAllPixels(scene.getBackgroundColor());
    Vec2f point(0, 0);
    float deltax = 1.0f / width, deltay = 1.0f / height;
    float tmin = camera->getTMin();
    Vec3f color;
    Vec3f ambient = scene.getAmbientLight();
    int light_num = scene.getNumLights();
    bool hit_back;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            point.Set(j * deltax, i * deltay);
            Ray ray = camera->generateRay(point);
            Hit hit(numeric_limits<float>::max(), nullptr, Vec3f(0, 0, -1.0f));
            if (group->intersect(ray, hit, tmin)) {
                hit_back = ray.getDirection().Dot3(hit.getNormal()) > 0;
                if (hit_back && !shade_back) {
                    color.Set(0, 0, 0);
                } else {
                    color = ambient * hit.getMaterial()->getDiffuseColor();
                    for (int l = 0; l < light_num; l++) {
                        Light* light = scene.getLight(l);
                        Vec3f lp, dir, col;
                        light->getIllumination(lp, dir, col);
                        //Ray shadowRay(hit.getIntersectionPoint(), dir);
                        //Hit shadowHit(numeric_limits<float>::max(), nullptr, Vec3f(0, 0, -1.0f));
                        //if (!group->intersectShadowRay(shadowRay, shadowHit, 1e-3)) {
                        Vec3f normal = hit.getNormal();
                        if (hit_back) {
                            normal *= -1.0f;
                        }
                        float cosine = normal.Dot3(dir);
                        if (cosine > 0) {
                            color += cosine * col * hit.getMaterial()->getDiffuseColor();
                        }
                        //}
                    }
                }

                output.SetPixel(j, i, color);
                if (output_normals) {
                    Vec3f normal = hit.getNormal();
                    Vec3f normal_color(abs(normal.r()), abs(normal.g()), abs(normal.b()));
                    normals->SetPixel(j, i, normal_color);
                }
            }
        }
    }
    output.SaveTGA(output_file);
    if (output_normals) {
        normals->SaveTGA(normals_file);
        delete normals;
    }

    if (output_depth) {
        Image depth(width, height);
        depth.SetAllPixels(Vec3f(0, 0, 0));
        float depthColor;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                point.Set(j * deltax, i * deltay);
                Ray ray = camera->generateRay(point);
                Hit hit(depth_max, nullptr, Vec3f(0, 0, -1.0f));  
                if (group->intersect(ray, hit, depth_min)) {
                    depthColor = (depth_max - hit.getT()) / (depth_max - depth_min);
                    depth.SetPixel(j, i, Vec3f(depthColor, depthColor, depthColor));
                }
            }
        }
        depth.SaveTGA(depth_file);
    }

    return 0;
}
