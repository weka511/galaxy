 #ifndef _CONFIGURATION_HPP
 #define _CONFIGURATION_HPP
 
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
 */
 
#include <string> 
#include <memory>
#include <vector>
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
		virtual void visit(int i, Particle & particle) = 0;
	};
	
	/**
	 * Descendents of this class are used to iterate 
	 * through all pairs of Particles, visiting each pair in turn.
	 */
	class PairVisitor {
	  public:
		virtual void visit_pair(Particle & particle1,Particle & particle2) = 0;
	};
	
	/**
	 * Descendents of this class are used to iterate through all Particles,
	 * visiting each in turn,then through all pairs of Particles, visiting 
	 * each pair in turn. Typically the first pass is used for initialization
	 */
	class CompoundVisitor: public Visitor,  public PairVisitor{};
	
  private:
  
	 /**
	 * Count the number of lines == number of particles + 5 + 1
	 */
	static int _get_line_count(ifstream& inputFile);
	
    string _version; 
	int _iteration;    // FIXME - initialized but not used
	double _theta; 
	double _G; 
	double _dt; 
  public:                 //FIXME
	unique_ptr<Particle[]> _particles;
	int _n;
	
  public:
	Configuration(string file_name);
	
	const string get_version() { return _version;}
	
	const double get_theta() { return _theta;}
	
	const double get_G() { return _G;}
	
	const double get_dt() { return _dt;}
	
	/**
	 * iterate through all Particles, visiting each in turn
	 */
	void iterate(Visitor & visitor) {
		for (int i=0;i<_n;i++)
			visitor.visit(i,_particles[i]);	

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
			visitor.visit(i,_particles[i]);
	
		for (int i=0;i<_n;i++)
			for (int j=i+1;j<_n;j++)
				visitor.visit_pair(_particles[i],_particles[2]);
	}
	 
 };
 
 /**
  * encode
  *
  * Encode a floating value so it can be stored and retrieved without loss of significant digits
  *
  * Parameters:
  *    value       Number to be stored
  * Returns:
  *    Text string that can be written to a file
  */
 string encode(const double value);
 
 /**
  * decode
  *
  * Restore floating value stored by encode
  *
  * Parameters:
  *    value       Text string read from file
  * Returns:
  *    Corresponding floating point value
  */
 double decode(string str);
 
 /**
  * This class splits a string into a vector of tokens
  * https://medium.com/@ryan_forrester_/splitting-strings-in-c-a-complete-guide-cf162837f4ba
  */
 class StringSplitter {
  public:
    static vector<string> split(const string& str, const string& delimiters, bool keepEmpty = false) ;
};

 #endif //_CONFIGURATION_HPP
 
 