/**
 * Copyright (C) 2018 Greenweaves Software Limited
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>
 *
 * Integrate an Ordinary Differential Equation using the Verlet algorithm
 */
 
#ifndef _VERLET_H
#define _VERLET_H
#include <vector>

#include "particle.h"

/**
 *  Use Euler algorithm for first step. NB: this updates velocity only, so x
 *  remains at its initial value, which is what Verlet needs.
 */
void  euler(Particle* p,double dt);

/**
 *  First half of Verlet algorithm - update positions
 */
void  verlet_positions(Particle* p,double dt);

/**
 *  Second half of Verlet algorithm - update velocities
 */
void  verlet_velocities(Particle* p,double dt);

/**
 * Integrate by taking one Euler step, followed by repeated Verlet steps
 */
void run_verlet(void (*get_acceleration)(std::vector<Particle*>),
				int max_iter,double dt,
				std::vector<Particle*> particles,
				bool (*shouldContinue)(std::vector<Particle*>,int iter ));

#endif