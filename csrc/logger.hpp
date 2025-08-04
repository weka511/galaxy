#ifndef __LOGGER_HPP
#define __LOGGER_HPP

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
 
 #include <memory>
 #include <fstream>
 
 using namespace std;
 
 #define LOG(s) Logger::get_instance()->log(__FILE__,__LINE__,s)
 
 /**
  *  This class is the logger for galaxy.
  */
 class Logger {
	 
  private:
	static unique_ptr<Logger> _instance;
	static string directory_path;
	static string _base;
	static string _path;
	ofstream _output;
	
  public:
	static unique_ptr<Logger> & get_instance();
	static string get_file_name();
	static string to_str2(int n);
	Logger();
	virtual ~Logger();
	void log(string file, int line, string s);
  	
 };
 

#endif //__LOGGER_HPP

