#ifndef CHAR_ANIM_CONTROLLER_H
#define CHAR_ANIM_CONTROLLER_H

#include "vec.h"
#include "mat.h"

class CharAnimController {

public:
	CharAnimController();

	void update(const float dt);

	void turn(const Transform& transf_v);
	void turnXZ(const float& rot_angle_v);
	void accelerate(const float& speed_inc);
	void setVelocityMax(const float vmax);

	void setPosition(const Vector& p);
	void setVelocityNorm(const float v);

	const Vector position() const;
	const Vector direction() const;
	float velocity() const;

	Transform ctow();

protected:
    Transform m_ch2w;   // matrice du character vers le monde
                        // le personnage se déplace vers X
                        // il tourne autour de Y
                        // Z est sa direction droite
                                
	float m_v;          // le vecteur vitesse est m_v * m_ch2w * Vector(1,0,0)
	float m_vMax;       // ne peut pas accélérer plus que m_vMax
};

#endif