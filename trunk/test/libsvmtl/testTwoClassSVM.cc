/**************************************************************************
**       Title: test unit for TwoClassSVM classes
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
**  Revision 1.3  2005/03/29 18:10:22  ronneber
**  - adapted to new manual call of updateKernelCache() and clearKernelCache()
**
**  Revision 1.2  2004/09/08 14:45:49  ronneber
**  *** empty log message ***
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.5  2003/05/19 11:43:28  ronneber
**  - additional test for correct solution info parameters
**
**  Revision 1.4  2002/05/13 16:30:56  ronneber
**  - adapted to new Model and SupportVector class (without public attributes)
**
**  Revision 1.3  2002/05/10 11:07:03  ronneber
**  - removed FV template for all public classes, because Feature Vector type
**    can be extracted automatically from passed iterators or other
**    parameters -- this makes the public interface much more intuitive
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

#include <sstream>

#include "lmbunit.hh"
#include <libsvmtl/BasicFV.hh>
#include <libsvmtl/Kernel_LINEAR.hh>
#include <libsvmtl/Kernel_RBF.hh>
#include <libsvmtl/TwoClassSVMc.hh>
#include <libsvmtl/TwoClassSVMnu.hh>
#include <libsvmtl/SVM_Problem.hh>

#include "MyFeatureVector.hh"
#include "MyKernel.hh"
 

template<typename FV>
static void testTwoClassSVMc_rawInterface()
{
  FV featureVector0;
  featureVector0.resize( 3);
  featureVector0[0] = 0;
  featureVector0[1] = 0;
  featureVector0[2] = 0;

  
  FV featureVector1;
  featureVector1.resize( 3);
  featureVector1[0] = 1;
  featureVector1[1] = 0;
  featureVector1[2] = 0;

  svt::SVM_Problem<FV> prob( 2);
  prob.x[0] = &featureVector0;
  prob.y[0] = -1;
  prob.x[1] = &featureVector1;
  prob.y[1] = +1;
  prob.l = 2;


  svt::TwoClassSVMc< svt::Kernel_LINEAR> svm;
  
  svt::Model<FV> model;

  svm.setCost( 100);
  
  svm.updateKernelCache( prob.FV_begin(), prob.FV_end(), svt::DereferencingAccessor());
  svm.train( prob, model);
  svm.clearKernelCache();
  

  svt::StDataASCII params;
  model.saveParameters( params);
  params.debugPrint( LMBUNIT_DEBUG_STREAM);
  

  LMBUNIT_DEBUG_STREAM << model.trainingInfoPlainText();
    
  // classification without cache
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( featureVector0, model), 
                              -1, 0.000001);

  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( featureVector1, model), 
                              +1, 0.000001);

  //classification with cache
  featureVector0.setUniqueID( 0);
  featureVector1.setUniqueID( 1);
  
  std::vector<double> kernelResultByUID(2);
  
  
  svm.calcClassificationCache( featureVector0, 
                               model.allSupportVectors(),
                               model.allSupportVectors() + 2,
                               kernelResultByUID);

  LMBUNIT_ASSERT_EQUAL_DELTA( 
      svm.classifyWithCache( model, kernelResultByUID), -1, 0.000001);

  svm.calcClassificationCache( featureVector1, 
                               model.allSupportVectors(),
                               model.allSupportVectors() + 2,
                               kernelResultByUID);

  LMBUNIT_ASSERT_EQUAL_DELTA( 
      svm.classifyWithCache( model, kernelResultByUID), +1, 0.000001);

  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("iterations"), 1);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nu"),         0.02);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("cost"),       100);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("obj"),        -2);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nSV"),        2);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nBSV"),       0);



}

template<typename FV>
static void testTwoClassSVMc()
{
  std::vector<FV> featureVectors(2);

  featureVectors[0].setLabel(-1);
  featureVectors[0].resize( 3);
  featureVectors[0][0] = 0;
  featureVectors[0][1] = 0;
  featureVectors[0][2] = 0;
  
  
  featureVectors[1].setLabel(+1);
  featureVectors[1].resize( 3);
  featureVectors[1][0] = 1;
  featureVectors[1][1] = 0;
  featureVectors[1][2] = 0;

  svt::TwoClassSVMc< svt::Kernel_LINEAR> svm;
  
  svt::Model<FV> model;

  svm.setCost( 100);
  svm.updateKernelCache( featureVectors.begin(), featureVectors.end(),
                         svt::DirectAccessor());
  svm.train( featureVectors.begin(), featureVectors.end(), model);
  svm.clearKernelCache();

  LMBUNIT_ASSERT_EQUAL( model.size(), 2);
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( featureVectors[0], model), -1,
                              0.000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( featureVectors[1], model), +1,
                              0.000001);
  
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("iterations"), 1);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nu"),         0.02);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("cost"),       100);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("obj"),        -2);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nSV"),        2);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nBSV"),       0);
 

}



template<typename FV>
static void testTwoClassSVMnu()
{
  std::vector<FV> featureVectors(2);

  featureVectors[0].setLabel(-1);
  featureVectors[0].resize( 3);
  featureVectors[0][0] = 0;
  featureVectors[0][1] = 0;
  featureVectors[0][2] = 0;
  
  
  featureVectors[1].setLabel(+1);
  featureVectors[1].resize( 3);
  featureVectors[1][0] = 1;
  featureVectors[1][1] = 0;
  featureVectors[1][2] = 0;


  svt::TwoClassSVMnu< svt::Kernel_LINEAR> svm;
  
  svt::Model<FV> model;
 
  svm.updateKernelCache( featureVectors.begin(), featureVectors.end(),
                         svt::DirectAccessor());
  svm.train( featureVectors.begin(), featureVectors.end(), model);
  svm.clearKernelCache();
  
  LMBUNIT_ASSERT_EQUAL( model.size(), 2);
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( featureVectors[0], model), -1,
                              0.000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( featureVectors[1], model), +1,
                              0.000001);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("iterations"), 0);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nu"),         0.5);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("cost"),       4);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("obj"),        2);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nSV"),        2);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nBSV"),       0);

}


       

static void testOwnFVClassesWithSVMc()
{
  MyFeatureVector featureVectors[2];

  featureVectors[0].setXYAndLabel( 0,0, -1);
  featureVectors[1].setXYAndLabel( 0,2, +1);

  MyKernel kernel;
  svt::TwoClassSVMc< MyKernel> svm( kernel);
  svt::Model<MyFeatureVector> model;
  
  svm.updateKernelCache( featureVectors+0, featureVectors+2,
                         svt::DirectAccessor());
  svm.train( featureVectors, featureVectors+2, model);
  svm.clearKernelCache();
  

  LMBUNIT_ASSERT_EQUAL( model.size(), 2);
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( featureVectors[0], model), -1,
                              0.000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( featureVectors[1], model), +1,
                              0.000001);
  
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("iterations"), 1);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nu"),         0.5);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("cost"),       1);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("obj"),        -0.5);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nSV"),        2);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nBSV"),       0);
  
}

  
static void testOwnFVClassesWithSVMnu()
{
  MyFeatureVector featureVectors[2];

  featureVectors[0].setXYAndLabel( 0,0, -1);
  featureVectors[1].setXYAndLabel( 0,2, +1);

  MyKernel kernel;
  svt::TwoClassSVMnu< MyKernel> svm( kernel);
  svt::Model<MyFeatureVector> model;
  
  svm.updateKernelCache( featureVectors+0, featureVectors+2,
                         svt::DirectAccessor());
  svm.train( featureVectors, featureVectors+2, model);
  svm.clearKernelCache();

  LMBUNIT_ASSERT_EQUAL( model.size(), 2);
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( featureVectors[0], model), -1,
                              0.000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( featureVectors[1], model), +1,
                              0.000001);
  
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("iterations"), 0);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nu"),         0.5);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("cost"),       1);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("obj"),        0.5);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nSV"),        2);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nBSV"),       0);
   
}

template<typename FV>
static void testModelInputOutput()
{
  std::vector<FV> featureVectors(2);

  featureVectors[0].setLabel(-1);
  featureVectors[0].setUniqueID( 0);
  featureVectors[0].resize( 3);
  featureVectors[0][0] = 0;
  featureVectors[0][1] = 0;
  featureVectors[0][2] = 0;
  
  
  featureVectors[1].setLabel(+1);
  featureVectors[1].setUniqueID( 1);
  featureVectors[1].resize( 3);
  featureVectors[1][0] = 1;
  featureVectors[1][1] = 0;
  featureVectors[1][2] = 0;


  svt::TwoClassSVMnu< svt::Kernel_LINEAR> svm;
  
  svt::Model<FV> model;
 
  svm.updateKernelCache( featureVectors.begin(), featureVectors.end(),
                         svt::DirectAccessor());
  svm.train( featureVectors.begin(), featureVectors.end(), model);
  svm.clearKernelCache();
  
  /*-----------------------------------------------------------------------
   *  save model to map and load to other model using uids
   *-----------------------------------------------------------------------*/
  svt::StDataASCII stData;
  model.saveParametersWithUIDs( stData);

  stData.debugPrint( LMBUNIT_DEBUG_STREAM);

  svt::Model<FV> model2;
  std::vector<FV*> fvsByUID( 2);
  fvsByUID[featureVectors[0].uniqueID()] = &featureVectors[0];
  fvsByUID[featureVectors[1].uniqueID()] = &featureVectors[1];
  
  LMBUNIT_DEBUG_STREAM << "fvsByUID[0] = " << fvsByUID[0] << std::endl;
  LMBUNIT_DEBUG_STREAM << "fvsByUID[1] = " << fvsByUID[1] << std::endl;
  
  stData.setExceptionFlag( true);
  model2.loadParametersWithUIDs( stData, fvsByUID);
  
  LMBUNIT_ASSERT_EQUAL( model.size(), model2.size());
  LMBUNIT_ASSERT_EQUAL( model.rho(), model2.rho());
  LMBUNIT_ASSERT_EQUAL( model.alpha(0), model2.alpha(0));
  LMBUNIT_ASSERT_EQUAL( model.alpha(1), model2.alpha(1));
  LMBUNIT_ASSERT_EQUAL( model.supportVector(0), model2.supportVector(0));
  LMBUNIT_ASSERT_EQUAL( model.supportVector(1), model2.supportVector(1));
  
  /*-----------------------------------------------------------------------
   *  save model to map and load to other model using full featureVectors
   *-----------------------------------------------------------------------*/
  model.detachFromTrainingDataSet();
  svt::StDataASCII stData3;
  model.saveParameters( stData3);
  stData3.debugPrint( LMBUNIT_DEBUG_STREAM);

  svt::Model<FV> model3;
  stData3.setExceptionFlag( true);
  model3.loadParameters(stData3);

  LMBUNIT_ASSERT_EQUAL( model.size(), model3.size());
  LMBUNIT_ASSERT_EQUAL( model.rho(), model3.rho());
  LMBUNIT_ASSERT_EQUAL( model.alpha(0), model3.alpha(0));
  LMBUNIT_ASSERT_EQUAL( model.alpha(1), model3.alpha(1));
  LMBUNIT_ASSERT_EQUAL( *model.supportVector(0), *model3.supportVector(0));
  LMBUNIT_ASSERT_EQUAL( *model.supportVector(1), *model3.supportVector(1));
  
}



int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST_NOFORK( testTwoClassSVMc_rawInterface<svt::BasicFV>() );
  LMBUNIT_RUN_TEST_NOFORK( testTwoClassSVMc_rawInterface<svt::SparseFV>() );
  LMBUNIT_RUN_TEST_NOFORK( testTwoClassSVMc<svt::BasicFV>() );
  LMBUNIT_RUN_TEST_NOFORK( testTwoClassSVMc<svt::SparseFV>() );
  LMBUNIT_RUN_TEST_NOFORK( testTwoClassSVMnu<svt::BasicFV>() );
  LMBUNIT_RUN_TEST_NOFORK( testTwoClassSVMnu<svt::SparseFV>() );
  LMBUNIT_RUN_TEST_NOFORK( testOwnFVClassesWithSVMc() );
  LMBUNIT_RUN_TEST_NOFORK( testOwnFVClassesWithSVMnu() );
  LMBUNIT_RUN_TEST_NOFORK( testModelInputOutput<svt::BasicFV>() );
  LMBUNIT_RUN_TEST_NOFORK( testModelInputOutput<svt::SparseFV>() );
  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


