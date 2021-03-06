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
**       Title: 
**    $RCSfile$
**   $Revision: 509 $$Name$
**       $Date: 2004-09-03 13:35:04 +0200 (Fri, 03 Sep 2004) $
**   Copyright: LGPL $Author: ronneber $
** Description:
**
**    some simple definitions and helper functions
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2004/09/03 11:35:03  ronneber
**  - replaced Chi-Jen Lin's own min,max,swap with std::min,
**    std::max. std::swap to compile with programs, that "#include
**    <algorithm>" and do a "using namespace std;"
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef SVM_DEFINES_HH
#define SVM_DEFINES_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <cstdio>
#include <cstdarg>
#include <cstring> // memcpy
#include <algorithm> // std::min, std::max, std::swap

// If user specifies unique ID's above this, libsvmtl will
// throw an error
const unsigned int MAX_BELIEVABLE_UNIQUE_ID = 100000000;  


typedef float Qfloat;
typedef signed char schar;
/*-------------------------------------------------------------------------
 *  replaced Chi-Jen-Lin's min, max and swap everywhere with std::min,
 *  std::max, std::swap to compile with programs, that "#include
 *  <algorithm>" and do "using namespace std;"
 *-------------------------------------------------------------------------*/
// #ifndef min
// template <class T> inline T min(T x,T y) { return (x<y)?x:y; }
// #endif
// #ifndef max
// template <class T> inline T max(T x,T y) { return (x>y)?x:y; }
// #endif
// template <class T> inline void swap(T& x, T& y) { T t=x; x=y; y=t; }
template <class S, class T> inline void clone(T*& dst, S* src, int n)
{
	dst = new T[n];
	std::memcpy((void *)dst,(void *)src,sizeof(T)*n);
}
#define INF HUGE_VAL
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
#if 1
inline void info(char *fmt,...)
{
	va_list ap;
	va_start(ap,fmt);
	vprintf(fmt,ap);
	va_end(ap);
}
inline void info_flush()
{
	fflush(stdout);
}
#else
inline void info(char *fmt,...) {}
inline void info_flush() {}
#endif

#endif
