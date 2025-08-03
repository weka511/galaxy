/**
 * Copyright (C) 2018-2025 Greenweaves Software Limited
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
using namespace std;
double dt;
int n_orbits;
int max_iter;
int n_freq;
int iter;

ofstream logger;

void get_acceleration(vector<Particle*> particles) {
	get_acceleration(particles,G_solar_system);
}

bool record_orbit(vector<Particle*> particles,int iter) {
	if (iter%n_freq==0)
		for (int i=0;i<particles.size();i++){
			double x,y,z;
			particles[i]->getPos(x,y,z);
			logger << x << "," << y << "," << z << endl;
			// cout <<  "E: " << get_kinetic_energy(particles) + get_potential_energy(particles,G_solar_system,0) << endl;
			double px,py,pz;
			get_momentum(particles,px,py,pz);
			// cout<< "P: (" <<px << ", " <<py << ", " <<pz << ")" << endl;
			double cx,cy,cz;
			get_centre_of_mass(particles,cx,cy,cz);
			// cout<< "C: (" <<cx << ", " <<cy << ", " <<cz << ")" << endl;
			double lx,ly,lz;
			get_angular_momentum(particles,lx,ly,lz);
			// cout<< "L: (" <<lx << ", " <<ly << ", " <<lz << ")" << endl;
		}
	iter++;
	return true;
}

bool record_all(vector<Particle*> particles,int iter) {
	if (iter%n_freq==0)
		for (int i=0;i<particles.size();i++){
			cout<<i<<endl;
			double x,y,z;
			particles[i]->getPos(x,y,z);
			// cout << x << "," << y << "," << z << endl;
			double vx,vy,vz;
			particles[i]->getVel(vx,vy,vz);
			// cout << vx << "," << vy << "," << vz << endl;
			double ax,ay,az;
			particles[i]->getAcc(ax,ay,az);
			// cout << ax << "," << ay << "," << az << endl;
		}
	iter++;
	return true;
}


TEST_CASE( "Kepler Tests", "[kepler]" ) {
	
	SECTION("Simple Earth-Sun Kepler"){
		cout << "Simple Earth-Sun Kepler" << endl;
		dt=0.001;
		n_orbits=1;
		max_iter=(int)(2*M_PI*n_orbits/dt);
		n_freq=1;
		iter=0;
		logger.open("kepler.csv");
		vector<Particle*> particles;
		particles.push_back(new Particle(0,0,0,0,0,0,1));
		particles.push_back(new Particle(1,0,0,0,2*M_PI,0,mass_earth/mass_sun));
		run_verlet(&get_acceleration,max_iter, dt,  particles,&record_orbit,0);
		logger.close();
	}
	
	SECTION("Lagrange Points"){
		cout << "Lagrange Points" << endl;
		dt=0.001;
		n_orbits=1;
		max_iter=(int)(2*M_PI*n_orbits/dt);
		n_freq=1;
		iter=0;
		logger.open("lagrange.csv");
		vector<Particle*> particles;
		particles.push_back(new Particle(0,0,0,0,0,0,1));
		particles.push_back(new Particle(1,0,0, 0,2*M_PI,0, mass_earth/mass_sun));
		const double x2=0.5;
		const double y2=sqrt(3.0)/2;
		particles.push_back(new Particle(x2+0.05,y2-0.05,0.05, -2*M_PI*y2, 2*M_PI*x2,0,  mass_earth/mass_sun));
		run_verlet(&get_acceleration,max_iter, dt,  particles,&record_orbit,0);
		logger.close();
	}
}
