#ifndef __PARAMETERS_HPP
#define __PARAMETERS_HPP

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
 
 #include <string>
 #include <memory>
 #include <getopt.h>
 #include <filesystem>
 
using namespace std;
using namespace filesystem;

/**
 *  A class for storing parameters, both command line and environment variables.
 */
class Parameters {
	
  private:
  	string _config_file = "config.txt";
	
	/**
	 *   Maximum number of iterations for simulation
	 */
	int _max_iter = 10000;
	
	/**
	 *   Softening constant for gravitation field
	 */
	int _a = 1.0;
	
	/**
	 *   Base name for report files (a sequence number will be appended).
	 */
	string _base = "galaxy";
	
	/**
	 *   Folder name for report and config files
	 */
	path _path = "configs/";
	
	/**
	 *   Folder name for log files
	 */
	path _log_path = "logs/";
	
	/**
	 *   Gravitational constant
	 */
	float _G = 1.0;
	
	/**
	 *   Get time step for integration
	 */
	float _dt = 0.1;
	
	/**
	 *   Ratio for Barnes G=Hut cutoff (Barnes and Hut recommend 1.0)
	 */
	float _theta = 1.0;
	
	/**
	 *   Get Number of iterations that occur between reports
	 */
	int _frequency = 100;
	
	/**
	 * Used in conjunction with treecode: it determines whether to
     * verify that each particle is in the Tree once and only once.
	 */
	bool _verify_tree = false;
	
  public:
  
	/**
	 *  Read environment variables
	 */
	Parameters();
	
	/**
	 *  Parse command line parameters.
	 */
	static unique_ptr<Parameters>  get_options(int argc, char **argv);
	
	/**
	 *  Options for caommand line
	 */
	static struct option long_options[];
	
	/**
	 * Name of configuration file
	 */
	string get_config_file() {return _config_file;}
	
	/**
	 *   Get maximum number of iterations for simulation
	 */
	int get_max_iter() {return _max_iter;}
	
	/**
	 *   Get softening constant for gravitation field
	 */
	int get_a () {return _a;}
	
	/**
	 *   Get base name for report files (a sequence number will be appended).
	 */
	string get_base() {return _base;}
	
	/**
	 *   Get folder name for report files
	 */
	path get_path() {return _path;}
	
	/**
	 *   Get folder name for log files
	 */
	path get_log_path() {return _log_path;}

	
	/**
	 *   Get gravitational constant
	 */
	float get_G() {return _G;}
	
	/**
	 *   Get time step for integration
	 */
	float get_dt() {return _dt;}
	
	/**
	 *   Get ratio for Barnes G=Hut cutoff (Barnes and Hut recommend 1.0)
	 */
	float get_theta() {return _theta;}
	
	/**
	 *   Get number of iterations that occur between reports
	 */
	int get_frequency() {return _frequency;}
	
	/**
	 * Used in conjunction with treecode: it determines whether to
     * verify that each particle is in the Tree once and only once.
	 */
	bool should_verify_tree() {return _verify_tree;}
	
	/**
	 *  Show list of command line parameters.
	 */
	void usage();
	
  private:
	/**
	 * Convert an enviromnent variable to a full path name.
	 */
	path _get_path_name(const char* env_p);
};



#endif // __PARAMETERS_HPP
