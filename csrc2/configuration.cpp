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
#include "configuration.hpp"

using namespace std;

/**
 * Count the number of lines == number of particles + 5 + 1
 */
int Configuration::_get_line_count(ifstream& inputFile) {
	auto line_count = 0;
	string line;
	while (getline(inputFile, line))
		line_count++;

	inputFile.clear();
	inputFile.seekg(0);
	return line_count;
}

Configuration::Configuration(string file_name){
	ifstream inputFile(file_name);
	if (!inputFile.is_open()) 
		throw invalid_argument( "Could not open configuration file " + file_name);
	
	_n = Configuration::_get_line_count(inputFile) - 6;
	_particles = make_unique<Particle[]>(_n);
	
	auto line_number = 0;
	string line;
    while (getline(inputFile, line)) {
		if (line.compare(0,3,"End",0,3)==0) break;
		
		auto tokens = StringSplitter::split(line, "=,", true);
	
		switch (line_number) {
			case 0:
				if (tokens[0].compare("Version")==0)
					_version = tokens[1];
				else
					throw invalid_argument( "Error " + line);
				break;
			case 1:
				if (tokens[0].compare("iteration")==0)
					_iteration = stoi(tokens[1]);
				else
					throw invalid_argument( "Error " + line);
				break;
			case 2:
				if (tokens[0].compare("theta")==0)
					_theta = decode(tokens[1]);
				else
					throw invalid_argument( "Error " + line);
				break;
			case 3:
				if (tokens[0].compare("G")==0)
					_G = decode(tokens[1]);
				else
					throw invalid_argument( "Error " + line);
				break;
			case 4:
				if (tokens[0].compare("dt")==0)
					_dt = decode(tokens[1]);
				else
					throw invalid_argument( "Error " + line);
				break;
			default:
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
							mass = value;
							break;
						case 4:
						case 5:
						case 6:
							velocity[i-4] = value;
					}
				}
				_particles[line_number-5].init(position,velocity,mass);	
		}
		line_number++;
    }
	cout << __FILE__ << " " << __LINE__ << ": " << _n << " particles"<<endl;
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
 string encode(const double value) {
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
 double decode(string str){
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

 

