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
 
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "particle.h"

class Stepper {
  public:
	  Stepper(const int nthreads, const int from, const int to,std::vector<Particle*> particles);
	  
	  /**
	   * Start processing.
	   * 1. Start all threads
	   * 2. Wait until at least one thread is active
	   * 3. Wait again until all threads are quiescent
	   */	  
	  void start();
	  
	  /**
	   * Step through all the work that needs to be done
	   */
	  void step();
	  
	  ~Stepper() ;
	  
  private:
	bool _shouldContinue();
	void _process(int index);
	
  	const int 				_nthreads;
	const int 				_from;
	const int 				_to;
	int 					_ii;
	std::thread** 			_worker;
	int 					_next_index;
	
	/**
	 * Used to synchronize access to state variables
	 */
	std::mutex 				_mutex_state;
	
	/**
	 * Used to synchronize access to console for debugging
	 */
	std::mutex				_out_mutex;
	
	int 					_active_threads=0;
	std::vector<Particle*> 	_particles;
	
	/**
	 *  Used to let main know that threads have started
	 */
	std::mutex 				_mtx_starting;
	
	/**
	 *  Used to let main know that threads have started
	 */
	std::mutex 				_mtx_ending;
	
	/**
	 *  Used to let main know that threads are ending
	 */
	std::condition_variable _cv_starting;
	
	/**
	 *  Used to let main know that threads are ending
	 */
	std::condition_variable _cv_ending;	

};