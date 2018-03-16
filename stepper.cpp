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
 #include <chrono>
 #include <iostream>
 #include "stepper.h"
 #include "verlet.h"
 
Stepper::Stepper(const int nthreads, 
				const int from,
				const int to,
				std::vector<Particle*> particles,
				Node * (*precondition)(std::vector<Particle*>),
				void (*get_acceleration)(int i, std::vector<Particle*> particles,Node * root),
				const double dt,
				bool (*shouldContinue)(std::vector<Particle*> particles,int iter))
	: 	_nthreads(nthreads),
		_to(to),
		_iter(from),
		_particles(particles),
		_next_index(0),
		_precondition(precondition),
		_get_acceleration(get_acceleration),
		_dt(dt),
		_root(NULL),
		_shouldContinue(shouldContinue) {
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

// void Stepper::_increment_active_threads() {
	// _mutex_state.lock();
		// _active_threads++;
		// std::thread::id id=std::this_thread::get_id();
		// _thread_status[id]=Started;
		// _out_mutex.lock();
			// std::cout<<"step: incremented active threads"<<_active_threads<<", "<< id<<std::endl;
		// _out_mutex.unlock();
	// _mutex_state.unlock();

	// _cv_starting.notify_one();
// }	

int Stepper::_start_iteration(std::thread::id id) {
	_mutex_state.lock();

		int index = _next_index;
		_next_index++;
		
		if (index==0){
			if (!_shouldContinue(_particles,_iter))   ; // FIXME
			std::for_each(_particles.begin(),
							_particles.end(),
							[this](Particle* particle){verlet_positions(particle,this->_dt);});
			_root=_precondition(_particles);
			_thread_status[id]=Started;
			_cv_starting.notify_all();
		} else {
			if (_exists_some_equal(Started))
				_thread_status[id]=Started;
			else {
				_mutex_state.unlock();
				std::unique_lock<std::mutex> lck_starting(_mtx_starting);
				_cv_starting.wait(lck_starting,[this]{return this->_exists_some_equal(Started);});
				_mutex_state.lock();
				_thread_status[id]=Started;
			}
				
		}
		
	_mutex_state.unlock();
}  
void Stepper::step() {
	_mutex_state.lock();
		_active_threads++;
	_mutex_state.unlock();
	int index=-1;
	std::thread::id id=std::this_thread::get_id();
	_thread_status[id]=FreshlyCreated;
	
	// _increment_active_threads();
	_out_mutex.lock();
		std::cout<<__FILE__ << " "<< __LINE__ << 
		" ,iter="<<_iter<< ", to=" << _to << std::endl;
	_out_mutex.unlock();
	while (_iter<_to) {  // need to set index to 0 at end of iteration
		_out_mutex.lock();
			std::cout<<__FILE__ << " "<< __LINE__ << 
			" ,iter="<<_iter<< ", to=" << _to << ", index=" << index <<std::endl;
		_out_mutex.unlock();
		switch(_thread_status[id]) {
			case FreshlyCreated:
				index=_start_iteration(id);
				break;
			case Started:
							_out_mutex.lock();
					std::cout<<__FILE__ << " "<< __LINE__ << 
					" unexpected state " << _thread_status[id]<< std::endl;
				_out_mutex.unlock();
				break; 
			case Waiting:
							_out_mutex.lock();
					std::cout<<__FILE__ << " "<< __LINE__ << 
					" unexpected state " << _thread_status[id]<< std::endl;
				_out_mutex.unlock();
				break;
			case Restarting:
				index =_start_iteration(id);
				break;
			default:
				_out_mutex.lock();
					std::cout<<__FILE__ << " "<< __LINE__ << 
					" unexpected state " << _thread_status[id]<< std::endl;
				_out_mutex.unlock();
		};
		
			_out_mutex.lock();
			std::cout<<__FILE__ << " "<< __LINE__ << 
			" ,iter="<<_iter<< ", to=" << _to << ", index=" << index <<std::endl;
		_out_mutex.unlock();
		while (index<_particles.size()) {
			_process(index);
			_mutex_state.lock();
				index = _next_index;
				_next_index++;
			_mutex_state.unlock();
		}
		_mutex_state.lock();
	
			_thread_status[id]=Waiting;
			// _waiting_threads++;
			// _out_mutex.lock();
				// std::cout<<"step: waiting threads="<<_waiting_threads<<", "<< id<<std::endl;
			// _out_mutex.unlock();

			bool all_waiting = _all_equal(Waiting);
		_mutex_state.unlock();
		
		if (all_waiting) {
			std::for_each(_particles.begin(),
							_particles.end(),
							[this](Particle* particle){verlet_velocities(particle,this->_dt);});
			_mutex_state.lock();
				_iter++;
				_out_mutex.lock();
					std::cout<<__FILE__ << " "<< __LINE__ << 
					" ,iter="<<_iter<< ", to=" << _to << std::endl;
				_out_mutex.unlock();
				_next_index=0;
				delete _root;
				_root=NULL;
				_thread_status[id]=Restarting;
			_mutex_state.unlock();

			_cv_end_iter.notify_all();
		} else {
			std::unique_lock<std::mutex> lck_iter(_mtx_end_iter);
			_cv_end_iter.wait(lck_iter,[this]{return this->_exists_some_equal(Restarting);});
			_mutex_state.lock();
				_thread_status[id]=Restarting;
				// _restarted++;
				// if (_restarted==_nthreads-1) {
					// _restarted=0;
					// _waiting_threads=0;
					// _out_mutex.lock();
						// std::cout<<"reset "<<id << std::endl;
					// _out_mutex.unlock();
				// }
			_mutex_state.unlock();
		}
		
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
	_get_acceleration(index,_particles,_root);
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

