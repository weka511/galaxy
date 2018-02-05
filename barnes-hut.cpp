/**
 * Copyright (C) 2018 Greenweaves Software Limited
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
#include "barnes-hut.h"
#include "center-of-mass.h"
#include "physics.h"

/**
 * Calculate acceleration for all particles
 */
 
void get_acceleration(std::vector<Particle*>& particles,double theta,double G) {
	assert(Node::_count==0);   // Tree should have been removed at end of previous call
	Node * root=Node::create(particles);
	CentreOfMassCalculator calculator(particles);
	root->visit(calculator);
	calculator.check_all_paticles_processed();
	std::for_each(particles.begin(),
				particles.end(),
				[root,theta,G](Particle*me){
					BarnesHutVisitor visitor(me,theta,G);
					root->visit(visitor);
					visitor.store_accelerations();
				});
	delete root;
}

/**
 * Used to accumulate accelerations for each node
 */
Node::Visitor::Status BarnesHutVisitor::visit(Node * node) {
	double m,x,y,z;
	node->getPhysics(m,x,y,z);
	double dsq_node;
	switch (node->getStatus()) {
		case Node::Internal:
			dsq_node=dsq(x,y,z,_x,_y,_z);
			/*
			 * Is this node are enough away that its particles can be lumped?
			 */
			if (sqr(node->getSide())/dsq_node<_theta_squared) {
				_accumulate_acceleration(m,x,y,z,dsq_node);
				return Node::Visitor::Status::DontDescend;
			} else
				return Node::Visitor::Status::Continue;
		case Node::Unused:
			return Node::Visitor::Status::Continue;
		default: // External Node
			dsq_node=dsq(x,y,z,_x,_y,_z);
			if (dsq_node>0)   // FIXME: find nicer way to avoid accumulating "me"
				_accumulate_acceleration(m,x,y,z,dsq_node); 
			return Node::Visitor::Status::Continue;
	}
}

/**
 * Used at the end of calculation to store accelerations back into particle
 */
void BarnesHutVisitor::store_accelerations() {
	_me->setAcc(_acc_x,_acc_y,_acc_z);
}

	/**
	 * Used to add in the contribution to the acceleration from one Node
	 */
void BarnesHutVisitor::_accumulate_acceleration(double m,double x,double y,double z,double dsq){
	const double d_factor=pow(dsq,-3/2);
	_acc_x+=_G*m*(x-_x)*d_factor;
	_acc_y+=_G*m*(y-_y)*d_factor;
	_acc_z+=_G*m*(z-_z)*d_factor;
}
