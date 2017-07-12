/**************************************************************************
 *
 * Copyright (C) 2010 Thorsten Falk, Mario Emmenlauer
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

/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 1384 $$Name$
**       $Date: 2007-11-28 14:02:45 +0100 (Wed, 28 Nov 2007) $
**   Copyright: GPL $Author: fehr $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2007/11/28 13:02:45  fehr
**  Error class added, bugfix: shape=0 -> is_continous=false
**
**
**
**************************************************************************/

#ifndef BLITZFFTWERROR_HH
#define BLITZFFTWERROR_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <sstream>
#include <iostream>
#include <exception>

#ifdef _WIN32
#ifdef BlitzFFTW_EXPORTS
#define BLITZFFTWDLL_API __declspec(dllexport)
#else
#define BLITZFFTWDLL_API __declspec(dllimport)
#endif
#else
#define BLITZFFTWDLL_API
#endif

class BLITZFFTWDLL_API BlitzFFTWError : public std::exception
{
public:
  BlitzFFTWError() throw()
        {}
  
  BlitzFFTWError( const std::string& message) throw()
          :_message( message)
        {}

  virtual ~BlitzFFTWError() throw()
        {}
  
  template<class DataType>
  BlitzFFTWError & operator<<( const DataType &data) throw()
        {
          std::ostringstream os;
          os << data;
          _message += os.str();
          return *this;
        }

  std::string str() const throw()
        {
          return _message;
        }

  const char* what() const throw()
        {
          return _message.c_str();
        }
  
  
private:
  std::string _message;
  
};

#endif
