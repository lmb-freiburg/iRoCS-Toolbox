/***************************************************************************
 *   This source code is distributed under the license noted in the        *
 *   LICENSE file you should have received along with this source code.    *
 *   If you do not have received the LICENSE file, you can obtain a copy   *
 *   from http://www.xuvtools.org/license.                                 *
 *   The source code is copyright by its author(s). See the AUTHORS        *
 *   file for the list of authors.                                         *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 *                                                                         *
 *         Title:                                                          *
 *   Description:                                                          *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
#ifndef BASEENVIRONMENT_HH
#define BASEENVIRONMENT_HH BASEENVIRONMENT_HH

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
