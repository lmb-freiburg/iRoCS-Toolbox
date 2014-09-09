/**************************************************************************
**       Title: load/save policy for SVMApplication for Hdf5 files
**    $RCSfile: $
**   $Revision: $$Name:  $
**       $Date: $
**   Copyright: GPL $Author: $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: $
**
**
**************************************************************************/

#ifndef LOADSAVEHDF5_HH
#define LOADSAVEHDF5_HH

#include <iostream>
#include <fstream>
#include <cerrno>

#include "StDataHdf5.hh"
#include <StDataCmdLine.hh>
#include <PrettyOptionPrinter.hh>
#include <BasicSVMAdapter.hh>
#include <BasicSVMFactory.hh>
#include <ParamInfo.hh>

namespace svt
{
  template<typename STDATATYPE>
  class LoadSaveHdf5Templ
  {
  public:

     ~LoadSaveHdf5Templ()
          {}
    
    typedef STDATATYPE STDATA;
    

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
     *   StDataCmdLine
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
                    std::ostream& os) const;


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
        StDataCmdLine& cmdline) const;



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
     *   save classification results. 
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
        std::ostream& os) const;
  };

  typedef LoadSaveHdf5Templ<StDataHdf5> LoadSaveHdf5;

}

#include "LoadSaveHdf5.icc"

#endif
