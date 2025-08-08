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
 
#include <iostream>
#include "acceleration.hpp"
#include "verlet.hpp"
#include "logger.hpp"

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
	_configuration.initialize(_calculate_acceleration);
	_configuration.iterate(_calculate_acceleration);
	Euler euler(0.5*dt);
	Velocities velocities(dt);
	Positions positions(dt);
	_configuration.iterate(euler);
	for (int iter=0;iter<max_iter and _reporter.should_continue();iter++) {
		TIME();
		_configuration.iterate(positions);
		TIME();
		_configuration.initialize(_calculate_acceleration);
		TIME();
		_configuration.iterate(_calculate_acceleration);
		TIME();
		_configuration.iterate(velocities);
		LOG(_configuration.get_momentum());
		TIME();
		_reporter.report();
	}
}

/**
 *  Use Euler algorithm for first step. NB: this updates velocity only, so 
 *  position remains at its initial value, as Verlet expects.
 *
 *  dt                Time step
 *  particles         Vector of particles
 */
 
void Verlet::Euler::visit(int i,Particle & particle){
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
void Verlet::Positions::visit(int i,Particle & particle){
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
void Verlet::Velocities::visit(int i,Particle & particle){
	array<double,3>  velocity = particle.get_velocity();
	array<double,3> & acceleration = particle.get_acceleration();
	for (int i=0;i<3;i++)
		velocity[i] += _dt * acceleration[i];
	particle.set_velocity(velocity);	
}



