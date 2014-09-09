/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 476 $$Name$
**       $Date: 2004-08-26 10:36:59 +0200 (Thu, 26 Aug 2004) $
**   Copyright: LGPL $Author: ronneber $
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

#ifndef ONE_CLASS_Q_HH
#define ONE_CLASS_Q_HH

#include "SVM_Problem.hh"
#include "Kernel.hh"
#include "Cache.hh"

namespace svt
{
  template< typename FV, typename KF>
  class ONE_CLASS_Q: public Kernel<FV,KF>
  {
  public:
    ONE_CLASS_Q( const KF& kernel, const SVM_Problem<FV,KF>& prob)
            :Kernel( kernel, prob.l, prob.x)
          {
		cache = new Cache(prob.l,(int)(param.cache_size*(1<<20)));
          }
	
	Qfloat *get_Q(int i, int len) const
	{
		Qfloat *data;
		int start;
		if((start = cache->get_data(i,&data,len)) < len)
		{
			for(int j=start;j<len;j++)
				data[j] = (Qfloat)kernel_function(i,j);
		}
		return data;
	}

	void swap_index(int i, int j) const
	{
		cache->swap_index(i,j);
		Kernel::swap_index(i,j);
	}

	~ONE_CLASS_Q()
	{
		delete cache;
	}
private:
	Cache *cache;
  };
}

#endif
