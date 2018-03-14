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
 
#include <mutex>
#include <thread>
#include <vector>

#include "particle.h"

class Stepper {
  public:
	  Stepper(const int nthreads, const int from, const int to,std::vector<Particle*> particles);
	  void start();
	  void step();
	  
	  ~Stepper() ;
	  
  private:
	bool _shouldContinue();
	void _process(int me,int index);
	
  	const int 		_nthreads;
	const int 		_from;
	const int 		_to;
	int 			_ii;
	std::thread** 	_worker ;
	int 			_next_index=0;
	std::mutex 		_mutex,_out_mutex;
	int 			_active_threads=0;
	std::vector<Particle*> _particles;
};