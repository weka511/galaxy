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
 
#include <cmath>

#include "barnes-hut.hpp"

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
	_position = _me.get_position();
}

/**
 * Used to accumulate accelerations for each node
 */
Node::Visitor::Status BarnesHutVisitor::visit(Node * node) {
	double m;
	array<double,3> X;
	auto mass_and_centre = node->get_mass_and_centre();
	tie(m,X) = mass_and_centre;

	const int status = node->getStatus();
	switch (status) {
		case Node::Internal: {
			auto dsq_node=Particle::get_distance_sq(X,_position);
			/*
			 * Is this node distant enough that its particles can be lumped?
			 */
			auto l_sqr=sqr(node->getSide());
			if (l_sqr/dsq_node < _theta_squared) { // I have checked against Barnes and Hut's paper - they recommend 1.0 for theta
				_accumulate_acceleration(m,X,dsq_node);
				return Node::Visitor::Status::DontDescend;
			} else
				return Node::Visitor::Status::Continue;
		}
		case Node::Unused:
			return Node::Visitor::Status::Continue;
		default: // External Node - accumulate except don't accumulate self!
			if (status!=_index) 
				_accumulate_acceleration(m,X,Particle::get_distance_sq(X,_position)); 			
			return Node::Visitor::Status::Continue;
	}
}

/**
 * Used at the end of calculation to store accelerations back into particle
 */
void BarnesHutVisitor::store_accelerations() {
	auto acceleration = array<double,3>{_acc_x, _acc_y, _acc_z};
	_me.set_acceleration(acceleration);
}

/**
 * Used to add in the contribution to the acceleration from one Node
 * NB: there is a new instance of the visitor for each particle, so
 * acceleration is always zero at the start.
 */
void BarnesHutVisitor::_accumulate_acceleration(double m,array<double,3> X,double dsq){
	double acc_x, acc_y, acc_z;
	tie(acc_x, acc_y, acc_z) = _get_acceleration( m, X, dsq);
	_acc_x += acc_x;
	_acc_y += acc_y;
	_acc_z += acc_z;
}

tuple<double,double,double>  BarnesHutVisitor::_get_acceleration(double m,array<double,3> X,double dsq){
	auto d_factor = pow(dsq + _a*_a,-3/2);
	auto acc_x = _G*m*(X[0]-_position[0])*d_factor;
	auto acc_y = _G*m*(X[1]-_position[1])*d_factor;
	auto acc_z = _G*m*(X[2]-_position[2])*d_factor;
	return make_tuple(acc_x,acc_y,acc_z);
}
