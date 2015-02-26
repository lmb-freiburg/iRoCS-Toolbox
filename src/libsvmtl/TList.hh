/**************************************************************************
**       Title: template list classes
**    $RCSfile$
**   $Revision: 601 $$Name$
**       $Date: 2005-02-11 10:32:28 +0100 (Fri, 11 Feb 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2005/02/11 09:32:28  ronneber
**  - extended TLIST_.. macros until 16
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef TLIST_HH
#define TLIST_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

namespace svt
{
  /*-------------------------------------------------------------------------
   *  Template list for normal classes (e.g. int, Kernel_RBF, ...)
   *-------------------------------------------------------------------------*/
  template<typename T, typename Tnext> struct TList {
    typedef T val_t;
    typedef Tnext next_t;
  };
  
  struct TList_end {};

  /*-----------------------------------------------------------------------
   *  Convinience macros to create TList's
   *-----------------------------------------------------------------------*/
#define TLIST_1(T1)                                      svt::TList< T1, svt::TList_end >
#define TLIST_2(T1,T2)                                   svt::TList< T1, TLIST_1(T2) >
#define TLIST_3(T1,T2,T3)                                svt::TList< T1, TLIST_2(T2,T3) >
#define TLIST_4(T1,T2,T3,T4)                             svt::TList< T1, TLIST_3(T2,T3,T4) >
#define TLIST_5(T1,T2,T3,T4,T5)                          svt::TList< T1, TLIST_4(T2,T3,T4,T5) >
#define TLIST_6(T1,T2,T3,T4,T5,T6)                       svt::TList< T1, TLIST_5(T2,T3,T4,T5,T6) >
#define TLIST_7(T1,T2,T3,T4,T5,T6,T7)                    svt::TList< T1, TLIST_6(T2,T3,T4,T5,T6,T7) >
#define TLIST_8(T1,T2,T3,T4,T5,T6,T7,T8)                 svt::TList< T1, TLIST_7(T2,T3,T4,T5,T6,T7,T8) >
#define TLIST_9(T1,T2,T3,T4,T5,T6,T7,T8,T9)              svt::TList< T1, TLIST_8(T2,T3,T4,T5,T6,T7,T8,T9) >
#define TLIST_10(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)         svt::TList< T1, TLIST_9(T2,T3,T4,T5,T6,T7,T8,T9,T10) >
#define TLIST_11(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)     svt::TList< T1, TLIST_10(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) >
#define TLIST_12(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) svt::TList< T1, TLIST_11(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) >
#define TLIST_13(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13) svt::TList< T1, TLIST_12(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13) >
#define TLIST_14(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14) svt::TList< T1, TLIST_13(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14) >
#define TLIST_15(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15) svt::TList< T1, TLIST_14(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15) >
#define TLIST_16(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16) svt::TList< T1, TLIST_15(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16) >


//
//  template<typename T1=TList_end, 
//           typename T2=TList_end, 
//           typename T3=TList_end,
//           typename T4=TList_end,
//           typename T5=TList_end,
//           typename T6=TList_end,
//           typename T7=TList_end,
//           typename T8=TList_end,
//           typename T9=TList_end,
//           typename T10=TList_end>
//  struct MakeTList
//  {
//    typedef TList<
//        T1, TList<
//        T2, TList<
//        T3, TList<
//        T4, TList<
//        T5, TList<
//        T6, TList<
//        T7, TList<
//        T8, TList<
//        T9, TList<
//        T10, TList<TList_end, TList_end> > > > > > > > > > > the_list;
//  };
//
  /*-------------------------------------------------------------------------
   *  Template list for classes, that take 1 template parameter 
   *  (e.g. std::vector, TwoClassSVMc, MultiClassSVMOneVsOne, ...)
   *-------------------------------------------------------------------------*/
  template<template<typename> class T, typename Tnext> 
  struct TTList 
  {
    template<typename T2> 
    struct Traits
    { 
      typedef T<T2> val_t;
    };
    
    
    typedef Tnext next_t;
  };
  
//  template<typename DUMMY>
//  struct TTList_end {};

  /*-----------------------------------------------------------------------
   *  Convinience macros to create TTList's
   *-----------------------------------------------------------------------*/
#define TTLIST_1(T1)                                      svt::TTList< T1, svt::TList_end >
#define TTLIST_2(T1,T2)                                   svt::TTList< T1, TTLIST_1(T2) >
#define TTLIST_3(T1,T2,T3)                                svt::TTList< T1, TTLIST_2(T2,T3) >
#define TTLIST_4(T1,T2,T3,T4)                             svt::TTList< T1, TTLIST_3(T2,T3,T4) >
#define TTLIST_5(T1,T2,T3,T4,T5)                          svt::TTList< T1, TTLIST_4(T2,T3,T4,T5) >
#define TTLIST_6(T1,T2,T3,T4,T5,T6)                       svt::TTList< T1, TTLIST_5(T2,T3,T4,T5,T6) >
#define TTLIST_7(T1,T2,T3,T4,T5,T6,T7)                    svt::TTList< T1, TTLIST_6(T2,T3,T4,T5,T6,T7) >
#define TTLIST_8(T1,T2,T3,T4,T5,T6,T7,T8)                 svt::TTList< T1, TTLIST_7(T2,T3,T4,T5,T6,T7,T8) >
#define TTLIST_9(T1,T2,T3,T4,T5,T6,T7,T8,T9)              svt::TTList< T1, TTLIST_8(T2,T3,T4,T5,T6,T7,T8,T9) >
#define TTLIST_10(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)         svt::TTList< T1, TTLIST_9(T2,T3,T4,T5,T6,T7,T8,T9,T10) >
#define TTLIST_11(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)     svt::TTList< T1, TTLIST_10(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) >
#define TTLIST_12(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) svt::TTList< T1, TTLIST_11(T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) >

//  template<template<typename> class T1=TTList_end, 
//           template<typename> class T2=TTList_end, 
//           template<typename> class T3=TTList_end,
//           template<typename> class T4=TTList_end,
//           template<typename> class T5=TTList_end,
//           template<typename> class T6=TTList_end,
//           template<typename> class T7=TTList_end,
//           template<typename> class T8=TTList_end,
//           template<typename> class T9=TTList_end,
//           template<typename> class T10=TTList_end>
//  struct MakeTTList
//  {
//    typedef TTList<
//        T1, TTList<
//        T2, TTList<
//        T3, TTList<
//        T4, TTList<
//        T5, TTList<
//        T6, TTList<
//        T7, TTList<
//        T8, TTList<
//        T9, TTList<
//        T10, TTList<TTList_end, TList_end> > > > > > > > > > > the_list;
//  };
//

}

#endif
