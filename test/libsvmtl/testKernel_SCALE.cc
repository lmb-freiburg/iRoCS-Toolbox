/**************************************************************************
**       Title: test ScaleData class
**    $RCSfile$
**   $Revision: 508 $$Name$
**       $Date: 2004-09-03 11:42:39 +0200 (Fri, 03 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/09/03 09:42:39  ronneber
**  initial revision
**
**
**
**************************************************************************/

#include <iostream>
#include <fstream>
#include <vector>

#include "lmbunit.hh"

#include <libsvmtl/BasicFV.hh>
#include <libsvmtl/SparseFV.hh>
#include <libsvmtl/DirectAccessor.hh>
#include <libsvmtl/DereferencingAccessor.hh>
#include <libsvmtl/StDataASCII.hh>
#include <libsvmtl/Kernel_SCALE.hh>
#include <libsvmtl/Kernel_LINEAR.hh>

template< typename FV>
void _fillFV( FV& fv, int label, double f0, double f1, double f2)
{
  fv.resize( 3);
  fv.setLabel( label);
  fv[0] = f0;
  fv[1] = f1;
  fv[2] = f2;
}


template<typename FV>
static void testMinMax()
{
  svt::Kernel_SCALE<svt::Kernel_LINEAR> kern;
  std::vector<FV> featureVectors(2);

  featureVectors[0].setLabel(-1);
  featureVectors[0].resize( 5);
  featureVectors[0][0] = 0;
  featureVectors[0][1] = 1;
  featureVectors[0][2] = 3;
  featureVectors[0][3] = 0;
  featureVectors[0][4] = 42;

  featureVectors[1].setLabel(+1);
  featureVectors[1].resize( 5);
  featureVectors[1][0] = 2;
  featureVectors[1][1] = 4;
  featureVectors[1][2] = 0.5;
  featureVectors[1][3] = 0;
  featureVectors[1][4] = 42;

  kern.setAlgorithm( "minmax");
  kern.calcScaleAndOffset( featureVectors.begin(), featureVectors.end(),
                          svt::DirectAccessor());

  LMBUNIT_ASSERT_EQUAL_DELTA( kern.offset(0), -1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern.offset(1), -2.5, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern.offset(2), -1.75, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern.offset(3), 0, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern.offset(4), -42, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern.scaleFactor(0), 1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern.scaleFactor(1), 1.0/1.5, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern.scaleFactor(2), 1.0/1.25, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern.scaleFactor(3), 1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern.scaleFactor(4), 1, 0.00000001);

  kern.scaleSingleFV( featureVectors[0], featureVectors[0]);
  kern.scaleSingleFV( featureVectors[1], featureVectors[1]);

  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[0][0], -1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[0][1], -1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[0][2], 1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[0][3], 0, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[0][4], 0, 0.00000001);

  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[1][0], 1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[1][1], 1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[1][2], -1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[1][3], 0, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[1][4], 0, 0.00000001);


}

template<typename FV>
static void testMinMaxPointerArray()
{
  /*-----------------------------------------------------------------------
   *  Now test with DereferencingAccessor
   *-----------------------------------------------------------------------*/
  std::vector<FV> featureVectors(2);

  featureVectors[0].setLabel(-1);
  featureVectors[0].resize( 5);
  featureVectors[0][0] = 0;
  featureVectors[0][1] = 1;
  featureVectors[0][2] = 3;
  featureVectors[0][3] = 0;
  featureVectors[0][4] = 42;

  featureVectors[1].setLabel(+1);
  featureVectors[1].resize( 5);
  featureVectors[1][0] = 2;
  featureVectors[1][1] = 4;
  featureVectors[1][2] = 0.5;
  featureVectors[1][3] = 0;
  featureVectors[1][4] = 42;
  std::vector<FV*> feaVecs2(2);
  feaVecs2[0] = &featureVectors[0];
  feaVecs2[1] = &featureVectors[1];

  svt::Kernel_SCALE<svt::Kernel_LINEAR> kern2;
  kern2.setAlgorithm( "minmax");
  kern2.calcScaleAndOffset( feaVecs2.begin(), feaVecs2.end(),
                          svt::DereferencingAccessor());

  LMBUNIT_ASSERT_EQUAL_DELTA( kern2.offset(0), -1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern2.offset(1), -2.5, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern2.offset(2), -1.75, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern2.offset(3), 0, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern2.offset(4), -42, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern2.scaleFactor(0), 1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern2.scaleFactor(1), 1.0/1.5, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern2.scaleFactor(2), 1.0/1.25, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern2.scaleFactor(3), 1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( kern2.scaleFactor(4), 1, 0.00000001);

  kern2.scaleSingleFV( featureVectors[0], featureVectors[0]);
  kern2.scaleSingleFV( featureVectors[1], featureVectors[1]);

  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[0][0], -1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[0][1], -1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[0][2], 1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[0][3], 0, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[0][4], 0, 0.00000001);

  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[1][0], 1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[1][1], 1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[1][2], -1, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[1][3], 0, 0.00000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( featureVectors[1][4], 0, 0.00000001);



}

template<typename FV>
static void testLoadSave()
{
  /*-----------------------------------------------------------------------
   *  create data and save it
   *-----------------------------------------------------------------------*/
  std::vector<FV> featureVectors(4);

  _fillFV( featureVectors[0], 3,  1,  2, -3);
  _fillFV( featureVectors[1], 2, -4,  2,  0);
  _fillFV( featureVectors[2], 1,  7,  3, 10);
  _fillFV( featureVectors[3], 0, 21, -2, -7);


  svt::Kernel_SCALE<svt::Kernel_LINEAR>  kern;
  kern.setAlgorithm( "minmax");
  kern.calcScaleAndOffset( featureVectors.begin(), featureVectors.end(),
                           svt::DirectAccessor());

  svt::StDataASCII stdata;
  kern.saveParameters( stdata);

  /*-----------------------------------------------------------------------
   *  load scale data and compare it to original data
   *-----------------------------------------------------------------------*/
  svt::Kernel_SCALE<svt::Kernel_LINEAR> kern2;
  kern2.loadParameters( stdata);

  LMBUNIT_ASSERT_EQUAL( kern2.algorithm(), "");
  LMBUNIT_ASSERT_EQUAL( kern.nComponents(), kern2.nComponents());

  for( unsigned int i = 0; i < kern2.nComponents(); ++i)
  {
    LMBUNIT_ASSERT_EQUAL_DELTA( kern.scaleFactor(i), kern2.scaleFactor(i),
                                0.00000001);
    LMBUNIT_ASSERT_EQUAL_DELTA( kern.offset(i), kern2.offset(i),
                                0.00000001);
  }

}



int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST( testMinMax<svt::BasicFV>() );
  LMBUNIT_RUN_TEST( testMinMax<svt::SparseFV>() );
  LMBUNIT_RUN_TEST( testMinMaxPointerArray<svt::BasicFV>() );
  LMBUNIT_RUN_TEST( testMinMaxPointerArray<svt::SparseFV>() );
  LMBUNIT_RUN_TEST( testLoadSave<svt::BasicFV>() );
  LMBUNIT_RUN_TEST( testLoadSave<svt::SparseFV>() );

  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}
