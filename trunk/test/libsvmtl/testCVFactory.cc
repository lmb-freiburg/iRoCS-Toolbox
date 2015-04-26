/**************************************************************************
**       Title: 
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
**  Revision 1.3  2005/03/29 18:07:53  ronneber
**  - adapted to new manual call of updateKernelCache() and clearKernelCache()
**
**  Revision 1.2  2004/09/03 09:14:56  ronneber
**  *** empty log message ***
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#include <map>
#include <string>
#include <fstream>

#include "lmbunit.hh"
#include <libsvmtl/StDataASCII.hh>
#include <libsvmtl/BasicFV.hh>
#include <libsvmtl/SparseFV.hh>
#include <libsvmtl/Kernel_LINEAR.hh>
#include <libsvmtl/Kernel_RBF.hh>
#include <libsvmtl/Kernel_POLY.hh>
#include <libsvmtl/Kernel_SIGMOID.hh>
#include <libsvmtl/Kernel_MATRIX.hh>
#include <libsvmtl/TwoClassSVMc.hh>
#include <libsvmtl/TwoClassSVMnu.hh>
#include <libsvmtl/MultiClassSVMOneVsOne.hh>
#include <libsvmtl/MultiClassSVMOneVsRest.hh>
#include <libsvmtl/BasicCVFactory.hh>
#include <libsvmtl/BasicCVAdapter.hh>
#include <libsvmtl/CVFactory.hh>
#include <libsvmtl/CVAdapter.hh>
#include <libsvmtl/DirectAccessor.hh>


template< typename FV>
void _fillFV( FV& fv, int label, double f0, double f1, double f2)
{
  fv.resize( 3);
  fv.setLabel( label);
  fv[0] = f0;
  fv[1] = f1;
  fv[2] = f2;
}

static void testBasics( const std::string& mcName, 
                        const std::string& tcName, 
                        const std::string& kfName)
{
  
  svt::CVAdapter* cv =
      svt::CVFactory::create( mcName, tcName, kfName);
  
  svt::StDataASCII params;
  params.setValue("cost",           100000);
  params.setValue("nu",             0.01);
  params.setValue("verbose_level",    0);
  cv->loadParameters( params);
  
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
  cv->setTrainingData( &trainData);
  cv->updateKernelCache();
  cv->preprocessTrainingData();

  
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
  int nCorrect = cv->doFullCV( subsetIndexByUID, predictedClassLabelByUID);
  

  LMBUNIT_ASSERT_EQUAL( nCorrect, 8);
  for( unsigned int i = 0; i < featureVectors.size(); ++i)
  {
    LMBUNIT_ASSERT_EQUAL_DELTA( 
        featureVectors[i].getLabel(), 
        predictedClassLabelByUID[featureVectors[i].uniqueID()], 0.000001);
  }
    
}
int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_one", "c_svc", "linear") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_one", "c_svc", "rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_one", "c_svc", "poly") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_one", "c_svc", "sigmoid") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_one", "nu_svc", "linear") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_one", "nu_svc", "rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_one", "nu_svc", "poly") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_one", "nu_svc", "sigmoid") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_rest", "c_svc", "linear") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_rest", "c_svc", "rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_rest", "c_svc", "poly") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_rest", "c_svc", "sigmoid") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_rest", "nu_svc", "linear") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_rest", "nu_svc", "rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_rest", "nu_svc", "poly") );
  LMBUNIT_RUN_TEST_NOFORK( testBasics( "one_vs_rest", "nu_svc", "sigmoid") );

  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}

