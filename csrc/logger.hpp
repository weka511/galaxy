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
 #include <chrono>
 
 using namespace std;
 
 #define LOG(s) Logger::get_instance()->log(__FILE__,__LINE__,s)
 
 #define LOG2(s1,s2) Logger::get_instance()->log(__FILE__,__LINE__,s1,s2)
 
 #ifdef TIMER_ON
	#define TIME() Logger::get_instance()->time_point(__FILE__,__LINE__)
#else
	#define TIME() 
#endif

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
	chrono::time_point<chrono::steady_clock> _start_time;
	
  public:
	static unique_ptr<Logger> & get_instance();
	Logger();
	virtual ~Logger();
	void log(string file, int line, string s);
	void log(string file, int line, string s1, string s2);
	void time_point(string file, int line);
	
  private:
  	static string _get_file_name();
	static string _to_str2(int n);
	chrono::duration<double> _get_milliseconds_since_start();	
 };
 

#endif //__LOGGER_HPP

