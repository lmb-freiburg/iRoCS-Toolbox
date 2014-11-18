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

/*======================================================================*/
/*!
 *  \file RuntimeError.hh
 *  \brief Exception specialization for error handling within libArrayToolbox.
 */
/*======================================================================*/

#ifndef ATBRUNTIMEERROR_HH
#define ATBRUNTIMEERROR_HH

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
    RuntimeError(const char* message = NULL);

/*======================================================================*/
/*! 
 *   Destructor.
 */
/*======================================================================*/
    ~RuntimeError() throw();

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
    const char* what() const throw();

  private:
    
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
