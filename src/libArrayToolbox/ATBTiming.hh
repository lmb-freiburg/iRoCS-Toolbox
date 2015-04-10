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

/*======================================================================*/
/*!
 *  \file ATBTiming.hh
 *  \brief ATBTiming.hh provides the MyDateTime and Timer classes
 *    for high accuracy profiling
 */
/*======================================================================*/

#ifndef ATBTIMING_HH
#define ATBTIMING_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <iostream>

namespace atb
{

/*======================================================================*/
/*!
 *  \class MyDateTime <>
 *  \brief The MyDateTime struct provides pretty printed date - time
 *         strings with microsecond resolution
 *
 *  This struct provides pretty printed versions of the current system
 *  time in two formats: microseconds since system start, current date
 *  and time in the following format: Weekday Month Day Time Year
 */
/*======================================================================*/
  struct MyDateTime 
  {

/*======================================================================*/
/*! 
 *   Timestamp relative to system boot in microsecond accuracy of the form
 *   xxxxxxxxxxxx.xxxxxx (s.mus). The timestamps are pre-padded with zeros,
 *   to make them string-sortable.
 *
 *   \return timestamp string in microseconds
 */
/*======================================================================*/
    static std::string prettyTime();
    
/*======================================================================*/
/*! 
 *   Get the elapsed microseconds since system boot
 *
 *   \return microseconds since system boot
 */
/*======================================================================*/
    static long long time_us();
    
/*======================================================================*/
/*! 
 *   Get a pretty printed version of the current system date and time
 *
 *   \return A pretty printed date-time string
 */
/*======================================================================*/
    static std::string prettyDate();

/*======================================================================*/
/*! 
 *   Get a pretty printed version of the given time in microseconds as
 *   hh:mm:ss.mmm,uuu
 *
 *   \return A pretty printed time string to the given microseconds
 */
/*======================================================================*/
    static std::string prettyTime(long long time_us);

  };
  

/*======================================================================*/
/*!
 *  \class Timer <>
 *  \brief The Timer class provides a simple stop-watch in microsecond
 *         accuracy for simple profiling tasks (similar to tic - toc in
 *         MATLAB)
 *
 *  To use the stop-watch create a Timer object. Call tic(), to start the
 *  watch and toc() to stop it. Both functions will send a message to
 *  std::cout.
 */
/*======================================================================*/
  class Timer 
  {
    
  public:
    
/*======================================================================*/
/*! 
 *   Constructor - creating a new Timer
 *
 *   \param outStream The output of the Tmier will be sent to the
 *      output stream specified
 */
/*======================================================================*/ 
    Timer(std::ostream &outStream = std::cout);
    
/*======================================================================*/
/*! 
 *   Destructor - Cleaning up Timer ressources
 */
/*======================================================================*/ 
    ~Timer();
    
/*======================================================================*/
/*! 
 *   Start the timer showing the optional message output
 *
 *   \param output   A message that is shown additional to the default
 *                   "Starting stopwatch..." output
 */
/*======================================================================*/
    void tic(const std::string& output = "");
    
/*======================================================================*/
/*! 
 *   Stop the timer showing the elapsed time since the last timer start
 *   pretty printed in microsecond accuracy
 *
 *   Output format is hh:mm:ss.ms,mu
 *
 *   return The time in microseconds since the last tic() call
 */
/*======================================================================*/
    long long toc();

  private:
    
    long long _startTime;
    bool _running;
    std::ostream& _outStream;
    
  };
  
/*======================================================================*/
/*!
 *  \class Profiler <>
 *  \brief The Profiler class provides a simple stop-watch in microsecond
 *         accuracy for simple profiling tasks (similar to tic - toc in
 *         MATLAB). It produces no output, so it can be used for averaging
 *         over many fast operations.
 *
 *  To use the stop-watch create a Profiler object. Call tic(), to start the
 *  watch and toc() to stop it. There will be no output.
 */
/*======================================================================*/
  class Profiler
  {
    
  public:
    
/*======================================================================*/
/*! 
 *   Constructor - creating a new Profiler
 */
/*======================================================================*/ 
    Profiler();
    
/*======================================================================*/
/*! 
 *   Destructor - Cleaning up Profiler ressources
 */
/*======================================================================*/ 
    ~Profiler();
    
/*======================================================================*/
/*! 
 *   Start the profiler
 */
/*======================================================================*/
    void tic();
    
/*======================================================================*/
/*! 
 *   Stop the profiler
 *
 *   return The time in microseconds since the last tic() call
 */
/*======================================================================*/
    long long toc();

  private:
    
    long long _startTime;
    bool _running;
    
  };
  
}

#endif
