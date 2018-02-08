/* Simple C++ implementation of a Barnes-Hut galaxy simulator.
 * This file is part of the exercise series of the University of Geneva's
 * MOOC "Simulation and Modeling of Natural Processes".
 *
 * Author: Jonas Latt
 * E-mail contact: jonas.latt@unige.ch
 * Important: don't send questions about this code to the above e-mail address.
 * They will remain unanswered. Instead, use the resources of the MOOC.
 * 
 * Copyright (C) 2016 University of Geneva
 * 24 rue du Général-Dufour
 * CH - 1211 Genève 4
 * Switzerland
 *
 * This code is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * The library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <vector>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <cassert>

// Compute the square of a floating-point value.
inline double sqr(double a) {
    return a*a;
}

// This abstract class represents a node of the quad-tree, which can be
// either an end-node (i.e. a body) or an internal node.
class Node {
public:
    Node()
        : s(1.0)
    {
        relpos[0] = 0.;
        relpos[1] = 0.;
    }
    Node (Node const& rhs)
        : s(rhs.s)
    {
      relpos[0] = rhs.relpos[0];
      relpos[1] = rhs.relpos[1];
    }
    virtual ~Node() { }
    virtual bool isEndnode() const =0;
    // Get the mass of this node.
    virtual double getMass() const =0;
    // Get the current center-of-mass of this node.
    virtual void getPos(double& px, double& py) const =0;
    // For computational efficiency: get the center-of-mass, times mass.
    virtual void get_m_pos(double& m_px, double& m_py) const =0;
    // Update the mass and center-of-mass as a result of integrating another
    // node into the present quadrant.
    virtual void addMassCom(Node const* other) =0;
    // Set one of the four children of an internal node. It is not allowed to
    // overwrite an already existing child.
    virtual void setChild(int quadrant, Node* child) =0;
    // Get a read-only pointer to one of the four children of an internal node.
    virtual Node const* getChild(int quadrant) const =0;
    // Get a pointer to one of the four children and set the child to null.
    virtual Node* extractChild(int quadrant) =0;
    // Get the side-length of the current quadrant of the node.
    double getS() const {
        return s;
    }
    // Place the node into the appropriate newxt sub-quadrant.
    int intoNextQuadrant() {
        s *= 0.5;
        return subdivide(1) + 2*subdivide(0);
    }
    // Replace the node to the root of the quadtree.
    void resetToZerothQuadrant() {
        s = 1.0;
        getPos(relpos[0], relpos[1]);
    }
    // Square of distance between center-of-masses wrt. another node.
    double distSqr(Node const* other) const {
        double x, y, ox, oy;
        this->getPos(x, y);
        other->getPos(ox, oy);
        return sqr(x-ox)+sqr(y-oy);
    }
    // Compute the force of the present node onto another node, divided by
    // the other node's mass, and divided by the gravitational constant G.
    void accelerationOn(Node const* other, double& fx, double& fy, double dsqr) const {
        // Introduce a cut-off distance to avoid numerical instability in case two
        // nodes are too close to each other.
        static const double cutoff_dist = 0.002;
        static const double cutoff_dist_sqr = cutoff_dist*cutoff_dist;
        if (dsqr < cutoff_dist_sqr) {
            fx = fy = 0.;
        }
        else {
            double mx, my, o_x, o_y;
            this->get_m_pos(mx, my);
            other->getPos(o_x, o_y);
            double m = this->getMass();
            // The force goes like 1/r^2.
            double inv_d_cube = std::pow(dsqr, -3./2.);
            fx = (mx - o_x*m) * inv_d_cube;
            fy = (my - o_y*m) * inv_d_cube;
        }
    }
private:
    // Compute the index of the next sub-quadrant along a given direction
    // (there are two possibilities).
    int subdivide(int i) {
        // "relpos" is the relative coordinates of the center-of-mass with
        // respect to the current quadrant.
        relpos[i] *= 2.;
        if (relpos[i] < 1.0) {
            return 0;
        }
        else {
            relpos[i] -= 1.0;
            return 1;
        }
    }
private:
    double s; // Side-length of current quadrant.
    double relpos[2]; // Center-of-mass coordinates in current quadrant.
};


// A body is an end-node of the quad-tree.
class Body : public Node {
public:
    Body(double m, double x, double y, double vx, double vy)
        : mass(m),
          pos_x(x), pos_y(y),
          vel_x(vx),
          vel_y(vy)
    { }
    virtual bool isEndnode() const {
        return true;
    }
    // Get the mass of the present body.
    virtual double getMass() const {
        return mass;
    }
    // Get the current position of this body.
    virtual void getPos(double& px, double& py) const {
        px = pos_x;
        py = pos_y;
    }
    // Get the center-of-mass, times mass.
    virtual void get_m_pos(double& m_px, double& m_py) const {
        m_px = mass*pos_x;
        m_py = mass*pos_y;
    }
    // You can't add another node into an end-node.
    virtual void addMassCom(Node const* other) {
        std::cout << "Error: trying to change mass and center-of-mass of an end-node." << std::endl;
        assert( false );
    }
    // You can't add another node into an end-node.
    virtual void setChild(int quadrant, Node* child) {
        std::cout << "Error: trying to assign a child to an end-node." << std::endl;
        assert( false );
    }
    // You can't add another node into an end-node.
    virtual Node const* getChild(int quadrant) const {
        std::cout << "Error: trying to get child of an end-node." << std::endl;
        assert( false );
    }
    // You can't add another node into an end-node.
    virtual Node* extractChild(int quadrant) {
        std::cout << "Error: trying to get child of an end-node." << std::endl;
        assert( false );
    }
    // Verlet integration step.
    void advance(double ax, double ay, double dt) {
        vel_x += dt*ax;
        vel_y += dt*ay;
        pos_x += dt*vel_x;
        pos_y += dt*vel_y;
    }
private:
    double mass;
    double pos_x, pos_y;
    double vel_x, vel_y;
};

// An internal node of the quad-tree.
class InternalNode : public Node {
public:
    // When an internal node is crated, it replaces a single body,
    // and it inherits the body's mass and center-of-mass.
    InternalNode(Node const* node)
        : Node(*node),
          mass(node->getMass()),
          inv_mass(0.),
          inv_mass_computed(false)
    {
        // Instead of storing the center-of-mass, we store the
        // center-of-mass times mass. This makes it cheaper to update the
        // center-of-mass whenever another body is added into the quadrant.
        node->getPos(m_pos_x, m_pos_y);
        m_pos_x *= mass;
        m_pos_y *= mass;
        for (int i=0; i<4; ++i) {
            children[i] = 0;
        }
    }
    // When the quadtree is deleted, all internal nodes are removed recursively.
    virtual ~InternalNode() {
        for (int i=0; i<4; ++i) {
            // Don't delete end-nodes. These are the bodies: they survive from
            // one time-iteration step to the next.
            if (children[i] && !children[i]->isEndnode()) {
                delete children[i];
            }
        }
    }
    // This class represents only internal nodes.
    virtual bool isEndnode() const {
        return false;
    }
    // Get the mass of this node.
    virtual double getMass() const {
        return mass;
    }
    // Get the current center-of-mass of this node.
    virtual void getPos(double& px, double& py) const {
        // To get the center-of-mass, we need to divide by the mass. For better
        // efficiency, lazy evaluation is used to calculate the inverse-mass.
        if (!inv_mass_computed) {
            inv_mass = 1./mass;
            inv_mass_computed = true;
        }
        px = m_pos_x * inv_mass;
        py = m_pos_y * inv_mass;
    }
    // For computational efficiency: get the center-of-mass, times mass.
    virtual void get_m_pos(double& m_px, double& m_py) const {
        m_px = m_pos_x;
        m_py = m_pos_y;
    }
    // Update the mass and center-of-mass as a result of integrating another
    // node into the present quadrant.
    virtual void addMassCom(Node const* other) {
        // 1. Update the mass.
        mass += other->getMass();
        inv_mass_computed = false; // Trigger lazy-evaluation mechanism.
        // 2. Update the center-of-mass.
        double o_mx, o_my;
        other->get_m_pos(o_mx, o_my);
        m_pos_x += o_mx;
        m_pos_y += o_my;
    }
    // Set one of the four children of an internal node. It is not allowed to
    // overwrite an already existing child.
    virtual void setChild(int quadrant, Node* child) {
        assert( children[quadrant]==0 );
        children[quadrant] = child;
    }
    // Get a read-only pointer to one of the four children of an internal node.
    virtual Node const* getChild(int quadrant) const {
        return children[quadrant];
    }
    // Get a pointer to one of the four children and set the child to null.
    virtual Node* extractChild(int quadrant) {
        Node* child = children[quadrant];
        // Set the child to null, to allow setting it to another
        // node in the future.
        children[quadrant] = 0;
        return child;
    }
private:
    double mass;
    mutable double inv_mass;
    mutable bool inv_mass_computed;
    double m_pos_x, m_pos_y;
    Node* children[4];
};

// Barnes-Hut algorithm: Creation of the quad-tree. This function adds
// a new body into a quad-tree node. Returns an updated version of the node.
Node* add(Body* body, Node* node) {
    // To limit the recursion depth, set a lower limit for the size of quadrant.
    static const double smallest_quadrant = 1.e-4;
    Node* new_node = 0;
    // 1. If node n does not contain a body, put the new body b here.
    if (node==0) {
        new_node = body;
    }
    else {
        if (node->getS() < smallest_quadrant) {
            return node;
        }
        // 3. If node n is an external node, then the new body b is in conflict
        //    with a body already present in this region. ...
        if (node->isEndnode()) {
        //    ... Subdivide the region further by creating an internal node.
            new_node = new InternalNode(node);
        //    ... And to start with, insert the already present body recursively
        //        into the appropriate quadrant.
            int quadrant = node->intoNextQuadrant();
            new_node->setChild(quadrant, node);
        }
        // 2. If node n is an internal node, we don't to modify its child.
        else {
            new_node = node;
        }
        // 2. and 3. If node n is or has become an internal node ...
        //           ... update its mass and "center-of-mass times mass"
        new_node->addMassCom(body);
        // ... and recursively add the new body into the appropriate quadrant.
        int quadrant = body->intoNextQuadrant();
        new_node->setChild (
                      quadrant,
                      add(body, new_node->extractChild(quadrant)) );
    }
    return new_node;
}

// Compute the force of all other nodes onto a given node, divided by
// the node's mass, and divided by the gravitational constant G.
// This amounts to a recursive evaluation of the quad-tree created by
// the Barnes-Hut algorithm.
void accelerationOn( Body const* body, Node const* node, double theta,
                     double& ax, double& ay)
{
    // 1. If the current node is an external node, 
    //    calculate the force exerted by the current node on b.
    double dsqr = node->distSqr(body);
    if (node->isEndnode()) {
        node->accelerationOn(body, ax, ay, dsqr);
    }
    // 2. Otherwise, calculate the ratio s/d. If s/d < θ, treat this internal
    //    node as a single body, and calculate the force it exerts on body b.
    else if (sqr(node->getS()) < dsqr*sqr(theta)) {
        node->accelerationOn(body, ax, ay, dsqr);
    }
    // 3. Otherwise, run the procedure recursively on each child.
    else {
        ax = 0.;
        ay = 0.;
        for (int i=0; i<4; ++i) {
            Node const* c = node->getChild(i);
            if (c!=0) {
                double ax_, ay_;
                accelerationOn(body, c, theta, ax_, ay_);
                ax += ax_;
                ay += ay_;
            }
        }
    }
}

// Execute a time iteration according to the Verlet algorithm.
void verlet( std::vector<Body*>& bodies, Node* root,
             double theta, double G, double dt )
{
    for(size_t i=0; i<bodies.size(); ++i) {
        double ax, ay;
        accelerationOn(bodies[i], root, theta, ax, ay);
        ax *= G;
        ay *= G;
        bodies[i]->advance(ax, ay, dt);
    }
}

// Write the position of all bodies into a text file.
// The text file can be converted into an image with the
// Python script make_img.py
// Batch-processing of all text files is achieved with the
// shell script dat2img.
void save_bodies( std::vector<Body*>& bodies, int i)
{
    std::stringstream fNameStream;
    fNameStream << "body_" << std::setfill('0') << std::setw(6) << i << ".dat";
    std::ofstream ofile(fNameStream.str().c_str());
    for (unsigned i=0; i<bodies.size(); ++i) {
        double px, py;
        bodies[i] -> getPos(px, py);
        ofile << std::setprecision(12)
              << std::setw(20) << px
              << std::setw(20) << py << "\n";
    }
}

int main() {
    // Theta-criterion of the Barnes-Hut algorithm.
    const double theta = 0.5;
    // Mass of a body.
    const double mass = 1.0;
    // Initially, the bodies are distributed inside a circle of radius ini_radius.
    const double ini_radius = 0.1;
    // Initial maximum velocity of the bodies.
    const double inivel = 0.1;
    // The "gravitational constant" is chosen so as to get a pleasant output.
    const double G = 4.e-6;
    // Discrete time step.
    const double dt = 1.e-3;
    // outside the initial radius are removed).
    const int numbodies = 1000;
    // Number of time-iterations executed by the program.
    const int max_iter = 10000;
    // Frequency at which PNG images are written.
    const int img_iter = 20;

    // The pseudo-random number generator is initialized at a deterministic
    // value, for proper validation of the output for the exercise series.
    std::srand(1);
    // x- and y-pos are initialized to a square with side-length 2*ini_radius.
    std::vector<double> posx(numbodies), posy(numbodies);
    for (int i=0; i<numbodies; ++i) {
        posx[i] = ((double) std::rand() / (double)RAND_MAX) * 2.*ini_radius + 0.5-ini_radius;
        posy[i] = ((double) std::rand() / (double)RAND_MAX) * 2.*ini_radius + 0.5-ini_radius;
    }
    // Initially, the bodies have a radial velocity of an amplitude proportional to
    // the distance from the center. This induces a rotational motion creating a
    // "galaxy-like" impression.
    std::vector<Body*> bodies;
    for (int i=0; i<numbodies; ++i) {
        double px = posx[i];
        double py = posy[i];
        double rpx = px-0.5;
        double rpy = py-0.5;
        double rnorm = std::sqrt(sqr(rpx)+sqr(rpy));
        if ( rnorm < ini_radius ) {
            double vx = -rpy * inivel * rnorm / ini_radius;
            double vy =  rpx * inivel * rnorm / ini_radius;
            bodies.push_back( new Body(mass, px, py, vx, vy) );
        }
    }

    // Principal loop over time iterations.
    for (int iter=0; iter<max_iter; ++iter) {
        // The quad-tree is recomputed at each iteration.
        Node* root = 0;
        for (unsigned i=0; i<bodies.size(); ++i) {
            bodies[i] -> resetToZerothQuadrant();
            root = add(bodies[i], root);
        }
        // Computation of forces, and advancement of bodies.
        verlet(bodies, root, theta, G, dt);
        // De-allocate the quad-tree.
        delete root;
        // Output.
        if (iter%img_iter==0) {
            std::cout << "Writing images at iteration " << iter << std::endl;
            save_bodies(bodies, iter/img_iter);
        }
    }
}

