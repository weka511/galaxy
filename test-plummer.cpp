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

#include "catch.hpp"
#include "physics.h"
#include "plummer.h"
#include "utils.h"

TEST_CASE( "Plummer Tests for mass distribution", "[plummer]" ) {
	const double q1=std::pow(std::pow(2.0,4.0/3.0)-1,-0.5);
	const double q2=std::pow(std::pow(2.0,2.0/3.0)-1,-0.5);
	const double q3=std::pow(std::pow(2.0,4.0/3.0)*std::pow(3.0,-2.0/3.0)-1,-0.5);
	std::vector<double> q1s;
	std::vector<double> q2s;
	std::vector<double> q3s;
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

		std::vector<double> distances;
		for (int i=0;i<particles.size();i++){
			double x,y,z;
			particles[i]->getPos(x,y,z);
			distances.push_back(sqr(x-x0)+sqr(y-y0)+sqr(z-z0));
		}
		std::sort (distances.begin(), distances.end()); 

		q1s.push_back(std::sqrt(distances[distances.size()/4-1]) );
		q2s.push_back(std::sqrt(distances[distances.size()/2-1]) );
		q3s.push_back(std::sqrt(distances[3*distances.size()/4-1]) );
	}
	
	const double mean1 = mean(q1s);
	const double mean2 = mean(q2s);
	const double mean3 = mean(q3s);
	REQUIRE(mean1==Approx(q1).epsilon(stdev(q1s,q1)));
	REQUIRE(mean2==Approx(q2).epsilon(stdev(q2s,q2)));
	REQUIRE(mean3==Approx(q3).epsilon(stdev(q3s,q3)));
}

TEST_CASE( "Plummer Tests for energy", "[plummer]" ) {

	std::vector<double> Ts;
	std::vector<double> Vs;
	std::vector<double> ratios;
	for (int i=0;i<100;i++){
		PlummerFactory factory (1000,1,  1,  1,i);
		std::vector<Particle*> particles = factory.create( );
		double mass=factory.zero_centre_mass(particles);
		factory.zero_linear_momentum(particles,mass);
		const double T=get_kinetic_energy(particles);
		const double V=get_potential_energy(particles,1,1);
		ratios.push_back(-V/T);
		Ts.push_back(T);
		Vs.push_back(V);
	}

	const double meanT=mean(Ts);
	const double meanV=mean(Vs);
	std::cout << "<T>=" << meanT << "("<< stdev(Ts,meanT)<<"), <V>="<<  meanV <<  "("<< stdev(Vs,meanV)<<"), "<< std::endl;
	std::cout << "Ratio=(" <<
				-2.0 * meanT/meanV - stdev(Ts,meanT)+stdev(Vs,meanV) << ","<<
				-2.0 * meanT/meanV + stdev(Ts,meanT)+stdev(Vs,meanV)<< ")" << std::endl;
	std::cout << "Ratio =(" <<
		mean(ratios)- stdev(ratios,mean(ratios),true) << "," <<
		mean(ratios)+ stdev(ratios,mean(ratios),true) << ")"
		<<std::endl;
	REQUIRE(mean(ratios)==Approx(2).epsilon( stdev(ratios,2)) );
	REQUIRE(meanT==Approx(-0.5*meanV).epsilon(stdev(Ts,meanT)+stdev(Vs,meanV)));

}
