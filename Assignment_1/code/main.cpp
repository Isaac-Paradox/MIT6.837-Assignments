#include <iostream>
#include <cassert>
#include <limits>

#include "camera.h"
#include "group.h"
#include "material.h"
#include "scene_parser.h"
#include "image.h"


int main(int argc, char** argv) {
    char* input_file = NULL;
    int width = 100;
    int height = 100;
    char* output_file = NULL;
    float depth_min = 0;
    float depth_max = 1;
    char* depth_file = NULL;

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
        } else if (!strcmp(argv[i], "-depth")) {
            i++; assert(i < argc);
            depth_min = atof(argv[i]);
            i++; assert(i < argc);
            depth_max = atof(argv[i]);
            i++; assert(i < argc);
            depth_file = argv[i];
        } else {
            printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }

    SceneParser scene(input_file);

    Camera* camera = scene.getCamera();
    Group* group = scene.getGroup();
    Image output(width, height), depth(width, height);
    output.SetAllPixels(scene.getBackgroundColor());
    depth.SetAllPixels(Vec3f(0, 0, 0));
    Vec2f point(0, 0);
    float deltax = 1.0f / width, deltay = 1.0f / height;
    float tmin = camera->getTMin(), depthColor;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            point.Set(j * deltax, i * deltay);
            Ray ray = camera->generateRay(point);
            Hit hit((numeric_limits<float>::max)(), nullptr);
            if (depth_min > tmin) {
                if (group->intersect(ray, hit, tmin)) {
                    output.SetPixel(j, i, hit.getMaterial()->getDiffuseColor());
                    if (hit.getT() >= depth_min) {
                        depthColor = (depth_max - hit.getT()) / (depth_max - depth_min);
                        depth.SetPixel(j, i, Vec3f(depthColor, depthColor, depthColor));
                    }
                }
            } else {
                if (group->intersect(ray, hit, depth_min)) {
                    depthColor = (depth_max - hit.getT()) / (depth_max - depth_min);
                    depth.SetPixel(i, j, Vec3f(depthColor, depthColor, depthColor));
                    if (hit.getT() >= tmin) {
                        output.SetPixel(i, j, hit.getMaterial()->getDiffuseColor());
                    }
                }
            }
        }
    }
    output.SaveTGA(output_file);
    depth.SaveTGA(depth_file);
}
