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
 
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include "reporter.hpp"


using namespace std;

string Reporter::_get_file_name(int n){
	stringstream ss;
	ss << setw(10) << setfill('0') << _sequence;
	return _path + _base + ss.str() + "." + _extension;
}

/**
 *   Record configuration in a csv file
 */
void Reporter::report(){
	string file_name = _get_file_name();
	_output.open(file_name);
	if (_output.is_open()){
        _configuration.iterate(*this);
		_output.close();
		_sequence++;
    } else {
		stringstream message;
		message<<__FILE__ <<" " <<__LINE__<<" Error: Unable to open file " << file_name<<endl; 
		throw logic_error(message.str().c_str()); 
	}

}

void Reporter::visit(int i, Particle & particle) {
	_output << particle << endl;
}

