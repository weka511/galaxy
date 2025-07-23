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
#include <iostream>
#include <limits>
#include <stdexcept>
#include <sstream>
#include "treecode.hpp"
using namespace std;

int Node::_count=0;

/**
 * Create a Node for a given region of space. Set it unused until it has been added to Tree,
 */
 
#ifdef _RUNTIME_CHECKS
	Node::Node(double xmin,double xmax,double ymin,double ymax,double zmin,double zmax,string id)
	  : _particle_index(Unused),
		_xmin(xmin), _xmax(xmax), _ymin(ymin), _ymax(ymax), _zmin(zmin), _zmax(zmax),
		_xmean(0.5*(xmin+ xmax)), _ymean(0.5*(ymin+ ymax)), _zmean(0.5*(zmin+ zmax)),
		_m(0.0d),_x(0.0d),_y(0.0d),_z(0.0d),
		_id(id) {
		for (int i=0;i<N_Children;i++)
			_child[i]=NULL;
		_count++;
	}
#else
	Node::Node(double xmin,double xmax,double ymin,double ymax,double zmin,double zmax)
	  : _particle_index(Unused),
		_xmin(xmin), _xmax(xmax), _ymin(ymin), _ymax(ymax), _zmin(zmin), _zmax(zmax),
		_xmean(0.5*(xmin+ xmax)), _ymean(0.5*(ymin+ ymax)), _zmean(0.5*(zmin+ zmax)),
		_m(0.0d),_x(0.0d),_y(0.0d),_z(0.0d) {
		for (int i=0;i<N_Children;i++)
			_child[i]=NULL;
		_count++;
	}	
#endif




/**
 * Determine the bounding box for set of particles. Make it slightly 
 * larger than strictly needed, so everything is guaranteed to be inside box
 */
void Node::get_limits(vector<Particle*>& particles,double& xmin,double& xmax,double& ymin,double& ymax,double& zmin,double& zmax,const double epsilon){
	xmin=numeric_limits<double>::max();
	xmax=-xmin;
	ymin=numeric_limits<double>::max();
	ymax=-ymin;
	zmin=numeric_limits<double>::max();
	zmax=-zmin;
	for_each(particles.begin(),
					particles.end(),
					[&xmin,&xmax,&ymin,&ymax,&zmin,&zmax](Particle* particle){
						double x,y,z;
						particle->getPos(x,y,z);
						if (x<xmin) xmin=x;
						if (x>xmax) xmax=x;
						if (y<ymin) ymin=y;
						if (y>ymax) ymax=y;
						if (z<zmin) zmin=z;
						if (z>zmax) zmax=z;
					});
	
	zmin=min(xmin,min(ymin,zmin));
	zmax=max(xmax,max(ymax,zmax));
	if (zmin<0) zmin*=(1+epsilon); else zmin/=(1+epsilon);
	if (zmax<0) zmax/=(1+epsilon); else zmax*=(1+epsilon);
	xmin=ymin=zmin;
	xmax=ymax=zmax;
}

/**
 * Create an oct-tree from a set of particles
 */
Node * Node::create(vector<Particle*>& particles){
	double xmin, xmax, ymin, ymax, zmin, zmax;
	Node::get_limits(particles,xmin, xmax, ymin, ymax, zmin, zmax);
	#ifdef _RUNTIME_CHECKS
		Node * product=new Node(xmin,xmax,ymin,ymax,zmin,zmax,"0");
	#else
		Node * product=new Node(xmin,xmax,ymin,ymax,zmin,zmax);
	#endif
	for (int index=0;index<particles.size();index++)
		product->insert(index,particles);
	return product;
}

/**
 * Insert one particle in tree.
 *
 * Recursively descend until we find an empty node.
 */
void Node::insert(int new_particle_index,vector<Particle*>& particles) {

	#ifdef _RUNTIME_CHECKS
		double x,y,z; 
		particles[new_particle_index]->getPos(x,y,z);
		_check_range("x",x,_xmin,_xmax,__FILE__,__LINE__);
		_check_range("y",y,_ymin,_ymax,__FILE__,__LINE__);
		_check_range("z",z,_zmin,_zmax,__FILE__,__LINE__);
	#endif
	const double epsilon=1e-12;
	switch(_particle_index){
		case Unused:   // we can add particle to Unused Node
			_particle_index=new_particle_index;
			return;
		case Internal:  // we can add particle to appropriate subtree of Internal Node
			_child[_get_child_index(particles[new_particle_index])]->insert(new_particle_index,particles);
			return;
		default:     //oops - we already have a particle here, so have to move it
			const int incumbent=_particle_index;
			const double dsq=particles[new_particle_index]->get_distance_sq(particles[incumbent]);
			if (dsq<epsilon*(_xmax-_xmin)){
				stringstream message;
				message<<"Particles "<<new_particle_index << " and " << incumbent << " within " << epsilon*(_xmax-_xmin) << " f each other"; 
				throw logic_error(message.str().c_str());
			}
			_pass_down(new_particle_index,incumbent,particles);
	}
}

/**
 * Used when we have just split an External node, but the incumbent and new
 * node both want to occupy the same child.
 */
void Node::_pass_down(int new_particle_index,int incumbent,vector<Particle*>& particles) {
	_split_node();
	_insert_or_propagate(new_particle_index,incumbent,particles);
} 

/**
 * Used when we have just split an External node, so we need to pass
 * the incumbent and a new particle down the tree
 */
void Node::_insert_or_propagate(int particle_index,int incumbent,vector<Particle*>& particles) {
	const int child_index_new=_get_child_index(particles[particle_index]);
	const int child_index_incumbent=_get_child_index(particles[incumbent]);
	if (child_index_new==child_index_incumbent)
		_child[child_index_incumbent]->_pass_down(particle_index,incumbent,particles);
	else {
		_child[child_index_new]->insert(particle_index,particles);
		_child[child_index_incumbent]->insert(incumbent,particles);
	}
}

/**
 * Find correct subtree to store particle, using bounding rectangular box.
 * We split x, y, and z into halves, and determine which half each axis belongs to
 */
int Node::_get_child_index(Particle * particle) {
	double x,y,z;
	particle->getPos(x,y,z);
	const int i=x>_xmean;
	const int j=y>_ymean;
	const int k=z>_zmean;
	return _get_child_index(i,j,k);
}

/**
 * Convert an External Node into an Internal one, and
 * determine bounding boxes for children, so we can 
 * Propagate particle down
 */
void Node::_split_node() {
	_particle_index=Internal;
	double xmin, xmax, ymin, ymax, zmin, zmax;
	for (int i=0;i<2;i++) {
		if (i==0) {
			xmin=_xmin;
			xmax=_xmean;
		} else {
			xmin=_xmean;
			xmax=_xmax;
		}
		for (int j=0;j<2;j++) {
			if (j==0) {
				ymin=_ymin;
				ymax=_ymean;
			} else {
				ymin=_ymean;
				ymax=_ymax;
			}
			for (int k=0;k<2;k++) {
				if (k==0) {
					zmin=_zmin;
					zmax=_zmean;
				} else {
					zmin=_zmean;
					zmax=_zmax;
				}
				#ifdef _RUNTIME_CHECKS
					stringstream ss;
					ss<<_id<<_get_child_index(i,j,k);
					_child[_get_child_index(i,j,k)]=new Node(xmin, xmax, ymin, ymax, zmin, zmax,ss.str().c_str());
				#else
					_child[_get_child_index(i,j,k)]=new Node(xmin, xmax, ymin, ymax, zmin, zmax);
				#endif
			}	// k
		}		// j
	}			// i
} 

/**
 * Traverse Tree. The Visitor decides whether we continue all the way down.
 */
bool Node::visit(Visitor & visitor) {
	switch (visitor.visit(this)) {
		case Node::Visitor::Stop:
			return false;
		case Node::Visitor::Continue:
			if (_particle_index==Internal){
				bool should_continue=true;
				for (int i=0;i<N_Children&&should_continue;i++) {
					should_continue=_child[i]->visit(visitor);
					visitor.propagate(this,_child[i]);
				}
				return should_continue ? visitor.depart(this) : false;		
			}  else
				return true;

		case Node::Visitor::DontDescend:
			return true;
	}
	return true;
}

/**
 *   Used to calculate centre of mass for internal nodes.
 *   We accumulate mx,my,mz, so we need to treat External nodes differently
 */
void Node::accumulatePhysics(Node* other) {
	_m += other->_m;
	_x += other->_m * other->_x;
	_y += other->_m * other->_y;
	_z += other->_m * other->_z;
}

/**
 * Used to delete tree
 */ 
Node::~Node() {
	for (int i=0;i<N_Children;i++)
		if (_child[i]!=NULL)
			delete _child[i];
	Node::_count--;
}
