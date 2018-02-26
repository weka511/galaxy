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

#include <vector>
#include <random>

#include "particle.h"

class Factory {
  public:
	virtual std::vector<Particle*>  create()=0;
};

/**
 * Factory for instantiating configurations of particles
 */
class PlummerFactory : public Factory {
  public:
	PlummerFactory(const int numbodies,const double ini_radius, const double softening_length, const double M);
	
	/**
	 * Create a selection of particles that satisifes Plummer distribution
	 */
	std::vector<Particle*>  create( );
	
  private:

	void randomize_theta_phi(const double r,double & x,double & y,double& z);
	
	double sample_velocity(const double radius);
	
	const int    _numbodies;
	const double _ini_radius;
	const double _softening_length;
	const double _M;

  	std::default_random_engine generator;
	std::uniform_real_distribution<double> _uniform_distribution_theta;
	std::uniform_real_distribution<double> _uniform_distribution_phi;
	std::uniform_real_distribution<double> _uniform_distribution_radius;
	std::uniform_real_distribution<double> _uniform_distribution_x;
	std::uniform_real_distribution<double> _uniform_distribution_y;
	
};

#endif
