/**
 * Copyright (C) 2018-2025 Greenweaves Software Limited
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
 * This program was written to investigate issue 9, https://github.com/weka511/galaxy/issues/9,
 * by looking at the executing time of a task that can be split over multiple threads.
 */
#include <iostream>
#include <limits>
#include <cmath>
#include <thread>
#include <time.h> 

using namespace std;

int foo(int from, int to) {
	for (int i=from;i< to;i++){
		double x=log(i);
		double y=exp(x);
	}
}

int main() {
	for (int k=0;k<10;k++)
		for (int i=0;i<15;i++) {
			int from    = 0;
			int to      = 0;
			const int N = numeric_limits<int>::max();
			
			auto start             = chrono::system_clock::now();
			time_t start_time = chrono::system_clock::to_time_t(start);
			
			if (i==0) {
				to=N;
				foo(1,to);
			} else {
				const int incr = N/i;
				from           = 0;
				to             = 0;
				thread* ts[i];			
				for (int j=0;j<i;j++){
					from = to;
					to   += incr;
					if (j==i-1) to = N;
					ts[j]=new thread(foo,from,to);
				}
				for (int j=0;j<i;j++){
					ts[j]->join();
					delete ts[j];
				}
			}
			auto end = chrono::system_clock::now();
			chrono::duration<double> elapsed_seconds = end-start;
				 
			cout<<i<<","<<  elapsed_seconds.count()<< endl;
		}
 
    cout << "done" << endl;
    return 0;
}
