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
 
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "catch.hpp"
#include "particle.h"
#include "physics.h"
#include "utils.h"
#include "verlet.h"

double dt;
int n_orbits;
int max_iter;
int n_freq;
int iter;

void get_acceleration(std::vector<Particle*> particles) {
	get_acceleration(particles,G_solar_system);
}


std::ofstream logger;

bool shouldContinue(std::vector<Particle*> particles) {
	if (iter%n_freq==0)
		for (int i=0;i<particles.size();i++){
			double x,y,z;
			particles[i]->getPos(x,y,z);
			logger << x << "," << y << "," << z << std::endl;
		}
	iter++;
	return true;
}


TEST_CASE( "Kepler Tests", "[kepler]" ) {
	
	SECTION("Simple Earth-Sun Kepler"){
		dt=0.001;
		n_orbits=1;
		max_iter=(int)(2*M_PI*n_orbits/dt);
		n_freq=1;
		iter=0;
		logger.open("kepler.csv");
		std::vector<Particle*> particles;
		particles.push_back(new Particle(0,0,0,0,0,0,1));
		particles.push_back(new Particle(1,0,0,0,2*M_PI,0,mass_earth/mass_sun));
		run_verlet(&get_acceleration,max_iter, dt,  particles,&shouldContinue);
		logger.close();
	}
	
	SECTION("Lagrange Points"){
		dt=0.001;
		n_orbits=1;
		max_iter=(int)(2*M_PI*n_orbits/dt);
		n_freq=1;
		iter=0;
		logger.open("lagrange.csv");
		std::vector<Particle*> particles;
		particles.push_back(new Particle(0,0,0,0,0,0,1));
		particles.push_back(new Particle(1,0,0, 0,2*M_PI,0, mass_earth/mass_sun));
		const double x2=0.5;
		const double y2=sqrt(3.0)/2;
		
		particles.push_back(new Particle(x2,y2,0, -2*M_PI*y2, 2*M_PI*x2,0,  mass_earth/mass_sun));
		run_verlet(&get_acceleration,max_iter, dt,  particles,&shouldContinue);
		logger.close();
	}
}
