/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 2825 $$Name$
**       $Date: 2009-09-15 17:04:15 +0200 (Tue, 15 Sep 2009) $
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

#ifndef MINIMALFEATUREVECTOR_HH
#define MINIMALFEATUREVECTOR_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

namespace svt_check
{
  class MinimalFeatureVector
  {
  public:
    MinimalFeatureVector() {}
    
  private:
    MinimalFeatureVector( const MinimalFeatureVector&) {}
    void operator=( const MinimalFeatureVector&) {}
    
  };
}


#endif
