/**************************************************************************
**       Title: test unit for SVM factory
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
**  Revision 1.7  2005/03/29 18:09:15  ronneber
**  - adapted to new manual call of updateKernelCache() and clearKernelCache()
**
**  Revision 1.6  2005/02/24 12:57:50  fehr
**  some more 64-bit netcdf bugfixing
**
**  Revision 1.5  2005/02/14 15:57:57  fehr
**  new test which SVMFactory fails
**
**  Revision 1.4  2005/02/11 12:50:46  ronneber
**  *** empty log message ***
**
**  Revision 1.3  2005/02/11 12:41:42  fehr
**  testSVMFactory still fails
**
**  Revision 1.2  2004/09/08 14:45:49  ronneber
**  *** empty log message ***
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.1  2003/05/19 11:38:11  ronneber
**  - initial revision
**
**#
**
**************************************************************************/


#include <map>
#include <string>
#include <fstream>

#include "lmbunit.hh"
#include <libsvmtl/TList.hh>
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
#include <libsvmtl/TriangularMatrix.hh>
#include <libsvmtl/MultiClassSVMOneVsRest.hh>
#include <libsvmtl/BasicSVMAdapterTempl.hh>
#include <libsvmtl/SVMAdapter.hh>
#include <libsvmtl/SVMFactory.hh>

/*-------------------------------------------------------------------------
 *  specify multi-class Algorithms
 *-------------------------------------------------------------------------*/
typedef TTLIST_2( svt::MultiClassSVMOneVsOne, 
                  svt::MultiClassSVMOneVsRest) MyMultiClassList;

/*-------------------------------------------------------------------------
 *  specify two-class Algorithms
 *-------------------------------------------------------------------------*/
typedef TTLIST_2( svt::TwoClassSVMc, 
                  svt::TwoClassSVMnu) MyTwoClassList;

/*-------------------------------------------------------------------------
 *  specify kernel functions
 *-------------------------------------------------------------------------*/
typedef TLIST_6( svt::Kernel_LINEAR, 
                 svt::Kernel_RBF, 
                 svt::Kernel_POLY, 
                 svt::Kernel_SIGMOID,
                 svt::Kernel_MATRIX<svt::Kernel_LINEAR>,
                 svt::Kernel_MATRIX<svt::Kernel_RBF> ) MyKernelList;


template< typename FV>
void _fillFV( FV& fv, int label, double f0, double f1, double f2)
{
  fv.resize( 3);
  fv.setLabel( label);
  fv[0] = f0;
  fv[1] = f1;
  fv[2] = f2;
}


static void testSVMAdapterStuff()
{
  svt::BasicSVMAdapter<svt::BasicFV, svt::StDataASCIIFile>* svm = 
      new svt::BasicSVMAdapterTempl< svt::BasicFV, svt::StDataASCIIFile,
      svt::MultiClassSVMOneVsRest< svt::TwoClassSVMc< svt::Kernel_LINEAR> > >;

  std::vector<svt::BasicFV > featureVectors(4);

  _fillFV( featureVectors[0], 3,  0, 0, 0);
  _fillFV( featureVectors[1], 2,  1, 0, 0);
  _fillFV( featureVectors[2], 1,  0, 1, 0);
  _fillFV( featureVectors[3], 0,  0, 0, 1);
  svt::adjustUniqueIDs( featureVectors);

  svt::StDataASCII params;
  params.setValue("cost", 100);
  svm->loadParameters( params);
  
  svt::GroupedTrainingData<svt::BasicFV> trainData( featureVectors.begin(), 
                                                    featureVectors.end(), 
                                                    svt::DirectAccessor());
  svm->updateKernelCache( trainData);
  svm->train( trainData);
  svm->clearKernelCache();

  std::cerr<<svm->nSupportVectors()<<"\n";
  
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[0]), 3);
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[1]), 2);
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[2]), 1);
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[3]), 0);

  delete svm;
  
}

static void testSVMFactory( const std::string& mcName, 
                            const std::string& tcName, 
                            const std::string& kfName)
{
  typedef svt::BasicFV FV;
  
  std::vector<FV > featureVectors(4);

  _fillFV( featureVectors[0], 3,  0, 0, 0);
  _fillFV( featureVectors[1], 2,  1, 0, 0);
  _fillFV( featureVectors[2], 1,  0, 1, 0);
  _fillFV( featureVectors[3], 0,  0, 0, 1);
  svt::adjustUniqueIDs( featureVectors);



  svt::SVMAdapter* svm = svt::SVMFactory::create( mcName, tcName, kfName);

  svt::StDataASCII params;
  params.setValue("cost", 100);
  params.setValue("nu", 0.01);
  svm->loadParameters( params);
  
  svt::GroupedTrainingData<FV> trainData( featureVectors.begin(), featureVectors.end(), 
                                          svt::DirectAccessor());
  svm->updateKernelCache( trainData);
  svm->train( trainData);
  svm->clearKernelCache();

  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[0]), 3);
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[1]), 2);
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[2]), 1);
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[3]), 0);

  delete svm;
  

}

template< typename FV>
static void testBasicSVMFactory( const std::string& mcName, 
                                 const std::string& tcName, 
                                 const std::string& kfName)
{
  std::vector<FV > featureVectors(4);

  _fillFV( featureVectors[0], 3,  0, 0, 0);
  _fillFV( featureVectors[1], 2,  1, 0, 0);
  _fillFV( featureVectors[2], 1,  0, 1, 0);
  _fillFV( featureVectors[3], 0,  0, 0, 1);
  svt::adjustUniqueIDs( featureVectors);



  svt::BasicSVMAdapter<FV, svt::StDataASCIIFile>* svm 
      = svt::BasicSVMFactory<
      FV, 
      svt::StDataASCIIFile,
      MyMultiClassList,
      MyTwoClassList,
      MyKernelList
      >::create( mcName, tcName, kfName);

  svt::StDataASCII params;
  params.setValue("cost", 100);
  params.setValue("nu", 0.01);
  svm->loadParameters( params);

  svt::ProgressReporter pr;
  pr.setVerboseLevel( 0);
  
  svm->setProgressReporter( &pr);
  

  svt::GroupedTrainingData<FV> trainData( featureVectors.begin(), featureVectors.end(), 
                                          svt::DirectAccessor());
  svm->updateKernelCache( trainData);
  svm->train( trainData);
  svm->clearKernelCache();


  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[0]), 3);
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[1]), 2);
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[2]), 1);
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[3]), 0);

}

static void testSVMFactoryStDataASCII()
{
  std::vector<svt::BasicFV > trainVectors(4);

  _fillFV( trainVectors[0], 3,  0, 0, 0);
  _fillFV( trainVectors[1], 2,  1, 0, 0);
  _fillFV( trainVectors[2], 1,  0, 1, 0);
  _fillFV( trainVectors[3], 0,  0, 0, 1);
  svt::adjustUniqueIDs( trainVectors);


  svt::StDataASCII params;
  params.setValue("multi_class_type", "one_vs_one");
  params.setValue("two_class_type",         "c_svc");
  params.setValue("cost",             100);
  params.setValue("scale_algorithm",  "minmax");
  params.setValue("kernel_type",      "kmatrix_scaled_rbf");
  params.setValue("gamma",            1.234);
  params.setValue("verbose_level",    0);

  svt::SVMAdapter* svm = svt::SVMFactory::createFromStData( params);
  svm->loadParameters( params);
   
  LMBUNIT_DEBUG_STREAM << "training...\n";
  
  svt::GroupedTrainingData<svt::BasicFV> trainData( trainVectors.begin(),
                                                    trainVectors.end(), 
                                                    svt::DirectAccessor());
  svm->updateKernelCache( trainData);
  svm->train( trainData);
  svm->clearKernelCache();


  std::vector<svt::BasicFV > featureVectors(6);

  _fillFV( featureVectors[0], 2,  1, 0, 0);
  _fillFV( featureVectors[1], 1,  0, 1, 0);
  _fillFV( featureVectors[2], 0,  0, 0, 1);
  _fillFV( featureVectors[3], 3,  0, 0, 0);
  _fillFV( featureVectors[4], 3,  0, 0, 0);
  _fillFV( featureVectors[5], 3,  0, 0, 0);
  svt::adjustUniqueIDs( featureVectors);

  
    

  
  LMBUNIT_DEBUG_STREAM << "classification...\n";
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[0]), 2);
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[1]), 1);
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[2]), 0);
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[3]), 3);
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[4]), 3);
  LMBUNIT_ASSERT_EQUAL( svm->classify( featureVectors[5]), 3);

//  std::map< std::string, std::string> params2;
//  svm->saveToMap( params2);
//
//  LMBUNIT_ASSERT_EQUAL(params["multi_class_type"],params2["multi_class_type"]);
//  LMBUNIT_ASSERT_EQUAL( params["two_class_type"], params2["two_class_type"]);
//  LMBUNIT_ASSERT_EQUAL( params["cost"], params2["cost"]);
//  LMBUNIT_ASSERT_EQUAL( params["kernel_type"], params2["kernel_type"]);
//  LMBUNIT_ASSERT_EQUAL( params["gamma"], params2["gamma"]);
}


template<typename FV>
static void testSVMFactory2( const std::string& mcName, 
                             const std::string& tcName, 
                             const std::string& kfName)
{
  /*-----------------------------------------------------------------------
   *  do the folowing training-pattern:
   *
   *               3
   *
   *               0
   *
   *       2   0       0   1
   *
   *               0
   *
   *               4
   *
   *-----------------------------------------------------------------------*/
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


  svt::BasicSVMAdapter<FV, svt::StDataASCIIFile>* svm 
      = svt::BasicSVMFactory<
      FV, 
      svt::StDataASCIIFile,
      MyMultiClassList,
      MyTwoClassList,
      MyKernelList
      >::create( mcName, tcName, kfName);

  svt::StDataASCII params;
  params.setValue("cost", 1000);
  params.setValue("nu", 0.01);
  params.setValue("gamma", 0.001);
  svm->loadParameters( params);

  svt::GroupedTrainingData<FV> trainData( featureVectors.begin(), featureVectors.end(), 
                                          svt::DirectAccessor());
  svm->updateKernelCache( trainData);
  svm->train( trainData);
  svm->clearKernelCache();

  /*-----------------------------------------------------------------------
   *  create some test vectors, and check if they are correctly classified
   *-----------------------------------------------------------------------*/
  std::vector<FV> testVec(11);
  _fillFV( testVec[0], 0,   0,  0,    0);
  _fillFV( testVec[1], 0,   0,  0.5,  0);
  _fillFV( testVec[2], 0,   0,  1.4,  1.4);
  _fillFV( testVec[3], 0,   1,  0,    0);
  _fillFV( testVec[4], 1,   0,  1.6,   0);
  _fillFV( testVec[5], 1,   0,  42,   41);
  _fillFV( testVec[6], 2,   0,  -137,  0);
  _fillFV( testVec[7], 2,   0,  -1.9,  1.8);
  _fillFV( testVec[8], 3,   0,  0,     37 );
  _fillFV( testVec[9], 3,   0,  1.5,  1.6 );
  _fillFV( testVec[10], 4,   0,  0,  -1.6 );

  LMBUNIT_ASSERT_EQUAL( svm->classify( testVec[0]), testVec[0].getLabel());
  LMBUNIT_ASSERT_EQUAL( svm->classify( testVec[1]), testVec[1].getLabel());
  LMBUNIT_ASSERT_EQUAL( svm->classify( testVec[2]), testVec[2].getLabel());
  LMBUNIT_ASSERT_EQUAL( svm->classify( testVec[3]), testVec[3].getLabel());
  LMBUNIT_ASSERT_EQUAL( svm->classify( testVec[4]), testVec[4].getLabel());
  LMBUNIT_ASSERT_EQUAL( svm->classify( testVec[5]), testVec[5].getLabel());
  LMBUNIT_ASSERT_EQUAL( svm->classify( testVec[6]), testVec[6].getLabel());
  LMBUNIT_ASSERT_EQUAL( svm->classify( testVec[7]), testVec[7].getLabel());
  LMBUNIT_ASSERT_EQUAL( svm->classify( testVec[8]), testVec[8].getLabel());
  LMBUNIT_ASSERT_EQUAL( svm->classify( testVec[9]), testVec[9].getLabel());
  LMBUNIT_ASSERT_EQUAL( svm->classify( testVec[10]), testVec[10].getLabel());
}



//
//
//
//
//static void testSVMFactoryLoadSave( )
//{
//  typedef svt::FVwithMultiClassCoefs<svt::BasicFV> FV;
//
//  std::vector<FV> featureVectors(8);
//  _fillFV( featureVectors[0], 0,   0,  1,  0);
//  _fillFV( featureVectors[1], 0,   0, -1,  0);
//  _fillFV( featureVectors[2], 0,   0,  0,  1);
//  _fillFV( featureVectors[3], 0,   0,  0, -1);
//  _fillFV( featureVectors[4], 1,   0,  2,  0);
//  _fillFV( featureVectors[5], 2,   0, -2,  0);
//  _fillFV( featureVectors[6], 3,   0,  0,  2);
//  _fillFV( featureVectors[7], 4,   0,  0, -2);
//
//  std::map< std::string, std::string> params;
//  params["multi_class_type"] = "one_vs_one";
//  params["two_class_type"]         = "c_svc";
//  params["cost"]             = "100";
//  params["kernel_type"]      = "linear";
//  params["gamma"]            = "1.234";
//  params["verbose_level"]    = "0";
//  
//
//  svt::SVMAdapter* svm = svt::SVMFactory::createFromMap( params);
//  svm->train( featureVectors.begin(), featureVectors.end());
//
//  /*-----------------------------------------------------------------------
//   *  save Model
//   *-----------------------------------------------------------------------*/
//  std::map<std::string, std::string> params2;
//  svm->saveModel( params2);
//  svt::ParamMapWrapper pmw2( params2);
//  std::ofstream os( "test.model");
//  pmw2.save( os);
//  os.close();
//  
//  /*-----------------------------------------------------------------------
//   *  create new svm from saved model
//   *-----------------------------------------------------------------------*/
//  std::ifstream is( "test.model");
//  std::map<std::string, std::string> params3;
//  svt::ParamMapWrapper pmw3( params3);
//  pmw3.load( is);
//  is.close();
//  
//  svt::SVMAdapter* svm2 = svt::SVMFactory::createFromMap( params2);
//  svm2->loadModel( params3);
//  
//  
//  /*-----------------------------------------------------------------------
//   *  create some test vectors, and check if they are correctly classified
//   *-----------------------------------------------------------------------*/
//  std::vector<FV> testVec(11);
//  _fillFV( testVec[0], 0,   0,  0,    0);
//  _fillFV( testVec[1], 0,   0,  0.5,  0);
//  _fillFV( testVec[2], 0,   0,  1.4,  1.4);
//  _fillFV( testVec[3], 0,   1,  0,    0);
//  _fillFV( testVec[4], 1,   0,  1.6,   0);
//  _fillFV( testVec[5], 1,   0,  42,   41);
//  _fillFV( testVec[6], 2,   0,  -137,  0);
//  _fillFV( testVec[7], 2,   0,  -1.9,  1.8);
//  _fillFV( testVec[8], 3,   0,  0,     37 );
//  _fillFV( testVec[9], 3,   0,  1.5,  1.6 );
//  _fillFV( testVec[10], 4,   0,  0,  -1.6 );
//
//  LMBUNIT_ASSERT_EQUAL( svm2->classify( testVec[0]), testVec[0].getLabel());
//  LMBUNIT_ASSERT_EQUAL( svm2->classify( testVec[1]), testVec[1].getLabel());
//  LMBUNIT_ASSERT_EQUAL( svm2->classify( testVec[2]), testVec[2].getLabel());
//  LMBUNIT_ASSERT_EQUAL( svm2->classify( testVec[3]), testVec[3].getLabel());
//  LMBUNIT_ASSERT_EQUAL( svm2->classify( testVec[4]), testVec[4].getLabel());
//  LMBUNIT_ASSERT_EQUAL( svm2->classify( testVec[5]), testVec[5].getLabel());
//  LMBUNIT_ASSERT_EQUAL( svm2->classify( testVec[6]), testVec[6].getLabel());
//  LMBUNIT_ASSERT_EQUAL( svm2->classify( testVec[7]), testVec[7].getLabel());
//  LMBUNIT_ASSERT_EQUAL( svm2->classify( testVec[8]), testVec[8].getLabel());
//  LMBUNIT_ASSERT_EQUAL( svm2->classify( testVec[9]), testVec[9].getLabel());
//  LMBUNIT_ASSERT_EQUAL( svm2->classify( testVec[10]), testVec[10].getLabel());
//}
//
//


int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();
  LMBUNIT_RUN_TEST_NOFORK( testSVMAdapterStuff() );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_one", "c_svc", "linear") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_one", "c_svc", "rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_one", "c_svc", "poly") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_one", "c_svc", "sigmoid") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_one", "nu_svc", "linear") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_one", "nu_svc", "rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_one", "nu_svc", "poly") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_one", "nu_svc", "sigmoid") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_rest", "c_svc", "linear") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_rest", "c_svc", "rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_rest", "c_svc", "poly") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_rest", "c_svc", "sigmoid") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_rest", "nu_svc", "linear") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_rest", "nu_svc", "rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_rest", "nu_svc", "poly") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory( "one_vs_rest", "nu_svc", "sigmoid") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_one", "c_svc", "linear") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_one", "c_svc", "rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_one", "c_svc", "poly") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_one", "c_svc", "sigmoid") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::BasicFV>( "one_vs_one", "c_svc", "kmatrix_linear") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::BasicFV>( "one_vs_one", "c_svc", "kmatrix_rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_one", "nu_svc", "linear") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_one", "nu_svc", "rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_one", "nu_svc", "poly") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_one", "nu_svc", "sigmoid") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_rest", "c_svc", "linear") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_rest", "c_svc", "rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_rest", "c_svc", "poly") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_rest", "c_svc", "sigmoid") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::BasicFV>( "one_vs_rest", "c_svc", "kmatrix_linear") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::BasicFV>( "one_vs_rest", "c_svc", "kmatrix_rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_rest", "nu_svc", "linear") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_rest", "nu_svc", "rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_rest", "nu_svc", "poly") );
  LMBUNIT_RUN_TEST_NOFORK( testBasicSVMFactory<svt::SparseFV>( "one_vs_rest", "nu_svc", "sigmoid") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactoryStDataASCII() );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory2<svt::BasicFV>( "one_vs_one", "c_svc", "linear") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory2<svt::BasicFV>( "one_vs_one", "c_svc", "rbf") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory2<svt::BasicFV>( "one_vs_one", "nu_svc", "linear") );
  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory2<svt::BasicFV>( "one_vs_one", "nu_svc", "rbf") );


//  LMBUNIT_RUN_TEST_NOFORK( testSVMFactory_withC());
//  LMBUNIT_RUN_TEST_NOFORK( testSVMFactoryLoadSave());

  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


