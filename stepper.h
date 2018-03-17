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
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "particle.h"
#include "treecode.h"

class Stepper {
  public:
	/**
	 *  Status of a thread
	 */
    enum Status {
			FreshlyCreated, // => Started
			Started, 		// => Waiting
			Waiting,		// => Restarting || Finishing	
			Restarting,     // => Started
			Finishing
		};
		
	/**
	 * Create worker threads and initialize state
	 *
	 *  	nthreads			Number of threads
	 *  	from                Starting value for iterations
	 *  	to                  Final value for iterations
	 *      particles           The particles to be processed
	 *      precondition 		Used to build tree       
	 *      get_acceleration    Used to calculate acceleration of each particle
	 *      dt					Time interval
	 *      shouldContinue      Report results and decide whether to continue
	 */		
	  Stepper(  const int nthreads,
				const int from,
				const int to,
				std::vector<Particle*> particles,
				Node * (*precondition)(std::vector<Particle*>),
				void (*get_acceleration)(int i, std::vector<Particle*> particles,Node * root),
				const double dt,
				bool (*shouldContinue)(std::vector<Particle*> particles,int iter));
	  
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
	  
	  /**
	   * Dispose of threads
	   */
	  ~Stepper() ;
	  
  private:

  /**
	*  Build tree at start of each iteration
	*/
	int _start_iteration(std::thread::id id);

	/**
	 * Test: are all status values equal to specified value?
	 */
	bool _all_equal(Status s) {
		for ( const auto &p : _thread_status )
			if (p.second!=s) return false;

		return true;		
	}
	
	/**
	 * Test: is any status value equal to specified value?
	 */
	bool _exists_some_equal(Status s) {
		for ( const auto &p : _thread_status )
			if (p.second==s) return true;

		return false;		
	}
	
	int _thread_index(std::thread::id id) {
		int i=0;
		for ( const auto &p : _thread_status )
			if (p.first==id)
				return i;
			else i++;
		return -1;
	} 
	
	/**
	 *   Controls behaviour
	 */
	std::map<std::thread::id, Status> 	_thread_status;
	
	/**
	 *   Number of threads
	 */
  	const int 							_nthreads;

	/**
	 *  Iteration Control: finish value
	 */
	const int 							_to;
	
	/**
	 *  Iteration Control: current state
	 */
	int 								_iter;
	
	/**
	 *  Worker threads
	 */
	std::thread** 						_worker;
	
	/**
	 *   Index of next particle to be worked on
	 */
	int 								_next_index;
	
	/**
	 * Used to synchronize access to state variables
	 */
	std::mutex 							_mutex_state;
	
	/**
	 * Time interval
	 */
	const double 						_dt;

	/**
	 *  The octree
	 */
	Node * 								_root;
	
	/**
	 *  Particles to be processes
	 */
	std::vector<Particle*> 				_particles;
	
	/**
	 *  Used to let main know that tree has been built
	 */
	std::mutex 							_mtx_tree_has_been_built;

	std::condition_variable 			_cv_tree_has_been_built;
	
	/**
	 *  Used to let main know that threads have started
	 */
	std::mutex 							_mtx_starting;
	std::condition_variable 			_cv_starting;	

	/**
	 *  Used to let main know that threads are ending
	 */	
	std::mutex 							_mtx_finishing;
	std::condition_variable 			_cv_finishing;	

	/**
	 *  Used at end of an iteration, to wait until all threads have processed
	 *  their last particle
	 */
	std::mutex 				_mtx_end_iter;
	std::condition_variable _cv_end_iter;
		
	/**
	 *  Used to build tree
	 */
	Node * (*_precondition)(std::vector<Particle*>);
	
	/**
	 *  Used to calculate acceleration of each particle
	 */
	void (*_get_acceleration)(int i, std::vector<Particle*> particles,Node * root);
	
	/**
	 *    Report results and decide whether to continue
	 */
	bool (*_shouldContinue)(std::vector<Particle*> particles,int iter);
	

	
};