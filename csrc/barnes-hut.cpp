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
 * Used to accumulate accelerations for each node
 */
Node::Visitor::Status BarnesHutVisitor::visit(Node * node) {
	double m;
	array<double,3> X;
	tie(m,X) = node->get_mass_and_centre();

	const int status = node->getStatus();
	switch (status) {
		case Node::Internal: {
			const auto dsq_node=Particle::get_distance_sq(X,_position);
			/*
			 * Is this node distant enough that its particles can be lumped?
			 */
			const auto l_sqr=sqr(node->getSide());
			if (l_sqr/dsq_node < _theta_squared) { // I have checked against Barnes and Hut's paper - they recommend 1.0 for theta
				_accumulate_acceleration(m,X,dsq_node);
				return Node::Visitor::Status::DontDescend;
			} else
				return Node::Visitor::Status::Continue;
		}
		case Node::Unused:
			return Node::Visitor::Status::Continue;
		default: // External Node - accumulate except don't accumulate self!
			if (status != _id) 
				_accumulate_acceleration(m,X,Particle::get_distance_sq(X,_position)); 			
			return Node::Visitor::Status::Continue;
	}
}

/**
 * Used at the end of calculation to store accelerations back into particle
 */
void BarnesHutVisitor::store_accelerations() {
	_me.set_acceleration(_acceleration);
}

/**
 * Used to add in the contribution to the acceleration from one Node
 * NB: there is a new instance of the visitor for each particle, so
 * acceleration is always zero at the start.
 */
void BarnesHutVisitor::_accumulate_acceleration(double m,array<double,3> X,double dsq){
	auto d_factor = pow(dsq + _a*_a,-3/2);
	for (int i=0;i<3;i++)
		 _acceleration[i] += _G*m*(X[i]-_position[i])*d_factor;
}

