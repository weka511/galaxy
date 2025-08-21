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
 #include <array>
 
 using namespace std;
 
 /**
  * This macro is used to record one entity.
  */
 #define LOG(s) Logger::get_instance()->log(__FILE__,__LINE__,s)
 
 /**
  * This macro is used to record two entities.
  */
 #define LOG2(s1,s2) Logger::get_instance()->log(__FILE__,__LINE__,s1,s2)
 
 /**
  * This macro is used to record the time that a statement was reached
  * if the metavaiable TIMER_ON is set.
  */
 #ifdef TIMER_ON
	#define TIME() Logger::get_instance()->time_point(__FILE__,__LINE__)
#else
	#define TIME() 
#endif

 /**
  *  This class is the logger for Galaxy. Only
  *  one instance is allowed.
  */
 class Logger {
	 
  private:
	/**
	 *  Unique instance of logger
	 */
	static unique_ptr<Logger> _instance;
	static string directory_path;
	static string _base;
	static string _path;
	ofstream _output;
	chrono::time_point<chrono::steady_clock> _start_time;
	
  public:
  
	static void set_paths( string base,  string path);
	
	/**
	 *   Accessor for unique instance
	 */
	static unique_ptr<Logger> & get_instance();
	
	Logger();
	
	virtual ~Logger();
	
	/**
	 *  This function is invoked by the LOG macro to log a single integer
	 */
	void log(string file, int line, int n);
	
	/**
	 *  This function is invoked by the LOG macro to log a 3 vector
	 */
	void log(string file, int line, array<double,3> v);
	
	/**
	 *  This function is invoked by the LOG macro to log a single string
	 */
	void log(string file, int line, string s);
	
	/**
	 *  This function is invoked by the LOG macro to log two strings
	 */
	void log(string file, int line, string s1, string s2);
	
	/**
	 *  This function is invoked by the TIME macro to record when 
	 *  something occurred
	 */
	void time_point(string file, int line);
	
  private:
  	static string _get_file_name();
	static string _to_str2(int n);
	chrono::duration<double> _get_milliseconds_since_start();	
 };
 

#endif //__LOGGER_HPP
