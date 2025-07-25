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
 *
 * Molecular dynamics simulation for hard disks or hard spheres, as described
 * in Statistical Mechanics: Algorithms and Computations, by Werner Krauth,
 * ISBN 978-0-19-851535-7.
 */
 
 #ifndef _CONFIGURATION_HPP
 #define _CONFIGURATION_HPP
 
#include <string> 
#include <memory>
#include "particle.hpp"

using namespace std;
 


/**
 *  This class manages a collection of Particles.
 */
class Configuration {
	/**
	 *  Descendents of this class are used to 
	 * iterate through all Particles, visiting each in turn
	 */
  public:
	class Visitor{
	  public:
		virtual void visit(Particle & particle) = 0;
	};
	/**
	 * Descendents of this class are used to iterate 
	 * through all pairs of Particles, visiting each pair in turn.
	 */
	class PairVisitor {
	  public:
		virtual void visit_pair(Particle & particle1,Particle & particle2) = 0;
	};
	class CompoundVisitor: public Visitor,  public PairVisitor{};
	
  private:
    string _version;   //FIXME: this has no accessor
	int _iteration;     //FIXME: this has no accessor
	double _theta;      //FIXME: this has no accessor
	double _G;          //FIXME: this has no accessor
	double _dt;          //FIXME: this has no accessor
	unique_ptr<Particle[]> _particles;  //FIXME: this has no accessor
	int _n;
  public:
	Configuration(string file_name);
	
	/**
	 * iterate through all Particles, visiting each in turn
	 */
	void iterate(Visitor & visitor) {
		for (int i=0;i<_n;i++)
			visitor.visit(_particles[i]);	
	}
	
	/**
	 * iterate through all pairs of Particles, visiting each pair in turn.
	 */
	void iterate(PairVisitor & visitor) {
		for (int i=0;i<_n;i++)
			for (int j=i+1;j<_n;j++)
			visitor.visit_pair(_particles[i],_particles[2]);	
	}
	
	/**
	 * iterate through all Particles, visiting each in turn,
	 * then through all pairs of Particles, visiting each pair in turn.
	 */
	void iterate(CompoundVisitor & visitor) {
		for (int i=0;i<_n;i++)
			visitor.visit(_particles[i]);	
		for (int i=0;i<_n;i++)
			for (int j=i+1;j<_n;j++)
			visitor.visit_pair(_particles[i],_particles[2]);	
	}
	 
 };
 
 #endif //_CONFIGURATION_HPP
 
 