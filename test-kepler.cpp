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
#include <vector>
#include <cmath>
#include "catch.hpp"
#include "particle.h"
#include "physics.h"
#include "utils.h"
#include "verlet.h"

double dt=0.001;
int max_iter=(int)(2*M_PI/dt);

void get_acceleration(std::vector<Particle*> particles) {
	for (int i=1;i<particles.size();i++)
		for (int j=0;j<i;j++){
			double x0,y0,z0;
			particles[i]->getPos(x0,y0,z0);
			double x1,y1,z1;
			particles[j]->getPos(x1,y1,z1);
			const double distance= sqrt(sqr(x0-x1)+sqr(y0-y1)+sqr(z0-z1));
			double d_factor=1/(distance*distance*distance);
			const double acc_x  =G_solar_system*(x1-x0);
			const double acc_y  =G_solar_system*(y1-y0);
			const double acc_z  =G_solar_system*(z1-z0);
			const double acc_x0 =  particles[j]->getMass()*acc_x;
			const double acc_y0 =  particles[j]->getMass()*acc_y;
			const double acc_z0 =  particles[j]->getMass()*acc_z;
			particles[i]->setAcc(acc_x0,acc_y0,acc_z0);
			const double acc_x1 =  particles[i]->getMass()*acc_x;
			const double acc_y1 =  particles[i]->getMass()*acc_y;
			const double acc_z1 =  particles[i]->getMass()*acc_z;
			particles[j]->setAcc(acc_x1,acc_y1,acc_z1);
	}
}

bool shouldContinue(std::vector<Particle*> particles) {
	for (int i=0;i<particles.size();i++) {
		double x,y,z;
		particles[i]->getPos(x,y,z);
		std::cout << x << "," << y << "," << z << std::endl;
	}
	return true;
}



TEST_CASE( "Kepler Tests", "[kepler]" ) {
     
	 SECTION( "Test my assumptions about physical constants" ) {
        REQUIRE( 4*M_PI*M_PI==Approx(G_solar_system).epsilon(0.0001) );
    }
	
	SECTION("Simple Earth-Sun Kepler"){
		std::vector<Particle*> particles;
		particles.push_back(new Particle(0,0,0,0,0,0,1));
		particles.push_back(new Particle(1,0,0,0,2*M_PI,0,mass_earth/mass_sun));
		run_verlet(&get_acceleration,max_iter, dt,  particles,&shouldContinue);
	}
}
