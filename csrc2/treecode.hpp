#ifndef _TREECODE_HPP
#define _TREECODE_HPP

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
 


#include <memory>

#include <tuple>
#include "particle.hpp"

using namespace std;

/**
 *  Represents one node in an Oct Tree. The space is partitioned into cubes,
 *  each associated with one node.
 *
 *  Node                                Associated Cube
 *  Unused   - Terminal Node            Empty cube
 *  External - Terminal Node            Cube contains precisely one particle
 *  Internal  - precosly 8 child nodes  Cube is subdivided into smaller cubes
 */
class Node {
  public:
	/**
	*  Used to traverse tree depth first
	*/
	class Visitor {
	  public:
		enum Status{
			Stop,        // abandon the traversal
			Continue,    // keep traversing
			DontDescend  // do not vist any more children of this node
		};
		/**
		 * Called once for each node in tree, before any children are processed
		 */
		virtual Status visit(Node * node) = 0;
			
		/**
		 *  Called once for each child Node, immediately after vist
		 */
		virtual void propagate(Node * node,Node * child){;}
			
		/**
		 *  Called once for each Internal Node, after all children have been visited
		 */
		virtual bool depart(Node * node) {return true;}
	  };
  
   /**
	  * Indicates type of node. External Nodes use the index of the
      * associated particle instead of one of these values.
	  */
  	enum Status {Internal=-2, Unused=-1};
	
 private:	
	/**
	 *   Used to ensure we have an octree
	 */
	enum {N_Children=8};
	
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
	
	/**
	 * Descendants of this node - only for an Internal Node
	 */
	Node * _child[N_Children];
	
	/**
	 * Number of nodes allocated: used in testing
	 */
	 
	 static int _count;
	 
	 /**
	  *  Create one node for tree. I have made this private, 
	  *  as clients should use the factory method Node::create(...)
	  */	
	Node(double xmin,double xmax,double ymin,double ymax,double zmin,double zmax);
	
  public:
  
	/**
	 * Create an oct-tree from a set of particles
	 */
	static unique_ptr<Node> create(unique_ptr<Particle[]> &particles, int n); //FIXME - Issue 61
	
	/**
	 * Number of nodes allocated: used in testing
	 */
	 
	 static int get_count();


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
	bool traverse(Visitor& visitor);
	
	/**
	 * Indicates type of node. External Nodes use the index of the
     * associated particle instead of one of these values.
	 */
	int getStatus() { return _particle_index;}
	
	/**
	 * Get mass and centre of mass
	 */
	tuple <double,double,double,double>  get_mass_and_centre() {
		return make_tuple(_m,_x,_y,_z);
	}
	

	/**
	 * Set mass and centre of mass
	 */
	void set_mass_and_centre(double m, double x, double y, double z) {
		_verify_range("x",x,_xmin,_xmax,__FILE__,__LINE__);
		_verify_range("y",y,_ymin,_ymax,__FILE__,__LINE__);
		_verify_range("z",z,_zmin,_zmax,__FILE__,__LINE__);
		_m = m;
		_x = x;
		_y = y;
		_z = z;
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
     * Determine a cube that will serve as a bounding box for set of particles. 
     * Make it slightly larger than strictly needed,
     * so everything is guaranteed to be inside box
    */
	static tuple<double,double> get_limits(unique_ptr<Particle[]> &particles, int n, const double epsilon=0.0001);
	
	/**
	 * Used by center-of-mass.cpp as a sanity check.
	 */
	void validate(double x, double y, double z);
	
  private:
	
	/**
	 * Used to map a triple to an octant
	 */
	inline int _triple_to_octant(int i, int j, int k) {return 4*i+2*j+k;}
	
	/**
	 * Find correct subtree to store particle, using bounding rectangular box
	 */
	int _get_octant_number(Particle & particle);
	
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
	 *  Check that a point really does belong in this cube.
	 */
	void _verify_range(string wname,double w,double wmin,double wmax,string file=__FILE__,int line=__LINE__);
	 
	/**
	 *   Used when we split the box associated with a Node
	 */
	inline tuple<double,double> _get_refined_bounds(int i,double wmin, double wmax, double wmean){
		if (i == 0)
			return make_tuple(wmin, wmean);
		else
			return make_tuple(wmean, wmax);
	}
};

#endif   // _TREECODE_HPP
