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
#include <fstream>
#include <getopt.h>
#include "galaxy.hpp"
#include "verlet.hpp"
#include "barnes-hut.hpp"

using namespace std;

int main(int argc, char **argv) {
	string config_file;
	int max_iter;
	double softening_length;
	tie(config_file,max_iter,softening_length) = get_options(argc,argv);

	auto start = chrono::high_resolution_clock::now();
	cout << "galaxy: " << VERSION << endl;

	try {
		cout << "executing" << endl;
		Configuration configuration(config_file);
		AccelerationVisitor calculate_acceleration(configuration, configuration.get_theta(),configuration.get_G(),softening_length);
		FileReporter reporter(configuration);
		Verlet integrator(configuration,  calculate_acceleration,reporter);
		integrator.run(max_iter,configuration.get_dt());
	}  catch (const exception& e) {
        cerr << "Terminating because of errors: " << e.what() << endl;
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
tuple <string,int,double> get_options(int argc, char **argv) {
	auto config_file = "../configs/config.txt";
	auto max_iter = 100;
	auto softening_length = 1.0;
	static struct option long_options[] ={ 
		{"config", required_argument, NULL, 'c'},
		{"max_iter", required_argument, NULL, 'N'},
		{"softening_length", required_argument, NULL, 's'},
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
		case 's':
			softening_length = atof(optarg); 
			break;
		}
	}
	cout << config_file<<","<<max_iter<<","<<softening_length<<endl;
	return make_tuple(config_file,max_iter,softening_length);
}

void FileReporter::report() {
	
}

bool FileReporter::should_continue() {
	ifstream file(_killfile);
	if (!file.is_open()) return true;
	cout << "Found killfile: " <<_killfile<<endl;
	file.close();
	remove(_killfile.c_str());
	return false;
}
