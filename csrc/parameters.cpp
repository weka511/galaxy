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
 * This file contains a class to extract command line parameters and 
 * environment variables. Parameters that can vary from one run to another
 * are specified using the command line; ones that shoudl remain fixed are
 * specified using environment varabales.
 */

#include <iostream>
#include <filesystem>

#include "parameters.hpp"

using namespace std;
using namespace filesystem;

/**
 *  Options for command line
 */
struct option Parameters::long_options[] ={ 
	{"config", required_argument, NULL, 'c'},
	{"max_iter", required_argument, NULL, 'N'},
	{"softening_length", required_argument, NULL, 'a'},
	{"G", required_argument, NULL, 'G'},
	{"dt", required_argument, NULL, 'd'},
	{"theta", required_argument, NULL, 'e'},
	{"report", required_argument, NULL, 'r'},
	{"frequency",required_argument,NULL,'f'},
	{"help",no_argument,NULL,'h'},
	{NULL, 0, NULL, 0}
};


/**
 *  Read environment variables
 */
Parameters::Parameters() {
	if (const char* env_p = getenv("GALAXY_BASE"))
		_base = env_p;
	
	if (const char* env_p = getenv("GALAXY_PATH"))
		_path = _get_path_name(env_p);
	
	if (const char* env_p = getenv("GALAXY_LOG_PATH"))
		_log_path = _get_path_name(env_p);
}

/**
 *  Parse command line parameters.
 */
unique_ptr<Parameters> Parameters::get_options(int argc, char **argv){
	unique_ptr<Parameters> parameters = make_unique<Parameters>();
	char ch;
	while ((ch = getopt_long(argc, argv, "c:N:s:f:a:G:d:e:h", long_options, NULL)) != -1){
	  switch (ch)    {
		 case 'c':
			 parameters->_config_file = optarg; 
			 break;
		case 'N':
			parameters->_max_iter = atoi(optarg); 
			break;
		case 'f':
			parameters->_frequency = atoi(optarg); 
			break;
		case 'a':
			parameters->_a = atof(optarg); 
			break;
		case 'G':
			parameters->_G = atof(optarg); 
			break;
		case 'd':
			parameters->_dt = atof(optarg); 
			break;
		case 'e':
			parameters->_theta = atof(optarg); 
			break;
		case 'h':
			parameters->usage(); 
			exit(0);
		default:
			parameters->usage();
			exit(2);
		}
	}
	return parameters;
}

/**
 *  Show list of command line parameters.
 */
void Parameters::usage() {
	cout << "Galaxy " << VERSION << endl;
	cout << "Implementation of the Barnes Hut algorithm to simulate the evolution of a galaxy." << endl << endl;
	cout << "\t-c" << "\t--config" << endl;
	cout << "\t-N" << "\t--max_iter" << endl;
	cout << "\t-a" << "\t--softening_length" << endl;
	cout << "\t-G" << "\t--G -- Gravitational constant" << endl;
	cout << "\t-d" << "\t--dt" << endl;
	cout << "\t-e" << "\t--theta" << endl;
	cout << "\t-f" << "\t--frequency" << endl;
	cout <<"\t-h" << "\t--help"  << endl;
}

/**
 * Convert an enviromnent variable to a full path name.
 */
string Parameters::_get_path_name(const char* env_p){
	path path1 = getenv("HOME");
	path1 /= env_p;
	return path1;
}