/**************************************************************************
 *
 * Copyright (C) 2004-2015 Olaf Ronneberger, Florian Pigorsch, Jörg Mechnich,
 *                         Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

/**************************************************************************
**       Title: Kernel cache
**    $RCSfile$
**   $Revision: 1035 $$Name$
**       $Date: 2007-01-10 10:50:27 +0100 (Wed, 10 Jan 2007) $
**   Copyright: GPL $Author: fehr $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2007/01/10 09:50:26  fehr
**  cache size bug sixed
**
**  Revision 1.2  2004/09/03 11:35:04  ronneber
**  - replaced Chi-Jen Lin's own min,max,swap with std::min,
**    std::max. std::swap to compile with programs, that "#include
**    <algorithm>" and do a "using namespace std;"
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**  Revision 1.1  2002/04/18 14:52:22  ronneber
**  imported sources
**
**  Revision 1.1  2002/03/13 13:53:58  pigorsch
**  * initial revision
**
**
**
**************************************************************************/

#include "Cache.hh"

#include <algorithm>
#include <cstdlib>

#include <iostream>

svt::Cache::Cache(long l_,long size_):l(l_),size(size_)
{
	head = (head_t *)calloc(l,sizeof(head_t));	// initialized to 0
	size /= sizeof(Qfloat);
	size -= l * sizeof(head_t) / sizeof(Qfloat);
	lru_head.next = lru_head.prev = &lru_head;

	if (size < 0)
	{
	    CacheToSmall err;
	    err << "Cache is to small! Size: "<< size_ << " but "<< l * sizeof(head_t)  << " bytes needed. use -cs flag to set cache size.\n";
	    throw err;
	}
}


svt::Cache::~Cache()
{
	for(head_t *h = lru_head.next; h != &lru_head; h=h->next)
		free(h->data);
	free(head);
}

void svt::Cache::lru_delete(head_t *h)
{
	// delete from current location
	h->prev->next = h->next;
	h->next->prev = h->prev;
}

void svt::Cache::lru_insert(head_t *h)
{
	// insert to last position
	h->next = &lru_head;
	h->prev = lru_head.prev;
	h->prev->next = h;
	h->next->prev = h;
}

long svt::Cache::get_data(const long index, Qfloat **data, long len)
{
	head_t *h = &head[index];
	if(h->len) lru_delete(h);
	long more = len - h->len;

	if(more > 0)
	{
		// free old space
		while(size < more)
		{
			head_t *old = lru_head.next;
			lru_delete(old);
			free(old->data);
			size += old->len;
			old->data = 0;
			old->len = 0;
		}

		// allocate new space
		h->data = (Qfloat *)realloc(h->data,sizeof(Qfloat)*len);
		size -= more;
		std::swap(h->len,len);
	}

	lru_insert(h);
	*data = h->data;
	return len;
}

void svt::Cache::swap_index(long i, long j)
{
	if(i==j) return;

	if(head[i].len) lru_delete(&head[i]);
	if(head[j].len) lru_delete(&head[j]);
	std::swap(head[i].data,head[j].data);
	std::swap(head[i].len,head[j].len);
	if(head[i].len) lru_insert(&head[i]);
	if(head[j].len) lru_insert(&head[j]);

	if(i>j) std::swap(i,j);
	for(head_t *h = lru_head.next; h!=&lru_head; h=h->next)
	{
		if(h->len > i)
		{
			if(h->len > j)
				std::swap(h->data[i],h->data[j]);
			else
			{
				// give up
				lru_delete(h);
				free(h->data);
				size += h->len;
				h->data = 0;
				h->len = 0;
			}
		}
	}
}

