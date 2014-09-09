/**************************************************************************
**       Title: 
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
**  Revision 1.3  2005/02/10 17:30:33  ronneber
**  further integration of plattprob stuff
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

#include <sstream>
#include <cstdlib> // rand()

#include "lmbunit.hh"
#include <libsvmtl/BasicFV.hh>
#include <libsvmtl/SparseFV.hh>
#include <libsvmtl/Kernel_LINEAR.hh>
#include <libsvmtl/Kernel_RBF.hh>
#include <libsvmtl/Kernel_POLY.hh>
#include <libsvmtl/Kernel_SIGMOID.hh>
#include <libsvmtl/Kernel_MATRIX.hh>
#include <libsvmtl/StDataASCII.hh>
#include <libsvmtl/DirectAccessor.hh>


/*! \test test linear and RBF kernels k_function()
 *  \ingroup test
*/
template<typename FV>
static void testLinearAndRBF()
{
  FV fv1, fv2;
  fv1.resize( 3);
  fv1[0] = 1;
  fv1[1] = 2;
  fv1[2] = 3;
  
  fv2.resize( 3);
  fv2[0] = 3;
  fv2[1] = 4;
  fv2[2] = 5;

  svt::Kernel_LINEAR kernelLin;
 
  LMBUNIT_ASSERT_EQUAL_DELTA( kernelLin.k_function( fv1, fv2),
                              fv1[0] * fv2[0] +  
                              fv1[1] * fv2[1] +
                              fv1[2] * fv2[2], 0.0000001);
  
  svt::Kernel_RBF kernelRBF(2);
  
  LMBUNIT_ASSERT_EQUAL_DELTA( kernelRBF.k_function( fv1, fv2),
                              exp( -2 * 
                                   ((fv1[0] - fv2[0]) * (fv1[0] - fv2[0]) +
                                    (fv1[1] - fv2[1]) * (fv1[1] - fv2[1]) +
                                    (fv1[2] - fv2[2]) * (fv1[2] - fv2[2]))),
                              0.0000001);
  fv2[0] = 42;
  
  LMBUNIT_ASSERT_EQUAL_DELTA( kernelLin.k_function( fv1, fv2),
                              fv1[0] * fv2[0] +  
                              fv1[1] * fv2[1] +
                              fv1[2] * fv2[2], 0.0000001);
  
  LMBUNIT_ASSERT_EQUAL_DELTA( kernelRBF.k_function( fv1, fv2),
                              exp( -2 * 
                                   ((fv1[0] - fv2[0]) * (fv1[0] - fv2[0]) +
                                    (fv1[1] - fv2[1]) * (fv1[1] - fv2[1]) +
                                    (fv1[2] - fv2[2]) * (fv1[2] - fv2[2]))),
                              0.0000001);
}




template< typename KERNEL>
static void testKernelMatrix()
{
  LMBUNIT_ASSERT_EQUAL( std::string( svt::Kernel_MATRIX<KERNEL>::name()),
                        std::string( "kmatrix_") + KERNEL::name());

  // create 42 random feature vectors
  std::vector<svt::BasicFV> featureVectors(42);
  
  for( unsigned int i = 0; i < featureVectors.size(); ++i)
  {
    featureVectors[i].resize( 9);
    for( int j = 0; j < 9; ++j)
    {
      featureVectors[i][j] = double(std::rand())/RAND_MAX;
    }
  }

  svt::adjustUniqueIDs( featureVectors);
   
  
  KERNEL kern;
  svt::Kernel_MATRIX< KERNEL> matKern;

  LMBUNIT_ASSERT_EQUAL( std::string(matKern.name()), std::string( "kmatrix_") + kern.name());
  
  
  matKern.updateCache( featureVectors.begin(), featureVectors.end(), 
                       svt::DirectAccessor());
  
  for( unsigned int i = 0; i < featureVectors.size(); ++i)
  {
    for( unsigned int j = 0; j < featureVectors.size(); ++j)
    {
      LMBUNIT_ASSERT_EQUAL_DELTA( kern.k_function( featureVectors[i], 
                                                   featureVectors[j]),
                                  matKern.k_function( featureVectors[i], 
                                                      featureVectors[j]),
                                  0.0000001);
    }
  }
  
}


static void testRBFDefault()
{
  svt::Kernel_RBF kernel;
  svt::StDataASCII stdata;
  kernel.saveParameters( stdata);
  LMBUNIT_ASSERT_EQUAL( stdata.asDouble("gamma"), 1.0);
  
  stdata.setValue( "gamma", 2.7);
  kernel.loadParameters( stdata);

  svt::StDataASCII stdata2;
  kernel.saveParameters( stdata2);
  LMBUNIT_ASSERT_EQUAL( stdata2.asDouble("gamma"), 2.7);
  stdata2.setValue( "feature_vector_dim", 10);
  kernel.loadParameters( stdata2);
  LMBUNIT_ASSERT_EQUAL( kernel.gamma(), 2.7);
  
  svt::StDataASCII stdata4;
  stdata4.setValue( "feature_vector_dim", 4);
  kernel.loadParameters( stdata4);
  LMBUNIT_ASSERT_EQUAL( kernel.gamma(), 0.25);
  
}



int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();
  LMBUNIT_RUN_TEST( testLinearAndRBF<svt::BasicFV>() );
  LMBUNIT_RUN_TEST( testLinearAndRBF<svt::SparseFV>() );
  LMBUNIT_RUN_TEST( testKernelMatrix<svt::Kernel_LINEAR>() );
  LMBUNIT_RUN_TEST( testKernelMatrix<svt::Kernel_RBF>() );
  LMBUNIT_RUN_TEST( testKernelMatrix<svt::Kernel_POLY>() );
  LMBUNIT_RUN_TEST( testKernelMatrix<svt::Kernel_SIGMOID>() );
  LMBUNIT_RUN_TEST( testRBFDefault() );
  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


