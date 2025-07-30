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
 */

#include <assert.h> 
#include <cmath>
#include <iostream>
#include "physics.hpp"

using namespace std;


const double Physics::seconds_in_one_year = 365.25*24*60*60;

const double Physics::mass_earth = 5.9722e24;  // https://en.wikipedia.org/wiki/Earth_mass

const double Physics::mass_sun = 1.98855e30;  // https://en.wikipedia.org/wiki/Solar_mass

const double Physics::metres_per_AU = 149597870700; // https://en.wikipedia.org/wiki/Astronomical_unit

const double Physics::au_per_m = 1/metres_per_AU;

const double Physics::G_SI = 6.67408e-11; // https://en.wikipedia.org/wiki/Gravitational_constant

const double Physics::G_solar_system = G_SI * au_per_m*au_per_m*au_per_m *(mass_sun)*(seconds_in_one_year *seconds_in_one_year);
	
/**
 *  Calculate kinetic energy for a system of particles
 */
double Physics::get_kinetic_energy(unique_ptr<Particle[]> &particles, int n) {
	auto  sum=0.0;
	for (int i=0;i<n;i++) {
		auto mass = particles[i].get_mass();
		auto velocity = particles[i].get_velocity();
		for (int j=0;j<3;j++)
			sum+=mass*sqr(velocity[j]);
	}
	return 0.5*sum;
}

/**
 *  Calculate gravitational potential energy for a system of particles
 */
double Physics::get_potential_energy(unique_ptr<Particle[]> &particles, int n,const double G,const double a) {
	double sum=0;
	for (int i=1;i<n;i++)
		for (int j=0;j<i;j++)
			sum += particles[i].get_mass() * particles[j].get_mass() /
					sqrt( particles[i].get_distance_sq(particles[j]) + sqr(a) );

	return -G*sum;
}

/**
 * Find linear momentum of a set of particles
 */
void Physics::get_momentum(unique_ptr<Particle[]> &particles, int n,double& px,double& py,double &pz) { 
	px=0;py=0;pz=0;
	for (int i=0;i<n;i++){
		const double m=particles[i].get_mass();
		auto V = particles[i].get_velocity();
		px += m*V[0];
		py += m*V[1];
		pz += m*V[2];
	}
}

/**
 * Find centre of mass of a set of particles.
 */
void Physics::get_centre_of_mass(unique_ptr<Particle[]> &particles, int n,double& x0,double& y0,double &z0) {
	x0=0; y0=0; z0=0;
	double m0=0;
	for (int i=0;i<n;i++){
		auto X = particles[i].get_position();
		const double m=particles[i].get_mass();
		m0 += m;
		x0 += m*X[0];
		y0 += m*X[1];
		z0 += m*X[2];
	}
	x0/=m0; y0/=m0; z0/=m0;
}

/**
 * Find angular momentum of a set of particles
 */
void Physics::get_angular_momentum(unique_ptr<Particle[]> &particles, int n,double& lx,double& ly,double &lz) {
	lx=0;ly=0;lz=0;
	double x0,y0,z0;
	get_centre_of_mass(particles,n,x0,y0,z0);
	for (int i=0;i<n;i++){
		// double x,y,z;
		auto X = particles[i].get_position();
		const double m=particles[i].get_mass();
		// Calculate unit radius vector from centre of mass
		const double d=sqrt(Physics::dsq(x0,y0,z0,X[0],X[1],X[2]));
		const double rx=(X[0]-x0)/d;
		const double ry=(X[1]-y0)/d;
		const double rz=(X[2]-z0)/d;
				
		auto V = particles[i].get_velocity();
		
		lx+=m*(ry*V[2]-rz*V[1]);
		ly+=m*(rz*V[0]-rx*V[2]);
		lz+=m*(rx*V[1]-ry*V[0]);
	}
}

