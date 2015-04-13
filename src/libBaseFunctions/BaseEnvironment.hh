/**************************************************************************
 *
 * This file is part of the XuV Tools suite. see http://www.xuvtools.org
 * for more information
 *
 * Copyright (C) 2015 Mario Emmenlauer
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

#ifndef BASEENVIRONMENT_HH
#define BASEENVIRONMENT_HH BASEENVIRONMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>


class BaseEnvironment
{
public:
  /**
   * UserHomeDirectory returns the path to the users home
   * directory.
   *
   * @return string the path to the user home directory
   */
  static std::string UserHomeDirectory();

  /**
   * TempDirectory returns the path to the users temporary files
   * directory. This can be user-specific, or globally unique for
   * the machine (depending on the OS).
   *
   * @return string the path to the user temp directory
   */
  static std::string UserTempDirectory();

  /**
   * UserAppDataDirectory returns the path to the users
   * application data directory. This can be zero, if the
   * operating system does not support the concept of application
   * data directories (depending on the OS).
   *
   * @return string the path to the application data directory
   */
  static std::string UserAppDataDirectory();

  /**
   * UserHomeDirectory returns the name of the host.
   *
   * @return string the name of the host.
   */
  static std::string HostName();

private:
#ifdef _WIN32
#endif

};

#endif
