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
 
#ifndef _TREECODE_H
#define _TREECODE_H

#include <cstdlib>
#include <vector>

#include "particle.h"

/**
 *  Represents one node in Barnes Hut Octree
 */
class Node {
  public:
	  class Visitor {
		public:
			enum Status{Stop, Continue, Sideways};
			virtual Status visit(Node * node)=0;
			virtual void propagate(Node * node,Node * child){;}
			virtual bool depart(Node * node) {return true;};
	  };
  	enum Status {Internal=-2, Unused=-1};
	enum {N_Children=8};
	
	/**
	 *  Create one node for tree
	 */
    Node(double xmin,double xmax,double ymin,double ymax,double zmin,double zmax);
	
	void insert(int particle_index,std::vector<Particle*> particles);
	
	virtual ~Node() {
		for (int i=0;i<N_Children;i++)
			if (_child[i]!=NULL)
				delete _child[i];
	}
	
	bool visit(Visitor& visitor);
	 
	int getStatus() { return _particle_index;}
	
	void getPhysics(double& m, double& x, double& y, double &z) {m=_m;x=_x;y=_y;z=_z;}
	
	void setPhysics(double m, double x, double y, double z) {_m=m,_x=x;_y=y;_z=z;}
	
	/**
	 *   Used to calculate centre of mass for internal nodes.
	 */
	void accumulatePhysics(Node* other) {
		_m+=other->_m;
		const int mult = other->getStatus() >=0 ? other->_m : 1;
		_x += mult * other->_x;
		_y += mult * other->_y;
		_z += mult * other->_z;
	}
	
	const double _xmin, _xmax, _ymin, _ymax, _zmin, _zmax, _xmean, _ymean, _zmean;
	
	static Node * create(std::vector<Particle*> particles);
	
	static get_limits(std::vector<Particle*> particles,double& xmin,double& xmax,double& ymin,double& ymax,double& zmin,double& zmax);
	
	double getSide() {return _xmax - _xmin;}
  private:
	int _get_child_index(int i, int j, int k) {return 4*i+2*j+k;}
	
	int _get_child_index(Particle * particle);
	
	void _pass_down(int particle_index,int incumbent,std::vector<Particle*> particles);
	
	void _insert_or_propagate(int particle_index,int incumbent,std::vector<Particle*> particles);
	
	void _split_node();
	
	int _particle_index;
	
	
	
	Node * _child[N_Children];
	
	double _m, _x, _y, _z;
};



#endif
