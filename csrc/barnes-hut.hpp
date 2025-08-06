#ifndef _BARNES_HUT_HPP
#define _BARNES_HUT_HPP

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
 */
 
#include <array>
#include <tuple>
#include "configuration.hpp"
#include "treecode.hpp"


using namespace std;


/**
 *  This class is used to calculate the acceleration of one particle 
 *  using the Barnes Hut algorithm
 *
 */
class BarnesHutVisitor :  public Node::Visitor{
  private:
  	/**
	 * Keep track of particle index so we don't calculate acceleration of particle caused by itself!
	 */
	const int _index = -1;
	
	/**
	 * The particle whose acceleration is being calculated
	 */
	Particle & _me;
	
	/**
	 * Store squared theta to simplify comparisons
	 */ 
	const double _theta_squared;
	
	/**
	 * Gravitational constant
	 */
	const double _G;

	/**
	 * Position of the particle whose acceleration is being calculated
	 */
	array<double,3> _position;
	/**
	 * Softening length
	 */
	const double _a;
	
	/**
	 * We accumulate the acceleration here
	 */
	array<double,3> _acceleration;
  
  public:
   /**
   * Initialize BarnesHutVisitor for a specific particle
   *
   *  index   Keep track of particle index so we don't calculate acceleration of particle caused by itself!
   *  me      Particle being processed
   *  theta   Ratio for Barnes G=Hut cutoff (Barnes and Hut recommend 1.0)
   *  G       Gravitational constant
   *  a       Softening length
   */
	BarnesHutVisitor(const int index,Particle& me,const double theta, const double G,const double a);
	
	/**
	 * Used to accumulate accelerations for each node
	 */
	virtual Node::Visitor::Status visit(Node * node);
	
	/**
	 * Used at the end of calculation to store accelerations back into particle
	 */
	void store_accelerations();
	
	
  private:
  
	/**
	 * Used to add in the contribution to the acceleration from one Node
	 */
	void _accumulate_acceleration(double m,array<double,3> X,double dsq);
		
};

#endif  //BARNES_HUT_HPP
