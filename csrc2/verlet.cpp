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
 * Integrate an Ordinary Differential Equation using Verlet algorithm.
 *
 * See http://physics.ucsc.edu/~peter/242/leapfrog.pdf
 */
 
 
#include <algorithm>
#include <cmath>
#include <iostream>
#include "verlet.hpp"

using namespace std;

/**
 * This function is responsible for integrating an ODE.
 *
 * Parameters:
 *     max_iter   Number of iterations
 *     dt         Time step
 */
void Verlet::run( int max_iter,const double dt){
	if (!_reporter.should_continue()) return;
	_configuration.iterate(_calculate_acceleration);
	Euler euler(0.5*dt);
	Velocities velocities(dt);
	Positions positions(dt);
	_configuration.iterate(euler);
	for (int iter=0;iter<max_iter and _reporter.should_continue();iter++) {
		_configuration.iterate(positions);
		_configuration.iterate(_calculate_acceleration);
		_configuration.iterate(velocities);
	}
}

/**
 *  Use Euler algorithm for first step. NB: this updates velocity only, so 
 *  position remains at its initial value, as Verlet expects.
 *
 *  dt                Time step
 *  particles         Vector of particles
 */
 
void Verlet::Euler::visit(Particle & particle){
	array<double,3>  velocity = particle.get_velocity();
	array<double,3>  acceleration = particle.get_acceleration();
	for (int i=0;i<3;i++)
		velocity[i] += _dt * acceleration[i];
	particle.set_velocity(velocity);
}

/**
 *  First half of Verlet algorithm - update positions
 *
 *  dt                Time step
 *  particles         Vector of particles
 */
void Verlet::Positions::visit(Particle & particle){
	array<double,3>  position = particle.get_position();
	array<double,3>  velocity = particle.get_velocity();
	for (int i=0;i<3;i++)
		position[i] += _dt * velocity[i];
	particle.set_position(position);
}

/**
 *  Second half of Verlet algorithm - update velocities
 *
 *  dt                Time step
 *  particles         Vector of particles
 */
void Verlet::Velocities::visit(Particle & particle){
	array<double,3>  velocity = particle.get_velocity();
	array<double,3> & acceleration = particle.get_acceleration();
	for (int i=0;i<3;i++)
		velocity[i] += _dt * acceleration[i];
	particle.set_velocity(velocity);	
}



// ================================= legacy code starts here ====================================


void  euler(Particle& particle,double dt){
	array<double,3>  velocity = particle.get_velocity();
	array<double,3>  acceleration = particle.get_acceleration();
	for (int i=0;i<3;i++)
		velocity[i] += dt * acceleration[i];
	particle.set_velocity(velocity);
}

/**
 *  First half of Verlet algorithm - update positions
 *
 *  dt                Time step
 *  particles         Vector of particles
 */
void  verlet_positions(Particle& particle,double dt) {
	array<double,3>  position = particle.get_position();
	array<double,3>  velocity = particle.get_velocity();
	for (int i=0;i<3;i++)
		position[i] += dt * velocity[i];
	particle.set_position(position);
}

/**
 *  Second half of Verlet algorithm - update velocities
 *
 *  dt                Time step
 *  particles         Vector of particles
 */
void  verlet_velocities(Particle& particle,double dt) {
	array<double,3>  velocity = particle.get_velocity();
	array<double,3> & acceleration = particle.get_acceleration();
	for (int i=0;i<3;i++)
		velocity[i] += dt * acceleration[i];
	particle.set_velocity(velocity);	
}


/**
 * Integrate by taking one Euler step, followed by repeated Verlet steps. 
 *
 *  get_acceleration  Used to determine acceleration of all particles
 *  dt                Time step
 *  particles         Vector of particles
 *  shouldContinue    Used after each iteration, for reporting and to determine whher to continue
 *  start_iterations  Initial iteration number. Normally zero, but non-zero if we resume after an earlier run
 */
void run_verlet(void (*get_acceleration)(unique_ptr<Particle[]>&,int),
				int max_iter,
				double dt,
				unique_ptr<Particle[]> particles,
				int n,
				bool (*shouldContinue)(unique_ptr<Particle[]>& particles,int iter),
				int start_iterations) {
	if (start_iterations==0){
		get_acceleration(particles,n);
		// Take a half step, so acceleration corresponds to 0.5dt, 1.5dt, etc.
		for (int i=0;i<n;i++)
			euler(particles[i],0.5*dt);
	}
	
	/**
	 * Iterate through future times: notice that we need to increment iter before checking shouldContinue
	 * in order to fix issue #43 - On restart, galaxy.exe loses some time steps
	 */
	for (int iter=1+start_iterations;iter<max_iter+start_iterations && shouldContinue(particles,++iter);) {
		for (int i=0;i<n;i++)
			verlet_positions(particles[i],dt);
		get_acceleration(particles,n);
		for (int i=0;i<n;i++)
			verlet_velocities(particles[i],dt);
	}
}
