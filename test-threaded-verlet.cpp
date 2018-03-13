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
 * This file exercises the Verlet algorithm for some simple N-body cases
 * without using the Barnes Hut approximations.
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

// double dt;
// int n_orbits;
// int max_iter;
// int n_freq1;
// int iter;

//td::ofstream logger;

void get_acceleration1(std::vector<Particle*> particles) {
	get_acceleration(particles,G_solar_system);
}

bool record_orbit1(std::vector<Particle*> particles,int iter) {

	return true;
}

bool record_all1(std::vector<Particle*> particles,int iter) {

	return true;
}


TEST_CASE( "Threading Tests", "[threading]" ) {
	
	SECTION("Simple Earth-Sun Kepler"){
		std::cout << "Simple Earth-Sun Kepler" << std::endl;
		// dt=0.001;
		// n_orbits=1;
		// max_iter=(int)(2*M_PI*n_orbits/dt);
//		n_freq=1;
//		iter=0;
//		logger.open("kepler.csv");
		std::vector<Particle*> particles;
		for (int i=0;i<100;i++)
			particles.push_back(new Particle(0,0,0,0,0,0,1));

		run_verlet(&get_acceleration1,100, 0.01,  particles,&record_orbit1,0,4);
//		logger.close();
	}
	
	
}
