/**
 * Copyright (C) 2018 Greenweaves Software Limited
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
 
#ifndef _UTILS_H
#define _UTILS_H

#include <limits>
#include <string>
#include <vector>

 /**
  * Encode a floating value so it can be stored and retrieved without loss of significant digits
  */
 std::string encode(const double small);
 
 /**
  * Restore value stored by encode
  */
 double decode(std::string str);
 
 /**
  * Check for presence of killfile
  */
 bool killed(std::string killfile="kill");
 
 /**
 * If file exists, copy to backup
 */
 void backup(std::string file_name, std::string backup="~");
 
/**
 *  Test string to see whether it ends with a prescribed substring.
 */ 
bool ends_with(std::string const & value, std::string const & ending);

/**
  * Create a directory if necessary, e.g. for logfile
  */
void ensure_path_exists(std::string path);

/**
 *  Remove contents of config directory
 */
void remove_old_configs(std::string path);

/**
 * Square a number
 */ 
inline double sqr(double x) {return x*x;}

/**
 * Parse numeric command line parameter and validate:
 * 1. any extraneous parameters
 * 2. are value within range?
 */
int get_number(std::string name, char * param, int high=std::numeric_limits<int>::max(),int low=0);

/**
 * Parse numeric command line parameter and validate:
 * 1. any extraneous parameters
 * 2. are value within range?
 */
double get_double(std::string name,char * param, double high=std::numeric_limits<double>::max(),double low=0);

/**
 * Determine the mean of observations stored in a vector
 */
double mean(std::vector<double> values);

/**
 * Determine the standard deviation of observations stored in a vector
 */
double stdev(std::vector<double> q1s,double mean,bool bessel=false);

#endif
 