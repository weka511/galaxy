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
#include <iostream>
#include <limits>
#include <cmath>
#include <thread>
#include <time.h> 

int N=std::numeric_limits<int>::max();

int foo() {
	for (int i=1;i< N;i++){
		double x=std::log(i);
		double y=std::exp(x);
	}
}

int main() {
	for (int i=0;i<10;i++) {

		auto start = std::chrono::system_clock::now();
		std::time_t start_time = std::chrono::system_clock::to_time_t(start);
		if (i==0)
			foo();
		else {
			N=std::numeric_limits<int>::max()/i;
			std::thread* ts[i];
			for (int j=0;j<i;j++)
				ts[j]=new std::thread(foo);
			for (int j=0;j<i;j++){
				ts[j]->join();
				delete ts[j];
			}
		}
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-start;
			 
		std::cout<<i<<","<< i*N<<"," << elapsed_seconds.count()<< std::endl;
	}
 
    std::cout << "done" << std::endl;
    return 0;
}
