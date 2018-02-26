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
#include "spdlog/spdlog.h"

namespace spd = spdlog;

PlummerFactory::PlummerFactory() 
 : 	uniform_distribution_theta(std::uniform_real_distribution<double>(-1,1)),
	uniform_distribution_phi(std::uniform_real_distribution<double>(0,2*M_PI)),
	uniform_distribution_radius(std::uniform_real_distribution<double>(0.0,1)),
	uniform_distribution_x(std::uniform_real_distribution<double>(0,1)),
	uniform_distribution_y(std::uniform_real_distribution<double>(0,0.1)){}
	
std::vector<Particle*>  PlummerFactory::create(const int numbodies,const double ini_radius, const double softening_length, const double M ){
	std::vector<Particle*> product;
	for (int i=0;i<numbodies;i++) {
		const double radius=ini_radius*softening_length / 
		(std::sqrt(std::pow(uniform_distribution_radius(generator),-(2.0/3.0))-1.0)); 
        double x; double y; double z;
		randomize_theta_phi(radius,x,y,z);
	
		double vx; double vy;double vz  ;
		randomize_theta_phi(sample_velocity(radius,softening_length),vx,vy,vz);

        product.push_back( new Particle( x, y, z, vx, vy,vz, M/numbodies) );
    }

//	zero_centre_mass_and_linear_momentum(product,0);
	spdlog::get("galaxy")->info("{0} {1}: initialized {2} bodies.",__FILE__,__LINE__,numbodies);
	return product;
}

/**
 * Convert a scalar, r, into a vector with the same length, and a random orientation
 */
void PlummerFactory::randomize_theta_phi(const double r,double & x,double & y,double& z) {
	const double acos_theta   = uniform_distribution_theta(generator);
	const double theta        = std::acos(acos_theta);
	const double phi          = uniform_distribution_phi(generator);
	
	x = r * std::sin(theta)*std::cos(phi);
	y = r * std::sin(theta)*std::sin(phi);
	z = r * acos_theta;
}

/**
 * Sample velocities, ensuring that the initial velocity of any
 * star does not exceed escape velocity
 */
double PlummerFactory::sample_velocity(const double radius,const double softening_length) {
	double x=0;
	double y=0.1;
	while (y > sqr(x)*std::pow(1.0-sqr(x),3.5)){
		x=uniform_distribution_x(generator);
		y=uniform_distribution_y(generator);
	}
	return  x * M_SQRT2 * std::pow( sqr(softening_length) + sqr(radius),-0.25);
}