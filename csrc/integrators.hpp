#ifndef _INTEGRATORS_HPP
#define _INTEGRATORS_HPP

/**
 * Copyright (C) 2025 Simon Crase
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
 * Integrate an Ordinary Differential Equation using the Leapfrog algorithm
 */
 

#include "configuration.hpp"
#include "treecode.hpp"
#include "barnes-hut.hpp"
#include "reporter.hpp"

using namespace std;

/**
 *  Use Euler algorithm for first step. NB: this updates velocity only, so x
 *  remains at its initial value, which is what Leapfrog needs.
 *
 */
class Euler : public Configuration::Visitor {
  private:
	/**
	 *  _dt  Time step - set to half the timestep for Leapfrog
	 */ 
	const double _dt;
	
  public:
	Euler(const double dt) :_dt(dt){;}
	
	void visit(Particle & particle);
};

class Leapfrog {


	/**
	 * Class used to implement second half of Verlet algorithm - update positions
	 */	
	class PositionUpdater : public Configuration::Visitor {
	  private:
		const double _dt;
		
	  public:
		PositionUpdater(const double dt) :_dt(dt){;}
		
		/**
		 * Calculate position one step ahead
		 */
		void visit(Particle & particle);
	};	

	/**
	 * Class used to implement first half of Verlet algorithm - update velocities
	 */
	class VelocityUpdater : public Configuration::Visitor {
	  private:
		const double _dt;
		
	  public:
		VelocityUpdater(const double dt) :_dt(dt){;}
		
		/**
		 * Calculate velocity one step ahead, 
		 * which is half a step ahead of position
		 */
		void visit(Particle & particle);
	};	

  private:
	Configuration & _configuration;
	AccelerationVisitor &_calculate_acceleration;
	Reporter & _reporter;
	
  public:
	Leapfrog(Configuration & configuration, AccelerationVisitor &calculate_acceleration,Reporter & reporter)
	:  	_configuration(configuration),
		_calculate_acceleration(calculate_acceleration),
		_reporter(reporter) {};
	
	/**
	 * This function is responsible for integrating an ODE.
	 *
	 * Parameters:
	 *     max_iter   Number of iterations
	 *     dt         Time step
	 */
	void run( int max_iter,const double dt);
};


#endif  // _INTEGRATORS_HPP