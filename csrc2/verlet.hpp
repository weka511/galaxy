/**
 * Copyright (C) 2018-2025 Greenweaves Software Limited
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
 
#ifndef _VERLET_HPP
#define _VERLET_HPP

#include "particle.hpp"
#include "treecode.hpp"

using namespace std;
/**
 *  Use Euler algorithm for first step. NB: this updates velocity only, so x
 *  remains at its initial value, which is what Verlet needs.
 *
 *  dt                Time step
 *  particles         Vector of particles
 */
void  euler(Particle* particles,double dt);

/**
 *  First half of Verlet algorithm - update positions
 *
 *  dt                Time step
 *  particles         Vector of particles
 */
void  verlet_positions(Particle* particles,double dt);

/**
 *  Second half of Verlet algorithm - update velocities
 *
 *  dt                Time step
 *  particles         Vector of particles
 */
void  verlet_velocities(Particle* particles,double dt);

/**
 * Integrate by taking one Euler step, followed by repeated Verlet steps. 
 *
 *  get_acceleration  Used to determine acceleration of all particles
 *  dt                Time step
 *  particles         Vector of particles
 *  shouldContinue    Used after each iteration, for reporting and to determine whher to continue
 *  start_iterations  Initial iteration number. Normally zero, but non-zero if we resume after an earlier run
 */
void run_verlet(void (*get_acceleration)(vector<Particle*>),
				int max_iter,
				double dt,
				vector<Particle*> particles,
				bool (*shouldContinue)(vector<Particle*>,int iter),
				int start_iterations);
				
void run_verlet(Node * (*precondition)(vector<Particle*>),
				void (*get_acceleration)(int i, vector<Particle*> particles,Node * root),
				int max_iter,
				double dt,
				vector<Particle*> particles,
				bool (*shouldContinue)(vector<Particle*>,int iter),
				int start_iterations,
				int nthreads);

void step(int to);

#endif  // _VERLET_HPP