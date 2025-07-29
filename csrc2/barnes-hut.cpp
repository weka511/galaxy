/**
 * Copyright (C) 2018-2025 Greenweaves Software Limited
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
 
#include <algorithm>
#include <assert.h>
#include <limits>
#include <string>
#include <cmath>
#include <iostream>

#include "barnes-hut.hpp"
#include "center-of-mass.hpp"
#include "physics.hpp"

using namespace std;


/**
* Initialize BarnesHutVisitor for a specific particle
*
*  index   Keep track of particle index so we don't calculate acceleration of particle caused by itself!
*  me      Particle being processed
*  theta   Angle for Barnes G=Hut cutoff
*  G       Gravitational constant
*  a       Softening length
*/ 
BarnesHutVisitor::BarnesHutVisitor(const int index,Particle& me,const double theta, const double G,const double a) :
  _index(index),_me(me),_theta_squared(sqr(theta)),_G(G),_a(a),_acc_x(0),_acc_y(0),_acc_z(0) {
	_me.getPos(_x,_y,_z);
}

/**
 * Used to accumulate accelerations for each node
 */
Node::Visitor::Status BarnesHutVisitor::visit(Node * node) {
	// cout << __FILE__ << " " << __LINE__ << endl;
	double m,x,y,z;
	auto mass_and_centre = node->get_mass_and_centre();
	tie(m,x,y,z) = mass_and_centre;
	double dsq_node;
	double l_sqr;
	const int status = node->getStatus();
	switch (status) {
		case Node::Internal:
			dsq_node=dsq(x,y,z,_x,_y,_z);
			/*
			 * Is this node are enough away that its particles can be lumped?
			 */
			l_sqr=sqr(node->getSide());
			if (l_sqr/dsq_node<_theta_squared) {
				_accumulate_acceleration(m,x,y,z,dsq_node);
				return Node::Visitor::Status::DontDescend;
			} else
				return Node::Visitor::Status::Continue;
		case Node::Unused:
			return Node::Visitor::Status::Continue;
		default: // External Node - accumulate except don't accumulate self!
			if (status!=_index) 
				_accumulate_acceleration(m,x,y,z,dsq(x,y,z,_x,_y,_z)); 			
			return Node::Visitor::Status::Continue;
	}
}

/**
 * Used at the end of calculation to store accelerations back into particle
 */
void BarnesHutVisitor::store_accelerations() {
		cout << __FILE__ << " " << __LINE__ << " " << _acc_x<< " " << _acc_y << " " << _acc_z<<endl;
	auto acceleration = array<double,3>{_acc_x, _acc_y, _acc_z};
	_me.set_acceleration(acceleration);
}

/**
 * Used to add in the contribution to the acceleration from one Node
 * NB: there is a new instance of the visitor for each particle, so
 * acceleration is always zero at the start.
 */
void BarnesHutVisitor::_accumulate_acceleration(double m,double x,double y,double z,double dsq){
	double acc_x, acc_y, acc_z;
	get_acceleration( m, x, y, z, _x, _y, _z, dsq,_a, _G, acc_x,  acc_y,  acc_z);
	_acc_x += acc_x;
	_acc_y += acc_y;
	_acc_z += acc_z;
}
