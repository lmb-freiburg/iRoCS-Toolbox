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
**       Title: basic compiler class
**    $RCSfile$
**   $Revision: 16 $$Name$
**       $Date: 2002-03-26 08:36:28 +0100 (Tue, 26 Mar 2002) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2002/03/26 07:36:28  ronneber
**  restructuring for autoconf
**
**  Revision 1.1.1.1  2002/03/22 13:45:07  pigorsch
**  moved from polsoft repository
**
**
**
**************************************************************************/

#ifndef COMPILER_HH
#define COMPILER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>

/*======================================================================*/
/*!
 *  \class Compiler Compiler.hh
 *  \brief The Compiler class is used by CmdArg classes to convert
 *  strings into desired datatypes.
 */
/*======================================================================*/
template<typename T>
class Compiler
{
public:
  
  /*====================================================================*/
  /*! 
   *   Convert the given string into the desired datatype T. If the
   *   compilation was not successful, false is returned and the error
   *   message is set. If endptr!=0 the function sets endptr to the first
   *   unrecognized char in arg or to 0 if there are no unrecognized chars.
   *   NOTE: This function must always successfully accept NULL strings.
   *
   *   \param arg    the string that is to be converted
   *   \param endptr must point to first unrecognized char in arg
   *   \param value  here the converted value is stored
   *
   *   \return true, if the compilation was successful
   */
  /*====================================================================*/
  bool 
  compile(
      const char* arg,
      const char*& enptr,
      T& value);

  
  /*====================================================================*/
  /*! 
   *   Return the error message of the last compilation.
   *
   *   \return error message
   */
  /*====================================================================*/  
  const std::string& 
  errorMsg() const
        {
          return pErrorMsg;
        };


protected:

  /*====================================================================*/
  /*! 
   *   Set the error message.
   *
   *   \param msg error message
   */
  /*====================================================================*/  
  void
  setErrorMsg(const std::string& msg)
        {
          pErrorMsg=msg;
        };
  
  
  std::string pErrorMsg;
};

#endif
