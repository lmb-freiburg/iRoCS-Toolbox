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
