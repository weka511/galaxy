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
 
#include "center-of-mass.hpp"

using namespace std;

CentreOfMassCalculator::CentreOfMassCalculator(unique_ptr<Particle[]> &particles) 
 : _particles(particles) {}

/**
 * Called for each External Node: record the position and mass of the particle
 */

Node::Visitor::Status CentreOfMassCalculator::visit_external(Node * external_node) {
	const int particle_index = external_node->get_index(); 
	Particle & particle = _particles[particle_index];
	external_node->set_mass_and_centre(particle.get_mass(),particle.get_position());
	return Node::Visitor::Status::Continue;
}


/**
 *  We need to accumulate the mass and positions for each child of an internal node
 */
void CentreOfMassCalculator::accumulate(Node * node,Node * child){
	node->accumulate_center_of_mass(child);
}


/**
 * This is called when we finish processing an internal Node, after all children 
 * have been processed. At this stage we have accumulated the total mass, and a weighted 
 * sum of positions of centres for children. Divide weighted sum by total mass,
 * and store total mass and centre of mass.
 */
void CentreOfMassCalculator::depart(Node * node)  {
	array<double,NDIM> X;
	double m;
	tie(m,X) = node->get_mass_and_centre();
	for (int i=0;i<NDIM;i++)
		X[i] /= m;
	node->set_mass_and_centre(m,X);
}

