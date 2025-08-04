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
#include <time.h>
#include "logger.hpp"
 
using namespace std;
namespace fs = std::filesystem;

unique_ptr<Logger> Logger::_instance = NULL;
string Logger::_base = "logger";
string Logger::_path = "../logs";
	
 unique_ptr<Logger> & Logger::get_instance() {
	 if (Logger::_instance == NULL)
		 Logger::_instance = make_unique<Logger>();
	 return Logger::_instance;
 }
 
 string Logger::to_str2(int n){
	stringstream result;
	if (n < 10)
		 result << "0" << n;
	else
		 result << n;
	return result.str();
 }
 string Logger::get_file_name(){
	auto theTime = time(NULL);
	auto *aTime = localtime(&theTime);
	auto day = aTime->tm_mday;
	auto month = aTime->tm_mon + 1; 
	auto year = aTime->tm_year + 1900;
	auto hour = aTime->tm_hour;
	auto minute = aTime->tm_min;
	auto second = aTime->tm_sec;
	stringstream file_name;
	file_name << _base << "-" <<
				year<< "-" << to_str2(month)<< "-" << to_str2(day) << "-" 
				<< to_str2(hour) <<"-" << to_str2(minute) <<"-" << to_str2(second) <<".log";
	fs::path full_path = _path;
    full_path /= file_name.str();
	return full_path;
}

 Logger::Logger() {
	auto full_path = Logger::get_file_name();
	_output.open(full_path);
	if (_output.is_open()){
   		_output << "Opened" << endl;
    } else {
		stringstream message;
		message<<__FILE__ <<" " <<__LINE__<<" Error: Unable to open log file " <<full_path << endl; 
		throw logic_error(message.str().c_str()); 
	}
 }
 
 void Logger::log(string file, int line, string s) {
	 _output << file << " " << line << ": " << s << endl;
}

Logger::~Logger() {
	_output.close();
}