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
 * Integrate an Ordinary Differential Equation using Verlet algorithm.
 *
 * See http://physics.ucsc.edu/~peter/242/leapfrog.pdf
 */
 
 
#include <algorithm>
#include <cmath>
#include <iostream>
#include <mutex>
#include <thread>
#include "verlet.h"

/**
 *  Use Euler algorithm for first step. NB: this updates velocity only, so x
 *  remains at its initial value, which is what Verlet needs.
 *
 *  dt                Time step
 *  particles         Vector of particles
 */
void  euler(Particle* particle,double dt){
	double vx,vy,vz;
	particle->getVel( vx, vy,  vz);
	double ax,ay,az;
	particle->getAcc( ax, ay,  az);
	particle->setVel( vx+dt*ax, vy+dt*ay,  vz+dt*az);
}

/**
 *  First half of Verlet algorithm - update positions
 *
 *  dt                Time step
 *  particles         Vector of particles
 */
void  verlet_positions(Particle* particle,double dt) {
	double x,y,z;
	particle->getPos( x, y,  z);
	double vx,vy,vz;
	particle->getVel( vx, vy,  vz);
	particle->setPos( x+dt*vx, y+dt*vy,  z+ dt*vz);
}

/**
 *  Second half of Verlet algorithm - update velocities
 *
 *  dt                Time step
 *  particles         Vector of particles
 */
void  verlet_velocities(Particle* particle,double dt) {
	double vx,vy,vz;
	particle->getVel( vx, vy,  vz);
	double ax,ay,az;
	particle->getAcc( ax, ay,  az);
	particle->setVel( vx+dt*ax, vy+dt*ay,  vz+dt*az);
	
}

/**
 * Integrate by taking one Euler step, followed by repeated Verlet steps. 
 *
 *  get_acceleration  Used to determine acceleration of all particles
 *  dt                Time step
 *  particles         Vector of particles
 *  shouldContinue    Used after each iteration, for reporting and to determine whher to continue
 *  start_iterations  Initial iteration number. Normally zero, but non-zero if we resume after an earlier run
 */
void run_verlet(void (*get_acceleration)(std::vector<Particle*>),
				int max_iter,
				double dt,
				std::vector<Particle*> particles,
				bool (*shouldContinue)(std::vector<Particle*> particles,int iter),
				int start_iterations) {
	if (start_iterations==0){
		get_acceleration(particles);
		// Take a half step, so acceleration corresponds to 0.5dt, 1.5dt, etc.
		std::for_each(particles.begin(),particles.end(),[dt](Particle* particle){euler(particle,0.5*dt);});
	}
	
	for (int iter=1+start_iterations;iter<max_iter+start_iterations && shouldContinue(particles,iter);iter++) {
		std::for_each(particles.begin(),particles.end(),[dt](Particle* particle){verlet_positions(particle,dt);});
		get_acceleration(particles);
		std::for_each(particles.begin(),particles.end(),[dt](Particle* particle){verlet_velocities(particle,dt);});
	}
}

int next_index=0;
std::mutex _mutex,_out_mutex;
int active_threads=0;

void process(int me,int index) {
	_out_mutex.lock();
	std::cout<<me<<" process: "<<index<<std::endl;
	for (int i=0;i< 10000;i++){
		double x=std::log(i);
		double y=std::exp(x);
	}
	_out_mutex.unlock();
}

void step(int to) {
	std::this_thread::yield();
	_mutex.lock();
	const int me=active_threads;
	active_threads++;
	_out_mutex.lock();
	std::cout<<"step: "<<active_threads<<std::endl;
	_out_mutex.unlock();
	int index = next_index;
	next_index++;
	_mutex.unlock();
	
	while (index<to) {
		process(me,index);
		_mutex.lock();
		index = next_index;
		next_index++;
		_mutex.unlock();
	}
	_mutex.lock();
	active_threads--;
		_out_mutex.lock();
	std::cout<<"step: "<<active_threads<<std::endl;
		_out_mutex.unlock();
	if (active_threads>0)
		while (active_threads>0){
			_mutex.unlock();
			std::this_thread::yield();
			_mutex.lock();
		}
	_mutex.unlock();
}
void run_verlet(void (*get_acceleration)(std::vector<Particle*>),
				int max_iter,
				double dt,
				std::vector<Particle*> particles,
				bool (*shouldContinue)(std::vector<Particle*> particles,int iter),
				int start_iterations,
				int nthreads) {
	std::thread* worker[nthreads];
	next_index=0;
	std::cout << "next index " << next_index <<std::endl;
	for  (int i=0;i<nthreads;i++) 
		worker[i]=new std::thread(step, particles.size());

	
	// if (start_iterations==0){
		// get_acceleration(particles);
//		Take a half step, so acceleration corresponds to 0.5dt, 1.5dt, etc.
		// std::for_each(particles.begin(),particles.end(),[dt](Particle* particle){euler(particle,0.5*dt);});
	// }
	
	// for (int iter=1+start_iterations;iter<max_iter+start_iterations && shouldContinue(particles,iter);iter++) {
		// std::for_each(particles.begin(),particles.end(),[dt](Particle* particle){verlet_positions(particle,dt);});
		// get_acceleration(particles);
		// std::for_each(particles.begin(),particles.end(),[dt](Particle* particle){verlet_velocities(particle,dt);});
	// }
	
	for (int i=0;i<nthreads;i++){
		std::cout << "joining " << i <<std::endl;
		worker[i]->join();
		delete worker[i];
	}
	

}



