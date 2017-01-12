#include "CharAnimController.h"

void CharAnimController::update(const float dt);
void CharAnimController::turn(const Transform& transf_v);
void CharAnimController::turnXZ(const float& rot_angle_v);
void CharAnimController::accelerate(const float& speed_inc);
void CharAnimController::setVelocityMax(const float vmax);
void CharAnimController::setPosition(const Vector& p);
void CharAnimController::setVelocityNorm(const float v);
const Vector& CharAnimController::position() const;
const Vector& CharAnimController::direction() const;
float CharAnimController::velocity() const;
