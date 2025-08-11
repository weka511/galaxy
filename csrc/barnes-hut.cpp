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
Node::Visitor::Status BarnesHutVisitor::visit_internal(Node * node) { //FIXME
	double m;
	array<double,3> X;
	tie(m,X) = node->get_mass_and_centre();
	const auto dsq_node=Particle::get_distance_sq(X,_position);
	/*
	 * Is this node distant enough that its particles can be lumped?
	 * I have checked against Barnes and Hut's paper - they recommend 1.0 for theta.
	 */
	if ( sqr(node->getSide())/dsq_node < _theta_squared ) {
		_accumulate_acceleration(m,X,dsq_node);
		return Node::Visitor::Status::DontDescend;
	}
	return Node::Visitor::Status::Continue;
}

Node::Visitor::Status BarnesHutVisitor::visit_external(Node * node) {  //FIXME
	double m;
	array<double,3> X;
	tie(m,X) = node->get_mass_and_centre();

	if (node->get_index() != _id) //  accumulate except don't accumulate self!
		_accumulate_acceleration(m,X,Particle::get_distance_sq(X,_position)); 			
	return Node::Visitor::Status::Continue;
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

