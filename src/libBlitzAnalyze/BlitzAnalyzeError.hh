/**************************************************************************
 *
 * Copyright (C) 2010 Robert Bensch
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

#ifndef ANALYZEERROR_HH
#define ANALYZEERROR_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <sstream>
#include <iostream>

  /*======================================================================*/
  /*!
   *  \class BlitzAnalyzeError BlitzAnalyzeError.hh
   *  \brief Error class for BlitzAnalyzeFile
   */
   /*======================================================================*/
class BlitzAnalyzeError {
public:
  /*======================================================================*/
  /*!
   *   Default constructor. Creates an object without error message.
   *
   *   \sa BlitzAnalyzeError::operator<<
   */
   /*======================================================================*/
  BlitzAnalyzeError() {}

  /*======================================================================*/
  /*!
   *   Creates an error object with the given message.
   *
   *   \overload
   *
   *   \param message  error message
   */
   /*======================================================================*/
  BlitzAnalyzeError(const std::string& message)
    :_message(message) {}

  /*======================================================================*/
  /*!
   *   Appends the given data to the message string.
   *
   *   \param data
   */
   /*======================================================================*/
  template<class DataType>
  BlitzAnalyzeError & operator<<(const DataType &data) {
    std::ostringstream os;
    os << data;
    _message += os.str();
    return *this;
  }

  /*======================================================================*/
  /*!
   *   Returns the error message.
   *
   *   \return error message
   */
   /*======================================================================*/
  std::string str() const {
    return _message;
  }

  const char* what() const {
    return _message.c_str();
  }

private:
  std::string _message;

};

#endif
