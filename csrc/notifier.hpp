#ifndef _NOTIFIER_HPP
#define _NOTIFIER_HPP

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
 
 #include <string>
 
 
 using namespace std;
 
 /**
 * This class notifies the program should stop
 */
class Notifier {
  private:
   string _killfile;

  public:  
	Notifier(string killfile) : _killfile(killfile){;}  
	/**
	 *   Verify that program should continue executing,
	 *   i.e. killfile not present
	 */
    bool should_continue();
};
 
 #endif //_NOTIFIER_HPP