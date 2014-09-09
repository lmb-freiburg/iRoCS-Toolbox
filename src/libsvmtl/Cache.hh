/**************************************************************************
**       Title: Kernel cache
**    $RCSfile$
**   $Revision: 2825 $$Name$
**       $Date: 2009-09-15 17:04:15 +0200 (Tue, 15 Sep 2009) $
**   Copyright: LGPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2007/01/10 09:50:26  fehr
**  cache size bug sixed
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**  Revision 1.1  2002/03/26 12:44:02  ronneber
**  restructured for autoconf
**
**  Revision 1.3  2002/03/13 13:53:42  pigorsch
**  * put sources in Cache.cc
**  * added comments
**
**  Revision 1.2  2002/01/28 13:40:40  pigorsch
**  * fixed license
**
**  Revision 1.1  2001/12/11 11:03:00  pigorsch
**  Initial Revision
**
**
**
**************************************************************************/

#ifndef CACHE_HH
#define CACHE_HH

#include "svm_defines.hh"
#include "SVMError.hh"

namespace svt
{
  //
  // Kernel Cache
  //
  // l is the number of total data items
  // size is the cache size limit in bytes
  //
  class Cache
  {
  public:
	Cache(long l,long size);
	~Cache();
  private:
    // forbid copying of Cache
    Cache( const Cache&) {}
    void operator=( const Cache&) {}
  public:
    

	// request data [0,len)
	// return some position p where [p,len) need to be filled
	// (p >= len if nothing needs to be filled)
	long get_data(const long index, Qfloat **data, long len);
	void swap_index(long i, long j);	// future_option
  private:
	long l;
	long size;
	struct head_t
	{
		head_t *prev, *next;	// a cicular list
		Qfloat *data;
		long len;		// data[0,len) is cached in this entry
	};

	head_t* head;
	head_t lru_head;
	void lru_delete(head_t *h);
	void lru_insert(head_t *h);
  };
}

#endif
