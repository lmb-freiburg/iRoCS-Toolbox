/**************************************************************************
**       Title: load/save policy for SVMApplication for ASCII files
**    $RCSfile$
**   $Revision: 759 $$Name$
**       $Date: 2005-10-26 09:15:47 +0200 (Wed, 26 Oct 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.5  2005/10/26 07:14:48  ronneber
**  - added getParamInfosForLoadSubsetLabels(),
**    checkParamsForLoadSubsetLabels() and loadSubsetLabels() for using
**    predefined subsets in cross validation
**  - classifcation details are now given in key-value pairs, which allows
**    more flexibility
**  - classification details are now saved to the extra file
**    <outpufilename>_details in the format "<uid>_<key> <value>" one per
**    line, where uid is the unique id of the feature vector and key,
**    value the given key value pair from the given details-vector
**
**  Revision 1.4  2004/09/09 14:38:41  ronneber
**  - cosmetic changes
**
**  Revision 1.3  2004/09/08 14:31:18  ronneber
**  - adapted to new ParamInfo class
**
**  Revision 1.2  2004/08/27 09:53:55  ronneber
**  - fixed bug in saving crossval results
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef LOADSAVEASCII_HH
#define LOADSAVEASCII_HH

#include <iostream>
#include <fstream>
#include <cerrno>

#include "StDataASCIIFile.hh"
#include "StDataCmdLine.hh"
#include "PrettyOptionPrinter.hh"
#include "BasicSVMAdapter.hh"
#include "BasicSVMFactory.hh"
#include "ParamInfo.hh"

namespace svt
{
  class LoadSaveASCII
  {
  public:

    typedef StDataASCIIFile STDATA;

     ~LoadSaveASCII()
          {}
    
    /*======================================================================*/
    /*! 
     *   get information about the parameters, that are used in
     *   loadFeatureVectors(). The Infos are appended
     *   to the passed array
     *
     *   \param p  array of ParamInfos. The Infos will be appended to
     *             this array
     */
    /*======================================================================*/
    static void getParamInfosForLoadFeatureVectors( std::vector<ParamInfo>& p);
    
    /*======================================================================*/
    /*! 
     *   Just accesses all parameters, that are used in
     *   loadFeatureVectors() so that they are no longer unknwon to
     *   StDataASCIIFile 
     *
     *   \param cmdline command line
     */
    /*======================================================================*/
    void checkParamsForLoadFeatureVectors(StDataCmdLine& cmdline) const;
    

    /*======================================================================*/
    /*! 
     *   load feature vectors
     *
     *   \param cmdline         command line parameters
     *   \param featureVectors  the loaded feature vectors
     *
     *   \exception svt::LoadError file could not be opened 
     */
    /*======================================================================*/
    template< typename FV>
    void loadFeatureVectors( StDataCmdLine& cmdline,  
                             std::vector<FV>& featureVectors);
    

    /*======================================================================*/
    /*! 
     *   get information about the parameters, that are used in
     *   loadSubsetLabels(). The Infos are appended
     *   to the passed array
     *
     *   \param p  array of ParamInfos. The Infos will be appended to
     *             this array
     */
    /*======================================================================*/
    static void getParamInfosForLoadSubsetLabels( std::vector<ParamInfo>& p);

    /*======================================================================*/
    /*! 
     *   checks wether the "--subset_att" parameter was given.
     *
     *   \param cmdline command line
     *
     *   \return true: subset_att parameter was given
     */
    /*======================================================================*/
    bool checkParamsForLoadSubsetLabels(StDataCmdLine& cmdline) const;
    
    
    /*======================================================================*/
    /*! 
     *   load feature vectors
     *
     *   \param cmdline          command line parameters
     *   \param subsetIndexByUID subset Index by Unique ID, could be
     *                           directly used for Cross-validati on
     *
     *   \exception svt::LoadError file could not be opened 
     */
    /*======================================================================*/
    void loadSubsetLabels( StDataCmdLine& cmdline, 
                           std::vector<int>& subsetIndexByUID);
    
    
    /*======================================================================*/
    /*! 
     *   get information about the parameters, that are used in
     *   saveModel(). The Infos are appended
     *   to the passed array
     *
     *   \param p  array of ParamInfos. The Infos will be appended to
     *             this array
     */
    /*======================================================================*/
    static void getParamInfosForSaveModel( std::vector<ParamInfo>& p);
    

    /*======================================================================*/
    /*! 
     *   Just accesses all parameters, that are used in
     *   saveModel() so that they are no longer unknwon to
     *   StDataASCIIFile 
     *
     *   \param cmdline command line
     */
    /*======================================================================*/
    void checkParamsForSaveModel(StDataCmdLine& cmdline) const;
    

    /*======================================================================*/
    /*! 
     *   save Model. 
     *
     *   \param cmdline         command line parameters
     *   \param svm             the trained SVM
     *   \param detailLevel     wether to save additional training
     *                          infos to model. 0: nothing, 1: only
     *                          statistics, 2: training infos from all
     *                          two-class trainings   
     *
     *   \exception svt::SaveError output file could not be created
     */
    /*======================================================================*/
    template< typename FV>
    void saveModel( StDataCmdLine& cmdline, 
                    BasicSVMAdapter<FV,STDATA>* svm,
                    int detailLevel,
                    std::ostream& os);


    /*======================================================================*/
    /*! 
     *   get information about the parameters, that are used in
     *   createSVMAndLoadModel(). The Infos are appended
     *   to the passed array
     *
     *   \param p  array of ParamInfos. The Infos will be appended to
     *             this array
     */
    /*======================================================================*/
    static void getParamInfosForCreateSVMAndLoadModel( 
        std::vector<ParamInfo>& p);

    
    /*======================================================================*/
    /*! 
     *   create SVM from file and load Model data
     *
     *   \param cmdline         command line parameters
     *   \param modelFileName   file name of modelfile
     *   \param svm             (output) 
     *
     *   \exception svt::LoadError file could not be opened
     *
     *   \return created SVM with loaded Model. The ownership of this object
     *                          goes to caller. So you are responsible
     *                          to delete it after usage 
     */
    /*======================================================================*/
    template< typename FV, typename ALGORITHMS>
    BasicSVMAdapter<FV,STDATA>* createSVMAndLoadModel( 
        StDataCmdLine& cmdline);


    
    /*======================================================================*/
    /*! 
     *   get information about the parameters, that are used in
     *   createSVMAndLoadModel(). The Infos are appended
     *   to the passed array
     *
     *   \param p  array of ParamInfos. The Infos will be appended to
     *             this array
     */
    /*======================================================================*/
    static void getParamInfosForSaveClassificationResults( 
        std::vector<ParamInfo>& p);
    
    /*======================================================================*/
    /*! 
     *   Just accesses all parameters, that are used in
     *   saveClassificationResults() so that they are no longer unknwon to
     *   StDataASCIIFile 
     *
     *   \param cmdline command line
     */
    /*======================================================================*/
    bool checkParamsForSaveClassificationResults(
        StDataCmdLine& cmdline) const;


    /*======================================================================*/
    /*! 
     *   save classification results. output file name is taken from
     *   cmdline parameters. If requested, classification
     *   details are saved to the extra file <outpufilename>_details
     *   in the format "<uid>_<key> <value>" one per line, where uid
     *   is the unique id of the feature vector and key, value the
     *   given key value pair from the given details-vector  
     *
     *   \param cmdline  the command line
     *   \param results  predicted labels for each feature vector
     *   \param details  additional classification details in
     *                   key-value pairs for each feature vector 
     *   \param saveDetails wether to save the additional details in
     *                   details vector
     *   \param os       output stream (usually std::cout) for
     *                   additionally infos, like "saving ...", etc.
     *
     */
    /*======================================================================*/
    void saveClassificationResults( 
        StDataCmdLine& cmdline,
        const std::vector<double>& results,
        const std::vector< StDataASCII>& details,
        bool saveDetails,
        std::ostream& os);
    
  };
}

#include "LoadSaveASCII.icc"

#endif
