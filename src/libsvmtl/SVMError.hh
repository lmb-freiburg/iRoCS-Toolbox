/**************************************************************************
**       Title: simple exception class
**    $RCSfile$
**   $Revision: 2827 $$Name$
**       $Date: 2009-09-16 11:49:43 +0200 (Wed, 16 Sep 2009) $
**   Copyright: LGPL $Author: ronneber $
** Description:
**
**
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.5  2007/01/10 09:50:27  fehr
**  cache size bug sixed
**
**  Revision 1.4  2006/10/06 13:50:05  fehr
**  linear model optimizer added
**
**  Revision 1.3  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.2  2004/09/01 14:43:36  ronneber
**  changed IterToPointerTraits stuff to
**  DirectAccessor and DereferencingAccessor, to make code more
**  intuitive understandable
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.3  2003/05/19 11:11:27  ronneber
**  - now inherits from std::exception
**  - therfore changed str() method to what()
**
**  Revision 1.2  2003/03/06 14:18:53  ronneber
**  - moved some local Error classes to this file
**
**  Revision 1.1  2002/09/04 10:27:24  pigorsch
**  - initial revision
**
**
**
**************************************************************************/

#ifndef SVMERROR_HH
#define SVMERROR_HH

// std includes
#include <iostream>
#include <sstream>
#include <string>
#include <exception>

namespace svt
{
  /*======================================================================*/
  /*!
   *  \class SVMError
   *  \brief The SVMError class is the parent class for all errors
   *         that are thrown by the LIBSVMTL 
   */
  /*======================================================================*/
  class SVMError : public std::exception
  {
  public:
    SVMError()
          {}

    SVMError(const SVMError& copy)
            :std::exception( copy),
             pMessage(copy.pMessage)
             
          {}

    virtual ~SVMError() throw()
          {}
    
    
    
    template<typename T>
    SVMError& operator<<(const T& data)
          {
            std::ostringstream oss;
            oss << data;
            pMessage+=oss.str();
            return *this;
          }
    
    virtual const char* what() const throw()
          {
            return pMessage.c_str();
          }
    
  protected:
    std::string pMessage;
  };


  /*-----------------------------------------------------------------------
   *  General Error Classes
   *-----------------------------------------------------------------------*/
  class WrongParameterError : public SVMError {};
  class LoadError : public SVMError {};
  class SaveError : public SVMError {};

  /*-----------------------------------------------------------------------
   *  Error classes for ParamMapWrapper
   *-----------------------------------------------------------------------*/
  class KeyNotFoundError : public SVMError {};
  class InvalidDataTypeError : public SVMError {};
  class ContainerTooSmallError : public SVMError {};
  class NotEnoughValuesError : public SVMError {};
  
  /*-----------------------------------------------------------------------
   *  Error classes for SVMFactroy
   *-----------------------------------------------------------------------*/
  class UnknownClassNameError : public SVMError {};
  class SVMRuntimeError: public SVMError {};


  /*-----------------------------------------------------------------------
   *  Error classes for StData
   *-----------------------------------------------------------------------*/
  class ParseCmdLineError : public SVMError {};
  class CmdLineError : public SVMError {};
  
  /*-----------------------------------------------------------------------
   *  Error classes for GridAxis
   *-----------------------------------------------------------------------*/
  class ParseError  : public SVMError {};
  class GridSearchError : public SVMError {};
 
   /*-----------------------------------------------------------------------
    *  Error classes for SparseFV
    *  --------------------------------------------------------------------*/
  class SparseError : public SVMError {};

  /*-----------------------------------------------------------------------
   *  Assertions
   *-----------------------------------------------------------------------*/
  class SVMAssertionFailed: public SVMError {};


    /*-----------------------------------------------------------------
     * Cache to small Error
     * --------------------------------------------------------------*/
  class CacheToSmall : public SVMError {};  

#define SVM_ASSERT( condition)                                          \
if (!(condition))                                                       \
{                                                                       \
  svt::SVMAssertionFailed err;                                          \
  err << __FILE__ << ":" << __LINE__ << ": " "assertion '"              \
      << (#condition) << "' failed";                                   \
  std::cerr  << std::endl << err.what() << std::endl;                   \
  throw( err);                                                         \
}                                                                       \



}

#endif
