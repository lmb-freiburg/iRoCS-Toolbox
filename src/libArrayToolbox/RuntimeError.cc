/**************************************************************************
 **       Title: General Exception for ArrayToolbox
 **    $RCSfile:$
 **   $Revision:$ $Name:$
 **       $Date:$
 **   Copyright: GPL $Author:$
 ** Description:
 **
 **   Exceptions of this type are thrown, when the user tries to do nasty
 **   things with the ArrayToolbox functions which have no defined behaviour.
 **
 **************************************************************************/

#include "RuntimeError.hh"

namespace atb
{
  
  RuntimeError::RuntimeError(const char* message)
          : _message()
  {
    if (message != NULL) _message = message; 
  }
  
  RuntimeError::~RuntimeError() throw()
  {}
  
  const char* RuntimeError::what() const throw()
  {
    return _message.c_str();
  }

}
