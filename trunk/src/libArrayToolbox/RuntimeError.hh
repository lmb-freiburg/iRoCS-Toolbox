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
 *  \file RuntimeError.hh
 *  \brief Exception specialization for error handling within libArrayToolbox.
 */
/*======================================================================*/

#ifndef ATBRUNTIMEERROR_HH
#define ATBRUNTIMEERROR_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <stdexcept>
#include <sstream>

namespace atb
{
  
/*======================================================================*/
/*!
 *  \class RuntimeError RuntimeError.hh "libArrayToolbox/RuntimeError.hh"
 *  \brief Objects of the RuntimeError class are thrown in
 *         case of errors while processing ArrayToolbox functions.
 */
/*======================================================================*/
  class RuntimeError : public std::exception
  {
    
  public:
    
/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   Creates a new RuntimeError object with specified error
 *   message string
 *
 *   \param message The error message explaining what went wrong
 */
/*======================================================================*/
    RuntimeError(char const *message = NULL);

/*======================================================================*/
/*! 
 *   Constructor.
 *
 *   Creates a new RuntimeError object with specified error
 *   message string
 *
 *   \param message The error message explaining what went wrong
 */
/*======================================================================*/
    RuntimeError(std::string const &message);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    virtual ~RuntimeError() throw();

/*======================================================================*/
/*! 
 *   Appends the given data to the message string.
 *
 *   \param data
 */
/*======================================================================*/
    template<typename DataT>
    RuntimeError &operator<<(DataT const &data);

/*======================================================================*/
/*! 
 *   Returns the error description.
 *
 *   \return The error description string.
 */
/*======================================================================*/
    virtual const char* what() const throw();

  protected:
    
    std::string _message;
    
  };
 
  template<typename DataT>
  RuntimeError &RuntimeError::operator<<(DataT const &data)
  {
    std::ostringstream os;
    os << data;
    _message += os.str();
    return *this;
  }

}

#endif
