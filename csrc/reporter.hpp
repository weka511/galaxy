#ifndef _REPORTER_HPP
#define _REPORTER_HPP

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
 
 #include <fstream>
 #include <string>
 #include "configuration.hpp"
 
 using namespace std;

class IReporter : public Visitor<Particle> {
  public:
	/**
	 *   Record configuration in a csv file
	 */
	virtual void report() = 0;
};


/**
 *  This class is used to record the configuration periodically.
 */
class Reporter : public IReporter {
	
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
	
 
	/**
	 *   Gap between sequnce numbers for files.
	 */
	const int _frequency;
	
	/**
	 *  Used to determine whether it is time to output a file.
	 */
	int _count_down;
	
  public:
    Reporter(Configuration & configuration,string base="galaxy",string path="configs/",string extension="csv", int frequency=1)
 	: 	_configuration(configuration),
		_output(),_base(base),_path(path),_extension(extension),_sequence(0),
		_frequency(frequency),_count_down(frequency) {;}
	
	/**
	 *   Record configuration in a csv file
	 */
	void report();
	
	/**
	 * Output velocity and position for one particle.
	 */
	void visit(Particle & particle);
	
  private:
	/**
	 *  Used to establish name for report file, including sequence number
	 */
	string _get_file_name();
};


 
 #endif // #_REPORTER_HPP
 