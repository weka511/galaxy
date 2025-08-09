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

/**
 *  This class is used to record the configuration periodically.
 */
class Reporter : public Configuration::Visitor {
  private:
	Configuration & _configuration;
	ofstream _output;
	string _base;
	string _path;
	string _extension;
	
	/**
	 *   Sequence number for files. This is incremented every time report() is called,
	 *   so the file numbers have gap equal to _frequency.
	 */
	int _sequence;
	
    string _killfile;
	/**
	 *   Gap between sequnce numbers for files.
	 */
	const int _frequency;
	
	/**
	 *  Used to determine whether it is time to output a file.
	 */
	int _count_down;
	
  public:
    Reporter(Configuration & configuration,string base="galaxy",string path="configs/",string extension="csv", string killfile="kill",int frequency=1)
 	: 	_configuration(configuration),
		_output(),_base(base),_path(path),_extension(extension),_sequence(0),
		_killfile(killfile),_frequency(frequency),_count_down(frequency) {;}
	
	/**
	 *   Record configuration in a csv file
	 */
	void report();
	
	/**
	 * Output velocity and position for one particle.
	 */
	void visit(Particle & particle);
	
	/**
	 *   Verify that parogram should continue executing,
	 *   i.e. killfile not present
	 */
    bool should_continue();
	
  private:
	/**
	 *  Used to establish name for report file, including sequence number
	 */
	string _get_file_name(int n=4);
};


 
 #endif // #_REPORTER_HPP
 