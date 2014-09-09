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

#include <string>
#include <sstream>
#include <iostream>
#include <exception>

class BlitzFFTWError : public std::exception
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
