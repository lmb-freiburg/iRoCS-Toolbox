/**************************************************************************
**       Title: compilers for various datatypes
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
**  Revision 1.2  2008/04/06 13:00:16  emmenlau
**   - cast strtod() to float, to avoid compiler warnings
**
**  Revision 1.1  2002-04-11 08:42:01  ronneber
**  - moved Compilers.hh to Compiler.cc (template specializations must be in a .cc file!)
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

#ifndef COMPILERS_HH
#define COMPILERS_HH

#include <cctype>
#include <cstdlib>
#include <iostream>

#include "Compiler.hh"

template<>
bool
Compiler<int>::compile(
    const char* arg,
    const char*& endptr,
    int& value)
{
  if (arg==0) // null string - nothing to do
  {
    return true;
  }
  else if (*arg==0) // empty string
  {
    setErrorMsg("empty integer value specified");
    return false;
  }

  // compile the string into an integer
  char* ptr;
  long result=strtol(arg, &ptr, 0);

  if (ptr==arg) // first char is invalid
  {
    setErrorMsg("invalid integer value \""+std::string(arg)+"\"");
    return false;
  }

  endptr=ptr;
  value=(int)result;
  return true;
}

template<>
bool
Compiler<float>::compile(
    const char* arg,
    const char*& endptr,
    float& value)
{
  if (arg==0) // null string - nothing to do
  {
    return true;
  }
  else if (*arg==0) // empty string
  {
    setErrorMsg("empty integer value specified");
    return false;
  }

  // compile the string into an integer
  char* ptr;
  float result=(float)strtod(arg, &ptr);

  if (ptr==arg) // first char is invalid
  {
    setErrorMsg("invalid float value \""+std::string(arg)+"\"");
    return false;
  }

  endptr=ptr;
  value=result;
  return true;
}


template<>
bool
Compiler<double>::compile(
    const char* arg,
    const char*& endptr,
    double& value)
{
  if (arg==0) // null string - nothing to do
  {
    return true;
  }
  else if (*arg==0) // empty string
  {
    setErrorMsg("empty integer value specified");
    return false;
  }

  // compile the string into an integer
  char* ptr;
  double result=strtod(arg, &ptr);

  if (ptr==arg) // first char is invalid
  {
    setErrorMsg("invalid double value \""+std::string(arg)+"\"");
    return false;
  }

  endptr=ptr;
  value=result;
  return true;
}

template<>
bool
Compiler<std::string>::compile(
    const char* arg,
    const char*& endptr,
    std::string& value)
{
  if (arg!=0)
  {
    endptr=0;
    value=arg;
  }

  return true;
}

template<>
bool
Compiler<bool>::compile(
    const char* arg,
    const char*& endptr,
    bool& value)
{
  if (arg==0) // no argument -> true
  {
    value=true;
    return true;
  }
  else if (*arg==0) // empty string -> true
  {
    value=true;
    return true;
  }

  std::string s(arg);
  for (std::string::iterator p=s.begin();
       p!=s.end();
       ++p)
  {
    *p=static_cast<char>(tolower(*p));
  }

  if (s=="1" || s=="on" || s=="true")
  {
    endptr=0;
    value=true;
    return true;
  }
  else if (s=="0" || s=="off" || s=="false")
  {
    endptr=0;
    value=false;
    return true;
  }

  return false;
}

#endif
