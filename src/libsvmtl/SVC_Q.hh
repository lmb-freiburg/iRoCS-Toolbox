/**************************************************************************
**       Title: SVC kernel
**    $RCSfile$
**   $Revision: 4820 $$Name$
**       $Date: 2011-11-08 10:57:01 +0100 (Tue, 08 Nov 2011) $
**   Copyright: LGPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2004/09/03 11:35:04  ronneber
**  - replaced Chi-Jen Lin's own min,max,swap with std::min,
**    std::max. std::swap to compile with programs, that "#include
**    <algorithm>" and do a "using namespace std;"
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.2  2002/05/06 12:25:29  ronneber
**  - removed Parameter struct dependencies (now passes chache size directly)
**
**  Revision 1.1  2002/03/26 12:44:02  ronneber
**  restructured for autoconf
**
**  Revision 1.5  2002/03/13 13:41:53  pigorsch
**  * made parameter param of constructor const
**
**  Revision 1.4  2002/01/21 13:46:18  pigorsch
**  * removed parameter "params" from constructor of CachedKernel
**  * child class Parameters of class Model is now separate class
**
**  Revision 1.3  2002/01/14 14:02:45  pigorsch
**  * cosmetic code fixes
**
**  Revision 1.2  2001/12/17 13:18:30  pigorsch
**  *** empty log message ***
**
**  Revision 1.1  2001/12/11 11:03:00  pigorsch
**  Initial Revision
**
**
**
**************************************************************************/

#ifndef SVC_Q_HH
#define SVC_Q_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <algorithm>  

#include "SVM_Problem.hh"
#include "Kernel.hh"
#include "Cache.hh"

namespace svt
{
  //
  // Q matrices for various formulations
  //
  template< typename FV, typename KF>
  class SVC_Q: public Kernel<FV,KF>
  { 
  public:
    
    SVC_Q( const KF& kernel, const SVM_Problem<FV>& prob, float cacheSizeMB,
           const schar *y_)
            :Kernel<FV,KF>( kernel, prob.l, prob.x)
	{
		clone(y,y_,prob.l);
		cache = new Cache(prob.l,(int)(cacheSizeMB*(1<<20)));
	}
	
  private:
    SVC_Q( const SVC_Q<FV,KF>&); // forbid copying
  public:
    

	Qfloat *get_Q(int i, int len) const
	{
		Qfloat *data;
		int start;
		if((start = static_cast<int>(cache->get_data(i,&data,len))) <
                   len)
		{
                  for(int j = start; j < len; j++)
                      data[j] = static_cast<Qfloat>(
                          y[i] * y[j] * this->kernel_function(i,j));
		}
		return data;
	}

	void swap_index(int i, int j) const
	{
		cache->swap_index(i,j);
		Kernel<FV,KF>::swap_index(i,j);
		std::swap(y[i],y[j]);
	}

	~SVC_Q()
	{
		delete[] y;
		delete cache;
	}
private:
	schar *y;
	Cache *cache;
  };
}



//  old code from libsvmtl
//
  
//    class SVC_Q: public svt::CachedKernel<FV, KF>
//    {
//    public:
//      template< typename FVIter, typename YIter> 
//      SVC_Q(
//          const KF& kernel, 
//          int cacheSizeBytes,
//          FVIter FV_begin, const FVIter& FV_end, 
//          YIter Y_begin, const YIter& Y_end)
//            :svt::CachedKernel<FV, KF>(kernel, FV_begin, FV_end),
//             p_y(Y_begin, Y_end),
//             cache(p_x.size(), cacheSizeBytes)
//            {};
//    
//      float*
//      get_Q(
//          int i, 
//          int len) const
//            {
//              float* data;
//              int start;
//              if ((start=cache.get_data(i, &data, len))<len)
//              {
//                for (int j=start; j<len; j++)
//                {
//                  data[j]=(float)(p_y[i]*p_y[j]*(k_function)(i, j));
//                }
//              }
//              return data;
//            };
//    
//      void 
//      swap_index(
//          int i, 
//          int j)
//            {
//              cache.swap_index(i, j);
//              svt::CachedKernel<FV, KF>::swap_index(i, j);
//              std::swap(p_y[i], p_y[j]);
//            };
//      
//    private:
//      std::vector<signed char> p_y;
//      mutable svt::Cache cache; 
//    };
//  }
//  
#endif
