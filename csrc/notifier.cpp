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
 */
 
#include <iostream>
#include <fstream>
#include "notifier.hpp"

using namespace std;

/**
 *   Verify that program should continue executing,
 *   i.e. killfile not present
 */
bool Notifier::should_continue() {
	ifstream file(_killfile);
	if (file.is_open()) {
		cout << "Found killfile: " <<_killfile<<endl;
		file.close();
		remove(_killfile.c_str());
		return false;
	} else
		return true;
}
