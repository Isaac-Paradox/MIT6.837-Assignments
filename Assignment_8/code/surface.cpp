#include "surface.h"
#include"triangle_mesh.h"
#include"curve.h"

SurfaceOfRevolution::SurfaceOfRevolution(Curve* curve)
    :m_pCurve(curve){}

TriangleMesh* SurfaceOfRevolution::OutputTriangles(ArgParser* args) {
    std::vector<Vec3f> curve_points;
    m_pCurve->GetCurvePoints(curve_points, args->curve_tessellation);

    TriangleNet* net = new TriangleNet(curve_points.size() - 1, args->revolution_tessellation);

    float theta = 0;
    float delta_theta = 2.f * 3.1415926f / args->revolution_tessellation;
    for (int r = 0; r <= args->revolution_tessellation; r++) {
        for (int c = 0, c_length = curve_points.size(); c < c_length; c++) {
            const Vec3f& p = curve_points[c];
            net->SetVertex(c, r, Vec3f(std::cos(theta) * p.x(), p.y(), std::sin(theta) * p.x()));
        }
        theta += delta_theta;
    }
   
    return net;
}

void BezierPatch::Paint(ArgParser* args) {
    {
        std::vector<Vec3f> ctl_points{ m_vControlPoint[0],
            m_vControlPoint[1],
            m_vControlPoint[2],
            m_vControlPoint[3],
            m_vControlPoint[7],
            m_vControlPoint[11],
            m_vControlPoint[15],
            m_vControlPoint[14],
            m_vControlPoint[13],
            m_vControlPoint[12],
            m_vControlPoint[8],
            m_vControlPoint[4],
        };
        PaintControlLine(ctl_points);
        PaintPoints(ctl_points);
    }
    {
        std::vector<Vec3f> ctl_points(m_vControlPoint.data() + 4, m_vControlPoint.data() + 7);
        PaintControlLine(ctl_points);
        PaintPoints(ctl_points);
    }
    {
        std::vector<Vec3f> ctl_points(m_vControlPoint.data() + 8, m_vControlPoint.data() + 11);
        PaintControlLine(ctl_points);
        PaintPoints(ctl_points);
    }
    {
        std::vector<Vec3f> ctl_points{ m_vControlPoint[1], m_vControlPoint[5], m_vControlPoint[9], m_vControlPoint[13] };
        PaintControlLine(ctl_points);
        PaintPoints(ctl_points);
    }
    {
        std::vector<Vec3f> ctl_points{ m_vControlPoint[2], m_vControlPoint[6], m_vControlPoint[10], m_vControlPoint[14] };
        PaintControlLine(ctl_points);
        PaintPoints(ctl_points);
    }
}

TriangleMesh* BezierPatch::OutputTriangles(ArgParser* args) {
    int tessellation = args->patch_tessellation;
    TriangleNet* net = new TriangleNet(tessellation, tessellation);
    std::array<std::vector<Vec3f>, 4> v_control_points;
    for (int p = 0; p < 4; p++) {
        BezierCurve::GetWindowCurve({ m_vControlPoint[p * 4],
            m_vControlPoint[p * 4 + 1],
            m_vControlPoint[p * 4 + 2],
            m_vControlPoint[p * 4 + 3] }, v_control_points[p], tessellation);
    }
    
    for (int u = 0; u <= tessellation; u++) {
        std::vector<Vec3f> mesh_point;
        BezierCurve::GetWindowCurve({ v_control_points[0][u],
            v_control_points[1][u],
            v_control_points[2][u], 
            v_control_points[3][u], }, mesh_point, tessellation);
        for (int v = 0; v <= tessellation; v++) {
            net->SetVertex(u, v, mesh_point[v]);
        }
    }
    
    return net;
}
