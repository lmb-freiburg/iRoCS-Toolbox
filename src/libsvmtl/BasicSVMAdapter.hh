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
 **   $Revision: 907 $$Name$
 **       $Date: 2006-10-06 15:50:05 +0200 (Fri, 06 Oct 2006) $
 **   Copyright: LGPL $Author: fehr $
 ** Description:
 **
 **    
 **
 **-------------------------------------------------------------------------
 **
 **  $Log$
 **  Revision 1.12  2006/10/06 13:50:05  fehr
 **  linear model optimizer added
 **
 **  Revision 1.11  2006/01/25 08:53:51  fehr
 **  added HistIntersect to Kmatrix Kernels and new BasicSVMAdapter interface to TwoClassModels
 **
 **  Revision 1.10  2005/10/26 06:57:05  ronneber
 **  - added set/getStoreClassificationDetailsFlag()
 **  - instead of classify() that returns a double detailedResultsVec, now
 **    the details can be requested after classify() with the more general
 **    saveClassificationDetailsASCII()
 **
 **  Revision 1.9  2005/07/19 13:03:59  haasdonk
 **  removed redundant cout-messages, erroneous filename and added a new function
 **  for computing a training-kernel matrix in BasicSVMAdapter*
 **
 **  Revision 1.8  2005/06/06 21:23:31  haasdonk
 **  added updateCache() with two FV-lists, required for classification with precomputed kernel-matrices
 **
 **  Revision 1.7  2005/03/29 17:50:29  ronneber
 **  - added updateKernelCache() and clearKernelCache()
 **
 **  Revision 1.6  2005/02/24 16:58:17  fehr
 **  sometimes... it's better to go home: FINAL bugfix
 **
 **  Revision 1.5  2005/02/24 13:45:49  fehr
 **  enable SV access through SVMAdapter
 **
 **  Revision 1.4  2005/02/24 12:57:50  fehr
 **  some more 64-bit netcdf bugfixing
 **
 **  Revision 1.3  2004/09/08 14:15:07  ronneber
 **  - added saveOnlyKernelParameters()
 **
 **  Revision 1.2  2004/09/01 14:43:36  ronneber
 **  changed IterToPointerTraits stuff to
 **  DirectAccessor and DereferencingAccessor, to make code more
 **  intuitive understandable
 **
 **  Revision 1.1  2004/08/26 08:36:58  ronneber
 **  initital import
 **
 **  Revision 1.3  2003/05/19 11:09:03  ronneber
 **  - moved all code into templated BasicBasicSVMAdapter class, to be able to
 **    use it with other and own FV classes
 **
 **  Revision 1.2  2002/09/05 13:04:15  pigorsch
 **  - added typedef MCFV
 **
 **  Revision 1.1  2002/09/04 10:29:57  pigorsch
 **  - initial revision, copied from libpollenpp and modified
 **
 **
 **
 **************************************************************************/

#ifndef BASICSVMADAPTER_HH
#define BASICSVMADAPTER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// std includes
#include <map>
#include <string>
#include <vector>

// libsvmtl includes
#include "GroupedTrainingData.hh"
#include "SVM_Problem.hh"
#include "SVMError.hh"
#include "BasicFV.hh"
#include "StDataASCIIFile.hh"
#include "StDataCmdLine.hh"
#include "ProgressReporter.hh"
#include "DirectAccessor.hh"
#include "Model.hh"

namespace svt
{

  template<typename FV, typename STDATA = StDataASCIIFile>
  class BasicSVMAdapter
  {
  public:

    BasicSVMAdapter()
            :_storeClassificationDetailsFlag( false)
          {}
    
    
    virtual ~BasicSVMAdapter()
          {}
    
    /*======================================================================*/
    /*! 
     *   set progress reporter object. 0 means no progress
     *   reporting. You are still the owner of this object,
     *   so you are responsible, that it exists during further calls
     *   to train(), etc. and that it is deleted afterwards
     *
     *   \param pr pointer to ProgressReporter object or 0 if no
     *             progress reposting is desired 
     */
    /*======================================================================*/
    virtual void setProgressReporter( ProgressReporter* pr) = 0;
    


    /*======================================================================*/
    /*!  
     *   Read all parameters (e.g. cost, gamma, etc) for the 
     *   SVM from given structured data (e.g. an StDataASCII
     *   object). For non-existing keys the 
     *   parameters remain unchanged, e.g. if key "cost" does not
     *   exist in the given map, the cost of the TwoClassSVMc will not
     *   be changed
     *
     *   \param  stData  structured data containing parameters
     *                   as key/value pairs  
     *
     *   \exception . depending on your STDATA class
     */
    /*======================================================================*/
    virtual void loadParameters( STDATA& stData) = 0;

    virtual void loadParameters( StDataASCII& stData) = 0;


    /*======================================================================*/
    /*! 
     *   Same as loadParameters, but for Parameters from command
     *   line. The StDataCmdLine keeps track of unused parameters
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    virtual void loadParameters( StDataCmdLine& stData) = 0;
    


    /*======================================================================*/
    /*! 
     *   load model data from stData into SVM's. See
     *   documentation of this method in inherited classes for
     *   details. 
     *
     *   \param stData  structured data containing parameters
     *                  as key/value pairs  
     *
     *   \exception . depending on your STDATA class
     */
    /*======================================================================*/
    virtual void loadModel(  STDATA& stData) = 0;
    

    /*======================================================================*/
    /*! 
     *   store all parameters of the SVM to given map
     *
     *   \param stData  (output) structured data containing parameters
     *                  as key/value pairs  
     *
     */
    /*======================================================================*/
   virtual void saveParameters( STDATA& stData) const = 0;


    /*======================================================================*/
    /*! 
     *   same as saveParameters(), but with fixed StDataASCII class
     *   independent from given STDATA template parameter. 
     */
    /*======================================================================*/
   virtual void saveParameters( StDataASCII& stData) const = 0;
    
    /*======================================================================*/
    /*! 
     *   save only Kernel Parameters (this is used for user information and
     *   to detect in grid search, wether a kernel parameter is changed
     */
    /*======================================================================*/
   virtual void saveOnlyKernelParameters( StDataASCII& stData) const = 0;
    
    /*======================================================================*/
    /*! 
     *   save resulting model from training process into given map
     *
     *   \param stData  (output) structured data containing parameters
     *                  as key/value pairs  
    *
     */
    /*======================================================================*/
    virtual void saveModel( STDATA& stData) const = 0;

    /*======================================================================*/
    /*! 
     *   save additional trainnig infos to given structured data
     *
     *   \param stData      (output) structured data containing parameters
     *                      as key/value pairs 
     *   \param detailLevel 0: nothing, 1: only statistics, 2: training infos 
     *                      from all two-class trainings. Default 1
     *
     */
    /*======================================================================*/
    virtual void saveTrainingInfos( STDATA& stData,
                                    int detailLevel = 1) = 0;
    
    /*======================================================================*/
    /*! 
     *   same as saveTrainingInfos(), but with fixed StDataASCII class
     *   independent from given STDATA template parameter. 
     */
    /*======================================================================*/
    virtual void saveTrainingInfos( StDataASCII& stData,
                                    int detailLevel = 1) = 0;
 

    /*======================================================================*/
    /*! 
     *   call updateCache() of selected kernel, e.g., for Kernel_MATRIX
     *   this will setup the internal cache matrix. For pure
     *   Kernel_LINEAR or similar, this is an empty operation, so you
     *   can call it for every Kernel without negative effects.
     *   Before calling, please ensure that your feature vectors have
     *   correct uniqueId()'s 
     *
     *   \param trainData Structure with pointers to feature-vectors
     *
     */
    /*======================================================================*/
    virtual void updateKernelCache( const GroupedTrainingData<FV>& trainData) = 0;


    /*======================================================================*/
    /*! 
     *   same as previous updateKernelCache() method, just for feature
     *   vectors that are stored within an SVM_Problem object
     *   Before calling, please ensure that your feature vectors have
     *   correct uniqueId()'s 
     *
     *   \param  problem  Structure with pointers to feature-vectors
     */
    /*======================================================================*/
    virtual void updateKernelCache( const SVM_Problem<FV>& problem) = 0;
    

    /*======================================================================*/
    /*! 
     *   Load Test Kernel Cache. 
     *
     *   \param FV_begin begin of test-feature-vectors. 
     *   \param FV_end end of test-feature-vectors. 
     *
     */
    /*==============================x========================================*/
    virtual void updateTestKernelCache(typename 
                                       std::vector<FV>::iterator FV_begin,
                                       const typename 
                                       std::vector<FV>::iterator& FV_end) = 0; 


    /*======================================================================*/
    /*! 
     *   train SVM with given training data. (multi class interface)
     *   this is the basic method, all others will call this one. The
     *   The resulting model is stored internally and can be accessd
     *   via saveModel(), or used for classify()
     *
     *   \param trainData Structure with pointers to feature-vectors
     *                    sorted by classes, the class start indizes
     *                    and a classIndexToLabel translastion table
     *
     */
    /*======================================================================*/
    virtual void train( const GroupedTrainingData<FV>& trainData) = 0;


    /*====================================================================*/
    /*! 
     *   train SVM with given Feature Vectors.  (general SVM interface
     *   -- may also be used for one-class problems in future)
     *
     *   \param problem  Structure with pointers to feature-vectors and y's
     */
    /*====================================================================*/
    virtual void train( const SVM_Problem<FV>& problem) = 0; 
    
   /*======================================================================*/
    /*! 
     *   train the Multi Class SVM with the given feature vectors. 
     *   The FeatureVector-Container must contain the Feature vectors
     *   directly, e.g., std::vector<MyFeatureVec>.
     *   
     *   \param FV_begin  iterator to the first element of your
     *                    FeatureVector-Container 
     *   \param FV_end    iterator to one past the last element of your
     *                    FeatureVector-Container 
     */
    /*======================================================================*/
    template< typename ForwardIter>
    void train( ForwardIter FV_begin, const ForwardIter& FV_end)
          {
            train( FV_begin, FV_end, DirectAccessor());
          }
    
    /*======================================================================*/
    /*! 
     *   same as train(), but you can specify an Accessor
     *   for accessing the elements of your container. This accessor
     *   must return a reference to the corresponding feature
     *   vector. Use DirectAccessor for containers like
     *   std::vector<FV> and DereferencingAccessor for containers like
     *   std::vector<FV*> 
     *
     *   \param FVP_begin iterator to the first element of your
     *                    PointerToFeatureVector-Container 
     *   \param FVP_end   iterator to one past the last element of your
     *                    PointerToFeatureVector-Container 
     *   \param accessor  functor, which accesses the feature vector
     *                    in the container
     */
    /*======================================================================*/
    template< typename ForwardIter, typename Accessor>
    void train( ForwardIter FV_begin,  
                const ForwardIter& FV_end,
                Accessor accessor) 
          {
            GroupedTrainingData<FV> trainData( FV_begin, FV_end, accessor);
            train( trainData);
          }

    /*======================================================================*/
    /*! 
     *   call clearCache() method of selected kernel, e.g., for
     *   Kernel_MATRIX this will clear the internal cache matrix.  You
     *   should call this method if you used updateCache() before
     *   training and now want to use the SVM for the classification
     *   of vectors, that are not within the training data.
     * 
     *   ATTENTION: If you don't clear the Cache before
     *   classification with a kernel like Kernel_MATRIX or
     *   Kernel_SCALE, only the uniqueID() of the feature vectors 
     *   will be used
     *
     */
    /*======================================================================*/
    virtual void clearKernelCache() = 0;
    

    void setStoreClassificationDetailsFlag( bool f)
          {
            _storeClassificationDetailsFlag = f;
          }
    
    bool getStoreClassificationDetailsFlag() const
          {
            return _storeClassificationDetailsFlag;
          }
    

    /*======================================================================*/
    /*! 
     *   classify the given Feature Vector. 
     *
     *   \param testObject test object. 
     *
     *   \exception 
     *
     *   \return Label of best matching class or predicted value for
     *   One-class SVM's
     */
    /*======================================================================*/
    virtual double classify( const FV& testObject) = 0;

//    /*====================================================================*/
//    /*! 
//     *   classify the given Feature Vector. 
//     *
//     *   \param      testObject         test object. 
//     *   \param[out] detailedResultsVec vector with detailed results
//     *                                  (results of all two class
//     *                                  classifications)  
//     *
//     *   \return Label of best matching class or predicted value for
//     *   One-class SVM's
//     */
//    /*====================================================================*/
//    virtual double classify( const FV& testObject, 
//                             std::vector<double>& detailedResultsVec) = 0;
    

    /*======================================================================*/
    /*! 
     *   save classification details. Here the ordered labeles as
     *   "labels" and the according decision values as
     *   "dec_values". For this to work you must have called
     *   setStoreClassificationDetailsFlag( true) before classify()   
     *
     *   \param resultVector results from the classification
     *   \param stData       structured data, where to store the details
     */
    /*======================================================================*/
    virtual void saveClassificationDetailsASCII( StDataASCII& stData) const =0;
    

    /*======================================================================*/
    /*! 
     *  Evaluate kernel matrix of the svm. A square matrix is generated
     *  with the current 2-class-svm-kernel. Assumption: memory is already 
     *  allocated for kmatrix.
     */
    /*======================================================================*/
    virtual void computeTrainKernelMatrix(float** kmatrix, 
                                          const std::vector<FV>& featureVectors) = 0;
    
    /*======================================================================*/
    /*! 
     *   get number of Classes
     *   (loadModel() or train() must have been called before) 
     *
     *   \return number of Classes
     */
    /*======================================================================*/
    virtual int nClasses() const = 0;
    

    /*======================================================================*/
    /*! 
     *   map classIndex to label
     *   (loadModel() or train() must have been called before) 
     *
     *   \param classIndex the index of the class (when sorted by labels)
     *
     *   \return the according label
     */
    /*======================================================================*/
    virtual double classIndexToLabel( unsigned int classIndex) const = 0;
  
 
    /*======================================================================*/
    /*!
     *  returns number of SV in model
     */
    /*======================================================================*/
    virtual size_t nSupportVectors() const = 0;
    
    /*======================================================================*/
    /*!
     *  returns ith SV in model
     * 
     *  \param i the index of the SV
     */  
    /*======================================================================*/
    virtual FV* getSupportVector(int i) const = 0;

    
    /*======================================================================*/
    /*!
     *  returns vector containing pointers to all TwoClassModels
     *
     */  
    /*======================================================================*/
    virtual std::vector<const typename svt::Model<FV>* > getTwoClassModels() const =0;
    virtual std::vector<typename svt::Model<FV>* > getTwoClassModels() =0;
  
    /*======================================================================*/
    /*!
     * optimize model for faster classification
     */
     /*======================================================================*/
    virtual void optimizeLinearModel()=0;

  protected:
    bool _storeClassificationDetailsFlag;
  
  };

}


#endif
