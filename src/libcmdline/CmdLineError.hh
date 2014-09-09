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
