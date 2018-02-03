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
void get_acceleration(std::vector<Particle*>&,double theta,double G);


/**
 *  This class is used to calculate acceleration using the Barnes Hut algorithm
 *
 */
class BarnesHutVisitor :  public Node::Visitor{
  public:
	BarnesHutVisitor(Particle* me,const double theta, const double G) :
	  _me(me),_theta2(sqr(theta)),_G(G),_acc_x(0),_acc_y(0),_acc_z(0) {
		_me->getPos(_x,_y,_z);
	}
	
	virtual Node::Visitor::Status visit(Node * node);
	
	void store_accelerations(Particle*me);
	
  private:
	void _accumulate_acceleration(double m,double x,double y,double z,double dsq);
	Particle * _me;
	const double _theta2;   //Theta squared
	
	double _x;
	double _y;
	double _z;
	
	double _G;
	
	double _acc_x;
	double _acc_y;
	double _acc_z;
};

#endif