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
 */
 
#ifndef _PLUMMER_H
#define _PLUMMER_H


#include "factory.h"
#include "mt.h"

/**
 * Factory for instantiating configurations of particles in accordance with Plummer model
 */
class PlummerFactory : public Factory {
  public:
	PlummerFactory(const int numbodies,const double ini_radius, const double a, const double M,long int seed);
	
	/**
	 * Create a selection of particles that satisfies Plummer distribution
	 */
	std::vector<Particle*>  create( );
	
  private:

	/**
	 * Convert a scalar, r, into a vector with the same length, and a random orientation
	 */
	void 			_randomize_theta_phi(const double r,double & x,double & y,double& z);
	
	/**
	 * Sample velocities, ensuring that the initial velocity of any
	 * star does not exceed escape velocity
	 */
	double 			_sample_velocity(const double radius);
	
	/**
	 * Number of bodies 
	 */
	const int    	_numbodies;
	
	/**
	 * Initially, the bodies are distributed inside a sphere of radius ini_radius.
	 */
	const double 	_ini_radius; 
	
	/**
	 * Softening length
	 */
	const double 	_a; 
	
	/**
	 * Total mass
	 */
	const double 	_M;
	
	/**
	 * Random number generator
	 */
  	MersenneTwister _mt;	
};

#endif
