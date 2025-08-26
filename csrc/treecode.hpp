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
#include <vector>

#include "particle.hpp"

using namespace std;

/**
 *  Represents one node in an Oct Tree. The space is partitioned into cubes,
 *  each associated with one node.
 *
 *  Node                                Associated Cube
 *  Unused   - Terminal Node            Empty cube
 *  External - Terminal Node            Cube contains precisely one particle
 *  Internal  - precisely 8 child nodes  Cube is subdivided into smaller cubes
 *
 *  When we build the tree we will sometimes have to split External nodes if another
 *  particle wants to live in the associated cube.
 */
class Node {
  friend class TreeVerifier;	
  public:
	/**
	 *   Used to ensure we have an octree. Box is divided into 2 halves (high and low)
	 *   along each of x, y, and z axes.
	 */
	enum {N_Children=2*2*2};

  private:	
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
	 *  Total mass of all particles in or below this node
	 */
	double _m;
	
	/**
	 *  Centre of mass of all particles in or below this node
	 */
	array<double,NDIM> _center_of_mass;

	/**
	 * Bounding box for Node. This will be subdivided as we move down the tree
	 */
	array<double,NDIM> _Xmin, _Xmax, _Xmean;
	
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
	  * Indicates type of node. External Nodes use the index of the
      * associated particle (>=0) instead of one of these values.
	  */
  	enum Type {
		Internal=-2,
		Unused=-1
	};
	
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
		 * Called once for each internal node in tree, before node's children are processed
		 */
		virtual Status visit_internal(Node * node) = 0;
		
		/**
		 * Called once for each external node in tree
		 */
		virtual Status visit_external(Node * node) = 0;
			
		/**
		 *  Called once for each child of current Node, immediately after visit,
		 *  e.g. to accumulate centre of mass
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
		virtual void depart(Node * node) {;}
	  };

  public:
  
  	/**
	 * Number of nodes allocated: used in testing
	 */
	 static int get_count();
  
	/**
	 * Create an oct-tree from a set of particles
	 */
	static unique_ptr<Node> create(unique_ptr<Particle[]> &particles, const int n, const bool verify=false, const double pad=1.0e-4);
	
	/**
	 *  Create one node for tree. I have made this private, 
	 *  as clients should use the factory method Node::create(...)
	 */	
	Node(array<double,NDIM> Xmin, array<double,NDIM> Xmax);

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
	inline int get_index() { return _particle_index;}
	
	/**
	 * Get mass
	 */
	inline double  get_mass() {return _m;}
	
	/**
	 * Get centre of mass
	 */
	inline array<double,NDIM>  get_centre_of_mass() {return _center_of_mass;}
	
	/**
	 * Set mass and centre of mass
	 */
	inline void set_mass(double m) {_m = m;	}
	
	/**
	 * Set mass and centre of mass
	 */
	inline void set_centre_of_mass(array<double,NDIM> X) {	_center_of_mass = X;}
	
	/**
	 *   Used to calculate centre of mass for internal nodes.
	 */
	void accumulate_center_of_mass(Node* child);

	/**
	 * Determine length of any side of cube.
	 */
	inline double get_side() {return _Xmax[0] - _Xmin[0];}

  private:
  
	/**
     * Determine a cube that will serve as a bounding box for the
	 * set of particles.  Make it slightly larger than strictly
	 * needed, so everything is guaranteed to be inside box
	 *
	 * Parameters:
	 *     particles    The paritcles
	 *     n			Number of particles
	 *     pad			Box will be expanded by a factor of (1+pad)
     */
	static tuple<double,double> _get_limits(unique_ptr<Particle[]> &particles, int n, const double pad=1.0e-4);
		
	/**
	 * Used to map an array of 3 ints to an octant
	 */
	inline int _triple_to_octant(array<int,NDIM> indices) {
		return 2*(2*indices[0] + indices[1]) + indices[2];
	}
	
	/**
	 * Used to map a triple to an octant
	 */
	inline int _triple_to_octant(int i, int j, int k) {return 2*(2*i + j) + k;}

	/**
	 * Find correct subtree to store particle, using bounding rectangular box
	 */
	int _get_octant_number(Particle & particle);
	
	/**
	 * Split an External node, and insert the new particle and the incumbent into subtrees.
	 */
	void _split_and_insert_below(int particle_index,int incumbent,unique_ptr<Particle[]> &particles);
	
	/**
	 * Used when we have just split an External node, so we need to pass
	 * the incumbent and a new particle down the tree
	 */
	void _insert_or_propagate(int new_particle_index,int incumbent,unique_ptr<Particle[]> &particles);
	
	/**
	 * Convert an External Node into an Internal one, and
	 * determine bounding boxes for children, so we can 
	 * Propagate particle down
	 */
	void _split_node();
	 
	/**
	 *   Used when we split the box associated with a Node
	 *
	 *   Parameters:
	 *      i      Identifies whether we are computing the lower half (0) or upper half (1) of the split
	 *		wmin   Lower bound of  box along one dimension 
	 *      wmax   Upper bound of box along one dimension 
	 *      wmean  Mid point of box along one dimension 
	 *
	 *   Returns:
	 *      Lower and upper bound of half along one dimension
	 */
	inline tuple<double,double> _get_refined_bounds(int i,double wmin, double wmax, double wmean){
		if (i == 0)
			return make_tuple(wmin, wmean);
		else
			return make_tuple(wmean, wmax);
	}
};

/**
 * This class that Tree has been built correctly
 */
class TreeVerifier: public Node::Visitor{
  private:
    unique_ptr<Particle[]> &_particles;

	const int _n;
 
	vector<bool> _particle_verified;
	
	vector<array<array<array<bool,2>,2>,2>> _child_within_limits;
	
   public:
	
	TreeVerifier(unique_ptr<Particle[]> &particles, const int n);
	
	/**
	 *  This function initializes the data structure that is used to verify
	 *  that subtrees are nested properly. The data structure is filled in
	 *  by accumulate() and checked by depart() 
	*/ 
	Node::Visitor::Status visit_internal(Node * node);

	/**
	 *  Used to verify that all particles have been added
	 */
	Node::Visitor::Status visit_external(Node * node);
	
	/**
	 *  This function is used to verify that subtrees are nested properly. 
	 *  It tests a child node to see which octant it belongs to. Later the 8
	 *  and checked by depart() 
	 */ 
	void accumulate(Node * node,Node * child);
	
	/**
	 *  This function is used to verify that subtrees are nested properly. 
	 *  It checks the collection of octants build by visit_internal()
	 *  and accumulate() to ensure that it is complete.
	 */ 
	void depart(Node * node);
	
	/**
	 *  Used to verify that all particles have been added
	 */
	bool has_been_verified();
	
  private:
	/**
	 *  This function is used to verify that subtrees are nested properly. It tests one axis to see
	 *  whether it belongs to upper or lower half
	 */
    int _get_index(Node * node,Node * child,const int i, double tolerance=0.000001);
			
};

#endif   // _TREECODE_HPP 
