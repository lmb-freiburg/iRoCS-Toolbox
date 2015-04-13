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
**       Title: two class C-SVM
**    $RCSfile$
**   $Revision: 2824 $$Name$
**       $Date: 2009-09-14 09:30:46 +0200 (Mon, 14 Sep 2009) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2004/09/08 14:40:16  ronneber
**  - changed key for two class svm selection from "svm_type" to "two_class_type"
**  - adapted to new ParamInfo class
**
**  Revision 1.2  2004/09/01 14:43:36  ronneber
**  changed IterToPointerTraits stuff to
**  DirectAccessor and DereferencingAccessor, to make code more
**  intuitive understandable
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.8  2003/05/19 11:27:01  ronneber
**  - converted from MapTools to ParamMapWrapper
**
**  Revision 1.7  2002/09/05 13:10:54  pigorsch
**  - modified to use new MapTools
**
**  Revision 1.6  2002/06/07 11:41:17  ronneber
**  - made ModelType in train() an own template parameter to make
**    it more flexible (e.g. when using a slightly different but
**    compatible Feature Vector class)
**
**  - added two additional interfaces for train() method
**
**  - removed obsolete train_one() and solve_c_svc() method
**
**  Revision 1.5  2002/06/05 17:18:31  mechnich
**  Made minor corrections for compilation under Win32
**
**  Revision 1.4  2002/05/21 18:31:31  ronneber
**  - now all parameters can be queried
**
**  Revision 1.3  2002/05/10 11:07:03  ronneber
**  - removed FV template for all public classes, because Feature Vector type
**    can be extracted automatically from passed iterators or other
**    parameters -- this makes the public interface much more intuitive
**
**  Revision 1.2  2002/05/06 12:28:17  ronneber
**  - removed Parameters struct and added setCost() and setWeightOfPositiveClass() instead
**
**  Revision 1.1  2002/03/26 12:44:02  ronneber
**  restructured for autoconf
**
**  Revision 1.1  2002/03/13 14:07:21  pigorsch
**  * initial revision
**
**
**
**************************************************************************/

#ifndef TWOCLASSSVMC_HH
#define TWOCLASSSVMC_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// std includes
#include <map>
#include <set>
#include <string>

// libsvmtl includes

#include "TwoClassSVM.hh"
#include "SVC_Q.hh"
#include "Solver.hh"
#include "SVM_Problem.hh"
#include "SolutionInfo.hh"
#include "StDataASCII.hh"
#include "DirectAccessor.hh"

// requirements of template parameters
#include "svt_check/RequireKernel_k_function.hh"
#include "svt_check/RequireStData.hh"


namespace svt
{
  /*-------------------------------------------------------------------------
   *  default values for C-SVM (same as original libsvm)
   *-------------------------------------------------------------------------*/
  static const double COST_DEFAULT = 1.0;
  static const double POSITIVE_CLASS_WEIGHT_DEFAULT = 1.0;

  /*======================================================================*/
  /*!
   *  \class TwoClassSVMc TwoClassSVMc.hh
   *  \brief The TwoClassSVMc class (is/provides/specifies/contains)
   *
   *  (description)
   */
  /*======================================================================*/
  template< typename KF>
  class TwoClassSVMc: public TwoClassSVM< KF>
  {
  public:
    template< typename FV>
    struct Traits
    {
      typedef Model<FV> ModelType;
    };
    
    

    /*====================================================================*/
    /*!
     *   (description)
     */
    /*====================================================================*/
    TwoClassSVMc()
            :TwoClassSVM<KF>(),
             _cost( COST_DEFAULT),
             _positiveClassWeight( POSITIVE_CLASS_WEIGHT_DEFAULT)
          {}



    /*====================================================================*/
    /*!
     *   (description)
     *
     *   \param
     */
    /*====================================================================*/
    TwoClassSVMc(const KF& kernel)
            :TwoClassSVM<KF>(kernel),
             _cost( COST_DEFAULT),
             _positiveClassWeight( POSITIVE_CLASS_WEIGHT_DEFAULT)
          {}


    /*====================================================================*/
    /*!
     *   (description)
     */
    /*====================================================================*/
    ~TwoClassSVMc()
          {}
    

    /*====================================================================*/
    /*! 
     *   train SVM with given Feature Vectors.  (raw interface)
     *
     *   this is the basic method, all others will call this one
     *
     *   \param problem  Structure with pointers to feature-vectors
     *   \param model   (output) model containing pointers to feature
     *                  vectors, that became support vectors, alphas
     *                  and rho   
     */
    /*====================================================================*/
    template<typename FV>
    void train( const SVM_Problem<FV>& problem,
                Model<FV>& model) const;

    /*====================================================================*/
    /*!
     *   train (STL-like interface)
     *   Your feature vector class must provide a method double
     *   getLabel(), which returns +1 or -1. 
     *   The ModelType is svt::Model<FV>. Positive and
     *   negative labled feature vectors may be in any order in the
     *   given Container.
     *
     *   \param FV_begin  iterator pointing to first element of the
     *                    container with the feature vectors
     *   \param FV_end    iterator pointing one past the last element of the
     *                    container with the feature vectors
     *   \param model   (output) model containing pointers to feature
     *                  vectors, that became support vectors, alphas
     *                  and rho   
     */
    /*====================================================================*/
    template<typename ForwardIter>
    void train( ForwardIter FV_begin, const ForwardIter& FV_end,
                svt::Model<typename std::iterator_traits<ForwardIter>::value_type>& model) const
          {
            train( FV_begin, FV_end, model, DirectAccessor());
          }


    /*====================================================================*/
    /*!
     *   train (STL-like interface with additional Accessor)
     *
     *   same as train(), but you can specify an Accessor
     *   for accessing the elements of your container. This accessor
     *   must return a reference to the corresponding feature
     *   vector. Use DirectAccessor for containers like
     *   std::vector<FV> and DereferencingAccessor for containers like
     *   std::vector<FV*> 
     *
     *   \param FV_begin  iterator pointing to first element of the
     *                    container with the feature vectors
     *   \param FV_end    iterator pointing one past the last element of the
     *                    container with the feature vectors
     *   \param model   (output) model containing pointers to feature
     *                  vectors, that became support vectors, alphas
     *                  and rho   
     *   \param accessor functor, which converts an iterator
     *                    of your container into a pointer to a
     *                    feature vector 
     */
    /*====================================================================*/
    template<typename ForwardIter, typename Accessor>
    void train( ForwardIter FV_begin, const ForwardIter& FV_end,
                svt::Model<typename Accessor::template Traits<ForwardIter>::value_type>& model,
                Accessor accessor) const;



    /*======================================================================*/
    /*!
     *   set the parameter C for C-SVM (cost for outliers). Default: 1.0
     *
     *   \param c  parameter C
     *
     */
    /*======================================================================*/
    void setCost( double c)
          {
            _cost = c;
          }

    double cost() const
          {
            return _cost;
          }


    /*======================================================================*/
    /*!
     *   set the parameter C of positive class (labeled with +1) to weight*C.
     *   Default: 1.0
     *
     *   \param weight new weight
     *
     */
    /*======================================================================*/
    void setWeightOfPositiveClass( double weight)
          {
            _positiveClassWeight = weight;
          }

    double weightOfPositiveClass() const
          {
            return _positiveClassWeight;
          }


    /*======================================================================*/
    /*!
     *   Load and save parameters cost and weight from map
     */
    /*======================================================================*/
    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);

            TwoClassSVM<KF>::loadParameters(stData);
            stData.getValue( "cost", _cost);
            stData.getValue( "weight", _positiveClassWeight);
          }
    
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);

            TwoClassSVM<KF>::saveParameters( stData);
            stData.setValue( "two_class_type", name());
            stData.setValue( "cost", _cost);
            stData.setValue( "weight", _positiveClassWeight);
          }

    static const char* name()
          {
            return "c_svc";
          }

    static const char* description()
          {
            return "Two class SVM using C-SVC algorithm for training";
          }

    /*======================================================================*/
    /*! 
     *   get information about the parameters, that are used in
     *   loadParameters() and saveParameters(). The Infos are appended
     *   to the passed array
     *
     *   \param p  array of ParamInfos. The Infos will be appended to
     *             this array
     */
    /*======================================================================*/
    static void getParamInfos( std::vector<ParamInfo>& p)
          {
            p.push_back(
                ParamInfo( "cost", "c", "value", 
                           "penalty cost for wrong training vectors "
                           "in C-SVC. (default 1)"));
            p.push_back(
                ParamInfo( "weight", "w", "value",
                           "weight for positive class samples in "
                           "two-class C-SVC. (default 1)"));
            
            TwoClassSVM< KF>::getParamInfos( p);
            
          }

  protected:
    template< typename FV>
    void solve_c_svc( const SVM_Problem<FV> *prob,
                      double *alpha, SolutionInfo* si,
                      Model<FV>& model) const;
    

  private:
    double _cost;                 // parameter 'C': Cost for outliers
    double _positiveClassWeight;  // weight for positive class


  };

}

#include "TwoClassSVMc.icc"

#endif
