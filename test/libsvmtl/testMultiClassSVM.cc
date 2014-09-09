/**************************************************************************
**       Title: test unit for MultiClassSVM classes
**    $RCSfile$
**   $Revision: 797 $$Name$
**       $Date: 2006-01-25 09:53:51 +0100 (Wed, 25 Jan 2006) $
**   Copyright: GPL $Author: fehr $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.6  2006/01/25 08:53:51  fehr
**  added HistIntersect to Kmatrix Kernels and new BasicSVMAdapter interface to TwoClassModels
**
**  Revision 1.5  2005/10/26 09:32:40  ronneber
**  - corrected some comments and variable names (confusion between alpha
**    and decision value)
**
**  Revision 1.4  2005/03/29 18:08:32  ronneber
**  - adapted to new manual call of updateKernelCache() and clearKernelCache()
**
**  Revision 1.3  2004/09/09 14:44:26  ronneber
**  *** empty log message ***
**
**  Revision 1.2  2004/09/03 09:14:56  ronneber
**  *** empty log message ***
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.13  2003/05/19 11:41:24  ronneber
**  - now writes all output to LMBUNIT_DEBUG_STREAM
**  - added testMultiClassSVMOneVsRestC2() (not yet ready)
**
**  Revision 1.12  2003/02/10 07:38:18  ronneber
**  - adapted to g++-3.2
**
**  Revision 1.11  2002/07/09 06:41:41  ronneber
**  added progress reporter for OneVsRest
**
**  Revision 1.10  2002/07/08 13:46:50  ronneber
**  - added copySVCoef... methods for OneVsRest SVM
**
**  Revision 1.9  2002/06/07 12:27:16  ronneber
**  *** empty log message ***
**
**  Revision 1.8  2002/05/22 17:36:23  ronneber
**  - added testProgressReporter()
**
**  Revision 1.7  2002/05/14 10:22:03  ronneber
**  - adapted to new TriangularMatrix (now using upper triangular matrix)
**
**  Revision 1.6  2002/05/13 16:30:07  ronneber
**  *** empty log message ***
**
**  Revision 1.5  2002/05/10 11:35:14  ronneber
**  - now using default constructors of MultiClassSVM's
**
**  Revision 1.4  2002/05/10 11:07:03  ronneber
**  - removed FV template for all public classes, because Feature Vector type
**    can be extracted automatically from passed iterators or other
**    parameters -- this makes the public interface much more intuitive
**
**  Revision 1.3  2002/05/10 05:30:50  ronneber
**  sync
**
**  Revision 1.2  2002/05/08 10:36:11  ronneber
**  - added some debugging stuff
**
**  Revision 1.1  2002/05/06 13:47:29  ronneber
**  initial revision
**
**
**
**************************************************************************/
#include <map>
#include <iostream>
#include <fstream>

#include "lmbunit.hh"
#include <libsvmtl/BasicFV.hh>
#include <libsvmtl/Kernel_LINEAR.hh>
#include <libsvmtl/Kernel_RBF.hh>
#include <libsvmtl/TwoClassSVMc.hh>
#include <libsvmtl/TwoClassSVMnu.hh>
#include <libsvmtl/MultiClassSVMOneVsOne.hh>
#include <libsvmtl/TriangularMatrix.hh>
#include <libsvmtl/MultiClassSVMOneVsRest.hh>
#include <libsvmtl/ProgressReporter.hh>
#include <libsvmtl/StDataASCIIFile.hh>
#include <libsvmtl/DirectAccessor.hh>

#include "MyFeatureVector.hh"
#include "MyKernel.hh"



template< typename FV>
void _fillFV( FV& fv, int label, double f0, double f1, double f2)
{
  fv.resize( 3);
  fv.setLabel( label);
  fv[0] = f0;
  fv[1] = f1;
  fv[2] = f2;
}



static void testMultiClassSVMOneVsOneC()
{
  std::vector<svt::BasicFV> featureVectors(4);

  _fillFV( featureVectors[0], 3,  0, 0, 0);
  _fillFV( featureVectors[1], 2,  1, 0, 0);
  _fillFV( featureVectors[2], 1,  0, 1, 0);
  _fillFV( featureVectors[3], 0,  0, 0, 1);

  svt::adjustUniqueIDs( featureVectors);
  

  svt::MultiClassSVMOneVsOne< svt::TwoClassSVMc< svt::Kernel_LINEAR> > svm;
  svm.twoClassSVM().setCost( 100);
  
      
  svt::Model_MC_OneVsOne<svt::Model<svt::BasicFV> > mcModel;
  svt::GroupedTrainingData<svt::BasicFV> trainData( featureVectors.begin(), 
                                                    featureVectors.end(),
                                                    svt::DirectAccessor());
    
  LMBUNIT_ASSERT_EQUAL( trainData.nClasses(), 4);
  

  LMBUNIT_ASSERT_EQUAL( trainData.classIndexToLabel(0), 0);
  LMBUNIT_ASSERT_EQUAL( trainData.classIndexToLabel(1), 1);
  LMBUNIT_ASSERT_EQUAL( trainData.classIndexToLabel(2), 2);
  LMBUNIT_ASSERT_EQUAL( trainData.classIndexToLabel(3), 3);

  svm.updateKernelCache( featureVectors.begin(), 
                         featureVectors.end(),
                         svt::DirectAccessor());
  svm.train( trainData, mcModel);
  svm.clearKernelCache();
  

  /*-----------------------------------------------------------------------
   *  print training statistics
   *-----------------------------------------------------------------------*/
  for( int i = 0; i < 4; ++i)
  {
    for( int j = i+1; j < 4; ++j)
    {
      LMBUNIT_DEBUG_STREAM << "training statistics for classes " 
                           << i << " vs. " << j << std::endl;
      LMBUNIT_DEBUG_STREAM << mcModel.twoClassModel(i,j).trainingInfoPlainText();
      for( unsigned int k = 0; k < mcModel.twoClassModel(i,j).size(); ++k)
      {
        LMBUNIT_DEBUG_STREAM << k << ". sv: (" << mcModel.twoClassModel(i,j).alpha(k) << ") " << *mcModel.twoClassModel(i,j).supportVector(k) << std::endl;
      }
      
    }
  }
  
  LMBUNIT_ASSERT_EQUAL( mcModel.collectedSupportVectors().size(), 4);
  LMBUNIT_ASSERT_EQUAL( mcModel.collectedSupportVectors()[0],
                        &featureVectors[0]);
  LMBUNIT_ASSERT_EQUAL( mcModel.collectedSupportVectors()[1],
                        &featureVectors[1]);
  LMBUNIT_ASSERT_EQUAL( mcModel.collectedSupportVectors()[2],
                        &featureVectors[2]);
  LMBUNIT_ASSERT_EQUAL( mcModel.collectedSupportVectors()[3],
                        &featureVectors[3]);
  
  LMBUNIT_ASSERT_EQUAL( mcModel.maxUniqueID(),3);
  

  /*-----------------------------------------------------------------------
   * check training statistics for FullModel
   *-----------------------------------------------------------------------*/
  svt::StDataASCII statistics;
  mcModel.saveTCTrainingInfos( statistics);
  mcModel.saveTrainingInfoStatistics( statistics);

  LMBUNIT_ASSERT_EQUAL( statistics.asUint( "sum_twoclass_nFV"), 12);
  


  svt::TriangularMatrix<double> resultMatrix(4);

  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[0], mcModel, resultMatrix), 3);
#if 1
  LMBUNIT_DEBUG_STREAM << std::endl;
  for( int row = 0; row < 3; ++row)
  {
    for( int indent = 0; indent < row; ++indent)
    {
      LMBUNIT_DEBUG_STREAM << "-\t";
    }
    
    for( int col = row+1; col < 4; ++col)
    {
      LMBUNIT_DEBUG_STREAM << resultMatrix(row,col) << "\t";
    }
    LMBUNIT_DEBUG_STREAM << std::endl;
  }
#endif
  
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[1], mcModel), 2);
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[2], mcModel), 1);
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[3], mcModel), 0);
}

static void testMultiClassSVMOneVsOneC_2()
{
  typedef svt::BasicFV FV;

  std::vector<FV> featureVectors(8);
  _fillFV( featureVectors[0], 0,   0,  1,  0);
  _fillFV( featureVectors[1], 0,   0, -1,  0);
  _fillFV( featureVectors[2], 0,   0,  0,  1);
  _fillFV( featureVectors[3], 0,   0,  0, -1);
  _fillFV( featureVectors[4], 1,   0,  2,  0);
  _fillFV( featureVectors[5], 2,   0, -2,  0);
  _fillFV( featureVectors[6], 3,   0,  0,  2);
  _fillFV( featureVectors[7], 4,   0,  0, -2);
  
  svt::adjustUniqueIDs( featureVectors);

  svt::MultiClassSVMOneVsOne< svt::TwoClassSVMc< svt::Kernel_LINEAR> > svm;
  svm.twoClassSVM().setCost( 100);

  svt::Model_MC_OneVsOne< svt::Model<FV> > mcModel;
  
  svm.updateKernelCache( featureVectors.begin(), featureVectors.end(), 
                         svt::DirectAccessor());
  svm.train( featureVectors.begin(), featureVectors.end(), mcModel);
  svm.clearKernelCache();
  
  /*-----------------------------------------------------------------------
   *  Check if all Models have exact two support vectors
   *-----------------------------------------------------------------------*/
  for( unsigned int firstClass = 0; 
       firstClass < mcModel.nClasses()-1; ++firstClass)
  {
    for( unsigned int secondClass = firstClass+1; 
         secondClass < mcModel.nClasses(); ++secondClass)
    {
      LMBUNIT_ASSERT_EQUAL( mcModel.twoClassModel( firstClass, secondClass).size(), 2);
    }
  }

  /*-----------------------------------------------------------------------
   *  check if feature vectors are correctly classified 
   *-----------------------------------------------------------------------*/
  for( unsigned int i = 0; i < featureVectors.size(); ++i)
  {
    LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[i], mcModel), featureVectors[i].getLabel());
  }
  

  /*-----------------------------------------------------------------------
   *  create some test vectors, and check if they are correctly classified
   *-----------------------------------------------------------------------*/
  std::vector<FV> testVectors(11);
  _fillFV( testVectors[0], 0,   0,  0,    0);
  _fillFV( testVectors[1], 0,   0,  0.5,  0);
  _fillFV( testVectors[2], 0,   0,  1.4,  1.4);
  _fillFV( testVectors[3], 0,   1,  0,    0);
  _fillFV( testVectors[4], 1,   0,  1.6,   0);
  _fillFV( testVectors[5], 1,   0,  42,   41);
  _fillFV( testVectors[6], 2,   0,  -137,  0);
  _fillFV( testVectors[7], 2,   0,  -1.9,  1.8);
  _fillFV( testVectors[8], 3,   0,  0,     37 );
  _fillFV( testVectors[9], 3,   0,  1.5,  1.6 );
  _fillFV( testVectors[10], 4,   0,  0,  -1.6 );
  

  for( unsigned int i = 0; i < testVectors.size(); ++i)
  {
    LMBUNIT_ASSERT_EQUAL( svm.classify( testVectors[i], mcModel),
                          testVectors[i].getLabel());
  }

}

  


static void testMultiClassSVMOneVsOneNu()
{
  std::vector<svt::BasicFV> featureVectors(4);

  _fillFV( featureVectors[0], 3,  0, 0, 0);
  _fillFV( featureVectors[1], 2,  1, 0, 0);
  _fillFV( featureVectors[2], 1,  0, 1, 0);
  _fillFV( featureVectors[3], 0,  0, 0, 1);

  svt::adjustUniqueIDs( featureVectors);

  svt::MultiClassSVMOneVsOne< svt::TwoClassSVMnu< svt::Kernel_LINEAR> > svm;
      
  svt::Model_MC_OneVsOne< svt::Model<svt::BasicFV> > mcModel;

  svm.updateKernelCache( featureVectors.begin(), featureVectors.end(),
                         svt::DirectAccessor());
  svm.train( featureVectors.begin(), featureVectors.end(), mcModel);
  svm.clearKernelCache();
  
  svt::TriangularMatrix<double> resultMatrix(4);

  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[0], mcModel, resultMatrix), 3);
#if 1
  LMBUNIT_DEBUG_STREAM << std::endl;
  for( int row = 0; row < 3; ++row)
  {
    for( int col = row+1; col < 4; ++col)
    {
      LMBUNIT_DEBUG_STREAM << resultMatrix(row,col) << "\t";
    }
    LMBUNIT_DEBUG_STREAM << std::endl;
  }
#endif
  
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[1], mcModel), 2);
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[2], mcModel), 1);
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[3], mcModel), 0);
}

static void testMultiClassSVMOneVsRestC()
{
  std::vector<svt::BasicFV> featureVectors(4);
  int nClasses = 4;
  
  _fillFV( featureVectors[0], 3,  0, 0, 0);
  _fillFV( featureVectors[1], 2,  1, 0, 0);
  _fillFV( featureVectors[2], 1,  0, 1, 0);
  _fillFV( featureVectors[3], 0,  0, 0, 1);
  svt::adjustUniqueIDs( featureVectors);

  svt::MultiClassSVMOneVsRest< svt::TwoClassSVMc< svt::Kernel_LINEAR> > svm;
  svm.twoClassSVM().setCost( 100);
  
      
  svt::Model_MC_OneVsRest< svt::Model<svt::BasicFV> > mcModel;

  svm.updateKernelCache( featureVectors.begin(), featureVectors.end(),
                         svt::DirectAccessor());
  svm.train( featureVectors.begin(), featureVectors.end(), mcModel);
  svm.clearKernelCache();
  
  std::vector<double> resultVector(nClasses);

  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[0], mcModel, resultVector), 3);
#if 1
  LMBUNIT_DEBUG_STREAM << std::endl;
  for( int classIndex = 0; classIndex < nClasses; ++classIndex)
  {
    LMBUNIT_DEBUG_STREAM << resultVector[classIndex] << "\t";
  }
#endif
  
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[1], mcModel), 2);
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[2], mcModel), 1);
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[3], mcModel), 0);
}

static void testMultiClassSVMOneVsRestC2()
{
  typedef svt::BasicFV FV;

  int nClasses = 5;

  std::vector<FV> featureVectors(8);
  _fillFV( featureVectors[0], 0,   0,  1,  0);
  _fillFV( featureVectors[1], 0,   0, -1,  0);
  _fillFV( featureVectors[2], 0,   0,  0,  1);
  _fillFV( featureVectors[3], 0,   0,  0, -1);
  _fillFV( featureVectors[4], 1,   0,  2,  0);
  _fillFV( featureVectors[5], 2,   0, -2,  0);
  _fillFV( featureVectors[6], 3,   0,  0,  2);
  _fillFV( featureVectors[7], 4,   0,  0, -2);
  svt::adjustUniqueIDs( featureVectors);
  
  svt::MultiClassSVMOneVsRest< svt::TwoClassSVMc< svt::Kernel_RBF> > svm;
  svm.twoClassSVM().setCost( 100);
  svm.twoClassSVM().kernel().setGamma( 1);
  

  svt::Model_MC_OneVsRest< svt::Model<svt::BasicFV> > mcModel;

  svm.updateKernelCache( featureVectors.begin(), featureVectors.end(),
                         svt::DirectAccessor());
  svm.train( featureVectors.begin(), featureVectors.end(), mcModel);
  svm.clearKernelCache();
 

  /*-----------------------------------------------------------------------
   *  check if feature vectors are correctly classified 
   *-----------------------------------------------------------------------*/
  std::vector<double> decValueVector( nClasses);
  for( unsigned int i = 0; i < featureVectors.size(); ++i)
  {
    LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[i], 
                                        mcModel,
                                        decValueVector),
                          featureVectors[i].getLabel());
    LMBUNIT_DEBUG_STREAM << "reclassifying feature vector " 
                         << i << ": decision values:\n";
    for( int j = 0; j < nClasses; ++j)
    {
      LMBUNIT_DEBUG_STREAM << decValueVector[j] << ", ";
    }
    LMBUNIT_DEBUG_STREAM << std::endl;
  }
  
}


static void testMultiClassRetrain()
{
  typedef svt::BasicFV FV;

  int nClasses = 5;

  std::vector<FV> featureVectors(8);
  _fillFV( featureVectors[0], 0,   0,  1,  0);
  _fillFV( featureVectors[1], 0,   0, -1,  0);
  _fillFV( featureVectors[2], 0,   0,  0,  1);
  _fillFV( featureVectors[3], 0,   0,  0, -1);
  _fillFV( featureVectors[4], 1,   0,  2,  0);
  _fillFV( featureVectors[5], 2,   0, -2,  0);
  _fillFV( featureVectors[6], 3,   0,  0,  2);
  _fillFV( featureVectors[7], 4,   0,  0, -2);
  svt::adjustUniqueIDs( featureVectors);

  svt::MultiClassSVMOneVsRest< svt::TwoClassSVMc< svt::Kernel_RBF> > svm;
  svm.twoClassSVM().setCost( 100);
  svm.twoClassSVM().kernel().setGamma( 1);


  svt::Model_MC_OneVsRest< svt::Model<svt::BasicFV> > mcModel;

  svm.updateKernelCache( featureVectors.begin(), featureVectors.end(),
                         svt::DirectAccessor());
  svm.train( featureVectors.begin(), featureVectors.end(), mcModel);
  svm.clearKernelCache();


  /*-----------------------------------------------------------------------
   *  check if feature vectors are correctly classified 
   *-----------------------------------------------------------------------*/
  std::vector<double> decValueVector( nClasses);
  for( unsigned int i = 0; i < featureVectors.size(); ++i)
  {
    LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[i],
                                        mcModel,
                                        decValueVector),
                          featureVectors[i].getLabel());
    LMBUNIT_DEBUG_STREAM << "reclassifying feature vector "
                         << i << ": decision values:\n";
    for( int j = 0; j < nClasses; ++j)
    {
      LMBUNIT_DEBUG_STREAM << decValueVector[j] << ", ";
    }
    LMBUNIT_DEBUG_STREAM << std::endl;
  }


  featureVectors.resize(5);
  //_fillFV( featureVectors[8], 6,   1,  0,  2);
  //_fillFV( featureVectors[9], 4,   5,  0,  5);
  svt::adjustUniqueIDs( featureVectors); 
  svm.updateKernelCache( featureVectors.begin(), featureVectors.end(),
                         svt::DirectAccessor());
  svm.train( featureVectors.begin(), featureVectors.end(), mcModel);
  svm.clearKernelCache();

  /*-----------------------------------------------------------------------
   *  check if feature vectors are correctly classified 
   *-----------------------------------------------------------------------*/
  for( unsigned int i = 0; i < featureVectors.size(); ++i)
  {
    LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[i],
                                        mcModel,
                                        decValueVector),
                          featureVectors[i].getLabel());
    LMBUNIT_DEBUG_STREAM << "reclassifying feature vector "
                         << i << ": decision values:\n";
    for( int j = 0; j < nClasses; ++j)
    {
      LMBUNIT_DEBUG_STREAM << decValueVector[j] << ", ";
    }
    LMBUNIT_DEBUG_STREAM << std::endl;
  }
    

}



static void testInOutOneVsOne()
{
  typedef svt::BasicFV FV;

  std::vector<FV> featureVectors(8);
  _fillFV( featureVectors[0], -1,   0,  1,  0);
  _fillFV( featureVectors[1], -1,   0, -1,  0);
  _fillFV( featureVectors[2], -1,   0,  0,  1);
  _fillFV( featureVectors[3], -1,   0,  0, -1);
  _fillFV( featureVectors[4], 1,   0,  2,  0);
  _fillFV( featureVectors[5], -2,   0, -2,  0);
  _fillFV( featureVectors[6], 3,   0,  0,  2);
  _fillFV( featureVectors[7], -4,   0,  0, -2);
  
  svt::adjustUniqueIDs( featureVectors);

  svt::MultiClassSVMOneVsOne< svt::TwoClassSVMc< svt::Kernel_LINEAR> > svm;
  svm.twoClassSVM().setCost( 100);

  svt::Model_MC_OneVsOne< svt::Model<FV> > mcModel;
  
  svm.updateKernelCache( featureVectors.begin(), featureVectors.end(),
                         svt::DirectAccessor());
  svm.train( featureVectors.begin(), featureVectors.end(), mcModel);
  svm.clearKernelCache();

  /*-----------------------------------------------------------------------
   *  save model to string-string map
   *-----------------------------------------------------------------------*/
  svt::StDataASCII stData;
    
  mcModel.saveParameters( stData);
  stData.debugPrint(LMBUNIT_DEBUG_STREAM);
  
  /*-----------------------------------------------------------------------
   *  load model back from map
   *-----------------------------------------------------------------------*/
  svt::Model_MC_OneVsOne< svt::Model<FV> > mcModel2;

  stData.setExceptionFlag( true);
  
  mcModel2.loadParameters( stData);
  
  LMBUNIT_ASSERT_EQUAL( mcModel.nClasses(), mcModel2.nClasses());
  LMBUNIT_ASSERT_EQUAL( mcModel.collectedSupportVectors().size(), 
                        mcModel2.collectedSupportVectors().size());
  for( unsigned int i = 0; i < mcModel.collectedSupportVectors().size(); ++i)
  {
    LMBUNIT_ASSERT_EQUAL( *(mcModel.collectedSupportVectors()[i]),
                          *(mcModel2.collectedSupportVectors()[i]));
  }
  
}


static void testInOutOneVsOne2()
{
  std::cout << "Reading " << TOP_SRC_DIR << "/test/libsvmtl/heart_scale.vec"
            << std::endl;
  /*-----------------------------------------------------------------------
   *  load heart_scale dataset
   *-----------------------------------------------------------------------*/
  typedef svt::BasicFV FV;

  std::vector<FV> featureVectors;
  std::ifstream inFile( TOP_SRC_DIR "/test/libsvmtl/heart_scale.vec");
  FV fv;
  while(inFile >> fv)
  {
    featureVectors.push_back(fv);
  }
  std::cout << "done" << std::endl;
  LMBUNIT_ASSERT_EQUAL( featureVectors.size(), 270);
  svt::adjustUniqueIDs( featureVectors);

  /*-----------------------------------------------------------------------
   *  train svm
   *-----------------------------------------------------------------------*/
  svt::MultiClassSVMOneVsOne< svt::TwoClassSVMc< svt::Kernel_RBF> > svm;
  svm.twoClassSVM().setCost( 100);
  svm.twoClassSVM().kernel().setGamma( 0.5);

  svt::Model_MC_OneVsOne< svt::Model<FV> > mcModel;
  
  svm.updateKernelCache( featureVectors.begin(), featureVectors.end(),
                         svt::DirectAccessor());
  svm.train( featureVectors.begin(), featureVectors.end(), mcModel);
  svm.clearKernelCache();

  /*-----------------------------------------------------------------------
   *  classify all training vectors (must be 100% correct)
   *-----------------------------------------------------------------------*/
  for( unsigned int i = 0; i < featureVectors.size(); ++i)
  {
    LMBUNIT_ASSERT_EQUAL( featureVectors[i].getLabel(), 
                          svm.classify( featureVectors[i], mcModel));
  }
  
  /*-----------------------------------------------------------------------
   *  now save model and svm
   *-----------------------------------------------------------------------*/
  svt::StDataASCIIFile modelData;
  svm.saveParameters( modelData);
  mcModel.saveParameters( modelData);
  
  std::ofstream modelFile( "/tmp/heart_scale.model");
  modelData.save( modelFile);
  modelFile.close();
  
  /*-----------------------------------------------------------------------
   *  load Model file
   *-----------------------------------------------------------------------*/
  svt::StDataASCIIFile modelData2;
  std::ifstream modelFile2( "/tmp/heart_scale.model");
  modelData2.load( modelFile2);
  
  svt::MultiClassSVMOneVsOne< svt::TwoClassSVMc< svt::Kernel_RBF> > svm2;
  svm2.loadParameters( modelData2);
  svt::Model_MC_OneVsOne< svt::Model<FV> > mcModel2;
  mcModel2.loadParameters( modelData2);
  
  /*-----------------------------------------------------------------------
   *  classify all training vectors with loaded model (must be 100% correct)
   *-----------------------------------------------------------------------*/
  for( unsigned int i = 0; i < featureVectors.size(); ++i)
  {
    LMBUNIT_ASSERT_EQUAL( featureVectors[i].getLabel(), 
                          svm2.classify( featureVectors[i], mcModel2));
  }
  


}



static void testInOutOneVsRest()
{
  typedef svt::BasicFV FV;
  
  std::vector<FV> featureVectors(8);
  _fillFV( featureVectors[0], 0,   0,  1,  0);
  _fillFV( featureVectors[1], 0,   0, -1,  0);
  _fillFV( featureVectors[2], 0,   0,  0,  1);
  _fillFV( featureVectors[3], 0,   0,  0, -1);
  _fillFV( featureVectors[4], 1,   0,  2,  0);
  _fillFV( featureVectors[5], 2,   0, -2,  0);
  _fillFV( featureVectors[6], 3,   0,  0,  2);
  _fillFV( featureVectors[7], 4,   0,  0, -2);
  
  svt::adjustUniqueIDs( featureVectors);
  
  svt::MultiClassSVMOneVsRest< svt::TwoClassSVMc< svt::Kernel_LINEAR> > svm;
  svm.twoClassSVM().setCost( 100);
  
  svt::Model_MC_OneVsRest< svt::Model<FV> > mcModel;
  
  svm.updateKernelCache( featureVectors.begin(), featureVectors.end(),
                         svt::DirectAccessor());
  svm.train( featureVectors.begin(), featureVectors.end(), mcModel);
  svm.clearKernelCache();
  
  /*-----------------------------------------------------------------------
   *  save model to string-string map
   *-----------------------------------------------------------------------*/
  svt::StDataASCII stData;
    
  mcModel.saveParameters( stData);
  stData.debugPrint(LMBUNIT_DEBUG_STREAM);

  /*-----------------------------------------------------------------------
   *  load model back from map
   *-----------------------------------------------------------------------*/
  svt::Model_MC_OneVsRest< svt::Model<FV> > mcModel2;

  stData.setExceptionFlag( true);
  mcModel2.loadParameters( stData);
  
  LMBUNIT_ASSERT_EQUAL( mcModel.nClasses(), mcModel2.nClasses());
  LMBUNIT_ASSERT_EQUAL( mcModel.collectedSupportVectors().size(), 
                        mcModel2.collectedSupportVectors().size());
  for( unsigned int i = 0; i < mcModel.collectedSupportVectors().size(); ++i)
  {
    LMBUNIT_ASSERT_EQUAL( *(mcModel.collectedSupportVectors()[i]),
                          *(mcModel2.collectedSupportVectors()[i]));
  }
  
}


static void testProgressReporterOneVsOne()
{
  typedef svt::BasicFV FV;
  
  std::vector<FV> featureVectors(8);
  _fillFV( featureVectors[0], 0,   0,  1,  0);
  _fillFV( featureVectors[1], 0,   0, -1,  0);
  _fillFV( featureVectors[2], 0,   0,  0,  1);
  _fillFV( featureVectors[3], 0,   0,  0, -1);
  _fillFV( featureVectors[4], 1,   0,  2,  0);
  _fillFV( featureVectors[5], 2,   0, -2,  0);
  _fillFV( featureVectors[6], 3,   0,  0,  2);
  _fillFV( featureVectors[7], 4,   0,  0, -2);
  svt::adjustUniqueIDs( featureVectors);
  
  svt::MultiClassSVMOneVsOne< svt::TwoClassSVMc< svt::Kernel_LINEAR> > svm;
  svm.twoClassSVM().setCost( 100);

  svt::ProgressReporter myProgressReporter( LMBUNIT_DEBUG_STREAM);
  
  svm.setProgressReporter( &myProgressReporter);

  svt::Model_MC_OneVsOne< svt::Model<FV> > mcModel;
  
  LMBUNIT_DEBUG_STREAM << std::endl;
  
  svm.updateKernelCache( featureVectors.begin(), featureVectors.end(),
                         svt::DirectAccessor());
  svm.train( featureVectors.begin(), featureVectors.end(), mcModel);
  svm.clearKernelCache();
}

static void testProgressReporterOneVsRest()
{
  typedef svt::BasicFV FV;
  
  std::vector<FV> featureVectors(8);
  _fillFV( featureVectors[0], 0,   0,  1,  0);
  _fillFV( featureVectors[1], 0,   0, -1,  0);
  _fillFV( featureVectors[2], 0,   0,  0,  1);
  _fillFV( featureVectors[3], 0,   0,  0, -1);
  _fillFV( featureVectors[4], 1,   0,  2,  0);
  _fillFV( featureVectors[5], 2,   0, -2,  0);
  _fillFV( featureVectors[6], 3,   0,  0,  2);
  _fillFV( featureVectors[7], 4,   0,  0, -2);

  adjustUniqueIDs( featureVectors);
  
  
  svt::MultiClassSVMOneVsRest< svt::TwoClassSVMc< svt::Kernel_LINEAR> > svm;
  svm.twoClassSVM().setCost( 100);

  svt::ProgressReporter myProgressReporter( LMBUNIT_DEBUG_STREAM);
  
  svm.setProgressReporter( &myProgressReporter);

  svt::Model_MC_OneVsRest< svt::Model<FV> > mcModel;
  
  LMBUNIT_DEBUG_STREAM << std::endl;
  
  svm.updateKernelCache( featureVectors.begin(), featureVectors.end(),
                         svt::DirectAccessor());
  svm.train( featureVectors.begin(), featureVectors.end(), mcModel);
  svm.clearKernelCache();
}



int main( int argc, char** argv)
{
  testInOutOneVsOne2();

  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST( testMultiClassSVMOneVsOneC() );
  LMBUNIT_RUN_TEST( testMultiClassSVMOneVsOneC_2() );
  LMBUNIT_RUN_TEST( testMultiClassSVMOneVsOneNu() );
  LMBUNIT_RUN_TEST( testMultiClassSVMOneVsRestC() );
  LMBUNIT_RUN_TEST( testMultiClassSVMOneVsRestC2() );
  LMBUNIT_RUN_TEST( testInOutOneVsOne() );
  LMBUNIT_RUN_TEST( testInOutOneVsOne2() );
  LMBUNIT_RUN_TEST( testInOutOneVsRest() );
  LMBUNIT_RUN_TEST( testProgressReporterOneVsOne() );
  LMBUNIT_RUN_TEST( testProgressReporterOneVsRest() );
  LMBUNIT_RUN_TEST( testMultiClassRetrain() );
  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


