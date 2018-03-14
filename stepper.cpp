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
 
 #include <chrono>
 #include <iostream>
 #include "stepper.h"
 
Stepper::Stepper(const int nthreads, const int from, const int to,std::vector<Particle*> particles)
	: _nthreads(nthreads),_from(from),_to(to),_ii(from),_particles(particles) {
	_worker = new std::thread*[_nthreads];
	std::cout << "Processing " << particles.size() << " particles" << std::endl;
}

void Stepper::start(){
	for  (int i=0;i<_nthreads;i++) 
		_worker[i]=new std::thread(&Stepper::step,this); //https://stackoverflow.com/questions/10673585/start-thread-with-member-function
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	while (_active_threads>0){
		std::this_thread::yield();
	}
}
	  
void Stepper::step() {
	std::this_thread::yield();
	_mutex.lock();
		const int me=_active_threads;
		_active_threads++;
		_out_mutex.lock();
			std::cout<<"step: "<<_active_threads<<std::endl;
		_out_mutex.unlock();
		int index = _next_index;
		_next_index++;
	_mutex.unlock(); 
	while (index<_particles.size()) {
		_process(me,index);
		_mutex.lock();
			index = _next_index;
			_next_index++;
		_mutex.unlock();
	}
	_mutex.lock();
		_active_threads--;
		_out_mutex.lock();
			std::cout<<"step: "<<_active_threads<<std::endl;
		_out_mutex.unlock();
		if (_active_threads>0)
			while (_active_threads>0){
				_mutex.unlock();
				std::this_thread::yield();
				_mutex.lock();
			}
	_mutex.unlock();
}

void Stepper::_process(int me,int index) {
	_out_mutex.lock();
		std::cout<<me<<" process: "<<index<<std::endl;
		for (int i=0;i< 10;i++){
			double x=std::log(i);
			double y=std::exp(x);
		}
	_out_mutex.unlock();
}

bool Stepper::_shouldContinue() {
	return _ii<_to;
}
	  
Stepper::~Stepper() {
	for (int i=0;i<_nthreads;i++){
		_out_mutex.lock();
			std::cout << "joining " << i <<std::endl;
			_worker[i]->join();
		_out_mutex.unlock();
	}
	for (int i=0;i<_nthreads;i++)
		delete _worker[i];
}

