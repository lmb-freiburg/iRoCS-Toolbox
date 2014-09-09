/**************************************************************************
**       Title: test cross validation
**    $RCSfile$
**   $Revision: 678 $$Name$
**       $Date: 2005-03-29 20:10:22 +0200 (Tue, 29 Mar 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.4  2005/03/29 18:08:19  ronneber
**  - adapted to new manual call of updateKernelCache() and clearKernelCache()
**
**  Revision 1.3  2004/09/03 09:14:56  ronneber
**  *** empty log message ***
**
**  Revision 1.2  2004/09/01 14:45:13  ronneber
**  changed IterToPointerTraits stuff to
**  DirectAccessor and DereferencingAccessor, to make code more
**  intuitive understandable
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.1  2003/05/19 11:38:11  ronneber
**  - initial revision
**
**
**
**************************************************************************/


#include <ctime>     // clock()
#include <iostream>
#include <fstream>

#include <libsvmtl/MultiClassSVMOneVsOne.hh>
#include <libsvmtl/MultiClassSVMOneVsRest.hh>
#include <libsvmtl/TwoClassSVMc.hh>
#include <libsvmtl/Kernel_LINEAR.hh>
#include <libsvmtl/Kernel_RBF.hh>
#include <libsvmtl/Kernel_MATRIX.hh>
#include <libsvmtl/BasicFV.hh>
#include <libsvmtl/CrossValidator.hh>
#include <libsvmtl/DirectAccessor.hh>


#include "lmbunit.hh"

template< typename FV>
void _fillFV( FV& fv, int label, double f0, double f1, double f2)
{
  fv.resize( 3);
  fv.setLabel( label);
  fv[0] = f0;
  fv[1] = f1;
  fv[2] = f2;
}

static void tryManualCrossValidation()
{
  typedef svt::MultiClassSVMOneVsOne< svt::TwoClassSVMc< svt::Kernel_LINEAR> > SVM_TYPE;
  
  // create cross validator object and adjust the svm parameters
  SVM_TYPE svm;
  svt::StDataASCII params;
  params.setValue("cost",             100000);
  params.setValue("verbose_level",    0);
  svm.loadParameters( params);
  
  // create feature vectors and pass them to the cross validator
  std::vector<svt::BasicFV> featureVectors(8);

  _fillFV( featureVectors[0], 3,  0, 0, 0);
  _fillFV( featureVectors[1], 2,  1, 0, 0);
  _fillFV( featureVectors[2], 1,  0, 1, 0);
  _fillFV( featureVectors[3], 0,  0, 0, 1);
  _fillFV( featureVectors[4], 3,  0.1, 0, 0);
  _fillFV( featureVectors[5], 2,  1.1, 0, 0);
  _fillFV( featureVectors[6], 1,  0.1, 1, 0);
  _fillFV( featureVectors[7], 0,  0.1, 0, 1);
  svt::adjustUniqueIDs( featureVectors);
  svt::GroupedTrainingData<svt::BasicFV> trainData( featureVectors.begin()+2,
                                                    featureVectors.end(),
                                                    svt::DirectAccessor());
  SVM_TYPE::Traits<svt::BasicFV>::ModelType model;

  svm.updateKernelCache( featureVectors.begin()+2,
                         featureVectors.end(),
                         svt::DirectAccessor());  
  svm.train( trainData, model);
  svm.clearKernelCache();
  
  
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[0], model), 3);
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[1], model), 2);
}


static void tryManualLeaveOneOut()
{
  typedef svt::MultiClassSVMOneVsRest< svt::TwoClassSVMc< svt::Kernel_LINEAR> > SVM_TYPE;
  
  // create cross validator object and adjust the svm parameters
  SVM_TYPE svm;
  svt::StDataASCII params;
  params.setValue("cost",             100000);
  params.setValue("verbose_level",    0);
  svm.loadParameters( params);
  
  // create feature vectors and pass them to the cross validator
  std::vector<svt::BasicFV> featureVectors(8);

  _fillFV( featureVectors[0], 3,  0, 0, 0);
  _fillFV( featureVectors[1], 2,  1, 0, 0);
  _fillFV( featureVectors[2], 1,  0, 1, 0);
  _fillFV( featureVectors[3], 3,  0.1, 0, 0);
  _fillFV( featureVectors[4], 2,  1.1, 0, 0);
  _fillFV( featureVectors[5], 1,  0.1, 1, 0);
  _fillFV( featureVectors[6], 0,  0.1, 0, 1);
  svt::adjustUniqueIDs( featureVectors);

  _fillFV( featureVectors[7], 0,  0, 0, 1);
  svt::GroupedTrainingData<svt::BasicFV> trainData( featureVectors.begin(),
                                                    featureVectors.end()-1,
                                                    svt::DirectAccessor());
  SVM_TYPE::Traits<svt::BasicFV>::ModelType model;
  
  svm.updateKernelCache( featureVectors.begin(),
                         featureVectors.end()-1,
                         svt::DirectAccessor());  
  svm.train( trainData, model);
  svm.clearKernelCache();
  
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[7], model), 0);
}





static void testBasics()
{
  typedef svt::MultiClassSVMOneVsOne< svt::TwoClassSVMc< svt::Kernel_LINEAR> > SVM_TYPE;
  
  // create cross validator object and adjust the svm parameters
  svt::CrossValidator<svt::BasicFV, SVM_TYPE> cv;
  svt::StDataASCII params;
  params.setValue("cost",             100000);
  params.setValue("verbose_level",    0);
  cv.svm()->loadParameters( params);
  
  // create feature vectors and pass them to the cross validator
  std::vector<svt::BasicFV> featureVectors(8);

  _fillFV( featureVectors[0], 3,  0, 0, 0);
  _fillFV( featureVectors[1], 2,  1, 0, 0);
  _fillFV( featureVectors[2], 1,  0, 1, 0);
  _fillFV( featureVectors[3], 0,  0, 0, 1);
  _fillFV( featureVectors[4], 3,  0.1, 0, 0);
  _fillFV( featureVectors[5], 2,  1.1, 0, 0);
  _fillFV( featureVectors[6], 1,  0.1, 1, 0);
  _fillFV( featureVectors[7], 0,  0.1, 0, 1);
  svt::adjustUniqueIDs( featureVectors);
  svt::GroupedTrainingData<svt::BasicFV> trainData( featureVectors.begin(),
                                                    featureVectors.end(),
                                                    svt::DirectAccessor());
  cv.setTrainingData( &trainData);
  
//  svt::ProgressReporter pr;
//  
//  cv.svm()->setProgressReporter(&pr);
//  

  // do the proprocessing of the training data (which is a training
  // with all training vectors)

  cv.updateKernelCache();  
  cv.preprocessTrainingData();

  LMBUNIT_DEBUG_STREAM << "Full model got "
                       << cv.fullModel().collectedSupportVectors().size()
                       << " support vectors\n";
  
  // print out support vectors of two-class models
  for( int c1 = 0; c1 < 4; ++c1)
  {
    for( int c2 = c1+1; c2 < 4; ++c2)
    {
      LMBUNIT_DEBUG_STREAM << "Two-class-model " << c1 << " vs. " 
                           << c2 << std::endl;
        LMBUNIT_DEBUG_STREAM 
            << "      rho = " << cv.fullModel().twoClassModel(c1,c2).rho()
            << std::endl;
        
      for(unsigned int i = 0; i < cv.fullModel().twoClassModel(c1,c2).size(); ++i)
      {
        LMBUNIT_DEBUG_STREAM 
            << "      "
            << cv.fullModel().twoClassModel(c1,c2).alpha(i)
            << " * "
            << *(cv.fullModel().twoClassModel(c1,c2).supportVector(i))
            << std::endl;
      }
    }
  }
  
      
  
  // Now perform the cross validation with 4 subsets
  std::vector<int> subsetIndexByUID;
  svt::generateSortedSubsets( featureVectors.size(), 4, subsetIndexByUID);
  LMBUNIT_DEBUG_STREAM << "subsetIndexByUID = ";
  for( unsigned int i = 0; i < subsetIndexByUID.size(); ++i)
  {
    LMBUNIT_DEBUG_STREAM << subsetIndexByUID[i] << ",";
  }
  LMBUNIT_DEBUG_STREAM << std::endl;
  

  std::vector<double> predictedClassLabelByUID;
  int nCorrect = cv.doFullCV( subsetIndexByUID, predictedClassLabelByUID);
  

  LMBUNIT_ASSERT_EQUAL( nCorrect, 8);
  for( unsigned int i = 0; i < featureVectors.size(); ++i)
  {
    LMBUNIT_ASSERT_EQUAL_DELTA( 
        featureVectors[i].getLabel(), 
        predictedClassLabelByUID[featureVectors[i].uniqueID()], 0.000001);
  }
    
}

template< typename SVM_TYPE>
static void testLeaveOneOut()
{
  // create cross validator object and adjust the svm parameters
  svt::CrossValidator<svt::BasicFV, SVM_TYPE> cv;
  svt::StDataASCII params;
  params.setValue("cost",             100000);
  params.setValue("verbose_level",    0);
  cv.svm()->loadParameters( params);
  
  // create feature vectors and pass them to the cross validator
  std::vector<svt::BasicFV> featureVectors(8);

  _fillFV( featureVectors[0], 3,  0, 0, 0);
  _fillFV( featureVectors[1], 2,  1, 0, 0);
  _fillFV( featureVectors[2], 1,  0, 1, 0);
  _fillFV( featureVectors[3], 0,  0, 0, 1);
  _fillFV( featureVectors[4], 3,  0.1, 0, 0);
  _fillFV( featureVectors[5], 2,  1.1, 0, 0);
  _fillFV( featureVectors[6], 1,  0.1, 1, 0);
  _fillFV( featureVectors[7], 0,  0.1, 0, 1);
  svt::adjustUniqueIDs( featureVectors);
  svt::GroupedTrainingData<svt::BasicFV> trainData( featureVectors.begin(),
                                                    featureVectors.end(),
                                                    svt::DirectAccessor());
  
  LMBUNIT_DEBUG_STREAM << "trainData looks like this:\n";
  for( unsigned int i = 0; i < trainData.nFeatureVectors(); ++i)
  {
    LMBUNIT_DEBUG_STREAM << *(trainData.featureVector(i)) << std::endl;
  }
  LMBUNIT_DEBUG_STREAM << "class start indizes:\n";
  for( unsigned int i = 0; i < trainData.nClasses(); ++i)
  {
    LMBUNIT_DEBUG_STREAM << "class " << i << " starts at " 
                         << trainData.classStartIndex( i) << std::endl;
  }
  

  cv.setTrainingData( &trainData);
  
  // do the proprocessing of the training data (which is a training
  // with all training vectors)
  cv.updateKernelCache();
  cv.preprocessTrainingData();

  
  // Now perform the cross validation with 8 subsets which 
  // is equivalent to leave one out 
  std::vector<int> subsetIndexByUID;
  svt::generateSortedSubsets( featureVectors.size(), 8, subsetIndexByUID);
  LMBUNIT_DEBUG_STREAM << "subsetIndexByUID = ";
  for( unsigned int i = 0; i < subsetIndexByUID.size(); ++i)
  {
    LMBUNIT_DEBUG_STREAM << subsetIndexByUID[i] << ",";
  }
  LMBUNIT_DEBUG_STREAM << std::endl;
  

  std::vector<double> predictedClassLabelByUID;
  int nCorrect = cv.doFullCV( subsetIndexByUID, predictedClassLabelByUID);

  LMBUNIT_ASSERT_EQUAL( nCorrect, 8);
  LMBUNIT_ASSERT_EQUAL_DELTA( predictedClassLabelByUID[0], 3, 0.000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( predictedClassLabelByUID[1], 2, 0.000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( predictedClassLabelByUID[2], 1, 0.000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( predictedClassLabelByUID[3], 0, 0.000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( predictedClassLabelByUID[4], 3, 0.000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( predictedClassLabelByUID[5], 2, 0.000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( predictedClassLabelByUID[6], 1, 0.000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( predictedClassLabelByUID[7], 0, 0.000001);
  

  // calculate statistics
//   svt::StDataASCII stat;
//   cv.saveStatistics( stat, predictedClassLabelByUID, 2);
//   LMBUNIT_ASSERT_EQUAL( stat.asDouble( "class0_label"), 0);
//   LMBUNIT_ASSERT_EQUAL( stat.asUint(   "class0_nCorrect"), 2);
//   LMBUNIT_ASSERT_EQUAL( stat.asUint(   "class0_nFV"), 2);
//   LMBUNIT_ASSERT_EQUAL( stat.asDouble( "class1_label"), 1);
//   LMBUNIT_ASSERT_EQUAL( stat.asUint(   "class1_nCorrect"), 2);
//   LMBUNIT_ASSERT_EQUAL( stat.asUint(   "class1_nFV"), 2);
//   LMBUNIT_ASSERT_EQUAL( stat.asDouble( "class2_label"), 2);
//   LMBUNIT_ASSERT_EQUAL( stat.asUint(   "class2_nCorrect"), 2);
//   LMBUNIT_ASSERT_EQUAL( stat.asUint(   "class2_nFV"), 2);
//   LMBUNIT_ASSERT_EQUAL( stat.asDouble( "class3_label"), 3);
//   LMBUNIT_ASSERT_EQUAL( stat.asUint(   "class3_nCorrect"), 2);
//   LMBUNIT_ASSERT_EQUAL( stat.asUint(   "class3_nFV"), 2);
  
}


template< typename SVM_TYPE>
static double measureCrossValidationTime( 
    std::vector<svt::BasicFV>& featureVectors)
{
  
  clock_t startTime = clock();
  
  svt::CrossValidator<svt::BasicFV, SVM_TYPE> cv;
  svt::StDataASCII params;
  params.setValue("cost",             100);
  params.setValue("verbose_level",    0);
  cv.svm()->loadParameters( params);
  
  svt::ProgressReporter pr;
  pr.setVerboseLevel(0);
  
  cv.setProgressReporter( &pr);
  
  svt::GroupedTrainingData<svt::BasicFV> trainData( featureVectors.begin(),
                                                    featureVectors.end(),
                                                    svt::DirectAccessor());
  cv.setTrainingData( &trainData);
  cv.updateKernelCache();
  cv.preprocessTrainingData();

  std::vector<int> subsetIndexByUID;
  svt::generateSortedSubsets( featureVectors.size(), 5, subsetIndexByUID);

  std::vector<double> predictedClassLabelByUID;
  cv.doFullCV( subsetIndexByUID, predictedClassLabelByUID);
 
  return double(clock() - startTime) / CLOCKS_PER_SEC;
}

  


static void testComputationTime()
{
  unsigned int nClasses = 5;
  unsigned int nFeatureVectorsPerClass = 10;
  unsigned int nComponents = 100;
  
  // create random feature vectors 
  std::vector<svt::BasicFV> featureVectors(nFeatureVectorsPerClass * nClasses);
  
  for( unsigned int i = 0; i < featureVectors.size(); ++i)
  {
    int label = i % nClasses;
    
    svt::BasicFV& fv = featureVectors[i];
    
    fv.resize( nComponents);
    fv.setLabel( label);
    fv[0] = label*2 + double(std::rand())/RAND_MAX;  // in feature 0 all
                                                        // classes are
                                                        // well
                                                        // separated 
    for( unsigned int j = 1; j < nComponents; ++j)
    {
      fv[j] = double(std::rand())/RAND_MAX;
    }
  }
  svt::adjustUniqueIDs( featureVectors);

  /*-----------------------------------------------------------------------
   *  measure time of cross validation without kernel caching 
   *-----------------------------------------------------------------------*/
  double time1 = measureCrossValidationTime<
      svt::MultiClassSVMOneVsRest< svt::TwoClassSVMc< svt::Kernel_LINEAR> > >( featureVectors);
  double time2 = measureCrossValidationTime<
      svt::MultiClassSVMOneVsRest< svt::TwoClassSVMc< svt::Kernel_MATRIX< svt::Kernel_LINEAR> > > >( featureVectors);

  LMBUNIT_DEBUG_STREAM << "cross validation without kernel matrix: " << time1 << " sec\n";
  LMBUNIT_DEBUG_STREAM  << "cross validation with kernel matrix: " << time2 << " sec\n";
  
  // This test sometimes fails, probably because the problem is too small
  //  SVM_ASSERT( time2 <= time1);
}


static void testException()
{
  // create cross validator object and adjust the svm parameters
  svt::CrossValidator<svt::BasicFV, svt::MultiClassSVMOneVsOne< svt::TwoClassSVMc< svt::Kernel_LINEAR> > > cv;
  
  try
  {
    // try to do a preprocessTrainingData without training data
    cv.preprocessTrainingData();
  }
  catch( svt::SVMAssertionFailed& err)
  {
    return;
  }
  
  LMBUNIT_WRITE_FAILURE( "cv.preprocessTrainingData() must throw exception, when setTrainingData() was not called before!");
}


int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();
  LMBUNIT_RUN_TEST_NOFORK( tryManualCrossValidation()); 
  LMBUNIT_RUN_TEST_NOFORK( tryManualLeaveOneOut()); 
  LMBUNIT_RUN_TEST_NOFORK( testBasics());
  LMBUNIT_RUN_TEST_NOFORK( testLeaveOneOut<svt::MultiClassSVMOneVsRest< svt::TwoClassSVMc< svt::Kernel_LINEAR> > >());
  LMBUNIT_RUN_TEST_NOFORK( testLeaveOneOut<svt::MultiClassSVMOneVsRest< svt::TwoClassSVMc< svt::Kernel_MATRIX<svt::Kernel_LINEAR> > > >());
  LMBUNIT_RUN_TEST_NOFORK( testComputationTime());
  LMBUNIT_RUN_TEST( testException());
  
  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


