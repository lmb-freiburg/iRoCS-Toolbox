/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 590 $$Name$
**       $Date: 2005-01-28 15:29:34 +0100 (Fri, 28 Jan 2005) $
**   Copyright: GPL $Author: fehr $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.4  2005/01/28 14:29:34  fehr
**  some more include bugfixes
**
**  Revision 1.3  2005/01/28 14:03:30  fehr
**  some include bugfixing for external library use
**
**  Revision 1.2  2004/09/03 09:13:04  ronneber
**  - adapted to new updateCache() interface of Kernels
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/
#ifndef REQUIRESVMKERNELUPDATECACHE_HH
#define REQUIRESVMKERNELUPDATECACHE_HH

#include "RequireHelpers.hh"
#include "../ProgressReporter.hh"
#include "../DirectAccessor.hh"
#include "../DereferencingAccessor.hh"

namespace svt_check
{
  
  /*======================================================================*/
  /*!
   *  Ensure that SVM provides a 
   *  kernel().updateCache( fv_begin, fv_end, progress) method
   */
  /*======================================================================*/
  template<typename SVM, typename FV>
  class RequireSVMKernelUpdateCache
  {
  public:
    ~RequireSVMKernelUpdateCache()
          {
            if( false)
            {
              SVM s;
              std::vector<FV> v;
              svt::ProgressReporter pr;
              s.kernel().updateCache( v.begin(), v.end(), 
                                      svt::DirectAccessor(), &pr);
              std::vector<FV*> v2;
              s.kernel().updateCache( v2.begin(), v2.end(), 
                                      svt::DereferencingAccessor(), &pr);
            }
          }
  };
}


#endif
