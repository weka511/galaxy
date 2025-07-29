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

void AccelerationVisitor::visit(Particle & particle){
	// cout << __FILE__ << " " << __LINE__ << endl;
}

void AccelerationVisitor::visit_pair(Particle & particle1,Particle & particle2) {
	// cout << __FILE__ << " " << __LINE__ << endl;
}
	
/**
 * Calculate acceleration for all particles
 */
 
 // void get_acceleration(vector<Particle*>& particles,const double theta,const double G,const double a) {
	// Node * root=create_tree(particles);
	// for (int i=0;i<particles.size();i++)
		// get_acceleration(i,particles,root,theta,G,a);
	
	// delete root;
// }

/**
 *  Construct oct-tree from particles
 *
 *    particles
 unique_ptr<Node> create(unique_ptr<Particle[]> &particles, int n);
 */
unique_ptr<Node> create_tree(unique_ptr<Particle[]> &particles, int n) {
	assert(Node::get_count()==0 && "Oct Tree should have been removed at end of previous call"); 
	unique_ptr<Node> product=Node::create(particles,n);
	CentreOfMassCalculator calculator(particles,n);
	product->visit(calculator);
	calculator.check_all_particles_processed();
	return product;
}

/**
 * Calculate acceleration for one specific particle
 */
void get_acceleration(int i, unique_ptr<Particle[]> &particles,Node * root,const double theta,const double G,const double a) {
	BarnesHutVisitor visitor(i,particles[i],theta,G,a);
	root->visit(visitor);
	visitor.store_accelerations();
} 

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
	 // _me.setAcc(_acc_x,_acc_y,_acc_z);
}

/**
 * Used to add in the contribution to the acceleration from one Node
 * NB: there is a new instance of the visitor for each particle, so
 * acceleration is always zero at the start.
 */
void BarnesHutVisitor::_accumulate_acceleration(double m,double x,double y,double z,double dsq){
	double acc_x, acc_y, acc_z;
	get_acceleration( m, x, y, z, _x, _y, _z, dsq,_a, _G, acc_x,  acc_y,  acc_z);
	_acc_x+=acc_x;
	_acc_y+=acc_y;
	_acc_z+=acc_z;
}
