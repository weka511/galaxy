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
 
#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <set>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include "configuration.hpp"
#include "logger.hpp"

using namespace std;

/**
 * Count the number of particles described in configuration file
 */
int Configuration::_get_line_count(ifstream& inputFile) {
	auto line_count = 0;
	auto ended = false;
	string line;
	while (getline(inputFile, line)){
		if (!line.contains("="))
			line_count++;
	
		ended = line.starts_with("End");
		if (ended)
			line_count -= 1;	
	}
	if (!ended)
		throw logic_error("Configuration file not ended cleanly");
	
	inputFile.clear();
	inputFile.seekg(0);
	return line_count + 5 + 1;
}

/**
 *  Create a configuration from a list of particles 
 *  that has been stored in a file.
 *
 *  Parameters:
 *      file_name   Name of file (created by configure.py)
 */

	 

Configuration::Configuration(string file_name){
	ifstream inputFile(file_name);
	if (!inputFile.is_open()) 
		throw invalid_argument( "Could not open configuration file " + file_name);
	
	_n = Configuration::_get_line_count(inputFile) - 6;
	_particles = make_unique<Particle[]>(_n);
	
	auto index = 0;
	string line;
    while (getline(inputFile, line)) {
		if (line.starts_with("End")) break;
		
		auto tokens = StringSplitter::split(line, "=,", true);
	
		if (tokens[0].compare("Version")==0)
			_version = tokens[1];
		else {
			if (tokens.size() != 7){
				stringstream message;
				message<<__FILE__ <<" " <<__LINE__<<" Error in line " << line << " of "<<file_name; 
				throw logic_error(message.str()); 
			}
			auto position = array{0.0,0.0,0.0};
			auto mass = 0.0;
			auto velocity = array{0.0,0.0,0.0};
			for (int i=0;i<7;i++) {
				auto value = decode(tokens[i]);
				switch(i){
					case 0:
					case 1:
					case 2:
						position[i] = value;
						break;
					case 3:
						mass = value;;
						break;
					case 4:
					case 5:
					case 6:
						velocity[i-4] = value;
					}
			}
			_particles[index].init(position,velocity,mass,index);	
			index++;			
		}
    }

	cout << __FILE__ << " " << __LINE__ << ": " << _n << " particles"<<endl;
}


/**
 *   Create a  configiration for testing.
 */
Configuration::Configuration(int n, double particles[]){
	_n = n;
	_particles = make_unique<Particle[]>(_n);
	for (int index=0;index<n;index++){
		auto position = array{particles[7*index],particles[7*index+1],particles[7*index+2]};
		auto mass = particles[7*index+3];
		auto velocity = array{particles[7*index+4],particles[7*index+5],particles[7*index+6]};
		_particles[index].init(position,velocity,mass,index);
	}
}

/**
 * Encode a floating value so it can be stored and retrieved without loss of significant digits
 * 
 * Parameters:
 *    value       Number to be stored
 * Returns:
 *    Text value that can be written to a file
 *
 * Snarfed from: https://stackoverflow.com/questions/27149246/how-to-implement-serialization-and-de-serialization-of-a-double
 */
 string Configuration::encode(const double value) {
	const size_t maxPrecision = numeric_limits<double>::digits;
	uint64_t* pi = (uint64_t*)&value;
	stringstream stream;
	stream.precision(maxPrecision);
	stream << *pi;
	return stream.str();
 }
 
 /**
  * Restore floating value stored by encode
  *
  * Parameters:
  *    value       Text string read from file
  * Returns:
  *    Corresponding floating point value
  */
 double Configuration::decode(string str){
	uint64_t out = stoull(str);
	double* pf = (double*)&out;
	return *pf;
 }
 
 /**
  *  Split a string into a vector of tokens
  */
 vector<string> StringSplitter::split(const string& str, const string& delimiters, bool keepEmpty) {
    auto tokens = vector<string> ();
    string::size_type pos = 0;
    string::size_type prev = 0;
        
    while ((pos = str.find_first_of(delimiters, prev)) != string::npos) {
        if (keepEmpty || pos > prev)
            tokens.push_back(str.substr(prev, pos - prev));
  
        prev = pos + 1;
    }
        
    if (prev < str.length())
        tokens.push_back(str.substr(prev));
    else if (keepEmpty && prev == str.length())
        tokens.push_back("");
        
    return tokens;
}

/**
 * Determine total linear momentum
 */
array<double,NDIM>  Configuration::get_momentum(){
	array<double,NDIM> momentum = {0.0,0.0,0.0};
	for (int i=0;i<_n;i++)
		for (int j=0;j<NDIM;j++)
			momentum[j] += _particles[i].get_mass() * _particles[i].get_velocity()[j];
		
	return momentum;
}

/**
 * iterate through all Particles, visiting each in turn
 */
void Configuration::iterate(Visitor & visitor) {
	for (int i=0;i<_n;i++) 
		visitor.visit(_particles[i]);	
}

/**
 * Used to initialize data structures that need to know about particles.
 */
void Configuration::initialize(ParticleInitializer & initializer){
	initializer.initialize(_particles,_n);
}

 

