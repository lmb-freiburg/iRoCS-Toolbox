/**************************************************************************
 **       Title: 
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
 **  Revision 1.2  2004/09/08 14:16:32  ronneber
 **  - changed key for two class svm selection from "svm_type" to "two_class_type"
 **
 **  Revision 1.1  2004/08/26 08:36:58  ronneber
 **  initital import
 **
 **  Revision 1.3  2003/05/19 11:12:04  ronneber
 **  - moved all code into templated BasicSVMFactory class, to be able to
 **    use it with other and own FV classes
 **
 **  Revision 1.2  2002/09/05 13:16:44  pigorsch
 **  *** empty log message ***
 **
 **  Revision 1.1  2002/09/04 10:29:57  pigorsch
 **  - initial revision, copied from libpollenpp and modified
 **
 **
 **
 **************************************************************************/

#ifndef BASICSVMFACTORY_HH
#define BASICSVMFACTORY_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "TList.hh"
#include "BasicSVMAdapter.hh"
#include "BasicSVMAdapterTempl.hh"

#include "MC_SVM_Finder.hh"


namespace svt
{

  template< typename FV, typename STDATA>
  struct BasicSVMProducer
  { 
    BasicSVMAdapter<FV,STDATA>* svm;
    
    template<typename SVMTYPE>
    void produce( SVMTYPE*)
          {
            svm = new BasicSVMAdapterTempl<FV,STDATA,SVMTYPE>;
          }
  };
  
  

  /*======================================================================*/
  /*!
   *  \class BasicSVMFactory BasicSVMFactory.hh
   *  \brief The BasicSVMFactory class is a factory, that creates the
   *         appropriate template class from given strings
   *
   *  BasicSVMFactory allows to do a runtime-creation of any Combination of
   *  Multi-Class, Two-Class and Kernel-Funktion types. For this the
   *  BasicSVMFactory needs the available Multi-Class types in MCLIST, the
   *  available Two-Class type in TCLIST and the available Kernel
   *  functions int KFLIST. To create these lists use MakeTTLIST for
   *  the SVM's and MakeTList for the kernels 
   *
   *  BasicSVMFactory< BasicFV, StDataASCII,
   *              MakeTTList< MultiClassSVMOneVsOne, MultiClassSVMOneVsRest>,
   *              MakeTTList< TwoClassSVMc, TwoClassSVMnu>,
   *              MakeTList< Kernel_LINEAR, Kernel_RBF, Kernel_POLY> >
   */
  /*======================================================================*/
  template< typename FV, typename STDATA, 
            typename MCLIST, typename TCLIST, typename KFLIST>
  class BasicSVMFactory
  {
  public:
    
    /*======================================================================*/
    /*! 
     *   create an BasicSVMAdapterTempl with the requested types. Each of the given 
     *   names must match one of the classes in MCLIST, TCLIST and KFLIST
     *
     *   \param mcName name of MultiClass type (empty string means take 
     *                 first element from MCLIST)
     *   \param tcName name of TwoClass type (empty string means take 
     *                 first element from TCLIST)
     *   \param kfName name of Kernel function type (empty string means take 
     *                 first element from KFLIST)
     *
     *   \exception UnknownClassNameError one of the requested classes
     *              could not be found
     *
     *   \return pointer to created BasicSVMAdapterTempl object downcasted to
     *           BasicSVMAdapter*. You are responsible to delete the object
     *           after usage.
     */
    /*======================================================================*/
    static BasicSVMAdapter<FV,STDATA>* create( const std::string& mcName,
                                               const std::string& tcName,
                                               const std::string& kfName)
          {
            BasicSVMProducer<FV,STDATA> p = MC_SVM_Finder< 
                BasicSVMProducer<FV,STDATA>, 
                MCLIST,
                TCLIST,
                KFLIST
                >::search( mcName, tcName, kfName);

            return p.svm;            
          }



    /*======================================================================*/
    /*! 
     *   (description)
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    template< typename STDATA2>
    static BasicSVMAdapter<FV,STDATA>* createFromStData( STDATA2& stData)
          {
            std::string mcName;
            std::string tcName;
            std::string kfName;
            stData.getValue( "multi_class_type", mcName);
            stData.getValue( "two_class_type", tcName);
            stData.getValue( "kernel_type", kfName);
            
            return create( mcName, tcName, kfName);
          }


  };
  
}

#endif
