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
 */
 

#ifndef _BARNES_HUT_H
#define _BARNES_HUT_H
#include <vector>
#include "particle.h"
#include "treecode.h"
#include "utils.h"

/**
 * Calculate acceleration for all particles
 */
void get_acceleration(std::vector<Particle*>&,const double theta,const double G,const double softening_length);


/**
 *  This class is used to calculate the acceleration of one particle 
 *  using the Barnes Hut algorithm
 *
 */
class BarnesHutVisitor :  public Node::Visitor{
  public:
   /**
   * Initialize BarnesHutVisitor for a specific particle
   */
	BarnesHutVisitor(Particle* me,const double theta, const double G,const double softening_length) :
	  _me(me),_theta_squared(sqr(theta)),_G(G),_acc_x(0),_acc_y(0),_acc_z(0),_softening_length_sq(sqr(softening_length)) {
		_me->getPos(_x,_y,_z);
	}
	
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
	void _accumulate_acceleration(double m,double x,double y,double z,double dsq);
	
	/**
	 * The particle whose acceleration is being calculated
	 */
	Particle * _me;
	
	/**
	 * Store squared theta to simplify comparisons
	 */ 
	const double _theta_squared;

	/**
	 * Position of the particle whose acceleration is being calculated
	 */
	double _x;
	double _y;
	double _z;
	
	/**
	 * Gravitational constant
	 */
	const double _G;
	
	const double _softening_length_sq;
	
	/**
	 * We accumulate the acceleration here
	 */
	double _acc_x;
	double _acc_y;
	double _acc_z;
};

#endif