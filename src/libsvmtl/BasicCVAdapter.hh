/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 752 $$Name$
**       $Date: 2005-10-25 14:09:55 +0200 (Tue, 25 Oct 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.4  2005/10/25 12:09:34  ronneber
**  - added classificationDetailsByUID() and
**    get/setStoreClassificationDetailsFlag()
**
**  Revision 1.3  2005/03/29 17:49:26  ronneber
**  - added updateKernelCache() and clearKernelCache()
**
**  Revision 1.2  2004/09/08 14:11:21  ronneber
**  - added saveOnlyKernelParameters()
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/


#ifndef BASICCVADAPTER_HH
#define BASICCVADAPTER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>

#include "GroupedTrainingData.hh"
#include "StDataASCII.hh"
#include "ProgressReporter.hh"
#include "StDataCmdLine.hh"


namespace svt
{

  template<typename FV, 
           typename PROBLEM = GroupedTrainingData<FV>, 
           typename STDATA = StDataASCII>
  class BasicCVAdapter
  {
  public:
    typedef FV        FV_TYPE;
    typedef PROBLEM   PROBLEM_TYPE;

    virtual ~BasicCVAdapter()
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
     *   \exception ... depending on your STDATA class
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
     *   store all parameters of the SVM to given mapstructured data
     *
     *   \param  stData  structured data containing parameters
     *                   as key/value pairs  
     *
     *   \exception ... depending on your STDATA class
     */
    /*======================================================================*/
    virtual void saveParameters( STDATA& stData) = 0;

    virtual void saveParameters( StDataASCII& stData) = 0;


    /*======================================================================*/
    /*! 
     *   save only Kernel Parameters (this is used for user information and
     *   to detect in grid search, wether a kernel parameter is changed
     */
    /*======================================================================*/
    virtual void saveOnlyKernelParameters( StDataASCII& stData) = 0;


    /*======================================================================*/
    /*! 
     *   set the training data. the CrossValidator only stores a
     *   pointer to it, so you are responsible that the training data
     *   keeps avaiblibe during further calls, and is deleted at the
     *   end of the program
     *
     *   \param problem  pointer to the training data
     *
     */
    /*======================================================================*/
    virtual void setTrainingData( const PROBLEM* problem) = 0;
    
    /*======================================================================*/
    /*! 
     *   get pointer to training data, that was set with setTrainingData().
     *
     *   \return pointer to training data or 0, if no training data
     *           was specified yet  
     */
    /*======================================================================*/
    virtual const PROBLEM* trainingData() const = 0;
    
    
    /*======================================================================*/
    /*! 
     *   call updateKernelCache() of selected svm with given problem.
     *   You are responsible to call this before
     *   preprocessTrainingData(), every time when kernel parameters
     *   or dataset was chenged. (usually every time before you call
     *   preprocessTrainingData())  
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    virtual void updateKernelCache() = 0;
    
    
    /*======================================================================*/
    /*! 
     *   call clearKernelCache() of selected svm.
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    virtual void clearKernelCache() = 0;
    
    /*======================================================================*/
    /*! 
     *   trains all two-class SVM's with the whole data set. This must
     *   be done _before_ calling doFullCV() of doPartialCV(). If
     *   Kernel_MATRIX is used, you must call updateKernelCache() before
     *
     */
    /*======================================================================*/
    virtual void preprocessTrainingData() = 0;
    
    
    /*======================================================================*/
    /*! 
     *   do a full cross validation. Just calls doPartialCV() for each
     *   subset index and collects the results. See there for
     *   the preconditions
     *
     *   \param subsetIndexByUID tells for each feature vector
     *                           (identified by its uniqueID) to which
     *                           subset it belongs. If the uniqueIDs
     *                           of your feature vectors are
     *                           consecutive numbers from
     *                           0...nFeatureVectors-1, then you can
     *                           create this array with
     *                           generateSortedSubsets() or
     *                           generateShuffledSubsets() 
     *
     *   \param predictedClassLabelByUID (output) stores the
     *                           predicted class label for each
     *                           feature vector. Will be resized to
     *                           same size as subsetIndexByUID
     *
     *   \return total number of correct classifications
     */
    /*======================================================================*/
    virtual int doFullCV( const std::vector<int>& subsetIndexByUID,
                          std::vector<double>& predictedClassLabelByUID) = 0;
    

    
    virtual void setClassificationDelta( double d) = 0;
   
    virtual double classificationDelta() const = 0;

    virtual void setStoreClassificationDetailsFlag( bool f) = 0;
        
    virtual bool getStoreClassificationDetailsFlag() const = 0;
    

    /*======================================================================*/
    /*! 
     *   save cross validation statistics. 
     *
     *   \param statistics       structured data, where to store the
     *                           statistics
     *   \param detailLevel      0: nothing, 
     *                           1: training statistics
     *                              from last call to doFullCV()
     */
    /*======================================================================*/
    virtual void saveStatistics( STDATA& statistics,
                                 int detailLevel = 1) = 0;

    virtual void saveStatistics( StDataASCII& statistics,
                                 int detailLevel = 1) = 0;


    /*======================================================================*/
    /*! 
     *   get classification details for each uid from last
     *   full CV or parital CV. Use
     *   setStoreClassificationDetailsFlag() before
     *   preprocessTrainingData() to enable storage of these infos
     *   during cross validation 
     *
     *   \return vector with classification details for each UID
     */
    /*======================================================================*/
    virtual const std::vector< StDataASCII>& classificationDetailsByUID() const
        = 0;
    
  };
  
}

#endif
