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
 */
 
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <ostream>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>

#include "utils.hpp"
#include "spdlog/spdlog.h"
using namespace std;

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
 * If file exists, copy to backup
 */
void backup(string file_name, string backup) {
	ifstream file(file_name);
	if (file.is_open()) {
		stringstream backup_file_name;
		backup_file_name << file_name<<backup;
		ifstream  src(file_name);
		ofstream  dst(backup_file_name.str().c_str());
		dst << src.rdbuf();
	}
}

/**
  * Used to check whether program should be running:
  * check for presence of killfile: if present, delete it
  * and return TRUE, otherwise return FALSE
  *
  * Parameters:
  *      killfile     Name of file used to stop execution
  */
 bool killed(string killfile) {
	ifstream file(killfile);
	bool result=file.is_open();
	if (result){
		cout << "Found killfile: " <<killfile<<endl;
		file.close();
		remove(killfile.c_str());
	}
	return result;
 }

 /**
  * Create a directory if necessary, e.g. for logfile
  */
void ensure_path_exists(string path) {
	stringstream command;
	command<<"exec mkdir --parents " << path;
	system(command.str().c_str());
}

/**
 *  Remove contents of config directory
 */
 void remove_old_configs(string path) {
	stringstream command;
	command<<"exec rm " << path << "/*";
	system(command.str().c_str());
}

/**
 *  Test string to see whether it ends with a prescribed substring.
 */
bool ends_with(string const & value, string const & ending){
    if (ending.size() > value.size()) return false;
    return equal(ending.rbegin(), ending.rend(), value.rbegin());
}

/**
 * Parse numeric command line parameter and validate:
 * 1. any extraneous parameters
 * 2. are value within range?
 */
double get_double(string name, char * param, double high,double low){
	// auto logger=spdlog::get("galaxy");
	string::size_type sz;	
	const double retval = stod (param,&sz);
	if (sz==strlen(param) && low<retval && retval<high){
		// logger->info("{0}={1}",name,retval);
		return retval;
	}	else{
		stringstream err;
		err<< name <<"=" <<retval<<". Should be in range ("<< low <<"," <<high <<")" <<endl;
		throw out_of_range(err.str());
	}
} 

/**
 * Parse numeric command line parameter and validate:
 * 1. any extraneous parameters
 * 2. are value within range?
 */
int get_number(string name, char * param, int high,int low){
	// auto logger=spdlog::get("galaxy");
	string::size_type sz;	
	const int retval = stoi (param,&sz);
	if (sz==strlen(param) && low<retval && retval<high){
		// logger->info("{0}={1}",name,retval);
		return retval;
	}	else{
		stringstream err;
		err<< name <<"=" <<retval<<". Should be in range ("<< low <<"," <<high <<")" <<endl;
		throw out_of_range(err.str());
	}
} 

/**
 * Determine the standard deviation of observations stored in a vector
 */
double stdev(vector<double> values,double mu,bool bessel) {
	double sq_sum=0;
	for (int i=0;i<values.size();i++){
		const double diff=values[i]-mu;
		sq_sum+=diff*diff;
	}
	return sqrt(sq_sum / (bessel ? values.size()-1 :values.size()));
}

/**
 * Determine the mean of observations stored in a vector
 */
double mean(vector<double> values) {
	return  accumulate(values.begin(), values.end(), 0.0) / values.size();
}

 