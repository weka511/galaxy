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

#include <iostream>
#include <chrono>
#include <getopt.h>
#include "acceleration.hpp"
#include "galaxy.hpp"
#include "verlet.hpp"
#include "barnes-hut.hpp"
#include "reporter.hpp"

using namespace std;

int main(int argc, char **argv) {
	string config_file;
	int max_iter;
	double a;
	double G;
	double dt;
	double theta;
	string base;
	string path;
	tie(config_file,max_iter,a,G,dt,theta,base,path) = get_options(argc,argv);

	auto start = chrono::high_resolution_clock::now();
	cout << __FILE__ << " " << __LINE__ << " galaxy: " << VERSION << endl;

	try {
		Configuration configuration(config_file);
		AccelerationVisitor calculate_acceleration(configuration, theta,G,a);
		Reporter reporter(configuration,base,path);
		Verlet integrator(configuration,  calculate_acceleration,reporter);
		integrator.run(max_iter,dt);
	}  catch (const exception& e) {
        cerr << __FILE__ << " " << __LINE__ << " Terminating because of errors: "<< endl;
		cerr  << e.what() << endl;
		exit(1);
    }
	auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;
	
    return 0;
}

/**
 *  Parse command line parameters.
 */
tuple <string,int,double,double,double,double,string,string> get_options(int argc, char **argv) {
	auto config_file = "../configs/config.txt";
	auto max_iter = 100;
	auto a = 1.0;
	auto base = "galaxy";
	auto path = "configs/";
	auto G = 1.0;
	auto dt = 0.1;
	auto theta = 1.0;
	static struct option long_options[] ={ 
		{"config", required_argument, NULL, 'c'},
		{"max_iter", required_argument, NULL, 'N'},
		{"softening_length", required_argument, NULL, 'a'},
		{"G", required_argument, NULL, 'G'},
		{"dt", required_argument, NULL, 'd'},
		{"theta", required_argument, NULL, 'h'},
		{"report", required_argument, NULL, 'r'},
		{"path", required_argument, NULL, 'p'},
		{NULL, 0, NULL, 0}
	};
	char ch;
	while ((ch = getopt_long(argc, argv, "c:N:s:", long_options, NULL)) != -1){
	  switch (ch)    {
		 case 'c':
			 config_file = optarg; 
			 break;
		 case 'N':
			max_iter = atoi(optarg); 
			break;
		case 'a':
			a = atof(optarg); 
			break;
		case 'G':
			G = atof(optarg); 
			break;
		case 'd':
			dt = atof(optarg); 
			break;
		case 'h':
			theta = atof(optarg); 
			break;
		case 'r':
			 base = optarg; 
			 break;
 		 case 'p':
			 path = optarg; 
			 break;
		}
	}
	return make_tuple(config_file,max_iter,a,G,dt,theta,base,path);
}

