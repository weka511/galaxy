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
#include <limits>
#include <stdexcept>
#include <sstream>
#include <string>
#include "treecode.hpp"

using namespace std;

int Node::_count=0;   // Initially tree is empty

/**
 * Create an oct-tree from a set of particles
 */
unique_ptr<Node> Node::create(unique_ptr<Particle[]> &particles, int n){  // FIXME Issue #61
	double zmin, zmax;
	tie(zmin,zmax) = Node::get_limits(particles,n);
	unique_ptr<Node> product = unique_ptr<Node>(new Node(zmin,zmax,zmin,zmax,zmin,zmax));

	for (int index=0;index<n;index++)
		product->insert(index,particles);

	return product;
}

/**
 * Determine a cube that will serve as a bounding box for the set of particles. 
 * Make it slightly larger than strictly needed,
 * so everything is guaranteed to be inside box
 */
tuple<double,double> Node::get_limits(unique_ptr<Particle[]>& particles,int n,const double epsilon){
	auto zmin = numeric_limits<double>::max();
	auto zmax = -zmin;
	for (int i=0;i<n;i++)
		for (int j=0;j<3;j++){
			zmin = min(zmin,particles[i].get_position()[j]);
			zmax = max(particles[i].get_position()[j],zmax);
		}

	if (zmin < 0)
		zmin *= (1+epsilon);
	else 
		zmin /= (1+epsilon);
	
	if (zmax < 0)
		zmax /= (1+epsilon);
	else
		zmax *= (1+epsilon);
	
	return make_tuple(zmin,zmax);
}


/**
 * Create a Node for a given region of space. Set it Unused until it has been added to Tree,
 */
 
Node::Node(double xmin,double xmax,double ymin,double ymax,double zmin,double zmax)
  : _id(_count),_particle_index(Unused),
  	_m(0.0d),_x(0.0d),_y(0.0d),_z(0.0d),
	_xmin(xmin), _xmax(xmax), _ymin(ymin), _ymax(ymax), _zmin(zmin), _zmax(zmax),
	_xmean(0.5*(xmin+ xmax)), _ymean(0.5*(ymin+ ymax)), _zmean(0.5*(zmin+ zmax)) {
	for (int i=0;i<N_Children;i++)
		_child[i] = NULL;
	_count++;
}


/**
 * Insert one particle in tree.
 *
 * Recursively descend until we find an empty node.
 */
void Node::insert(int new_particle_index,unique_ptr<Particle[]> &particles) {
	double x,y,z; 
	auto pos = particles[new_particle_index].get_position();
	x = pos[0];
	y = pos[1];
	z = pos[2];
	_verify_range("x",x,_xmin,_xmax,__FILE__,__LINE__);
	_verify_range("y",y,_ymin,_ymax,__FILE__,__LINE__);
	_verify_range("z",z,_zmin,_zmax,__FILE__,__LINE__);

	const double epsilon=1e-12;
	switch(_particle_index){
		case Unused:   // This Node is currently Unused, so we can add particle to it
			_particle_index=new_particle_index;
			return;
		case Internal:  // This Node is Internal, so we can add particle to the appropriate subtree
			_child[_get_octant_number(particles[new_particle_index])]->insert(new_particle_index,particles);
			return;
		default:     // This Node is External, so we already have a particle here; we have to move it
			const int incumbent = _particle_index;
			if (Particle::get_distance_sq(particles[new_particle_index],particles[incumbent]) < epsilon*(_xmax-_xmin)){
				stringstream message;
				message<<"Particles "<<new_particle_index << " and " << incumbent << " within " << epsilon*(_xmax-_xmin) << " of each other"; 
				throw logic_error(message.str().c_str());
			}
			_pass_down(new_particle_index,incumbent,particles);
	}
}

/**
 * Find correct subtree to store particle, using bounding rectangular box.
 * We split x, y, and z into halves, and determine which half each axis belongs to
 */
int Node::_get_octant_number(Particle &particle) {
	double x,y,z;
	auto pos = particle.get_position();
	x = pos[0];
	y = pos[1];
	z = pos[2];
	const int i = (x > _xmean);
	const int j = (y > _ymean);
	const int k = (z > _zmean);
	return _triple_to_octant(i,j,k);
}


/**
 * Used when we have just split an External node, but the incumbent and new
 * node both want to occupy the same child.
 */
void Node::_pass_down(int new_particle_index,int incumbent,unique_ptr<Particle[]> &particles) {
	_split_node();
	_insert_or_propagate(new_particle_index,incumbent,particles);
} 

/**
 * Used when we have just split an External node, so we need to pass
 * the incumbent and a new particle down the tree
 */
void Node::_insert_or_propagate(int particle_index,int incumbent,unique_ptr<Particle[]> &particles) {
	const int octant_number_new = _get_octant_number(particles[particle_index]);
	const int child_index_incumbent = _get_octant_number(particles[incumbent]);
	if (octant_number_new ==  child_index_incumbent)
		_child[ child_index_incumbent]->_pass_down(particle_index,incumbent,particles);
	else {
		_child[octant_number_new]->insert(particle_index,particles);
		_child[ child_index_incumbent]->insert(incumbent,particles);
	}
}



/**
 * Convert an External Node into an Internal one, and
 * partition bounding box into eight, one for each child, so we can 
 * assign particle to a member of the partition.
 */
void Node::_split_node() {
	_particle_index = Internal;
	
	for (int i=0;i<2;i++) {
		double xmin, xmax;
		tie (xmin,xmax) = _get_refined_bounds(i, _xmin,  _xmax,  _xmean);
		for (int j=0;j<2;j++) {
			double ymin, ymax;
			tie (ymin,ymax) = _get_refined_bounds(j, _ymin,  _ymax,  _ymean);
			for (int k=0;k<2;k++) {
				double zmin,zmax;
				tie (zmin,zmax) = _get_refined_bounds(k, _zmin,  _zmax,  _zmean);
				_child[_triple_to_octant(i,j,k)] = new Node(xmin, xmax, ymin, ymax, zmin, zmax);
			}
		}
	}
} 

/**
 * Traverse Tree. The Visitor decides whether we continue all the way down.
 */
bool Node::traverse(Visitor & visitor) {
	switch (visitor.visit(this)) {
		case Node::Visitor::Stop:
			return false;
		case Node::Visitor::Continue:
			if (_particle_index==Internal){
				bool should_continue=true;
				for (int i=0;i<N_Children && should_continue;i++) {
					should_continue = _child[i]->traverse(visitor);
					visitor.farewell(this,_child[i]);
				}
				if (should_continue)
					return visitor.depart(this);
				else
					return false;		
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

/**
 * Used to get number of Nodes - normally to make sure tree has been destructed.
 */
int Node::get_count() {return _count;}

/**
 * Used to establish that the center of mass is within the bounding box for it Node.
 */
bool Node::verify_within_bounding_box(){
	if ((_xmin < _x and _x < _xmax) and (_ymin < _y and _y < _ymax) and (_zmin < _z and _z < _zmax)) return true;
	
	cerr<<__FILE__ <<" " <<__LINE__<< ": Status: "<< getStatus()<<endl;
	cerr << "("<<_xmin << ", " << _x << ", " << _xmax << ")" << endl;
	cerr << "("<<_ymin << ", " << _y << ", " << _ymax << ")" << endl;
	cerr << "("<<_zmin << ", " << _z << ", " << _zmax << ")" << endl << endl;
	for (int i=0;i<N_Children;i++)
		switch(_child[i]->_particle_index) {
			case Unused:
				break;
			default:
				cerr << "("<<_child[i]->_x<< ", " << _child[i]->_y << ", " << _child[i]->_z << ") " << _child[i]->_m<< endl;
		}

	stringstream message;
	message<<__FILE__ <<", " <<__LINE__<<" Centre of mass out of range - see logfile."<<endl; 
	throw logic_error(message.str().c_str()); 
}

/**
 *  Verify that a point really does belong in this cube.
 */
void Node::_verify_range(string wname,double w,double wmin,double wmax,string file,int line) {
	if (w < wmin || w > wmax) {
		stringstream message;
		message<<file <<" " <<line << " particle " << _particle_index <<":"<<wname <<" out of range: " <<w<< " (" << wmin << "," << wmax << ")";
		throw  logic_error(message.str().c_str());
	}
}
