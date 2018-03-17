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
 #include "spdlog/spdlog.h"


namespace spd = spdlog;

 /**
  * Create worker threads and initialize state
  */
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
		_shouldContinue(shouldContinue)	{
	_worker = new std::thread*[_nthreads];
	std::cout << "Processing " << particles.size() << " particles" << std::endl;
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
    _cv_starting.wait(lck_starting,[this]{return this->_exists_some_equal(Started);});

	for  (int i=0;i<_nthreads;i++){
		std::thread::id id=_worker[i]->get_id();
		spdlog::get("galaxy")->info("{0} {1} {2} {3}",
								__FILE__,__LINE__ ,_thread_index(id),_thread_status[id]);
	}
	
	std::unique_lock<std::mutex> lck_finishing(_mtx_finishing);
	_cv_finishing.wait(lck_finishing,[this]{return this->_all_equal(Finishing);});
	for  (int i=0;i<_nthreads;i++){
		std::thread::id id=_worker[i]->get_id();
		spdlog::get("galaxy")->info("{0} {1} {2} {3}",
								__FILE__,__LINE__ ,_thread_index(id),_thread_status[id]);
	}
}

/**
 *  Build tree at start of each iteration
 */
int Stepper::_start_iteration(std::thread::id id) {
	_mutex_state.lock();

		int index = _next_index;
		_next_index++;
		
		if (index==0){

			std::for_each(_particles.begin(),
							_particles.end(),
							[this](Particle* particle){verlet_positions(particle,this->_dt);});
			delete _root;
			
			_root=_precondition(_particles);
			_thread_status[id]=Started;
			_cv_tree_has_been_built.notify_all();
			_cv_starting.notify_all();
		} else {
			if (_exists_some_equal(Started)){
				_thread_status[id]=Started;
				_cv_starting.notify_all();
			} else {
				_mutex_state.unlock();
				std::unique_lock<std::mutex> lock(_mtx_tree_has_been_built);
				_cv_tree_has_been_built.wait(lock,[this]{return this->_exists_some_equal(Started);});
				_mutex_state.lock();
				_thread_status[id]=Started;
			}
				
		}
		
	_mutex_state.unlock();
	return index;
}  
void Stepper::step() {
	
	int index=-1;
	std::thread::id id=std::this_thread::get_id();
	_thread_status[id]=FreshlyCreated;
	
	while (_iter<_to) {  // need to set index to 0 at end of iteration
		spdlog::get("galaxy")->info("{0} {1} {2} {3}: {4}<{5}",
									__FILE__,__LINE__ ,	_thread_index(id),_thread_status[id],
									_iter, _to );

		switch(_thread_status[id]) {
			case FreshlyCreated:
				index=_start_iteration(id);
				break;
			case Restarting:
				index =_start_iteration(id);
				break;
			default:
				spdlog::get("galaxy")->info("{0} {1} {2} {3}: unexpected",
									__FILE__,__LINE__ ,_thread_index(id),_thread_status[id]);
		};

		while (index<_particles.size()) {
			spdlog::get("galaxy")->info("{0} {1} {2} {3}: {4}<{5} ",
										__FILE__,__LINE__ ,_thread_index(id),_thread_status[id], 
										index,_particles.size());
			
			_process(index);
			_mutex_state.lock();
				index = _next_index;
				_next_index++;
			_mutex_state.unlock();
		}
		_mutex_state.lock();
			_thread_status[id]=Waiting;
			const bool all_waiting = _all_equal(Waiting);
		_mutex_state.unlock();
		
		if (all_waiting) {
			std::for_each(_particles.begin(),
							_particles.end(),
							[this](Particle* particle){verlet_velocities(particle,this->_dt);});
			_mutex_state.lock();

				if (!_shouldContinue(_particles,_iter))   {
					_thread_status[id]=Finishing;
					spdlog::get("galaxy")->info("{0} {1} {2} {3}: ",
										__FILE__,__LINE__ ,_thread_index(id),_thread_status[id]);
					_mutex_state.unlock();
					_cv_finishing.notify_all();
					_cv_end_iter.notify_all();
					return;
				} else {
					_iter++;
					_next_index=0;
					_thread_status[id]=Restarting;
				}
			_mutex_state.unlock();
			_cv_end_iter.notify_all();

		} else {
			std::unique_lock<std::mutex> lck_iter(_mtx_end_iter);
			_cv_end_iter.wait(lck_iter,[this]{
												return this->_exists_some_equal(Restarting) ||
															this->_exists_some_equal(Finishing);
											});
			_mutex_state.lock();
				if (this->_exists_some_equal(Finishing)){
					_thread_status[id]=Finishing;
					spdlog::get("galaxy")->info("{0} {1} {2} {3}",
									__FILE__,__LINE__ ,_thread_index(id),_thread_status[id]);
					_mutex_state.unlock();
					_cv_finishing.notify_all();
					return;
				}
				_thread_status[id]=Restarting;
				_next_index=0;
			_mutex_state.unlock();

		}
		
	}

	spdlog::get("galaxy")->info("{0} {1} {2} {3}: {4} {5}",
									__FILE__,__LINE__ ,_thread_index(id),_thread_status[id],_iter, _to);
	
	_mutex_state.lock();
		_thread_status[id]=Finishing;
	_mutex_state.unlock();

	spdlog::get("galaxy")->info("{0} {1} {2} {3}: {4} {5}",
									__FILE__,__LINE__ ,_thread_index(id),_thread_status[id],_iter,_to);
	
	_cv_finishing.notify_all();
}

void Stepper::_process(int index) {
	_get_acceleration(index,_particles,_root);
}


	  
Stepper::~Stepper() {
	if (_root!=NULL)
		delete _root;

	for (int i=0;i<_nthreads;i++){
		spdlog::get("galaxy")->info("{0} {1} {2}",
									__FILE__,__LINE__ ,i);
		_worker[i]->join();
	}
	for (int i=0;i<_nthreads;i++)
		delete _worker[i];
	
	delete [] _worker;
}

