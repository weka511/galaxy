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
 
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <limits>
#include <cassert>

#include "center-of-mass.hpp"

using namespace std;


/**
 * Called for each External Node: record the position and mass of the particle
 */

Node::Visitor::Status CentreOfMassCalculator::visit_external(Node * node) {
	const int particle_index = node->get_index(); 
	Particle & particle = _particles[particle_index];
	node->set_mass_and_centre(particle.get_mass(),particle.get_position());
	return Node::Visitor::Status::Continue;
}


/**
 *  We need to accumulate the mass and positions for each child of an internal node
 */
void CentreOfMassCalculator::accumulate(Node * node,Node * child){
	node->accumulate_center_of_mass(child);
}


/**
 * This is called when we finish processing a Node, which means that all children 
 * have been processed. At this statge we have accumulated total mass, and a weighted 
 * sum of positions of centres for children. Divide total by mass,
 * and store centre of mass.
 */
void CentreOfMassCalculator::depart(Node * node)  {
	array<double,3> X;
	double m;
	tie(m,X) = node->get_mass_and_centre();
	for (int i=0;i<3;i++)
		X[i] /= m;
	node->set_mass_and_centre(m,X);
}

