/**************************************************************************
**       Title: simple linear kernel class for MyFeatureVector
**    $RCSfile$
**   $Revision: 679 $$Name$
**       $Date: 2005-03-29 20:11:23 +0200 (Tue, 29 Mar 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2005/03/29 18:11:23  ronneber
**  - added updateCache() and clearCache() dummy methods
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.3  2003/05/19 11:33:57  ronneber
**  - converted from MapTools to ParamMapWrapper
**
**  Revision 1.2  2003/02/10 07:36:50  ronneber
**  - added dummy loadParameters and saveParameters
**
**  Revision 1.1  2002/05/06 13:47:29  ronneber
**  initial revision
**
**
**
**************************************************************************/

#ifndef MYKERNEL_HH
#define MYKERNEL_HH
#include <libsvmtl/ProgressReporter.hh>


class MyKernel
{
public:
    template< typename ForwardIter, typename Accessor>
    void updateCache( const ForwardIter& fvBegin,  
                      const ForwardIter& fvEnd,
                      Accessor accessor,
                      svt::ProgressReporter* pr = 0) const
          {
            //  nothing to do here
          }

    void clearCache() const
          {
            //  nothing to do here
          }

  double k_function( const MyFeatureVector& fv1, 
                     const MyFeatureVector& fv2) const
        {
          return (fv1.x() * fv2.x() + 
                  fv1.y() * fv2.y());
        }

  template<typename STDATA>
  void loadParameters( STDATA& stData)
        {
        }
  
  template<typename STDATA>
  void saveParameters( STDATA& stData) const
        {
        }
  
};

#endif
