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
 
#include <vector>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <unistd.h>
#include <getopt.h>
#include <random>
#include <algorithm>
#include "utils.h"

/**
 * Encode a floating value so it can be stored and retrieved without loss of significant digits
 *
 * Snarfed from: https://stackoverflow.com/questions/27149246/how-to-implement-serialization-and-de-serialization-of-a-double
 */
 std::string encode(const double small) {
	const std::size_t maxPrecision = std::numeric_limits<double>::digits;
	uint64_t* pi = (uint64_t*)&small;
	std::stringstream stream;
	stream.precision(maxPrecision);
	stream << *pi;
	return stream.str();
 }
 
 /**
  * Restore value stored by encode
  */
 double decode(std::string str){
	uint64_t out = std::stoull(str);
	double* pf = (double*)&out;
	return *pf;
 }
 
 /**
 * If file exists, copy to backup
 */
void backup(std::string file_name, std::string backup) {
	std::ifstream file(file_name);
	if (file.is_open()) {
		std::stringstream backup_file_name;
		backup_file_name << file_name<<backup;
		std::ifstream  src(file_name);
		std::ofstream  dst(backup_file_name.str().c_str());
		dst << src.rdbuf();
	}
}

/**
  * Check for presence of killfile
  */
 bool killed(std::string killfile) {
	std::ifstream file(killfile);
	bool result=file.is_open();
	if (result){
		std::cout << "Found killfile: " <<killfile<<std::endl;
		file.close();
		std::remove(killfile.c_str());
	}
	return result;
 }

 /**
  *  Sample points from surface of hypersphere
  *
  *  Use algorithm 1.22 from Werner Krauth, Statistical Mechanics: Algorithms and Computations,
  *  http://blancopeck.net/Statistics.pdf and http://www.oupcanada.com/catalog/9780198515364.html
  *
  */
std::vector<std::vector<double>> direct_surface(int d,int n){
	std::default_random_engine generator;
	std::normal_distribution<double> gaussian_distribution(0.0,1.0/std::sqrt(d));
	std::uniform_real_distribution<double> uniform_distribution(0.0,1.0);
	std::vector<std::vector<double>> samples;

	for (int i=0;i<n;i++){
		std::vector<double> x;
		double Sigma=0;
		for (int i=0;i<d;i++){
			const double xk=gaussian_distribution(generator);
			x.push_back(xk);
			Sigma+=sqr(xk);
		}
		for (std::vector<double>::iterator iter =x.begin();iter!=x.end();iter++)
			(*iter)/=std::sqrt(Sigma);

		samples.push_back(x);
	} 
	return samples;
}

 /**
  *  Sample points from hypersphere
  *
  *  Use algorithm 1.21 from Werner Krauth, Statistical Mechanics: Algorithms and Computations,
  *  http://blancopeck.net/Statistics.pdf and http://www.oupcanada.com/catalog/9780198515364.html
  *
  */
std::vector<std::vector<double>> direct_sphere(int d,int n){
  std::default_random_engine generator;
  std::normal_distribution<double> gaussian_distribution(0.0,1.0/std::sqrt(d));
  std::uniform_real_distribution<double> uniform_distribution(0.0,1.0);
  std::vector<std::vector<double>> samples;
  for (int i=0;i<n;i++){
	std::vector<double> x;
	double Sigma=0;

	for (int i=0;i<d;i++){
		const double xk=gaussian_distribution(generator);
		x.push_back(xk);
		Sigma+=sqr(xk);
	}

	const double upsilon=pow(uniform_distribution(generator),1.0/d);
	for (std::vector<double>::iterator iter =x.begin();iter!=x.end();iter++)
		(*iter)*=upsilon/std::sqrt(Sigma);

	samples.push_back(x);
  }

  return samples;
}

 /**
  *  Sample points a continuous distribution
  *
  *  Use algorithm 1.16 from Werner Krauth, Statistical Mechanics: Algorithms and Computations,
  *  http://blancopeck.net/Statistics.pdf and http://www.oupcanada.com/catalog/9780198515364.html
  *
  */
double reject_continuous(double (*pi)(double x),double x_min, double x_max, double pi_max) {
	std::default_random_engine generator;
	std::uniform_real_distribution<double> uniform_distribution_x(x_min,x_max);
	std::uniform_real_distribution<double> uniform_distribution_upsilon(0,pi_max);
	double x;
	bool not_found=true;
	while (not_found) {
		x=uniform_distribution_x(generator);
		const double upsilon=uniform_distribution_upsilon(generator);
		not_found=upsilon>pi(x);
	}
	return x;
}

 void remove_old_configs(std::string path) {
	std::stringstream command;
	command<<"exec rm -r " << path << "*";
	system(command.str().c_str());
}

bool ends_with(std::string const & value, std::string const & ending){
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

