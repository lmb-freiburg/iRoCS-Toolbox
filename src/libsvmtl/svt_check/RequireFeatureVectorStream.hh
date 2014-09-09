/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 538 $$Name$
**       $Date: 2004-09-13 12:04:04 +0200 (Mon, 13 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef REQUIREFEATUREVECTORSTREAM_HH
#define REQUIREFEATUREVECTORSTREAM_HH

#include "MinimalFeatureVector.hh"

namespace svt_check
{
  /*======================================================================*/
  /*!
   *  \class RequireFeatureVectorStream
   *  \ingroup assertions
   *  \brief The RequireFeatureVectorStream class ensures, that your
   *         veature vector class provides a input/output stream
   *
   *  \param FV your feature vector class
   */
  /*======================================================================*/
  template<  typename FV>
  class RequireFeatureVectorStream : public RequireFeatureVectorUniqueID
  {
  public:
    
    ~RequireFeatureVectorStream()
          {
            std::ostream& (*x1)(std::ostream&, const FV&) = 
                operator<<;
            std::istream& (*x2)(std::istream&, FV&) = 
                operator>>;
            avoidUnunsedVariableWarning( x1);
            avoidUnunsedVariableWarning( x2);
          }
  };
}

#endif
