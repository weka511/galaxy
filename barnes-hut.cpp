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
#include "barnes-hut.h"
#include "center-of-mass.h"
#include <iostream>

/**
 * Calculate acceleration for all particles
 */
 
void get_acceleration(std::vector<Particle*>& particles,double theta,double G) {
	assert(Node::_count==0);   // Tree should have been removed at end of previous call
	Node * root=Node::create(particles);
	CentreOfMassCalculator calculator(particles);
	root->visit(calculator);
	calculator.display();
	std::for_each(particles.begin(),
				particles.end(),
				[root,theta,G](Particle*me){
					BarnesHutVisitor visitor(me,theta,G);
					root->visit(visitor);
					visitor.store_accelerations(me);
				});
	delete root;
}

Node::Visitor::Status BarnesHutVisitor::visit(Node * node) {
	const int index= node->getStatus();
	double m,x,y,z;
	node->getPhysics(m,x,y,z);
	double dsq;
	switch (node->getStatus()) {
		case Node::Internal:
			x/=m;y/=m;z/=m;
			dsq=sqr(x-_x) + sqr(y-_y) + sqr(z-_z);
			if (sqr(node->getSide())/dsq<_theta2) {
				_accumulate_acceleration(m,x,y,z,dsq);
				return Node::Visitor::Status::Sideways;
			} else
				return Node::Visitor::Status::Continue;
		case Node::Unused:
			return Node::Visitor::Status::Continue;
		default: 
			dsq=sqr(x-_x) + sqr(y-_y) + sqr(z-_z);
			if (dsq>0)_accumulate_acceleration(m,x,y,z,dsq);
			return Node::Visitor::Status::Continue;
	}
}


void BarnesHutVisitor::store_accelerations(Particle*me) {
	me->setAcc(_acc_x,_acc_y,_acc_z);
}

void BarnesHutVisitor::_accumulate_acceleration(double m,double x,double y,double z,double dsq){
	const double d_factor=pow(dsq,-3/2);
	_acc_x+=_G*m*(x-_x)*d_factor;
	_acc_y+=_G*m*(y-_y)*d_factor;
	_acc_z+=_G*m*(z-_z)*d_factor;
}
