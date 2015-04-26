/**************************************************************************
 *
 * Copyright (C) 2015 Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

#include "ATBTiming.hh"

#if _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include <sys/stat.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace atb
{

  std::string MyDateTime::prettyTime() 
  {
    std::stringstream prettyTimeStream;
    
#ifdef _WIN32
    LARGE_INTEGER frequency;
    LARGE_INTEGER timeval;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&timeval);
    long long time_us = 1000000 * timeval.QuadPart / frequency.QuadPart;
    prettyTimeStream << std::setfill('0') << std::setw(12)
                     << time_us / 1000000
                     << "." << std::setfill('0') << std::setw(6)
                     << time_us % 1000000;
#else
    static struct timeval currentTime;
    gettimeofday( &currentTime, NULL);
    prettyTimeStream << std::setfill('0') << std::setw(12)
                     << currentTime.tv_sec
                     << "." << std::setfill('0') << std::setw(6)
                     << currentTime.tv_usec;
#endif       
    return prettyTimeStream.str();
  }
    
  long long MyDateTime::time_us() 
  {
#ifdef _WIN32
    LARGE_INTEGER frequency;
    LARGE_INTEGER timeval;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&timeval);
    return static_cast<long long>(
        1000000 * timeval.QuadPart / frequency.QuadPart);
#else
    static struct timeval currentTime;
    gettimeofday( &currentTime, NULL);          
    return static_cast<long long>(currentTime.tv_sec) * 1000000 +
        currentTime.tv_usec;
#endif
  }
    
  std::string MyDateTime::prettyDate() 
  {
    time_t currentDate = time(NULL);
    std::string datestring = ctime(&currentDate);
    datestring = datestring.substr(0,datestring.length() - 1);
    return datestring;
  }
  
  std::string MyDateTime::prettyTime(long long time_us)
  {
    long long mus = time_us % 1000;
    time_us /= 1000;
    long long millis = time_us % 1000;
    time_us /= 1000;
    long long secs = time_us % 60;
    time_us /= 60;
    long long mins = time_us % 60;
    time_us /= 60;
    long long hours = time_us % 24;
    std::stringstream out;
    out << hours << ":"
        << std::setw(2) << std::setfill('0') << mins << ":"
        << std::setw(2) << std::setfill('0') << secs << "."
        << std::setw(3) << std::setfill('0') << millis << ","
        << std::setw(3) << std::setfill('0') << mus;
    return out.str();
  }

  Timer::Timer(std::ostream &outStream)
          : _startTime(0), _running(false), _outStream(outStream)
  {}
    
  Timer::~Timer()
  {}
    
  void Timer::tic(const std::string& output)
  {
    _outStream << "Starting watch-clock... " << output << std::endl;
    _startTime = MyDateTime::time_us();
    _running = true;
  }
    
  long long Timer::toc()
  {
    if (!_running) return 0;
    _running = false;
    long long elapsedTime = MyDateTime::time_us() - _startTime;
    _outStream << "Stopping watch-clock. Elapsed time = "
               << MyDateTime::prettyTime(elapsedTime) << std::endl;
    return elapsedTime;
  }
  
  Profiler::Profiler()
          : _startTime(0), _running(false)
  {}
    
  Profiler::~Profiler()
  {}
    
  void Profiler::tic()
  {
    _startTime = MyDateTime::time_us();
    _running = true;
  }
    
  long long Profiler::toc()
  {
    if (!_running) return 0;
    _running = false;
    return MyDateTime::time_us() - _startTime;
  }
  
}
