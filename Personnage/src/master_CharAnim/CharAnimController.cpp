#include "CharAnimController.h"


CharAnimController::CharAnimController() : m_pos(Vector(0,0,0)), m_ch2w(Identity()), m_v(0.0), m_vMax(5.0) {

}

void CharAnimController::update(const float dt) {

}

void CharAnimController::turn(const Transform& transf_v) {

}

void CharAnimController::turnXZ(const float& rot_angle_v) {

}

void CharAnimController::accelerate(const float& speed_inc) {
	m_v += speed_inc;
}

void CharAnimController::setVelocityMax(const float vmax) {
	m_vMax = vmax;
}

void CharAnimController::setPosition(const Vector& p) {
	m_pos = p;
}

void CharAnimController::setVelocityNorm(const float v) {
	m_v = v;
}

const Vector& CharAnimController::position() const {
	return m_pos;
}

const Vector& CharAnimController::direction() const {

}

float CharAnimController::velocity() const {

}

