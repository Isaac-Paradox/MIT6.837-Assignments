#include"scene_parser.h"
#include"sphere.h"
#include"glCanvas.h"
#include"image.h"
#include"camera.h"
//#include"ray.h"
#include"hit.h"
#include"group.h"
#include"light.h"
//#include"material.h"
#include"rayTracer.h"
#include"rayTree.h"
#include"Grid.h"
#include"raytracing_stats.h"

#include<limits>
#include <cassert>
#include<iostream>
#include<GL/freeglut.h>

void RenderFunction();
void TraceRayFunction(float, float);
void TestRayFunction(float, float);

SceneParser *scene;
RayTracer* raytracer;

int width = 100;
int height = 100;

char* output_file = NULL;
//float depth_min = 0;
//float depth_max = 1;
//char* depth_file = NULL;
char* normals_file = NULL;
//bool output_depth = false;
bool output_normals = false;

bool visualize_grid = false;

bool has_grid = false;

int test_x = -1, test_y = -1;

int main(int argc, char** argv) {
    char* input_file = NULL;

    bool shade_back = false;
    bool cast_shadow = false;
    bool cast_transparent_shadows = false;
    int bounces = 1;
    float weight = 0;

    bool gui = false;

    int nx = 1, ny = 1, nz = 1;

    Grid* grid = nullptr;
    
    bool stats = false;

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
        } else if (!strcmp(argv[i], "-gui")) {
            gui = true;
        } else if (!strcmp(argv[i], "-tessellation")) {
            i++; assert(i < argc);
            int theta_step = atoi(argv[i]);
            i++; assert(i < argc);
            int phi_step = atoi(argv[i]);
            Sphere::SetSphereSteps(theta_step, phi_step);
        } else if (!strcmp(argv[i], "-gouraud")) {
            Sphere::s_bGouraud = true;
        } else if (!strcmp(argv[i], "-shade_back")) {
            shade_back = true;
        } else if (!strcmp(argv[i], "-shadows")) {
            cast_shadow = true;
        } else if (!strcmp(argv[i], "-bounces")) {
            i++; assert(i < argc);
            bounces = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-weight")) {
            i++; assert(i < argc);
            weight = atof(argv[i]);
        }else if (!strcmp(argv[i], "-normals")) {
            i++; assert(i < argc);
            normals_file = argv[i];
            output_normals = true;
        } else if (!strcmp(argv[i], "-grid")) {
            i++; assert(i < argc);
            nx = atoi(argv[i]);
            i++; assert(i < argc);
            ny = atoi(argv[i]);
            i++; assert(i < argc);
            nz = atoi(argv[i]);
            has_grid = true;
        } else if (!strcmp(argv[i], "-visualize_grid")) {
            visualize_grid = true;
        } else if (!strcmp(argv[i], "-transparent_shadows")) {
            cast_transparent_shadows = true;
        }
        else if (!strcmp(argv[i], "-stats")) {
            stats = true;
        }
        //else if (!strcmp(argv[i], "-depth")) {
        //    i++; assert(i < argc);
        //    depth_min = atof(argv[i]);
        //    i++; assert(i < argc);
        //    depth_max = atof(argv[i]);
        //    i++; assert(i < argc);
        //    depth_file = argv[i];
        //    output_depth = true;
        //} 
        else {
            printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }

    scene = new SceneParser(input_file);
    Group& group = *scene->getGroup();
    if (has_grid) {
        grid = new Grid(group.getBoundingBox(), nx, ny, nz);
        grid->SetGridVisualize(visualize_grid);
        group.insertIntoGrid(grid, nullptr);
    } 
    RayTracingStats::Initialize(width, height, group.getBoundingBox(), nx, ny, nz);

    raytracer = new RayTracer(scene,
        grid,
        bounces, 
        weight,
        cast_shadow, 
        shade_back, 
        cast_transparent_shadows);

    if (gui) {
        glutInit(&argc, argv);
        GLCanvas canvas;
        //canvas.initialize(scene, 
        //    RenderFunction, 
        //    TraceRayFunction,
        //    grid,
        //    visualize_grid);
        canvas.initialize(scene,
            RenderFunction,
            TestRayFunction,
            grid,
            visualize_grid);
    } else {
        RenderFunction();
        if (stats) {
            RayTracingStats::PrintStatistics();
        }
    }
    return 0;
}

void RenderFunction() {
    Camera* camera = scene->getCamera();
    Image output(width, height);
    Image* normals = nullptr;
    if (output_normals) {
        normals = new Image(width, height);
    }
    camera->rtInit();
    Vec2f point(0, 0);
    int max_size = (std::max)(width, height);
    float max_delta = 1.f / max_size;
    float margin_x = 0.5f * (max_size - width), margin_y = 0.5f * (max_size - height);
    float tmin = camera->getTMin();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            point.Set((x + margin_x), (y + margin_y));
            point *= max_delta;
            Ray ray = camera->generateRay(point);
            Hit hit((numeric_limits<float>::max)(), nullptr, Vec3f(0, 0, -1.0f));

            //if (x == test_x && y == height - 1 - test_y) {
            //    raytracer->traceRay(ray, tmin, 0, 1, 1, hit, has_grid);
            //    output.SetPixel(x, y, Vec3f(1.f, 1.f, 1.f));
            //}else
            output.SetPixel(x, y, raytracer->traceRay(ray, tmin, 0, 1, 1, hit, has_grid));
            
            if (output_normals) {
                Vec3f normal = hit.getNormal();
                Vec3f normal_color(abs(normal.r()), abs(normal.g()), abs(normal.b()));
                normals->SetPixel(x, y, normal_color);
            }
        }
    }
    output.SaveTGA(output_file);
    if (output_normals) {
        normals->SaveTGA(normals_file);
        delete normals;
    }

    //if (output_depth) {
    //    Image depth(width, height);
    //    depth.SetAllPixels(Vec3f(0, 0, 0));
    //    float depthColor;
    //    for (int i = 0; i < height; i++) {
    //        for (int j = 0; j < width; j++) {
    //            point.Set(j * deltax, i * deltay);
    //            Ray ray = camera->generateRay(point);
    //            Hit hit(depth_max, nullptr, Vec3f(0, 0, -1.0f));
    //            if (group->intersect(ray, hit, depth_min)) {
    //                depthColor = (depth_max - hit.getT()) / (depth_max - depth_min);
    //                depth.SetPixel(j, i, Vec3f(depthColor, depthColor, depthColor));
    //            }
    //        }
    //    }
    //    depth.SaveTGA(depth_file);
    //}
}

void TraceRayFunction(float x, float y) {
    //std::cout << "x:" << x << " y:" << y << std::endl;
    Vec2f point(x, y);
    Ray ray = scene->getCamera()->generateRay(point);
    Hit hit((numeric_limits<float>::max)(), nullptr, Vec3f(0, 0, -1.0f));
    raytracer->traceRay(ray, scene->getCamera()->getTMin(), 0, 1, 1, hit, has_grid);
    RayTree::SetMainSegment(ray, 0, visualize_grid ? (std::numeric_limits<float>().max)() : hit.getT());
}

void TestRayFunction(float x, float y) {
    Vec2f point((float)test_x / width, (float)(height - 1 - test_y) / height);
    Ray ray = scene->getCamera()->generateRay(point);
    Hit hit((numeric_limits<float>::max)(), nullptr, Vec3f(0, 0, -1.0f));
    raytracer->traceRay(ray, scene->getCamera()->getTMin(), 0, 1, 1, hit, has_grid);
    RayTree::SetMainSegment(ray, 0, visualize_grid ? (std::numeric_limits<float>().max)() : hit.getT());
}
