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

void AccelerationCalculator::visit(Particle & particle){
	particle.reset_acceleration();
}

void AccelerationCalculator::visit_pair(Particle & particle1,Particle & particle2){
	double x1,y1,z1;
	particle1.getPos(x1,y1,z1);
	double x2,y2,z2;
	particle2.getPos(x2,y2,z2);
	const double distance= sqrt(sqr(x1-x2)+sqr(y1-y2)+sqr(z1-z2));
	double d_factor=1/(distance*distance*distance);
	assert(particle1.get_mass()>0);
	assert(particle2.get_mass()>0);
	const double acc_x1 =  particle2.get_mass()*G*(x2-x1)*d_factor;
	const double acc_y1 =  particle2.get_mass()*G*(y2-y1)*d_factor;
	const double acc_z1 =  particle2.get_mass()*G*(z2-z1)*d_factor;
	particle1.accumulate_acceleration(acc_x1,acc_y1,acc_z1);
	const double acc_x2 =  particle1.get_mass()*G*(x1-x2)*d_factor;
	const double acc_y2 =  particle1.get_mass()*G*(y1-y2)*d_factor;
	const double acc_z2 =  particle1.get_mass()*G*(z1-z2)*d_factor;
	particle2.accumulate_acceleration(acc_x2,acc_y2,acc_z2);
	// check Newton's third law
	assert( fabs(particle1.get_mass()*acc_x1+particle2.get_mass()*acc_x2)<=epsilon*(fabs(particle1.get_mass()*acc_x1)+fabs(particle2.get_mass()*acc_x2)));
	assert( fabs(particle1.get_mass()*acc_y1+particle2.get_mass()*acc_y2)<=epsilon*(fabs(particle1.get_mass()*acc_y1)+fabs(particle2.get_mass()*acc_y2)));
	assert( fabs(particle1.get_mass()*acc_z1+particle2.get_mass()*acc_z2)<=epsilon*(fabs(particle1.get_mass()*acc_z1)+fabs(particle2.get_mass()*acc_z2)));
}

// ================================= legacy code starts here ====================================
	
void get_acceleration(unique_ptr<Particle[]> &particles, int n, double G) {
	for (int i=0;i<n;i++)
		particles[i].reset_acceleration();
	for (int i=1;i<n;i++)
		for (int j=0;j<i;j++)
			get_acceleration_between_pair(particles[i],particles[j],G);
}

const double epsilon=1e-8;  // tolerance for checking Newton's 3rd law

void get_acceleration_between_pair(Particle& p1,Particle& p2,double G) {
	double x1,y1,z1;
	p1.getPos(x1,y1,z1);
	double x2,y2,z2;
	p2.getPos(x2,y2,z2);
	const double distance= sqrt(sqr(x1-x2)+sqr(y1-y2)+sqr(z1-z2));
	double d_factor=1/(distance*distance*distance);
	assert(p1.get_mass()>0);
	assert(p2.get_mass()>0);
	const double acc_x1 =  p2.get_mass()*G*(x2-x1)*d_factor;
	const double acc_y1 =  p2.get_mass()*G*(y2-y1)*d_factor;
	const double acc_z1 =  p2.get_mass()*G*(z2-z1)*d_factor;
	p1.accumulate_acceleration(acc_x1,acc_y1,acc_z1);
	const double acc_x2 =  p1.get_mass()*G*(x1-x2)*d_factor;
	const double acc_y2 =  p1.get_mass()*G*(y1-y2)*d_factor;
	const double acc_z2 =  p1.get_mass()*G*(z1-z2)*d_factor;
	p2.accumulate_acceleration(acc_x2,acc_y2,acc_z2);
	// check Newton's third law
	assert( fabs(p1.get_mass()*acc_x1+p2.get_mass()*acc_x2)<=epsilon*(fabs(p1.get_mass()*acc_x1)+fabs(p2.get_mass()*acc_x2)));
	assert( fabs(p1.get_mass()*acc_y1+p2.get_mass()*acc_y2)<=epsilon*(fabs(p1.get_mass()*acc_y1)+fabs(p2.get_mass()*acc_y2)));
	assert( fabs(p1.get_mass()*acc_z1+p2.get_mass()*acc_z2)<=epsilon*(fabs(p1.get_mass()*acc_z1)+fabs(p2.get_mass()*acc_z2)));
}


/**
 *  Calculate kinetic energy for a system of particles
 */
double get_kinetic_energy(unique_ptr<Particle[]> &particles, int n) {
	double sum=0;
	for (int i=0;i<n;i++) {
		double vx,vy,vz;
		particles[i].getVel(vy,vz,vz);
		sum+=particles[i].get_mass()*(sqr(vx)+sqr(vy)+sqr(vz));
	}
	return 0.5*sum;
}

/**
 *  Calculate gravitational potential energy for a system of particles
 */
double get_potential_energy(unique_ptr<Particle[]> &particles, int n,const double G,const double a) {
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
void get_momentum(unique_ptr<Particle[]> &particles, int n,double& px,double& py,double &pz) { 
	px=0;py=0;pz=0;
	for (int i=0;i<n;i++){
		double vx,vy,vz;
		particles[i].getVel(vx,vy,vz);
		const double m=particles[i].get_mass();
		px+=m*vx;
		py+=m*vy;
		pz+=m*vz;
	}
}

/**
 * Find centre of mass of a set of particles.
 */
void get_centre_of_mass(unique_ptr<Particle[]> &particles, int n,double& x0,double& y0,double &z0) {
	x0=0; y0=0; z0=0;
	double m0=0;
	for (int i=0;i<n;i++){
		double x,y,z;
		particles[i].getPos(x,y,z);
		const double m=particles[i].get_mass();
		m0+=m;
		x0+=m*x;
		y0+=m*y;
		z0+=m*z;
	}
	x0/=m0; y0/=m0; z0/=m0;
}

/**
 * Find angular momentum of a set of particles
 */
void get_angular_momentum(unique_ptr<Particle[]> &particles, int n,double& lx,double& ly,double &lz) {
	lx=0;ly=0;lz=0;
	double x0,y0,z0;
	get_centre_of_mass(particles,n,x0,y0,z0);
	for (int i=0;i<n;i++){
		double x,y,z;
		particles[i].getPos(x,y,z);
		const double m=particles[i].get_mass();
		// Calculate unit radius vector from centre of mass
		const double d=sqrt(dsq(x0,y0,z0,x,y,z));
		const double rx=(x-x0)/d;
		const double ry=(y-y0)/d;
		const double rz=(z-z0)/d;
		
		double vx,vy,vz;
		particles[i].getVel(vx,vy,vz);
		
		lx+=m*(ry*vz-rz*vy);
		ly+=m*(rz*vx-rx*vz);
		lz+=m*(rx*vy-ry*vx);
	}
}

void get_acceleration(double m,double x,double y,double z,double _x,double _y,double _z,double dsq,double a, double G,
					double& acc_x, double& acc_y, double& acc_z){
	const double d_factor=pow(dsq+a*a,-3/2);
	acc_x=G*m*(x-_x)*d_factor;
	acc_y=G*m*(y-_y)*d_factor;
	acc_z=G*m*(z-_z)*d_factor;
}