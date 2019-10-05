/**
 * Copyright (C) 2019 Greenweaves Software Limited
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
 * This file provides support for serialization. 
 * https://stackoverflow.com/questions/55421144/conversion-from-hexadecimal-string-to-double-in-c
 */
 
#ifndef _SERIALIZATION_H
#define _SERIALIZATION_H

#include <iostream>

using namespace std;

double deserialize(const std::string& hexstr) {
	double d = 0.0;

    try{
        *reinterpret_cast<unsigned long long*>(&d) = std::stoull(hexstr, nullptr, 16);
    }
    catch(...){}

    return d;
}
#endif
