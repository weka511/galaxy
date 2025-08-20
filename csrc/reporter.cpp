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
 
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include "reporter.hpp"
#include "logger.hpp"

using namespace std;

/**
 *   Record configuration in a csv file
 */
void Reporter::report(){
	_sequence++;
	if (--_count_down > 0) return;
	_count_down = _frequency;
	string file_name = _get_file_name();
	_output.open(file_name);
	if (_output.is_open()){
        _configuration.iterate(*this);
		_output.close();
		LOG(_configuration.get_momentum());
    } else {
		stringstream message;
		message<<__FILE__ <<" " <<__LINE__<<" Error: Unable to open report file " << file_name<<endl; 
		throw logic_error(message.str().c_str()); 
	}
}

/**
 *  Output velocity and position for one particle.
 */
void Reporter::visit(Particle & particle) {
	_output << particle << endl;
}

/**
 *   Verify that parogram should continue executing,
 *   i.e. killfile not present
 */
bool Reporter::should_continue() {
	ifstream file(_killfile);
	if (!file.is_open()) return true;
	cout << "Found killfile: " <<_killfile<<endl;
	file.close();
	remove(_killfile.c_str());
	return false;
}

/**
 *  Used to establish name for report file, including sequence number
 */
string Reporter::_get_file_name(){
	stringstream ss;
	ss << setw(10) << setfill('0') << _sequence;
	return _path + _base + ss.str() + "." + _extension;
}