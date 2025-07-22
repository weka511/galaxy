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
 * Molecular dynamics simulation for hard disks or hard spheres, as described
 * in Statistical Mechanics: Algorithms and Computations, by Werner Krauth,
 * ISBN 978-0-19-851535-7.
 */
 
#include <array>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "configuration.hpp"
#include "utils.hpp"

using namespace std;



Configuration::Configuration(string file_name){
	ifstream inputFile(file_name);
	if (!inputFile.is_open()) 
		throw invalid_argument( "Could not open configuration file " + file_name);
		
	string line;
	auto line_number = 0;
    while (getline(inputFile, line)) {
		if (line.compare(0,3,"End",0,3)==0) break;
		
		auto tokens = StringSplitter::split(line, "=,", true);
	
		switch (line_number) {
			case 0:
				if (tokens[0].compare("Version")==0)
					version = tokens[1];
				else
					throw invalid_argument( "Error " + line);
				break;
			case 1:
				if (tokens[0].compare("iteration")==0)
					iteration = stoi(tokens[1]);
				else
					throw invalid_argument( "Error " + line);
				break;
			case 2:
				if (tokens[0].compare("theta")==0)
					theta = decode(tokens[1]);
				else
					throw invalid_argument( "Error " + line);
				break;
			case 3:
				if (tokens[0].compare("G")==0)
					G = decode(tokens[1]);
				else
					throw invalid_argument( "Error " + line);
				break;
			case 4:
				if (tokens[0].compare("dt")==0)
					dt = decode(tokens[1]);
				else
					throw invalid_argument( "Error " + line);
				break;
			default:

				auto position = array{0.0,0.0,0.0};
				auto mass = 0.0;
				auto velocity = array{0.0,0.0,0.0};
				for (int i=0;i<7;i++) 
					switch(i){
						case 0:
						case 1:
						case 2:
							position[i] = decode(tokens[i]);
							break;
						case 3:
							mass = decode(tokens[i]);
							break;
						case 4:
						case 5:
						case 6:
							velocity[i-4] = decode(tokens[i]);
					}
					cout << position[0] << ", " << position[1] << ", " << position[2] << ", " 
					<< mass << ", " << velocity[0] << ", " << velocity[1] << ", " << velocity[2]<< endl;
				
		}
		line_number++;
    }
	cout << file_name << endl;
	cout << version << endl;
	cout << theta << endl;
	cout << G << endl;
	cout << dt << endl;
}