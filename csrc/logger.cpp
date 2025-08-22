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
#include <stdexcept>
#include <filesystem>
#include <sstream>
#include <string>
#include <regex>
#include <ctime>
#include <iomanip>

#include "particle.hpp"
#include "logger.hpp"
 
using namespace std;
namespace fs = std::filesystem;

/**
 *  Unique instance of logger
 */
unique_ptr<Logger> Logger::_instance = NULL;

/**
 *   Prefix for names of log files
 */
string Logger::_base = "galaxy";

/**
 *   Folder name for log files
 */
string Logger::_path = "../logs";

/**
 *   Used to specifiy name and location for logfiles
 */
void Logger::set_paths( string base,  string path){
	_base = base;
	_path = path;
}

/**
 *   Accessor for unique instance
 */
	 
 unique_ptr<Logger> & Logger::get_instance() {
	 if (Logger::_instance == NULL)
		 Logger::_instance = make_unique<Logger>();
	 return Logger::_instance;
 }
 
 /**
  *   Assign a unique name to the logfile, and open it.
  */
 Logger::Logger() {
	 _start_time = chrono::steady_clock::now();
	auto full_path = Logger::_get_file_name();
	_output.open(full_path);
	if (_output.is_open()){
   		_output << "Opened" << endl;
    } else {
		stringstream message;
		message<<__FILE__ <<" " <<__LINE__<<" Error: Unable to open log file " <<full_path << endl; 
		throw logic_error(message.str().c_str()); 
	}
 }
 
 /**
  * Close logfile
  */
 Logger::~Logger() {
	_output.close();
}



/**
 *  This function is invoked by the LOG macro to log a single string
 */
 void Logger::log(string file, int line, string s) {
	 _output << file << " " << line << ": " << s  << endl << flush;
}

/**
 *  This function is invoked by the LOG macro to log a single integer
 */
void Logger::log(string file, int line, int n) {
	 _output << file << " " << line << ": " << n  << endl << flush;
}

/**
 *  This function is invoked by the LOG macro to log a 3 vector
 */
void Logger::log(string file, int line, array<double,NDIM> v) {
	 _output << file << " " << line << ": (" << v[0] << "," << v[1] <<"," << v[2] << ")"  << endl << flush;
}

/**
 *  This function is invoked by the LOG macro to log two strings
 */
void Logger::log(string file, int line, string s1, string s2) {
	 _output << file << " " << line << ": " << s1 << s2  << endl << flush;
}

/**
 *  This function is invoked by the TIME macro to record when 
 *  something occurred
 */
 void Logger::time_point(string file, int line) {
	 _output << file << " " << line << ": " << _get_milliseconds_since_start() <<  " ms." <<endl;
}

/**
 *  Create unique file name using date and time
 */
 string Logger::_get_file_name(){
	auto now = time(NULL);
	auto *aTime = localtime(&now);
	auto day = aTime->tm_mday;
	auto month = aTime->tm_mon + 1; 
	auto year = aTime->tm_year + 1900;
	auto hour = aTime->tm_hour;
	auto minute = aTime->tm_min;
	auto second = aTime->tm_sec;
	stringstream file_name;
	file_name << _base << "-" <<
				year<< "-" <<
				setw(2) << std::setfill('0') << month << "-" <<
				setw(2) << std::setfill('0') << day << "-" <<
				setw(2) << std::setfill('0') << hour << "-" <<
				setw(2) << std::setfill('0') << minute << "-" <<
				setw(2) << std::setfill('0') << second << ".log";
	fs::path full_path = _path;
    full_path /= file_name.str();
	return full_path;
}

 
 /**
  *  Calculate the time since logfile opened
  */
 chrono::duration<double> Logger::_get_milliseconds_since_start() {
	const auto now{chrono::steady_clock::now()};
    return now - _start_time;
 }
 