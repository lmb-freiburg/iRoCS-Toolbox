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
**  Revision 1.2  2005/11/07 22:18:37  mechnich
**  changes for g++ 4
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/


#ifndef MC_SVM_FINDER_HH
#define MC_SVM_FINDER_HH




namespace svt
{

  /*======================================================================*/
/*!
 *  \class MC_SVM_Finder MC_SVM_Finder.hh
 *  \brief The MC_SVM_Finder class finds the appropriate combination
 *  of Multiclass, Twoclass and Kernel specified by strings
 *
 */
/*======================================================================*/


  template< typename PRODUCER, 
            typename MC_ITER, 
            typename TC_ITER, 
            typename KF_ITER>
  struct MC_SVM_Finder
  {
    static PRODUCER search( const std::string& mcName,
                          const std::string& tcName,
                          const std::string& kfName)
        {

          /*-----------------------------------------------------------------
           *  extract the multiclass, twoclass and kernelfunction
           *  types from the iterators. Because Multiclass and
           *  Twoclass types are templated, 'int' is taken as dummy
           *  template argument
           *-----------------------------------------------------------------*/
          typedef typename MC_ITER::template Traits<int>::val_t MCTYPE;
          typedef typename TC_ITER::template Traits<int>::val_t TCTYPE;
          typedef typename KF_ITER::val_t              KFTYPE;

         /*-----------------------------------------------------------------
           *  Check if current MultiClass type has the correct name,
           *  otherwise instantiate MC_SVM_Finder with the next MultiClass type
           *-----------------------------------------------------------------*/
        
          if( mcName != "" && mcName != MCTYPE::name())
          {
            return MC_SVM_Finder<
                PRODUCER,
                typename MC_ITER::next_t, 
                TC_ITER, 
                KF_ITER
                >::search( mcName, tcName, kfName);
          }
          /*-----------------------------------------------------------------
           *  Check if current TwoClass type has the correct name,
           *  otherwise instantiate MC_SVM_Finder with the next TwoClass type
           *-----------------------------------------------------------------*/
          else if( tcName != ""  && tcName != TCTYPE::name())
          {
            return MC_SVM_Finder<
                PRODUCER, 
                MC_ITER, 
                typename TC_ITER::next_t, 
                KF_ITER
                >::search( mcName, tcName, kfName);
          }
          /*-----------------------------------------------------------------
           *  Check if current Kernel type has the correct name,
           *  otherwise instantiate MC_SVM_Finder with the next Kernel type
           *-----------------------------------------------------------------*/
          else if( kfName != "" && kfName != KFTYPE::name())
          {
            return MC_SVM_Finder<
                PRODUCER,
                MC_ITER, 
                TC_ITER, 
                typename KF_ITER::next_t
                >::search( mcName, tcName, kfName);
          }
          /*-----------------------------------------------------------------
           *  Ok, current  MultiClass type, TwoClass type and Kernel
           *  type have the correct name. Instatiate an
           *  BasicCVAdapterTempl with the given types
           *-----------------------------------------------------------------*/
          else
          {
            typedef typename MC_ITER::template Traits< 
                typename TC_ITER::template Traits< 
                typename KF_ITER::val_t >::val_t >::val_t  SVMTYPE;

            PRODUCER p;
            // ugly workaround, because p.produce<SVMTYPE>() does not work...
            SVMTYPE* s=0;
            p.produce( s);
            
            return p;
          }
        }
  };


  /*-------------------------------------------------------------------------
   *  partial specializations for MC_SVM_Finder, when it has reached the
   *  end of given List, without having found the requested name
   *-------------------------------------------------------------------------*/
  template< typename PRODUCER,
            typename TC_ITER, 
            typename KF_ITER>
  struct MC_SVM_Finder<PRODUCER, TList_end, TC_ITER, KF_ITER>
  {
    static PRODUCER search( const std::string& mcName,
                            const std::string&,
                            const std::string&)
        {
          UnknownClassNameError err;
          err << "Multi-Class type `" << mcName << "' does not exist!";
          throw err;
        }
  };
  

  template< typename PRODUCER,
            typename MC_ITER,
            typename KF_ITER>
  struct MC_SVM_Finder<PRODUCER, MC_ITER, TList_end, KF_ITER>
  {
    static PRODUCER search( const std::string&,
                            const std::string& tcName,
                            const std::string&)
        {
          UnknownClassNameError err;
          err << "Two-Class type `" << tcName << "' does not exist!";
          throw err;
        }
  };
  
  template< typename PRODUCER,
            typename MC_ITER,
            typename TC_ITER>
  struct MC_SVM_Finder<PRODUCER, MC_ITER, TC_ITER, TList_end >
  {
    static PRODUCER search( const std::string&,
                            const std::string&,
                            const std::string& kfName)
        {
          UnknownClassNameError err;
          err << "Kernel type `" << kfName << "' does not exist!";
          throw err;
        }
  };
}

#endif
