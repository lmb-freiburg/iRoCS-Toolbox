/**************************************************************************
 *
 * Copyright (C) 2005-2015 Olaf Ronneberger, Jörg Mechnich, Florian Pigorsch,
 *                         Mario Emmenlauer, Thorsten Schmidt
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
**       Title: basic exception
**    $RCSfile$
**   $Revision: 3319 $$Name$
**       $Date: 2010-01-23 17:10:43 +0100 (Sat, 23 Jan 2010) $
**   Copyright: GPL $Author: emmenlau $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2002/04/18 14:01:13  pigorsch
**  - added docu
**
**  Revision 1.1  2002/03/26 07:36:28  ronneber
**  restructuring for autoconf
**
**  Revision 1.1.1.1  2002/03/22 13:45:07  pigorsch
**  moved from polsoft repository
**
**
**
**************************************************************************/

#ifndef CMDLINEERROR_HH
#define CMDLINEERROR_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <sstream>
#include <string>

/*======================================================================*/
/*!
 *  \class CmdLineError CmdLineError.hh
 *  \brief The CmdLineError class is the parent class of all exceptions
 *  used in libCmdLine.
 *
 *  The CmdLineError class is the parent class of all exceptions
 *  used in libCmdLine. It has functions to access the error message.
 */
/*======================================================================*/
class CmdLineError
{
public:
  /*====================================================================*/
  /*! 
   *   Constructor.
   */
  /*====================================================================*/
  CmdLineError();

  /*====================================================================*/
  /*! 
   *   Copy constructor.
   *
   *   \param copy CmdLineError to copy
   */
  /*====================================================================*/      
  CmdLineError(const CmdLineError& copy);
    
  /*====================================================================*/
  /*! 
   *   Destructor.
   */
  /*====================================================================*/
  virtual ~CmdLineError();

  /*====================================================================*/
  /*! 
   *   Add something to the error message.
   *
   *   \param data 
   *
   *   \return modified CmdLineError
   */
  /*====================================================================*/
  template<typename T>
  CmdLineError& operator<<(const T& data)
  {
    std::ostringstream oss;
    oss << data;
    pMessage+=oss.str();
    return *this;
  }

  /*====================================================================*/
  /*! 
   *   Return error message
   *
   *   \return error message
   */
  /*====================================================================*/
  const std::string 
  str() const
  {
    return pMessage;
  }
  
protected:
  std::string pMessage;
};

#endif
