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
 
#include <array>
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
  private:	
	/**
	 *   Used to ensure we have an octree
	 */
	enum {N_Children=2*2*2};

	/**
	 *  Used to keep track of nodes for debugging
	 */
	int _id;
	
	/**
	 * Indicates type of node. External Nodes use the index of the
     * associated particle instead of one of these values.
	 */
	int _particle_index;
	
	/**
	 *  Mass and centre of mass
	 */
	double _m;
	
	array<double,3> _center_of_mass;

	/**
	 * Bounding box for Node. This will be subdivided as we move down the tree
	 */
	array<double,3> _Xmin;
	array<double,3> _Xmax;
	array<double,3>	_Xmean;
	
	/**
	 * Descendants of this node - only for an Internal Node
	 */
	Node * _child[N_Children];
	
	/**
	 * Number of nodes allocated: used in testing
	 */
	 static int _count;
	 
  public:
	/**
	*  Used to traverse tree depth first
	*/
	class Visitor {
	  public:
		enum Status {
			Continue,    // keep traversing
			DontDescend  // Used by barnes-hut.cpp to treat node as a proxy for its children
		};
		
		/**
		 * Called once for each node in tree, before any children are processed
		 */
		virtual Status visit(Node * node) = 0;
			
		/**
		 *  Called once for each child of current Node, immediately after visit
		 *
		 *  Parameters:
		 *      node          An internal node
		 *      child         The child that has just been processes
		 */
		virtual void accumulate(Node * node,Node * child){;}
			
		/**
		 *  Called once for each Internal Node, after all children have been visited
		 *
		 *  Returns:
		 *     An indication of whether traversal should continue
		 */
		virtual bool depart(Node * node) {return true;}
	  };
  
	/**
	  * Indicates type of node. External Nodes use the index of the
      * associated particle instead of one of these values.
	  */
  	enum Type {Internal=-2, Unused=-1};
	
  public:
  
  	/**
	 * Number of nodes allocated: used in testing
	 */
	 static int get_count();
  
	/**
	 * Create an oct-tree from a set of particles
	 */
	static unique_ptr<Node> create(unique_ptr<Particle[]> &particles, int n);
	
	/**
	 *  Create one node for tree. I have made this private, 
	 *  as clients should use the factory method Node::create(...)
	 */	
	Node(array<double,3> Xmin, array<double,3> Xmax);

	
	/**
	 * Destroy node and its descendants.
	 */
	virtual ~Node();

	/**
	 * Insert one particle in tree
	 */
	void insert(int new_particle_index,unique_ptr<Particle[]> &particles);
	
	/**
	 * Traverse Tree, visiting each node depth first.  The Visitor decides whether
	 * we continue all the way down. After visiting each child of the current node
	 * we call farewell() on the node itself, and then finally call depart() on 
	 * the current node after all children processed.
	 *
	 * Returns:
	 *     true iff traversal completed, i.e. no visit() decided to terminate processing.
	*/
	void traverse(Visitor& visitor);
	
	/**
	 * Indicates type of node. External Nodes use the index of the
     * associated particle instead of one of these values.
	 */
	int getType() { return _particle_index;}
	
	/**
	 * Get mass and centre of mass
	 */
	tuple <double,array<double,3>>  get_mass_and_centre() {
		return make_tuple(_m,_center_of_mass);
	}
	
	/**
	 * Set mass and centre of mass
	 */
	void set_mass_and_centre(double m, array<double,3> X) {
		for (int i=0;i<3;i++)
			_verify_range("X",X[i],_Xmin[i],_Xmax[i],__FILE__,__LINE__);// FIXME #59
		_m = m;
		_center_of_mass = X;
	}
	
	/**
	 *   Used to calculate centre of mass for internal nodes.
	 */
	void accumulate_center_of_mass(Node* other);

	/**
	 * Determine length of side: since Node is a cube, any side will do
	 */
	inline double getSide() {
		return _Xmax[0] - _Xmin[0];
	}
	
	/**
     * Determine a cube that will serve as a bounding box for set of particles. 
     * Make it slightly larger than strictly needed,
     * so everything is guaranteed to be inside box
    */
	static tuple<double,double> get_limits(unique_ptr<Particle[]> &particles, int n, const double epsilon=0.0001);
	
	/**
	 * Used to establish that a point
	 * is within the bounding box for its Node.
	 */
	bool verify_within_bounding_box();
	
  private:
	
	/**
	 * Used to map a triple to an octant
	 */
	inline int _triple_to_octant(array<int,3> indices) {
		return 2*(2*indices[0] + indices[1]) + indices[2];
	}
	
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
