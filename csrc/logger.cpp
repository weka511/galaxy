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
 
 #include <stdexcept>

 #include <sstream>
 #include "logger.hpp"
 
 using namespace std;
 
 unique_ptr<Logger> Logger::_instance = NULL;
 
 unique_ptr<Logger> & Logger::get_instance() {
	 if (Logger::_instance == NULL)
		 Logger::_instance = make_unique<Logger>();
	 return Logger::_instance;
 }
 
 Logger::Logger() {
	 _output.open("galaxy.log");
	 if (_output.is_open()){
   		_output << "Opened" << endl;
    } else {
		stringstream message;
		message<<__FILE__ <<" " <<__LINE__<<" Error: Unable to open log file " <<endl; 
		throw logic_error(message.str().c_str()); 
	}
 }
 
 void Logger::log(string file, int line, string s) {
	 _output << file << " " << line << " " << s << endl;
}

Logger::~Logger() {
	_output << "eof" << endl;
	_output.close();
}