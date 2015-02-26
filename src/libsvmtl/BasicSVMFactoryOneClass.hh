/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 512 $$Name$
**       $Date: 2004-09-08 16:16:52 +0200 (Wed, 08 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2004/09/08 14:16:52  ronneber
**  - changed key for two class svm selection from "svm_type" to "two_class_type"
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/
#ifndef BASICSVMFACTORYONECLASS_HH
#define BASICSVMFACTORYONECLASS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "BasicFV.hh"
#include "TList.hh"
#include "BasicSVMAdapter.hh"

namespace svt
{
  template< typename FV, 
            typename STDATA, 
            typename OC_ITER, 
            typename KF_ITER>
  struct SVMFinderOneClass
  {
    static BasicSVMAdapter<FV,STDATA>* search( const std::string& ocName,
                                               const std::string& kfName)
        {

          /*-----------------------------------------------------------------
           *  extract the oneclass and kernelfunction
           *  types from the iterators. Because Oneclass types are
           *  templated, 'int' is taken as dummy  
           *  template argument
           *-----------------------------------------------------------------*/
          typedef typename OC_ITER::Traits<int>::val_t OCTYPE;
          typedef typename KF_ITER::val_t              KFTYPE;

          /*-----------------------------------------------------------------
           *  Check if current OneClass type has the correct name,
           *  otherwise instantiate SVMFinder with the next OneClass type
           *-----------------------------------------------------------------*/
          if( ocName != ""  && ocName != OCTYPE::name())
          {
            return SVMFinderOneClass<
                FV, 
                STDATA, 
                typename OC_ITER::next_t, 
                KF_ITER
                >::search( pcName, kfName);
          }
          /*-----------------------------------------------------------------
           *  Check if current Kernel type has the correct name,
           *  otherwise instantiate SVMFinder with the next Kernel type
           *-----------------------------------------------------------------*/
          else if( kfName != "" && kfName != KFTYPE::name())
          {
            return SVMFinder<
                FV, 
                STDATA, 
                OC_ITER, 
                typename KF_ITER::next_t
                >::search( ocName, kfName);
          }
          /*-----------------------------------------------------------------
           *  Ok, current  MultiClass type, TwoClass type and Kernel
           *  type have the correct name. Instatiate an
           *  BasicSVMAdapterTempl with the given types
           *-----------------------------------------------------------------*/
          else
          {
            typedef typename OC_ITER::Traits< 
                typename KF_ITER::val_t >::val_t  SVMTYPE;
            
            return new BasicSVMAdapterTempl< FV, STDATA, SVMTYPE>;
          }
        }
  };


  /*-------------------------------------------------------------------------
   *  partial specializations for SVMFinderOneClass, when it has reached the
   *  end of given List, without having found the requested name
   *-------------------------------------------------------------------------*/
  template< typename FV, 
            typename STDATA,
            typename X, 
            typename KF_ITER>
  struct SVMFinderOneClass<FV, STDATA, TTList<TTList_end,X>, KF_ITER>
  {
    static BasicSVMAdapter<FV,STDATA>* search( const std::string& ocName,
                                          const std::string& kfName)
        {
          UnknownClassNameError err;
          err << "One-Class type `" << tcName << "' does not exist!";
          throw err;
        }
  };
  
  template< typename FV, 
            typename STDATA,
            typename OC_ITER , 
            typename X>
  struct SVMFinderOneClass<FV, STDATA, OC_ITER, TList<TList_end,X> >
  {
    static BasicSVMAdapter<FV,STDATA>* search( const std::string& ocName,
                                          const std::string& kfName)
        {
          UnknownClassNameError err;
          err << "Kernel type `" << kfName << "' does not exist!";
          throw err;
        }
  };
  





  /*======================================================================*/
  /*!
   *  \class BasicSVMFactoryOneClass BasicSVMFactoryOneClass.hh
   *  \brief The BasicSVMFactoryOneClass class is a factory, that creates the
   *         appropriate template class from given strings
   *
   *  BasicSVMFactoryOneClass allows to do a runtime-creation of any
   *  Combination of One-Class and Kernel-Funktion types. For this the
   *  BasicSVMFactory needs the available One-Class type in OCLIST and
   *  the available Kernel functions int KFLIST. 
   *  To create these lists use MakeTTLIST for the SVM's and MakeTList
   *  for the kernels  
   *
   *  BasicSVMFactory< BasicFV, StDataASCII,
   *              MakeTTList< OneClassSVMPlane, SVMRegression>,
   *              MakeTList< Kernel_LINEAR, Kernel_RBF, Kernel_POLY> >
   */
  /*======================================================================*/
  template< typename FV, typename STDATA, 
            typename OCLIST, typename KFLIST>
  class BasicSVMFactoryOneClass
  {
  public:
    
    /*======================================================================*/
    /*! 
     *   create an BasicSVMAdapterTempl with the requested types. Each of the given 
     *   names must match one of the classes in OCLIST and KFLIST
     *
     *   \param ocName name of OneClass type (empty string means take 
     *                 first element from OCLIST)
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
    static BasicSVMAdapter<FV,STDATA>* create( const std::string& ocName,
                                          const std::string& kfName)
          {
            return SVMFinder< 
                FV, 
                STDATA, 
                typename OCLIST::the_list,
                typename KFLIST::the_list
                >::search( ocName, kfName);
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
    static BasicSVMAdapter<FV,STDATA>* createFromStData( const STDATA& stData)
          {
            std::string ocName;
            std::string kfName;
            stData.getValue( "two_class_type", ocName);
            stData.getValue( "kernel_type", kfName);
            
            return create( ocName, kfName);
          }


  };
  
}

#endif
