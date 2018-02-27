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
 
#include <cmath>
 
#include "plummer.h"

PlummerFactory::PlummerFactory(const int numbodies,const double ini_radius, const double a, const double M, long int seed) 
 : 	_numbodies(numbodies),
	_ini_radius(ini_radius),
	_a(a),
	_M(M) {
		_mt.init_genrand(seed);
	}

/**
 * Create a selection of particles that satisfies Plummer distribution
 */	
std::vector<Particle*>  PlummerFactory::create(){
	std::vector<Particle*> product;
	for (int i=0;i<_numbodies;i++) {
		const double radius=_ini_radius*_a / (std::sqrt(std::pow(_mt.random(),-(2.0/3.0))-1.0)); 
        double x; double y; double z;
		_randomize_theta_phi(radius,x,y,z);
	
		double vx; double vy;double vz  ;
		_randomize_theta_phi(_sample_velocity(radius),vx,vy,vz);

        product.push_back( new Particle( x, y, z, vx, vy,vz, _M/_numbodies) );
    }

	return product;
}

/**
 * Convert a scalar, r, into a vector with the same length, and a random orientation
 */
void PlummerFactory::_randomize_theta_phi(const double r,double & x,double & y,double& z) {
	const double cos_theta   = -1+2.0*_mt.random();
	const double theta        = std::acos(cos_theta);
	const double phi          = 2*M_PI*_mt.random();
	
	x = r * std::sin(theta)*std::cos(phi);
	y = r * std::sin(theta)*std::sin(phi);
	z = r * cos_theta;
}

/**
 * Sample velocities, ensuring that the initial velocity of any
 * star does not exceed escape velocity
 */
double PlummerFactory::_sample_velocity(const double radius) {
	double x=0;
	double y=0.1;
	while (y > x*x*std::pow(1.0-x*x,3.5)){
		x=_mt.random();
		y=0.1*_mt.random();
	}
	return  x * M_SQRT2 * std::pow( _a*_a + radius*radius,-0.25);
}