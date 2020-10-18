#include "camera.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include"matrix.h"

void Camera::glInit(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void Camera::glPlaceCamera(void) {
    _UpdateCameraDirection();
    gluLookAt(m_vCenter.x(), m_vCenter.y(), m_vCenter.z(),
        (double)m_vCenter.x() + m_vForward.x(), (double)m_vCenter.y() + m_vForward.y(), (double)m_vCenter.z() + m_vForward.z(),
        m_vUp.x(), m_vUp.y(), m_vUp.z());
}

void Camera::dollyCamera(float dist) {
    m_vCenter += m_vForward * dist;
}

void Camera::truckCamera(float dx, float dy) {
    m_vCenter += m_vRight * dx + m_vUp * dy;
}

void Camera::rotateCamera(float rx, float ry) {
    // Don't let the model flip upside-down (There is a singularity
    // at the poles when 'up' and 'direction' are aligned)
    float tiltAngle = acos(m_vUp.Dot3(m_vForward));
    if ((double)tiltAngle - ry > 3.13)
        ry = tiltAngle - 3.13;
    else if ((double)tiltAngle - ry < 0.01)
        ry = tiltAngle - 0.01;

    Matrix rotMat = Matrix::MakeAxisRotation(m_vUp, rx);
    rotMat *= Matrix::MakeAxisRotation(m_vRight, ry);

    rotMat.Transform(m_vCenter);
    rotMat.TransformDirection(m_vForward);

    m_bCameraDirectionDirty = true;
}

bool Camera::_UpdateCameraDirection() {
    if (!m_bCameraDirectionDirty) { return false; }
    m_bCameraDirectionDirty = false;
    m_vForward.Normalize();
    Vec3f::Cross3(m_vRight, m_vForward, m_vOriginUp);
    m_vRight.Normalize();
    Vec3f::Cross3(m_vUp, m_vRight, m_vForward);
    m_vUp.Normalize();
    return true;
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

Ray OrthographicCamera::generateRay(Vec2f point) {
    point -= Vec2f(0.5f, 0.5f);
    Vec3f orig = m_vCenter + m_fSize * (m_vUp * point.y() + m_vRight * point.x());
    return Ray(orig, m_vForward);
}

// ====================================================================
// Place an orthographic camera within an OpenGL scene
// ====================================================================

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

Ray PerspectiveCamera::generateRay(Vec2f point) {
    //Vec3f direction = m_vLeftDownSide + (point.x() * m_vRight + point.y() * m_vUp) * m_fScreenScale;
    Vec3f direction = m_vLeftDownSide + (point.x() * m_vRight + point.y() * m_vUp);
    direction.Normalize();
    return Ray(m_vCenter, direction);
}

bool PerspectiveCamera::_UpdateCameraDirection() {
    if (!Camera::_UpdateCameraDirection()) { return false; }
    m_vLeftDownSide = m_vForward * (0.5f / tan(m_fAngle * 0.5f)) - (m_vUp + m_vRight) * 0.5;
    return true;
}

// ====================================================================
// Place a perspective camera within an OpenGL scene
// ====================================================================

// ====================================================================
// ====================================================================
