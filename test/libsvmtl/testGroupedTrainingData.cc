/**************************************************************************
**       Title: test GroupedTrainingData class
**    $RCSfile$
**   $Revision: 507 $$Name$
**       $Date: 2004-09-03 11:14:56 +0200 (Fri, 03 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2004/09/03 09:14:56  ronneber
**  *** empty log message ***
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/


#include "lmbunit.hh"
#include <libsvmtl/BasicFV.hh>
#include <libsvmtl/GroupedTrainingData.hh>
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


static void testCreateFromContainer()
{
  std::vector<svt::BasicFV> featureVectors(10);
  
  _fillFV( featureVectors[0], 4,  0, 0, 0);
  _fillFV( featureVectors[1], 4,  0, 0, 0);
  _fillFV( featureVectors[2], 4,  0, 0, 0);
  _fillFV( featureVectors[3], 4,  0, 0, 0);
  _fillFV( featureVectors[4], 3,  1, 0, 0);
  _fillFV( featureVectors[5], 3,  1, 0, 0);
  _fillFV( featureVectors[6], 1,  0, 1, 0);
  _fillFV( featureVectors[7], 1,  0, 1, 0);
  _fillFV( featureVectors[8], 0,  0, 0, 1);
  _fillFV( featureVectors[9], -1,  0, 0, 1);


  svt::GroupedTrainingData<svt::BasicFV> trainData( featureVectors.begin(),
                                                    featureVectors.end(),
                                                    svt::DirectAccessor());
  
  LMBUNIT_ASSERT_EQUAL( trainData.nClasses(), 5);
  LMBUNIT_ASSERT_EQUAL( trainData.classIndexToLabel(0), -1);
  LMBUNIT_ASSERT_EQUAL( trainData.classIndexToLabel(1), 0);
  LMBUNIT_ASSERT_EQUAL( trainData.classIndexToLabel(2), 1);
  LMBUNIT_ASSERT_EQUAL( trainData.classIndexToLabel(3), 3);
  LMBUNIT_ASSERT_EQUAL( trainData.classIndexToLabel(4), 4);
  
  LMBUNIT_ASSERT_EQUAL( trainData.classStartIndex(0), 0);
  LMBUNIT_ASSERT_EQUAL( trainData.classStartIndex(1), 1);
  LMBUNIT_ASSERT_EQUAL( trainData.classStartIndex(2), 2);
  LMBUNIT_ASSERT_EQUAL( trainData.classStartIndex(3), 4);
  LMBUNIT_ASSERT_EQUAL( trainData.classStartIndex(4), 6);
  
}



int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST( testCreateFromContainer() );

  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


