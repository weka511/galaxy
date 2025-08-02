#ifndef _REPORTER_HPP
#define _REPORTER_HPP

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
 *
 */
 
 #include <fstream>
 #include <string>
 #include "configuration.hpp"
 
 using namespace std;

class Reporter : public Configuration::Visitor {
  private:
	Configuration & _configuration;
	ofstream _output;
	string _base;
	string _path;
	string _extension;
	int _sequence;
	
  public:
    Reporter(Configuration & configuration,string base="foo",string extension="csv",string path="configs/")
 	: _configuration(configuration),_output(),_base(base),_path(path),_extension(extension),_sequence(0) {;}
	
	/**
	 *   Record configuration in a csv file
	 */
	void report();
	
	/**
	 * Outout velocity and position for one particle.
	 */
	void visit(int i, Particle & particle);
	
    virtual bool should_continue() = 0;
	
  private:
	string _get_file_name(int n=4);
};


 
 #endif // #_REPORTER_HPP
 