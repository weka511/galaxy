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
 
#ifndef _TREECODE_HPP
#define _TREECODE_HPP

#include <memory>
#include <sstream>
#include <string>
#include "particle.hpp"

using namespace std;
/**
 *  Represents one node in Barnes Hut Octree
 */
class Node {
	
  private:
	/**
	 * Indicates type of node. External Nodes use the index of the
     * associated particle instead of one of these values.
	 */
	int _particle_index;
	
	/**
	 *  Mass and centre of mass
	 */
	double _m;
	double _x;
	double _y;
	double _z;
	/**
	 * Bounding box for Node. This will be subdivided as we move down the tree
	 */
	double _xmin, _xmax, _ymin, _ymax, _zmin, _zmax, _xmean, _ymean, _zmean;
	
  public:
  
	/**
	 * Create an oct-tree from a set of particles
	 */
	static Node * create(unique_ptr<Particle[]> &particles, int n);
	  /**
	   * Number of nodes allocated: used in testing
	   */
	  static int _count;
	  
	  /**
	   *  Used to traverse tree depth first
	   */
	  class Visitor {
		public:
			enum Status{
				Stop,     // abandon the traversal
				Continue, // keep traversing
				DontDescend // do not vist any more children of this node
			};
			/**
			 * Called once for each node in tree, before any children are processed
			 */
			virtual Status visit(Node * node)=0;
			
			/**
			 *  Called once for each child Node, immediately after vist
			 */
			virtual void propagate(Node * node,Node * child){;}
			
			/**
			 *  Called once for each Internal Node, after all children have been visited
			 */
			virtual bool depart(Node * node) {return true;};
	  };
	  
	 /**
	  * Indicates type of node. External Nodes use the index of the
      * associated particle instead of one of these values.
	  */
  	enum Status {Internal=-2, Unused=-1};
	
	/**
	 *   Used to ensure we have an octree
	 */
	enum {N_Children=8};

	/**
	 *  Create one node for tree
	 */	
	Node(double xmin,double xmax,double ymin,double ymax,double zmin,double zmax,string id);

	/**
	 * Insert one particle in tree
	 */
	void insert(int new_particle_index,unique_ptr<Particle[]> &particles);
	
	/**
	 * Destroy node and its descendants.
	 */
	virtual ~Node();
	
	/**
	 * Used to traverse tree
	 */
	bool visit(Visitor& visitor);
	
	/**
	 * Indicates type of node. External Nodes use the index of the
     * associated particle instead of one of these values.
	 */
	int getStatus() { return _particle_index;}
	
	/**
	 * Get mass and centre of mass
	 */
	void getPhysics(double& m, double& x, double& y, double &z) {m=_m;x=_x;y=_y;z=_z;}
	

	/**
	 * Set mass and centre of mass
	 */
	void setPhysics(double m, double x, double y, double z) {
		#ifdef _RUNTIME_CHECKS
			_check_range("x",x,_xmin,_xmax,__FILE__,__LINE__);
			_check_range("y",y,_ymin,_ymax,__FILE__,__LINE__);
			_check_range("z",z,_zmin,_zmax,__FILE__,__LINE__);
		#endif
		_m=m;_x=x;_y=y;_z=z;
	}
	
	/**
	 *   Used to calculate centre of mass for internal nodes.
	 */
	void accumulatePhysics(Node* other);

	/**
	 * Determine length of side: since Node is a cube, any side will do
	 */
	inline double getSide() {return _xmax - _xmin;}
	

		
	/**
	 * Determine the bounding box for set of particles. Make it slightly 
	 * larger than strictly needed, so everything is guaranteed to be inside box
	 */
	static void get_limits(unique_ptr<Particle[]> &particles,
					int n,
					double& xmin,double& xmax,
					double& ymin,double& ymax,
					double& zmin,double& zmax,
					const double epsilon=0.0001);
	
	string _id;
	
  private:
	
	/**
	 * Used to map a triple to an octant
	 */
	inline int _get_child_index(int i, int j, int k) {return 4*i+2*j+k;}
	
	/**
	 * Find correct subtree to store particle, using bounding rectangular box
	 */
	int _get_child_index(Particle & particle);
	
	/**
	 * Used when we have just split an External node, but the incumbent and new
	 * node both want to occupy the same child.
	 */
	void _pass_down(int particle_index,int incumbent,unique_ptr<Particle[]> &particles);
	
	/**
	 * Used when we have just split an External node, so we need to pass
	 * the incumbent and a new particle down the tree
	 */
	void _insert_or_propagate(int particle_index,int incumbent,unique_ptr<Particle[]> &particles);
	
	/**
	 * Convert an External Node into an Internal one, and
	 * determine bounding boxes for children, so we can 
	 * Propagate particle down
	 */
	void _split_node();
	

	
	/**
	 * Descendants of this node - only for an Internal Node
	 */
	Node * _child[N_Children];
	
	/**
	 *  Check that a point really does belong in this cube.
	 */
	inline void _check_range(string wname,double w,double wmin,double wmax,string file=__FILE__,int line=__LINE__) {
		if (w<wmin||w>wmax) {
			stringstream message;
			message<<file <<" " <<line << " particle " << _particle_index <<":"<<wname <<" out of range: " <<w<< " (" << wmin << "," << wmax << ")";
			throw  logic_error(message.str().c_str());
		}
	}
	
	
};



#endif   // _TREECODE_HPP
