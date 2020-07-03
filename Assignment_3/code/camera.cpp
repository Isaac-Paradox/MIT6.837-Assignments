#include "camera.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include"matrix.h"

void Camera::glInit(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void Camera::glPlaceCamera(void) {
    gluLookAt(m_vCenter.x(), m_vCenter.y(), m_vCenter.z(),
        (double)m_vCenter.x() + m_vForward.x(), (double)m_vCenter.y() + m_vForward.y(), (double)m_vCenter.z() + m_vForward.z(),
        m_vUp.x(), m_vUp.y(), m_vUp.z());
}

void Camera::dollyCamera(float dist) {
    m_vCenter += m_vForward * dist;
}

void Camera::truckCamera(float dx, float dy) {
    Vec3f horizontal;
    Vec3f::Cross3(horizontal, m_vForward, m_vUp);
    horizontal.Normalize();

    Vec3f screenUp;
    Vec3f::Cross3(screenUp, horizontal, m_vForward);

    m_vCenter += horizontal * dx + screenUp * dy;
}

void Camera::rotateCamera(float rx, float ry) {
    Vec3f horizontal;
    Vec3f::Cross3(horizontal, m_vForward, m_vUp);
    horizontal.Normalize();

    // Don't let the model flip upside-down (There is a singularity
    // at the poles when 'up' and 'direction' are aligned)
    float tiltAngle = acos(m_vUp.Dot3(m_vForward));
    if ((double)tiltAngle - ry > 3.13)
        ry = tiltAngle - 3.13;
    else if ((double)tiltAngle - ry < 0.01)
        ry = tiltAngle - 0.01;

    Matrix rotMat = Matrix::MakeAxisRotation(m_vUp, rx);
    rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

    rotMat.Transform(m_vCenter);
    rotMat.TransformDirection(m_vForward);

    m_vForward.Normalize();
}

// ====================================================================
// Create an orthographic camera with the appropriate dimensions that
// crops the screen in the narrowest dimension.
// ====================================================================

void OrthographicCamera::glInit(int w, int h) {
    Camera::glInit(w, h);
    if (w > h)
        glOrtho(-m_fSize / 2.0, m_fSize / 2.0, -m_fSize * (float)h / (float)w / 2.0, m_fSize * (float)h / (float)w / 2.0, 0.5, 40.0);
    else
        glOrtho(-m_fSize * (float)w / (float)h / 2.0, m_fSize * (float)w / (float)h / 2.0, -m_fSize / 2.0, m_fSize / 2.0, 0.5, 40.0);
}

// ====================================================================
// Place an orthographic camera within an OpenGL scene
// ====================================================================



// ====================================================================
// dollyCamera, truckCamera, and RotateCamera
//
// Asumptions:
//  - up is really up (i.e., it hasn't been changed
//    to point to "screen up")
//  - up and direction are normalized
// Special considerations:
//  - If your constructor precomputes any vectors for
//    use in 'generateRay', you will likely to recompute those
//    values at athe end of the these three routines
// ====================================================================

// ====================================================================
// dollyCamera: Move camera along the direction vector
// ====================================================================

void OrthographicCamera::dollyCamera(float dist) {
    Camera::dollyCamera(dist);

    // ===========================================
    // ASSIGNMENT 3: Fix any other affected values
    // ===========================================


}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================

void OrthographicCamera::truckCamera(float dx, float dy) {
    Camera::truckCamera(dx, dy);

    // ===========================================
    // ASSIGNMENT 3: Fix any other affected values
    // ===========================================


}

// ====================================================================
// rotateCamera: Rotate around the up and horizontal vectors
// ====================================================================

void OrthographicCamera::rotateCamera(float rx, float ry) {
    Camera::rotateCamera(rx, ry);

    // ===========================================
    // ASSIGNMENT 3: Fix any other affected values
    // ===========================================


}

// ====================================================================
// ====================================================================

// ====================================================================
// Create a perspective camera with the appropriate dimensions that
// crops or stretches in the x-dimension as necessary
// ====================================================================

void PerspectiveCamera::glInit(int w, int h) {
    Camera::glInit(w, h);
    gluPerspective(m_fAngle * 180.0 / 3.14159, (float)w / float(h), 0.5, 40.0);
}

// ====================================================================
// Place a perspective camera within an OpenGL scene
// ====================================================================

// ====================================================================
// dollyCamera, truckCamera, and RotateCamera
//
// Asumptions:
//  - up is really up (i.e., it hasn't been changed
//    to point to "screen up")
//  - up and direction are normalized
// Special considerations:
//  - If your constructor precomputes any vectors for
//    use in 'generateRay', you will likely to recompute those
//    values at athe end of the these three routines
// ====================================================================

// ====================================================================
// dollyCamera: Move camera along the direction vector
// ====================================================================

void PerspectiveCamera::dollyCamera(float dist) {
    Camera::dollyCamera(dist);

    // ===========================================
    // ASSIGNMENT 3: Fix any other affected values
    // ===========================================


}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================

void PerspectiveCamera::truckCamera(float dx, float dy) {
    Camera::truckCamera(dx, dy);

    // ===========================================
    // ASSIGNMENT 3: Fix any other affected values
    // ===========================================


}

// ====================================================================
// rotateCamera: Rotate around the up and horizontal vectors
// ====================================================================

void PerspectiveCamera::rotateCamera(float rx, float ry) {
    Camera::rotateCamera(rx, ry);

    // ===========================================
    // ASSIGNMENT 3: Fix any other affected values
    // ===========================================


}

// ====================================================================
// ====================================================================
