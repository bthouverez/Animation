/*
 * Calculs.cpp : Application schemas explicite sur les objets.
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

/** \file Calculs.cpp
Fonctions de calculs communes aux objets simules.
\brief Fonctions de calculs communes aux objets simules.
*/

#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>

#include "vec.h"
#include "ObjetSimule.h"
#include "Viewer.h"

using namespace std;



/**
 * Calcul de l acceleration des particules
 * avec ajout de la gravite aux forces des particules
 * et ajout de la force due au vent sur une des particules du maillage
 * et reinitialisation des forces.
 */
void ObjetSimule::CalculAccel_ForceGravite(Vector g)
{
	Vector vent(2.0, 0.0, 1.0);
	for (int ii = 0; ii < _Nb_Sommets; ii++) {
		Force[ii] = Force[ii] + g + vent;
		A[ii] = M[ii] != 0 ? Force[ii] / M[ii] : 0;
	}
}//void


/**
 * Calcul de la position et de la vitesse des particules
 * au temps 0 (methode d integration Leap Frog).
 */
void ObjetSimule::CalculPositionVitesseTps0(float visco)
{
	for (int ii = 0; ii < _Nb_Sommets; ii++) {
		V[ii] = Vector(0,0, 0) + _delta_t * A[ii] * visco / 2;
	}
}


/**
 * Calcul de la position et de la vitesse des particules.
 */
void ObjetSimule::CalculPositionVitesse(float visco)
{
	for (int ii = 0; ii < _Nb_Sommets; ii++) {
		V[ii] = V[ii] + _delta_t * visco * A[ii];
		P[ii] = P[ii] + _delta_t * V[ii];
	}
}//void

