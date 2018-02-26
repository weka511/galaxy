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
 * This file exercises treecode. 
 */
 
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

#include "catch.hpp"
#include "plummer.h"
#include "utils.h"

TEST_CASE( "Plummer Tests", "[plummer]" ) {

	PlummerFactory factory (10000,1,  1,  1,time(NULL));
	std::vector<Particle*> particles = factory.create( );
	double x0=0,y0=0,z0=0,m_total=0;
	for (int i=0;i<particles.size();i++){
		double x,y,z;
		particles[i]->getPos(x,y,z);
		const double m=particles[i]->getMass();
		x0+=m*x;y0+=m*y;z0+=m*z;m_total+=m;
	}
	x0/=m_total;y0/=m_total;z0/=m_total;
	std::cout << "Centre of mass=(" << x0 << "," << y0 << "," << z0 << ")" << std::endl;
	std::vector<double> distances;
	for (int i=0;i<particles.size();i++){
		double x,y,z;
		particles[i]->getPos(x,y,z);
		distances.push_back(std::sqrt(sqr(x-x0)+sqr(y-y0)+sqr(z-z0)));
	}
	std::sort (distances.begin(), distances.end()); 
	// for (int i=0;i<distances.size();i++)
		// std::cout<< distances[i]<<std::endl;
	std::cout << "Q1="<<distances[distances.size()/4] << std::endl;
	std::cout << "Q2="<<distances[distances.size()/2] << std::endl;
	std::cout << "Q3="<<distances[3*distances.size()/4] << std::endl;
}
