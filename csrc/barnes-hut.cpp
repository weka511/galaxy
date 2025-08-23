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
  * Parameters:
  *  	me      Particle being processed
  *  	theta   Ratio for Barnes G=Hut cutoff (Barnes and Hut recommend 1.0)
  *  	G       Gravitational constant
  * 	a       Softening length
  */
BarnesHutVisitor::BarnesHutVisitor(Particle& me,const double theta, const double G,const double a)
	: _id(me.get_id()),_me(me),_theta_squared(sqr(theta)),_G(G),
	_position(me.get_position()),_a(a),_acceleration({0.0,0.0,0.0}){}
	
/**
 * Used to accumulate accelerations for each node
 *
 * Parameters:
 *     internal_node   Current node while iterating over tree
 *
 */
Node::Visitor::Status BarnesHutVisitor::visit_internal(Node * internal_node) {
	const auto m = internal_node->get_mass();
	const auto X = internal_node->get_centre_of_mass();
	const auto dsq_node=Particle::get_distance_sq(X,_position);
	/*
	 * Is this node distant enough that its particles can be lumped?
	 * I have checked against Barnes and Hut's paper - they recommend 1.0 for theta.
	 */
	if ( sqr(internal_node->get_side())/dsq_node < _theta_squared ) {
		_accumulate_acceleration(m,X,dsq_node);
		return Node::Visitor::Status::DontDescend;
	}
	return Node::Visitor::Status::Continue;
}

/**
 * Used to accumulate accelerations for each external node
 *
 * Parameters:
 *     external_node   Current node while iterating over tree
 *
 */
Node::Visitor::Status BarnesHutVisitor::visit_external(Node * external_node) {
	if (external_node->get_index() == _id) return Node::Visitor::Status::Continue;
	const auto m = external_node->get_mass();
	const auto X = external_node->get_centre_of_mass();
	const auto dsq_node=Particle::get_distance_sq(X,_position);
	_accumulate_acceleration(m,X,dsq_node); 
			
	return Node::Visitor::Status::Continue;
}


/**
 * Used to add in the contribution to the acceleration from one Node
 * NB: there is a new instance of the visitor for each particle, so
 * acceleration is always zero at the start.
 *
 * Parameters:
 *     m       Mass contained in contibuting Node
 *     X       Center of mass of contibuting Node
 *     dsq     Squared distance from corrent particle to centre of mass of contibuting Node
 */
void BarnesHutVisitor::_accumulate_acceleration(double m,array<double,NDIM> X,double dsq){
	auto d_factor = pow(dsq + _a*_a,-3/2);
	for (int i=0;i<NDIM;i++)
		 _acceleration[i] += _G*m*(X[i]-_position[i])*d_factor;
}

