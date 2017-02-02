#include "CharAnimController.h"


CharAnimController::CharAnimController() : m_ch2w(Identity()), m_v(0.0), m_vMax(5.0) {

}

void CharAnimController::update(const float dt) {
    m_ch2w = m_ch2w * Translation(m_v * dt, 0, 0);
}

void CharAnimController::turn(const Transform& transf_v) {
    m_ch2w = m_ch2w * transf_v;
}

void CharAnimController::turnXZ(const float& rot_angle_v) {
    m_ch2w = m_ch2w * RotationY(rot_angle_v);
}

void CharAnimController::accelerate(const float& speed_inc) {
	if(m_v + speed_inc <= m_vMax && m_v + speed_inc >= 0)
		m_v += speed_inc;
}

void CharAnimController::setVelocityMax(const float vmax) {
	m_vMax = vmax;
}

void CharAnimController::setPosition(const Vector& p) {
    m_ch2w[0][3] = p.x;
    m_ch2w[1][3] = p.y;
    m_ch2w[2][3] = p.z;
}

void CharAnimController::setVelocityNorm(const float v) {
	m_v = v;
}

const Vector CharAnimController::position() const {
    return Vector(m_ch2w(Point(1,0,0)));
}

const Vector CharAnimController::direction() const {
    return m_ch2w(Vector(1,0,0));
}

float CharAnimController::velocity() const {
	return m_v;

}

Transform CharAnimController::ctow() {
    return m_ch2w;
}