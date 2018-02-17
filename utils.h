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
 
#ifndef _UTILS_H
#define _UTILS_H
#include <string>

 /**
  * Encode a floating value so it can be stored and retrieved without loss of significant digits
  */
 std::string encode(const double small);
 
 /**
  * Restore value stored by encode
  */
 double decode(std::string str);
 
 /**
  * Check for presence of killfile
  */
 bool killed(std::string killfile="kill");
 
 /**
 * If file exists, copy to backup
 */
 void backup(std::string file_name, std::string backup="~");
 
 /**
  *  Sample points from surface of hypersphere
  */
 std::vector<std::vector<double>> direct_surface(int d=3,int n=1);
 
 /**
  *  Sample points from hypersphere
  */
 std::vector<std::vector<double>> direct_sphere(int d=3,int n=1);
 
bool ends_with(std::string const & value, std::string const & ending);

void remove_old_configs(std::string path);
 
inline double sqr(double x) {return x*x;}

#endif
 