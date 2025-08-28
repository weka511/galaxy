/**
 * Copyright (C) 2025 Simon Crase
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
 * Integrate an Ordinary Differential Equation using Leapfrog algorithm.
 *
 * See https://courses.physics.ucsd.edu/2019/Winter/physics141/Assignments/leapfrog.pdf
 */
 
#include <iostream>
#include "acceleration.hpp"
#include "integrators.hpp"
#include "logger.hpp"

using namespace std;

Leapfrog::Euler::Euler(const double dt) 
   :_dt(dt){}
   
/**
 *  Use Euler algorithm for first step. NB: this updates velocity only, so 
 *  position remains at its initial value, as Leapfrog expects.
 *
 *  particles         Vector of particles
 */
 
void Leapfrog::Euler::visit(Particle & particle){
	array<double,NDIM>  velocity = particle.get_velocity();
	array<double,NDIM>  acceleration = particle.get_acceleration();
	for (int i=0;i<NDIM;i++)
		velocity[i] += _dt * acceleration[i];
	particle.set_velocity(velocity);
}

/**
 *    Initialize Leapfrog.
 *
 *    Parameters:
 *        configuration             Container for particles
 *        calculate_acceleration    Used to calculate acceleration of each particle
 *        reporter                  Used to record results in a file
 */
Leapfrog::Leapfrog(Configuration & configuration, IAccelerationVisitor &calculate_acceleration,IReporter & reporter)
	:  	_configuration(configuration),
		_calculate_acceleration(calculate_acceleration),
		_reporter(reporter) {;}
		
/**
 * This function is responsible for integrating an ODE.
 *
 * Parameters:
 *     max_iter   Number of iterations
 *     dt         Time step
 */
void Leapfrog::run( int max_iter,const double dt){
	VelocityUpdater velocity_updater(dt);
	PositionUpdater position_updater(dt);
	/**
	 *  First, take a half step and update velocities
	 */
	_configuration.initialize(_calculate_acceleration);
	_configuration.iterate(_calculate_acceleration);
	Euler euler(0.5*dt);
	_configuration.iterate(euler);
	/**
	 *  Now the velocities are one half step ahead of the position. We keep
	 *  leapfrogging: use the "half ahead" velocity to update positions,
	 *  calculate accelerations for the new positions, then update velocity.
	 */
	for (int iter=0;iter<max_iter and _reporter.should_continue();iter++) {
		_configuration.iterate(position_updater);
		_configuration.initialize(_calculate_acceleration);
		_configuration.iterate(_calculate_acceleration);
		_configuration.iterate(velocity_updater);
		_reporter.report();
	}
}



/**
 *  First half of Leapfrog algorithm - update positions
 *
 *  dt                Time step
 *  particles         Vector of particles
 */
void Leapfrog::PositionUpdater::visit(Particle & particle){
	array<double,NDIM>  position = particle.get_position();
	array<double,NDIM>  velocity = particle.get_velocity();
	for (int i=0;i<NDIM;i++)
		position[i] += _dt * velocity[i];
	particle.set_position(position);
}

/**
 *  Second half of Leapfrog algorithm - update velocities
 *
 *  dt                Time step
 *  particles         Vector of particles
 */
void Leapfrog::VelocityUpdater::visit(Particle & particle){
	array<double,NDIM>  velocity = particle.get_velocity();
	array<double,NDIM>  acceleration = particle.get_acceleration();
	for (int i=0;i<NDIM;i++)
		velocity[i] += _dt * acceleration[i];
	particle.set_velocity(velocity);	
}



