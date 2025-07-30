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
 
#include <cmath>

#include "barnes-hut.hpp"
#include "center-of-mass.hpp"

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
	auto pos = _me.get_position();
	_x = pos[0];
	_y = pos[1];
	_z = pos[2];
}

/**
 * Used to accumulate accelerations for each node
 */
Node::Visitor::Status BarnesHutVisitor::visit(Node * node) {

	double m,x,y,z;
	auto mass_and_centre = node->get_mass_and_centre();
	tie(m,x,y,z) = mass_and_centre;

	const int status = node->getStatus();
	switch (status) {
		case Node::Internal: {
			auto dsq_node=_get_squared_distance(x,y,z,_x,_y,_z);
			/*
			 * Is this node distant enough that its particles can be lumped?
			 */
			auto l_sqr=sqr(node->getSide());
			if (l_sqr/dsq_node < _theta_squared) {
				_accumulate_acceleration(m,x,y,z,dsq_node);
				return Node::Visitor::Status::DontDescend;
			} else
				return Node::Visitor::Status::Continue;
		}
		case Node::Unused:
			return Node::Visitor::Status::Continue;
		default: // External Node - accumulate except don't accumulate self!
			if (status!=_index) 
				_accumulate_acceleration(m,x,y,z,_get_squared_distance(x,y,z,_x,_y,_z)); 			
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
void BarnesHutVisitor::_accumulate_acceleration(double m,double x,double y,double z,double dsq){
	double acc_x, acc_y, acc_z;
	tie(acc_x, acc_y, acc_z) = _get_acceleration( m, x, y, z, _x, _y, _z, dsq);
	_acc_x += acc_x;
	_acc_y += acc_y;
	_acc_z += acc_z;
}

tuple<double,double,double>  BarnesHutVisitor::_get_acceleration(double m,double x,double y,double z,double _x,double _y,double _z,double dsq){
	auto d_factor = pow(dsq + _a*_a,-3/2);
	auto acc_x = _G*m*(x-_x)*d_factor;
	auto acc_y = _G*m*(y-_y)*d_factor;
	auto acc_z = _G*m*(z-_z)*d_factor;
	return make_tuple(acc_x,acc_y,acc_z);
}
