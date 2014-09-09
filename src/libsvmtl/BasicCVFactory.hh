/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 2825 $$Name$
**       $Date: 2009-09-15 17:04:15 +0200 (Tue, 15 Sep 2009) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2004/09/08 14:14:46  ronneber
**  - changed key for two class svm selection from "svm_type" to "two_class_type"
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef BASICCVFACTORY_HH
#define BASICCVFACTORY_HH

#include "TList.hh"
#include "BasicCVAdapter.hh"
#include "BasicCVAdapterTempl.hh"
#include "MC_SVM_Finder.hh"

namespace svt
{





  template< typename FV, typename PROBLEM, typename STDATA>
  struct BasicCVProducer
  { 
    BasicCVAdapter<FV,PROBLEM,STDATA>* cv;
    
    template<typename SVMTYPE>
    void produce( SVMTYPE*)
          {
            cv = new BasicCVAdapterTempl<FV,PROBLEM, STDATA,SVMTYPE>;
          }
  };
  
  



  /*======================================================================*/
  /*!
   *  \class BasicCVFactory BasicCVFactory.hh
   *  \brief The BasicCVFactory class is a factory, that creates the
   *         appropriate template class from given strings
   *
   *  BasicCVFactory allows to do a runtime-creation of any Combination of
   *  Multi-Class, Two-Class and Kernel-Funktion types. For this the
   *  BasicCVFactory needs the available Multi-Class types in MCLIST, the
   *  available Two-Class type in TCLIST and the available Kernel
   *  functions int KFLIST. To create these lists use MakeTTLIST for
   *  the SVM's and MakeTList for the kernels 
   *
   *  BasicCVFactory< BasicFV, StDataASCII,
   *              MakeTTList< MultiClassSVMOneVsOne, MultiClassSVMOneVsRest>,
   *              MakeTTList< TwoClassSVMc, TwoClassSVMnu>,
   *              MakeTList< Kernel_LINEAR, Kernel_RBF, Kernel_POLY> >
   */
  /*======================================================================*/
  template< typename FV, typename PROBLEM, typename STDATA, 
            typename MCLIST, typename TCLIST, typename KFLIST>
  class BasicCVFactory
  {
  public:
    
    /*======================================================================*/
    /*! 
     *   create an BasicCVAdapterTempl with the requested types. Each of the given 
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
     *   \return pointer to created BasicCVAdapterTempl object downcasted to
     *           BasicCVAdapter*. You are responsible to delete the object
     *           after usage.
     */
    /*======================================================================*/
    static BasicCVAdapter<FV,PROBLEM,STDATA>* create( 
        const std::string& mcName,
        const std::string& tcName,
        const std::string& kfName)
          {
            BasicCVProducer<FV,PROBLEM,STDATA> p = MC_SVM_Finder< 
                BasicCVProducer<FV,PROBLEM,STDATA>, 
                MCLIST,
                TCLIST,
                KFLIST
                >::search( mcName, tcName, kfName);

            return p.cv;            
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
    static BasicCVAdapter<FV,PROBLEM,STDATA>* createFromStData( STDATA2& stData)
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
