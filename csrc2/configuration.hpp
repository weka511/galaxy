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
  public:
	
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
	static string encode(const double value);
 
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
	static double decode(string str);
	 
	/**
	 *  Descendents of this class are used to 
	 * iterate through all Particles, visiting each in turn
	 */
  
  
	/**
	 * Descendents of this class are used to initialize
	 * things that need access to the stored particles.
	 */
	class ParticleInitializer{
	  public:
  	    virtual void initialize(int n, unique_ptr<Particle[]> & particles) =  0;
	};
	
	/**
	 * Descendents of this class are used to iterate 
	 * through all Particles.
	 */
	class Visitor{
	  public:
		virtual void visit(int i, Particle & particle) = 0;
	};
		
  private:
  
	 /**
	 * Count the number of particles described in configuration file
	 */
	static int _get_line_count(ifstream& inputFile);
	
	/**
	 *  Version number for config file
	 */
    string _version; 
	
	/**
	 *   The particles making up the model
	 */
	unique_ptr<Particle[]> _particles;
	
	/**
	 *   The number of particles
	 */
	int _n;
	
  public:
	Configuration(string file_name);
	
	const string get_version() { return _version;}
	
	/**
	 * iterate through all Particles, visiting each in turn
	 */
	void iterate(Visitor & visitor) {
		for (int i=0;i<_n;i++)
			visitor.visit(i,_particles[i]);	
	}
	
	/**
	 * Used to initialize data structures that need to know about particles.
	 */
	void initialize(ParticleInitializer & initializer){
		initializer.initialize(_n,_particles);
	}
 };
 
 
 
 /**
  * This class splits a string into a vector of tokens
  * https://medium.com/@ryan_forrester_/splitting-strings-in-c-a-complete-guide-cf162837f4ba
  */
 class StringSplitter {
  public:
    static vector<string> split(const string& str, const string& delimiters, bool keepEmpty = false) ;
};

 #endif //_CONFIGURATION_HPP
 
 