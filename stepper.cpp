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
	: _nthreads(nthreads),
		_to(to),
		_iter(from),
		_particles(particles),
		_next_index(0) {
	_worker = new std::thread*[_nthreads];
	_out_mutex.lock();
		std::cout << "Processing " << particles.size() << " particles" << std::endl;
	_out_mutex.unlock();
}

/**
 * Start processing.
 * 1. Start all threads
 *    ..see https://stackoverflow.com/questions/10673585/start-thread-with-member-function
 * 2. Wait until at least one thread is active
 * 3. Wait again until all threads are quiescent
 */
void Stepper::start(){

	for  (int i=0;i<_nthreads;i++) 
		_worker[i]=new std::thread(&Stepper::step,this); 
	
	std::unique_lock<std::mutex> lck_starting(_mtx_starting);
    _cv_starting.wait(lck_starting,[this]{return this->_active_threads >0;});

	std::unique_lock<std::mutex> lck_ending(_mtx_ending);
	_cv_ending.wait(lck_ending,[this]{return this->_active_threads <=0;});

}

int Stepper::_increment_active_threads() {
	_mutex_state.lock();
		_active_threads++;
		_out_mutex.lock();
			std::cout<<"step: incremented active threads"<<_active_threads<<std::endl;
		_out_mutex.unlock();
		int index = _next_index;
		_next_index++;
	_mutex_state.unlock();
	
	_cv_starting.notify_one();
	
	return index;
}	
  
void Stepper::step() {
	int index=_increment_active_threads();
	
	while (index<_particles.size()) {
		_process(index);
		_mutex_state.lock();
			index = _next_index;
			_next_index++;
		_mutex_state.unlock();
	}
	
	_mutex_state.lock();
		_active_threads--;
		_out_mutex.lock();
			std::cout<<"step: decrementing active "<<_active_threads<<std::endl;
		_out_mutex.unlock();
	_mutex_state.unlock();
	
	_cv_ending.notify_all();
}

void Stepper::_process(int index) {
	_out_mutex.lock();
		std::cout<<std::this_thread::get_id()<<" process: "<<index <<std::endl;
		for (int i=0;i< 10;i++){
			double x=std::log(i);
			double y=std::exp(x);
		}
	_out_mutex.unlock();
}

bool Stepper::_shouldContinue() {
	return _iter<_to;
}
	  
Stepper::~Stepper() {
	for (int i=0;i<_nthreads;i++){
		_out_mutex.lock();
			std::cout << "joining " << i <<std::endl;
		_out_mutex.unlock();
		_worker[i]->join();
	}
	for (int i=0;i<_nthreads;i++)
		delete _worker[i];
}

