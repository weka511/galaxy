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
 #include <tuple>
 #include <memory>
 #include <getopt.h>
 using namespace std;

/**
 *  A class for storing command line parameters.
 */
class Parameters {
  public:
	/**
	 *  Parse command line parameters.
	 */
	static unique_ptr<Parameters>  get_options(int argc, char **argv);
	static struct option long_options[];
	
	string config_file = "../configs/config.txt";
	int max_iter = 10000;
	int a = 1.0;
	string base = "galaxy";
	string path = "configs/";
	float G = 1.0;
	float dt = 0.1;
	float theta = 1.0;
	int frequency = 100;
};




