/**************************************************************************
**       Title: test unit for Platts probabilities
**    $RCSfile$
**   $Revision: 598 $$Name$
**       $Date: 2005-02-10 18:30:33 +0100 (Thu, 10 Feb 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2005/02/10 17:30:33  ronneber
**  further integration of plattprob stuff
**
**  Revision 1.1  2005/02/01 12:36:35  ronneber
**  initial revision
**
**
**
**************************************************************************/

#include <sstream>

#include "lmbunit.hh"
#include <BasicFV.hh>
#include <Kernel_LINEAR.hh>
#include <Kernel_RBF.hh>
#include <TwoClassSVMc.hh>
#include <TwoClassSVMnu.hh>
#include <SVM_Problem.hh>
#include <PlattProb.hh>

template<typename FV>
static void testPlattProbTwoClassSVMc()
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

  svt::PlattProb< svt::TwoClassSVMc< svt::Kernel_LINEAR> > svm;
  
  svt::PlattProbModel< svt::Model<FV> > model;

  svm.setCost( 100);
  svm.train( featureVectors.begin(), featureVectors.end(), model);

  LMBUNIT_ASSERT_EQUAL( model.size(), 2);
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( featureVectors[0], model), 0.0,
                              0.000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( featureVectors[1], model), 1.0,
                              0.000001);
  
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("iterations"), 1);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nu"),         0.02);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("cost"),       100);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("obj"),        -2);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nSV"),        2);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nBSV"),       0);
 
}




int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST_NOFORK( testPlattProbTwoClassSVMc<svt::BasicFV>() );
  LMBUNIT_RUN_TEST_NOFORK( testPlattProbTwoClassSVMc<svt::SparseFV>() );

  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


