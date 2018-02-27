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
 * This file checks the mass distribution from plummer.cpp. 
 */
 
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>

#include "catch.hpp"
#include "physics.h"
#include "plummer.h"
#include "utils.h"

double stdev(std::vector<double> q1s,double mean1) {
	std::vector<double> diff(q1s.size());
	std::transform(q1s.begin(), q1s.end(), diff.begin(),
	std::bind2nd(std::minus<double>(), mean1));
	double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
	return std::sqrt(sq_sum / q1s.size());
}

double mean(std::vector<double> values) {
	return  std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

TEST_CASE( "Plummer Tests", "[plummer]" ) {
	const double q1=std::pow(std::pow(2.0,4.0/3.0)-1,-0.5);
	const double q2=std::pow(std::pow(2.0,2.0/3.0)-1,-0.5);
	const double q3=std::pow(std::pow(2.0,4.0/3.0)*std::pow(3.0,-2.0/3.0)-1,-0.5);
	std::vector<double> q1s;
	std::vector<double> q2s;
	std::vector<double> q3s;
	std::vector<double> Ts;
	std::vector<double> Vs;
	
	for (int i=0;i<100;i++){
		PlummerFactory factory (1000,1,  1,  1,i);
		std::vector<Particle*> particles = factory.create( );
		double x0=0,y0=0,z0=0,m_total=0;
		for (int i=0;i<particles.size();i++){
			double x,y,z;
			particles[i]->getPos(x,y,z);
			const double m=particles[i]->getMass();
			x0+=m*x;y0+=m*y;z0+=m*z;m_total+=m;
		}
		x0/=m_total;y0/=m_total;z0/=m_total;
		// std::cout << "Centre of mass=(" << x0 << "," << y0 << "," << z0 << ")" << std::endl;
		std::vector<double> distances;
		for (int i=0;i<particles.size();i++){
			double x,y,z;
			particles[i]->getPos(x,y,z);
			distances.push_back(sqr(x-x0)+sqr(y-y0)+sqr(z-z0));
		}
		std::sort (distances.begin(), distances.end()); 

		// std::cout << "Q1="<<std::sqrt(distances[distances.size()/4-1]) << "," << q1<<std::endl;
		// std::cout << "Q2="<<std::sqrt(distances[distances.size()/2-1]) << "," << q2<< std::endl;
		// std::cout << "Q3="<<std::sqrt(distances[3*distances.size()/4-1]) << "," << q3<< std::endl;
		q1s.push_back(std::sqrt(distances[distances.size()/4-1]) );
		q2s.push_back(std::sqrt(distances[distances.size()/2-1]) );
		q3s.push_back(std::sqrt(distances[3*distances.size()/4-1]) );
		const double T=get_kinetic_energy(particles);
		const double V=get_potential_energy(particles,1,1);
		std::cout << "T="<<T << ", \tV=" << V << ", \t-V/T="<< -V/T<<std::endl;
		Ts.push_back(T);
		Vs.push_back(V);
	}
	const double mean1 = mean(q1s);

	const double mean2 = mean(q2s);
	const double mean3 = mean(q3s);
	
	std::cout << "Expected quartiles: "<< q1 << ", " << q2 << ", " << q3 << ", " <<std::endl;

	std::cout << mean1 << "("<< stdev(q1s,mean1)<<"), "<<mean2 << "("<< stdev(q2s,mean2)<<"), " << mean3<< "("<< stdev(q3s,mean3)<<")"<< std::endl;
	
	const double meanT=mean(Ts);
	const double meanV=mean(Vs);
	std::cout << meanT << "("<< stdev(Ts,meanT)<<"), "<<  meanV <<  "("<< stdev(Vs,meanV)<<"), "<< std::endl;
}
