/*
 * CalculsMSS.cpp :
 * Copyright (C) 2016 Florence Zara, LIRIS
 *               florence.zara@liris.univ-lyon1.fr
 *               http://liris.cnrs.fr/florence.zara/
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** \file CalculsMSS.cpp
Programme calculant pour chaque particule i d un MSS son etat au pas de temps suivant
 (methode d 'Euler semi-implicite) : principales fonctions de calculs.
\brief Fonctions de calculs de la methode semi-implicite sur un systeme masses-ressorts.
*/

#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>

#include "vec.h"
#include "ObjetSimule.h"
#include "ObjetSimuleMSS.h"
#include "Viewer.h"

using namespace std;





/**
* Calcul des forces appliquees sur les particules du systeme masses-ressorts.
 */
void ObjetSimuleMSS::CalculForceSpring()
{
	for(unsigned int ii = 0; ii < _SytemeMasseRessort->GetPartList().size(); ii++) {
		_SytemeMasseRessort->GetPartList()[ii]->SetPosition(P[ii]);
	}
	for(auto part : _SytemeMasseRessort->GetPartList()) {
		Force[part->GetId()] = Vector(0.0, 0.0, 0.0);
		Vector fe(0.0, 0.0, 0.0), fv(0.0, 0.0, 0.0);
		for(auto ress : part->GetRessortList()) {
			float k = ress->GetRaideur();
			float v = ress->GetFactAmorti();
			float l = ress->GetLrepos();

			Particule *p_i = ress->GetParticuleA();
			Particule *p_j = ress->GetParticuleB();
			if(p_j == part) std::swap(p_i, p_j);
			Vector AB = p_j->GetPosition() - p_i->GetPosition();

			Vector u = length(AB) == 0 ? Vector(0.0, 0.0, 0.0) : AB / length(AB);

			//Fij : Kij( || Xi-Xj || - Lij )
			fe = fe + k * (length(p_i->GetPosition() - p_j->GetPosition()) - l) * u;
			fv = fv + v * cross(dot(V[p_i->GetId()] - V[p_j->GetId()], u), u);
		}
		Force[part->GetId()] = Force[part->GetId()] + fe + fv;
	}
}//void


/**
 * Gestion des collisions avec le sol - plan (x,y,z).
 */
void ObjetSimuleMSS::CollisionPlan(float x, float y, float z)
{
    for(unsigned int ii = 0; ii < P.size(); ii++)
        if(P[ii].y <= y)
            V[ii] = Vector(0.0f, 0.0f, 0.0f);
}// void

