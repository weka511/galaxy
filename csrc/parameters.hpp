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
 * Implementation of the Barnes Hut algorithm to simulate the evolution of a galaxy.
 */
 
 #include <string>
 #include <memory>
 #include <getopt.h>
 
 using namespace std;

/**
 *  A class for storing command line parameters.
 */
class Parameters {
  private:
  	string _config_file = "../configs/config.txt";
	int _max_iter = 10000;
	int _a = 1.0;
	string _base = "galaxy";
	string _path = "configs/";
	float _G = 1.0;
	float _dt = 0.1;
	float _theta = 1.0;
	int _frequency = 100;
	
  public:
	Parameters();
	
	/**
	 *  Parse command line parameters.
	 */
	static unique_ptr<Parameters>  get_options(int argc, char **argv);
	static struct option long_options[];
	
	string get_config_file() {return _config_file;}
	
	int get_max_iter() {return _max_iter;}
	
	int get_a () {return _a;}
	
	string get_base() {return _base;}
	
	string get_path() {return _path;}
	
	float get_G() {return _G;}
	
	float get_dt() {return _dt;}
	
	float get_theta() {return _theta;}
	
	int get_frequency() {return _frequency;}
	
};

/**
 *  Show list of command line parameters.
 */
void usage();

#endif // __PARAMETERS_HPP
