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
// #include "physics.hpp"

using namespace std;

inline double dsq(double x0,double y0,double z0,double x1,double y1,double z1) {return sqr(x0-x1) + sqr(y0-y1) + sqr(z0-z1);}





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
	// cout << __FILE__ << " " << __LINE__ << endl;
	double m,x,y,z;
	auto mass_and_centre = node->get_mass_and_centre();
	tie(m,x,y,z) = mass_and_centre;

	const int status = node->getStatus();
	switch (status) {
		case Node::Internal:{
			auto dsq_node=dsq(x,y,z,_x,_y,_z);
			/*
			 * Is this node are enough away that its particles can be lumped?
			 */
			auto l_sqr=sqr(node->getSide());
			if (l_sqr/dsq_node<_theta_squared) {
				_accumulate_acceleration(m,x,y,z,dsq_node);
				return Node::Visitor::Status::DontDescend;
			} else
				return Node::Visitor::Status::Continue;
		}
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
	tie(acc_x, acc_y, acc_z) = _get_acceleration( m, x, y, z, _x, _y, _z, dsq,_a, _G);
	_acc_x += acc_x;
	_acc_y += acc_y;
	_acc_z += acc_z;
}

tuple<double,double,double>  BarnesHutVisitor::_get_acceleration(double m,double x,double y,double z,double _x,double _y,double _z,double dsq,double a, double G){
	auto d_factor=pow(dsq+a*a,-3/2);
	auto acc_x = G*m*(x-_x)*d_factor;
	auto acc_y = G*m*(y-_y)*d_factor;
	auto acc_z = G*m*(z-_z)*d_factor;
	return make_tuple(acc_x,acc_y,acc_z);
}

// const double epsilon=1e-8;  // tolerance for checking Newton's 3rd law

// void _get_acceleration_between_pair(Particle& p1,Particle& p2,double G) {
	// double x1,y1,z1;
	// p1.getPos(x1,y1,z1);
	// double x2,y2,z2;
	// p2.getPos(x2,y2,z2);
	// const double distance= sqrt(sqr(x1-x2)+sqr(y1-y2)+sqr(z1-z2));
	// double d_factor=1/(distance*distance*distance);
	// assert(p1.get_mass()>0);
	// assert(p2.get_mass()>0);
	// const double acc_x1 =  p2.get_mass()*G*(x2-x1)*d_factor;
	// const double acc_y1 =  p2.get_mass()*G*(y2-y1)*d_factor;
	// const double acc_z1 =  p2.get_mass()*G*(z2-z1)*d_factor;
	// p1.accumulate_acceleration(acc_x1,acc_y1,acc_z1);
	// const double acc_x2 =  p1.get_mass()*G*(x1-x2)*d_factor;
	// const double acc_y2 =  p1.get_mass()*G*(y1-y2)*d_factor;
	// const double acc_z2 =  p1.get_mass()*G*(z1-z2)*d_factor;
	// p2.accumulate_acceleration(acc_x2,acc_y2,acc_z2);
	// check Newton's third law
	// assert( fabs(p1.get_mass()*acc_x1+p2.get_mass()*acc_x2)<=epsilon*(fabs(p1.get_mass()*acc_x1)+fabs(p2.get_mass()*acc_x2)));
	// assert( fabs(p1.get_mass()*acc_y1+p2.get_mass()*acc_y2)<=epsilon*(fabs(p1.get_mass()*acc_y1)+fabs(p2.get_mass()*acc_y2)));
	// assert( fabs(p1.get_mass()*acc_z1+p2.get_mass()*acc_z2)<=epsilon*(fabs(p1.get_mass()*acc_z1)+fabs(p2.get_mass()*acc_z2)));
// }


