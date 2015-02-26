/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 476 $$Name$
**       $Date: 2004-08-26 10:36:59 +0200 (Thu, 26 Aug 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/
#ifndef MINIMALFEATUREVECTORSTREAM_HH
#define MINIMALFEATUREVECTORSTREAM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <iostream>
#include "RequireHelpers.hh"
#include "MinimalFeatureVector.hh"

namespace svt_check
{
  class MinimalFeatureVectorStream : public MinimalFeatureVector
  {
  public:
    MinimalFeatureVectorStream() {}
    
  private:
    MinimalFeatureVectorStream( const MinimalFeatureVectorStream& x) {}
    void operator=( const MinimalFeatureVectorStream& x) {}
    
  };

  inline
  std::ostream& operator<<( std::ostream& os, const MinimalFeatureVectorStream& f)
  {
    return os;
  }
  
  inline
  std::istream& operator>>(std::istream& is, MinimalFeatureVectorStream& fv)
  {
    return is;
  }
  
}


#endif
