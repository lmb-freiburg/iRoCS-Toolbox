/**************************************************************************
 **    Title: BlitzAnalyzeError class
 **    $RCSfile$
 **    $Revision: $$Name$
 **    $Date: $
 **    Copyright: GPL $Author: $
 **    Description:
 **
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
class BlitzAnalyzeError
{
public:
  /*======================================================================*/
  /*!
   *   Default constructor. Creates an object without error message.
   *
   *   \sa BlitzAnalyzeError::operator<<
   */
  /*======================================================================*/
  BlitzAnalyzeError()
        {}

  /*======================================================================*/
  /*!
   *   Creates an error object with the given message.
   *
   *   \overload
   *
   *   \param message  error message
   */
  /*======================================================================*/
  BlitzAnalyzeError( const std::string& message)
          :_message( message)
        {}

  /*======================================================================*/
  /*!
   *   Appends the given data to the message string.
   *
   *   \param data
   */
  /*======================================================================*/
  template<class DataType>
  BlitzAnalyzeError & operator<<( const DataType &data)
        {
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
  std::string str() const
        {
          return _message;
        }

  const char* what() const
        {
          return _message.c_str();
        }

private:
  std::string _message;

};

#endif

